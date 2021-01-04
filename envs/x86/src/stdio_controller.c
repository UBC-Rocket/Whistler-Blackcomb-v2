
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

//are these amounts reasonable?
uint8_t packetBuffers[0x74][PACKET_BUFFER_SIZE][512] = {0};
//IDs, Packets, Packet Contents..?
int packetBuffersWriteIndex[0x74] = {0};

int packetBuffersReadIndex[0x74] = {0}; 

/*******************************************************************************
 * Variables
 ******************************************************************************/

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

static uint8_t readFromBuf(uint8_t data[],uint8_t id);
static void writeToBuf(uint8_t data[],uint8_t id,uint16_t length);


/*******************************************************************************
 * Implementations
 ******************************************************************************/


/* Craptastic FIFO buffer Accidentaly Circular*/
static uint8_t readFromBuf(uint8_t data[],uint8_t id){
    if(packetBuffersReadIndex[id]>=PACKET_BUFFER_SIZE){
        packetBuffersReadIndex[id]=0;
    }
    int readpoint = packetBuffersReadIndex[id];



    data[0]=packetBuffers[id][readpoint][0];
    for(int i=1;i<=packetBuffers[id][readpoint][0];i++){
        data[i]=packetBuffers[id][readpoint][i];
    }

    packetBuffersReadIndex[id]++;

    //return the length (may be useful?)
    return data[0];
}

static void writeToBuf(uint8_t data[],uint8_t id,uint16_t length){
    
    if(packetBuffersWriteIndex[id]>=PACKET_BUFFER_SIZE){
        packetBuffersWriteIndex[id]=0;
    }

    int writepoint = packetBuffersWriteIndex[id];

    packetBuffers[id][writepoint][0]=length;

    for(int i=1;i<=length;i++){
        packetBuffers[id][writepoint][i] = data[i];
    }
    packetBuffersWriteIndex[id]++;
    
}






/*
 * Grabs a char from stdin. Performs some sort of error checking (??)
 * To Do: Understand what the commented-out section does (It's C++ from FLARE)
 */
static uint8_t getCinForce() {
    uint8_t c;
    for(;;) {
        scanf("%c",&c);
        //if (std::cin.fail()) {        //I don't really understand at all what this does, need to ask.
        //    std::cin.clear();
        //    continue;
        //}
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

        //auto buf = std::vector<uint8_t>();
        uint8_t buf[512];

        //buf.reserve(length);


        for (int i = 0; i < length; i++) {
            //buf.push_back(getFilteredCin());
            buf[i]=getFilteredCin();
        }


        //I really do need to figure out this mutex stuff. 
        //and WTF is istream

        { // scope for lock-guard
        //    const std::lock_guard<std::mutex> lock(istream_mutex_);
        writeToBuf(buf,id,length);
            } // unlock mutex
        //}
    }


/*
 * outputs a single char to standard out and logs it in SIMlog.txt
 * 
 * @param c Char to output
 */
static void output(char c){
    printf("%c",c);
    FILE *logfile = fopen("SIMlog.txt","a+"); //This file manipulation code is a little suspect
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
static void putPacket(const u_int8_t id, const char *c, char const length){
    //TO DO: mutex stuff

    synOut(id);

    synOut((char)(length>>8));
    synOut((char)(length & 0xFF));

    for (char const *end = c + length; c != end; c++){
        synOut(*c);
    }

    fflush(stdout);

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
        scanf("%c",&readChar);
        assert(ack[i] == readChar);
    }
    putConfigPacket();
    
    for(;;){

        //constantly check for new packets
        extractPacket();
        
        //console_print("stdio polling...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/*
 * Output loop handles sending of rocket side of SIM handshake
 * Then continoutly loops though sending SIM packets 
 */
static void outputLoop(void *pv){
    printf("SYN"); /*this has to be the first thing to go out, I think*/
    fflush(stdout);
    for(;;){
        //check buffer
        //send what's in buffer
        
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

    /* For generation of normal threads if needed for testing */
    // pthread_t ioThread;
    // pthread_create( &ioThread, NULL, inputLoop, NULL);

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
    if((void*)handle->base == (void*)IMU_UART){
		uart_handles[0] = handle;
        if (xTaskCreate( 
			generateImuLoop, 
			"imu generator controller",
			200/sizeof(StackType_t),
			(void*)NULL,
			tskIDLE_PRIORITY+2,
			(TaskHandle_t*)NULL
			) != pdPASS) {
    	for(;;);
    }
	}
}