
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "stdio_controller.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "console.h"
#include "hal_uart.h"
#include "board.h"
#include "semphr.h"

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

enum sensors {
    uart_index_imu = 0, 
    uart_index_debug = 1, 
};

#define PACKET_BUFFER_SIZE 8
#define MESSAGE_BUFFER_SIZE 512
#define MAX_PACKET_IDS 0x74
#define EVER ;;
#define RX 0
#define TX 1

#define FLAGGED 1
#define UNFLAGGED 0

/*******************************************************************************
 * Variables
 ******************************************************************************/


//packet's recieved buffer
//are these amounts reasonable?
uint8_t packetBuffers[1][MAX_PACKET_IDS][PACKET_BUFFER_SIZE][MESSAGE_BUFFER_SIZE] = {0};
//[ID][Packet][Packet Contents]
int packetBuffersWriteIndex[1][MAX_PACKET_IDS] = {0};
int packetBuffersReadIndex[1][MAX_PACKET_IDS] = {0}; 
int packetBuffersReadLowerLimit[1][MAX_PACKET_IDS] = {0}; //needs to be initialized in the init function
int packetBuffersNewFlag[1][MAX_PACKET_IDS] = {0};

//the upper packet limit is the lower limit + the packet buffer size. It's the
//range of allowable readable values

SemaphoreHandle_t simInSemaphore;
SemaphoreHandle_t simOutSemaphore;

/* TODO: update for all sensors */
static hal_uart_handle_t *uart_handles[1];

/*******************************************************************************
 * Declarations
 ******************************************************************************/
static void inputLoop(void * pv);
static void outputLoop(void * pv);
static void generateImuLoop(void * pv);
static void output(char c);
static void synOut(char c);
static void putPacket(const uint8_t id, const char *c, char const length);
static void putConfigPacket();
static uint8_t getCinForce();
static uint8_t getFilteredCin();
static void extractPacket();
static void sendPackets();
static uint8_t readFromBuf(int mode, uint8_t data[],uint8_t id);
static void writeToBuf(int mode, uint8_t data[],uint8_t id,uint16_t length);


/*******************************************************************************
 * Implementations
 ******************************************************************************/


/**
 * Reads the oldest unread entry in the SIM rx buffer for the specified ID.
 * Once it has read all entries, will repeat sending the most recent
 * @param data The array you want to fill with the contents of a SIM packet. Minimum size PACKET_BUFFER_SIZE
 * @param id the SIM id of the buffer you want to read - e.g. radio or a specific sensor
 * @return the length of the packet.
 */
static void sendPackets(){

    //just thinking - should flag on any new data and should only unflag when the read head reaches the write head, (i.e all data read)

    for(int id=0; id<=MAX_PACKET_IDS;id++){
        //printf("TX buffer ID %d has value %d\n",id,packetBuffersNewFlag[TX][id]);
        if(packetBuffersNewFlag[TX][id]==FLAGGED){
            
            char buf[512];
            int length =readFromBuf(TX,buf,id);
            //putPacket(id, buf, length);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}


static uint8_t readFromBuf(int mode,uint8_t data[],uint8_t id){
    if(packetBuffersReadIndex[mode][id]>=PACKET_BUFFER_SIZE){
        packetBuffersReadIndex[mode][id]=0;
    }

    int readpoint = packetBuffersReadIndex[mode][id];

    for(int i=1;i<=packetBuffers[mode][id][readpoint][0];i++){
        data[i-1]=packetBuffers[mode][id][readpoint][i];
    }

    if(readpoint+1<packetBuffersReadLowerLimit[mode][id]+PACKET_BUFFER_SIZE){
        packetBuffersReadIndex[mode][id]++;
    } else {
        //packetBuffersNewFlag[mode][id]=UNFLAGGED;
    }
    
    return data[0];
}
/**
 * Internal function, used to write recieved packet to the approprite buffer
 */ 
static void writeToBuf(int mode,uint8_t data[],uint8_t id,uint16_t length){
    packetBuffersNewFlag[mode][id] = FLAGGED;
    printf("flag for ID %d changed to 1",id);

    if(packetBuffersWriteIndex[mode][id]>=PACKET_BUFFER_SIZE){
        packetBuffersWriteIndex[mode][id]=0;
    }

    int writepoint = packetBuffersWriteIndex[mode][id];

    packetBuffers[mode][id][writepoint][0]=length;

    for(int i=1;i<=length;i++){
        packetBuffers[mode][id][writepoint][i] = data[(i-1)];
    }

    packetBuffersWriteIndex[mode][id]++;
    packetBuffersReadLowerLimit[mode][id]++;
    if(packetBuffersReadLowerLimit[mode][id]>=PACKET_BUFFER_SIZE){
        packetBuffersReadLowerLimit[mode][id]=0;
    }
}

//sets up the sim rx buffer
static void initBuf(int mode){
    for(int i =0;i<MAX_PACKET_IDS;i++){
        packetBuffersReadLowerLimit[mode][i] = 0-PACKET_BUFFER_SIZE;
    }
    for(int i =0;i<MAX_PACKET_IDS;i++){
        packetBuffersReadLowerLimit[mode][i] = 0-PACKET_BUFFER_SIZE;
    }   
}

/*
 * Grabs a char from stdin. Does not grab empty chars. Blocking.
 */
static uint8_t getCinForce() {
    uint8_t c;
    for(;;) {
        if(scanf(" %c",&c)!=1){  
            continue;
        }

        return c;
    }
}

/*
 * recombines two sequential chars from stdin into a single char that carries
 * meaning from the groundstation
 * 
 * See the confluence docs section on reduced ASCII space
 */
static uint8_t getFilteredCin() {
    uint8_t msb = getCinForce() - 'A';
    uint8_t lsb = getCinForce() - 'A';

    return (msb << 4) | lsb;
}

/*
 * extracts a SIM packet from grounstation via stdin
 * TODO: Place it into the appropriate buffer
 */
static void extractPacket() {
        uint8_t id;
        uint16_t length;

        id = getFilteredCin();
        length = getFilteredCin();
        length <<= 8;
        length |= getFilteredCin();

        uint8_t buf[512];

        for (uint16_t i = 0; i <= length-1; i++) {
            buf[i]=getFilteredCin();
        }

        xSemaphoreTake(simInSemaphore,portMAX_DELAY);
        writeToBuf(RX,buf,id,length);
        xSemaphoreGive(simInSemaphore);


        //TODO: At some point in the future, it may be advisable to optimize this - seems a little clunky.
        //on the other hand it will never run on the board so maybe it's fine
        FILE *logfile = fopen("SIMrxlog.csv","a+");
        char str[MESSAGE_BUFFER_SIZE] = {0};
        int i =0;
        for(; i<=length;i++){
            str[i]=buf[i];
        }
        str[++i]='\0';
        fprintf(logfile,"%d,%d,%s\n",id,length,str);
        fclose(logfile);
    }

/*
 * outputs a single char to standard out and logs it in SIMlog.txt
 * 
 * @param c Char to output
 */
static void output(char c){
    printf("%c",c);
    FILE *logfile = fopen("SIMtxlog.txt","a+"); //This file manipulation code is a little suspect
    fprintf(logfile,"%c",c);
    fclose(logfile);
}

/*
 * outputs a char to stdio in the SIM format, splitting it up into two chars
 * due to the reduced ascii space.
 * 
 * @param c The char to send out to SIM
 */
static void synOut(char c){
    // Sends each byte in two bytes to reduce ascii range to [A, A + 16)
    // Effectively avoiding all special characters that may have varying 
    // behavior depending on the OS

    //this code taken straight from FLARE

    uint8_t b = c; // char might be unsigned

    char msb = (b >> 4) + 'A';
    char lsb = (b & 0x0F) + 'A';

    output(msb);
    output(lsb);
}

/*
 * Sends a SIM packet 
 * 
 * @param id packet ID, used in header
 * @param *c the packet message
 * @param length length of the packet message  
 */
static void putPacket(const uint8_t id, const char *c, char const length){
    synOut(id);

    synOut((char)(length>>8));
    synOut((char)(length & 0xFF));

    xSemaphoreTake(simOutSemaphore,portMAX_DELAY);

    for (char const *end = c + length; c != end; c++){
        synOut(*c);
    }

    fflush(stdout);

    xSemaphoreGive(simOutSemaphore);
}

/*
 * Sends the SIM config packet, consisting of an int and float.
 */
static void putConfigPacket() {
        uint8_t id = 0x01;
        uint32_t int_test = 0x04030201;
        float float_test = -2.0; // 0xC000 0000;

        char buf[8];
        memmove(buf, &int_test, 4);
        memmove(buf + 4, &float_test, 4);
        putPacket(id, buf, 8);
}

/*******************************************************************************
 * Loops for interacting with ground station interface
 ******************************************************************************/
/*
 * The input loops handles verification of the handshake from the groundstation 
 * and sends the config packet. It then continously loops though extractPacket(),
 * reading SIM packets from stdio.
 */
static void inputLoop(void *pv){
    /*handshake*/
    /*check for handshake acknowedgement*/
    char ack[4] = "ACK";
    char readChar;
    for (int i = 0; i < 3; i++) {
        readChar=getCinForce();
        assert(ack[i] == readChar);
    }
    putConfigPacket();
    
    for(EVER){
        //constantly check for new packets
        extractPacket();
        char data[0] = {1};
        vTaskDelay(pdMS_TO_TICKS(1000));
        //writeToBuf(TX,data,1,1);

    }
}

/*
 * Output loop handles sending of rocket side of SIM handshake
 * Then continoutly loops though sending SIM packets 
 */
static void outputLoop(void *pv){
    printf("SYN"); /*this has to be the first thing to go out, I think*/
    fflush(stdout);
    for(EVER){
        //check buffer
        //send what's in buffer
        sendPackets();



        

        
    }
    
}

static void generateImuLoop(void *pv){
    /* Useful for checking current working directory */
    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) 
    //     printf("Current working dir: %s\n", cwd);

    //printf("Starting IMU loop");

    /* Convenience variable to reduce repetition */
    const int ind = uart_index_imu;

    FILE *fp = fopen("../data/imu/imu_uart_data_3.txt", "r");

    while(1){
        vTaskDelay(pdMS_TO_TICKS(8));
        xSemaphoreTake(uart_handles[ind]->rxSemaphore, portMAX_DELAY);
        for(int i = 0; i < 40; ++i){
            if(uart_handles[ind]->buffer_size - uart_handles[ind]->cur_buffer_size <= 40)
                continue;
            if(!fscanf(fp, "%hhu ", &(uart_handles[ind]->buffer[uart_handles[ind]->cur_buffer_size]))){
                return;
            }
            uart_handles[0]->cur_buffer_size++;
        }
        xSemaphoreGive(uart_handles[ind]->rxSemaphore);

        xEventGroupSetBits(uart_handles[0]->rxEvent, HAL_UART_COMPLETE);
    }
}

void stdioInit(){
    /* Thread safe output */
    console_init();
    initBuf(RX);
    for(int id=0; id<=MAX_PACKET_IDS;id++){
        printf("TX buffer ID %d has value %d\n",id,packetBuffersNewFlag[TX][id]);
    }
    /* For generation of normal threads if needed for testing */
    // pthread_t ioThread;
    // pthread_create( &ioThread, NULL, inputLoop, NULL);


    //set up logging
    FILE *logfile = fopen("SIMrxlog.csv","w");
    fprintf(logfile,"id,length,message\n");
    fclose(logfile);

    //todo: verify I am using these correctly. 
    simInSemaphore = xSemaphoreCreateMutex();
    assert(simInSemaphore != NULL); //basic error checking

    simOutSemaphore = xSemaphoreCreateMutex();
    assert(simOutSemaphore !=NULL);

    if (xTaskCreate( 
	  		outputLoop, 
	  		"stdio out controller",
	  		50000/sizeof(StackType_t),
	  		(void*)NULL,
	  		tskIDLE_PRIORITY+2,
	  		(TaskHandle_t*)NULL
	  		) != pdPASS) {
      	for(;;);
      }
     if (xTaskCreate( 
	 		inputLoop, 
	 	"stdio in controller",
		1000/sizeof(StackType_t),
	 	(void*)NULL,
	 	tskIDLE_PRIORITY+2,
	 	(TaskHandle_t*)NULL
	 	) != pdPASS) {
     	for(;;);
     }
}

void stdioAssignUart(hal_uart_handle_t *handle){
    // if((void*)handle->base == (void*)IMU_UART){
	// 	uart_handles[0] = handle;
    //     if (xTaskCreate( 
	// 		generateImuLoop, 
	// 		"imu generator controller",
	// 		200/sizeof(StackType_t),
	// 		(void*)NULL,
	// 		tskIDLE_PRIORITY+2,
	// 		(TaskHandle_t*)NULL
	// 		) != pdPASS) {
    // 	for(;;);
    // }
	// }
}