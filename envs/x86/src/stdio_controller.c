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

enum sensors
{
    uart_index_imu = 0,
    uart_index_debug = 1,
};

// #define PACKET_BUFFER_SIZE 8
// #define MESSAGE_BUFFER_SIZE 512
// #define MAX_PACKET_IDS 0x74
// #define RX 0
// #define TX 1

// #define FLAGGED 1
// #define UNFLAGGED 0

/*******************************************************************************
 * Variables
 ******************************************************************************/

//buffers. Are sizes reasonable?
uint8_t packetBuffers[2][SIM_MAX_PACKET_IDS][SIM_PACKET_BUFFER_SIZE][SIM_MESSAGE_BUFFER_SIZE] = {0};
int packetBuffersWriteIndex[2][SIM_MAX_PACKET_IDS] = {0};
int packetBuffersReadIndex[2][SIM_MAX_PACKET_IDS] = {0};
int packetBuffersReadLowerLimit[2][SIM_MAX_PACKET_IDS] = {0}; //needs to be initialized in the init function
//the upper packet limit is the lower limit + the packet buffer size. It's the
//range of allowable readable values
SemaphoreHandle_t packetBuffersNewFlag[2][SIM_MAX_PACKET_IDS];
int handshakeRecieved = 0; //blocks sending until 1

SemaphoreHandle_t simInSemaphore;
SemaphoreHandle_t simOutSemaphore;

/* TODO: update for all sensors */
// static hal_uart_handle_t *uart_handles[1]; // TODO:

/*******************************************************************************
 * Declarations
 ******************************************************************************/
static void inputLoop(void *pv);
static void outputLoop(void *pv);
// static void generateImuLoop(void *pv); // TODO:
static void output(char c);
static void synOut(char c);
static void putPacket(const uint8_t id, const uint8_t *c, char const length);
static void putConfigPacket();
static uint8_t getCinForce();
static uint8_t getFilteredCin();
static void extractPacket();
static void sendPackets();
//static uint8_t readFromBuf(int mode, uint8_t data[], uint8_t id);//
//static void writeToBuf(int mode, uint8_t data[], uint8_t id, uint16_t length);//
static void initBuf(int mode);

/*******************************************************************************
 * Loops for interacting with ground station interface & The task intialization
 ******************************************************************************/
/*
 * The input loops handles verification of the handshake from the groundstation 
 * and sends the config packet. It then continously loops though extractPacket(),
 * reading SIM packets from stdio.
 */
static void inputLoop(void *pv)
{
    /*check for handshake acknowedgement*/
    char ack[4] = "ACK";
    char readChar;
    for (int i = 0; i < 3; i++)
    {
        readChar = getCinForce();
        assert(ack[i] == readChar);
    }
    putConfigPacket();
    handshakeRecieved = 1;

    for (;;)
    {
        //constantly check for new packets
        extractPacket();
    }
}

/*
 * Output loop handles sending of rocket side of SIM handshake
 * Then continoutly loops though sending SIM packets 
 */
static void outputLoop(void *pv)
{
    printf("SYN"); /*this has to be the first thing to go out, I think*/
    fflush(stdout);
    while (!handshakeRecieved)
    {
    }
    for (;;)
    {
        //check buffer
        //send what's in buffer
        sendPackets();
    }
}

void stdioInit()
{
    /* Thread safe output */
    console_init();
    initBuf(RX);
    initBuf(TX);

    /* For generation of normal threads if needed for testing */
    // pthread_t ioThread;
    // pthread_create( &ioThread, NULL, inputLoop, NULL);

    //set up logging
    FILE *logfile = fopen("SIMrxlog.csv", "w");
    fprintf(logfile, "id,length,message\n");
    fclose(logfile);

    for(int i = 0; i < 2; ++i){
        for(int j = 0; j < SIM_MAX_PACKET_IDS; ++j){
            packetBuffersNewFlag[i][j] = xSemaphoreCreateBinary();
        }
    }

    //todo: verify I am using these correctly.
    simInSemaphore = xSemaphoreCreateMutex();
    assert(simInSemaphore != NULL); //basic error checking

    simOutSemaphore = xSemaphoreCreateMutex();
    assert(simOutSemaphore != NULL);

    if (xTaskCreate(
            outputLoop,
            "stdio out controller",
            50000 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        for (;;)
            ;
    }
    if (xTaskCreate(
            inputLoop,
            "stdio in controller",
            1000 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        for (;;)
            ;
    }
}

/*******************************************************************************
 * Implementations
 ******************************************************************************/


/*======================
* Buffer Implementations
* ======================*/

/**
 * Reads a packet from either the sim TX or RX buffer. 
 * used by sendpackets() to send packets out or to recieve data from the sim RX buffer
 * @param mode specify TX or RX buffer
 * @param data where you will store the data you are recieveing. Will never be more than
 * MESSAGE_BUFFER_SIZE
 * @param id sim ID of what you want to read, in range 0-MAX_PACKET_IDS
 * @return the length of packet, i.e. the length of what's just been placed in data
 */
uint8_t readFromBuf(int mode, uint8_t data[], uint8_t id)
{
    int readpoint;
    if(xSemaphoreTake(packetBuffersNewFlag[mode][id], portMAX_DELAY)){
        if (packetBuffersReadIndex[mode][id] >= SIM_PACKET_BUFFER_SIZE)
        {
            packetBuffersReadIndex[mode][id] = 0;
        }

        readpoint = packetBuffersReadIndex[mode][id];

        for (int i = 1; i <= packetBuffers[mode][id][readpoint][0]; i++)
        {
            data[i - 1] = packetBuffers[mode][id][readpoint][i];
        }

        if (readpoint + 1 < packetBuffersReadLowerLimit[mode][id] + SIM_PACKET_BUFFER_SIZE)
        {
            packetBuffersReadIndex[mode][id]++;
            // xSemaphoreGive(packetBuffersNewFlag[mode][id]);
        }
    }
    return packetBuffers[mode][id][readpoint][0];
}

/**
 * Writes a packet to the sim buffer, used internally by extractPackets() or to 
 * add a packet to be sent out
 * 
 * @param mode specify TX ot RX
 * @param data the data you want to write to the buffer, i.e. the packet body
 * @param id sim ID of packet, in range 0-MAX_PACKET_IDS
 * @param length length of the packet, no more than MESSAGE_BUFFER_SIZE
 */

void writeToBuf(int mode, const uint8_t data[], uint8_t id, uint16_t length)
{

    if (packetBuffersWriteIndex[mode][id] >= SIM_PACKET_BUFFER_SIZE)
    {
        packetBuffersWriteIndex[mode][id] = 0;
    }

    int writepoint = packetBuffersWriteIndex[mode][id];

    packetBuffers[mode][id][writepoint][0] = length;

    for (int i = 1; i <= length; i++)
    {
        packetBuffers[mode][id][writepoint][i] = data[(i - 1)];
    }

    packetBuffersWriteIndex[mode][id]++;
    packetBuffersReadLowerLimit[mode][id]++;
    if (packetBuffersReadLowerLimit[mode][id] >= SIM_PACKET_BUFFER_SIZE)
    {
        packetBuffersReadLowerLimit[mode][id] = 0;
    }
    xSemaphoreGive(packetBuffersNewFlag[mode][id]);
}

//sets up the sim rx buffer
static void initBuf(int mode)
{
    for (int i = 0; i < SIM_MAX_PACKET_IDS; i++)
    {
        packetBuffersReadLowerLimit[mode][i] = 0 - SIM_PACKET_BUFFER_SIZE;
    }
}

/*=====================
* Other Implementations
* =====================*/

/*
 * extracts a SIM packet from grounstation via stdin
 */
static void extractPacket()
{
    uint8_t id;
    uint16_t length;

    id = getFilteredCin();

    // printf("id:%d ", id);

    length = getFilteredCin();
    length <<= 8;
    length |= getFilteredCin();

    // printf("len:%d ", length);

    uint8_t buf[512];

    for (uint16_t i = 0; i <= length - 1; i++)
    {
        buf[i] = getFilteredCin();
        // printf("%u ", buf[i]);
    }


    xSemaphoreTake(simInSemaphore, portMAX_DELAY);
    writeToBuf(RX, buf, id, length);
    xSemaphoreGive(simInSemaphore);

    //TODO: At some point in the future, it may be advisable to optimize this - seems a little clunky.
    //on the other hand it will never run on the board so maybe it's fine
    FILE *logfile = fopen("SIMrxlog.csv", "a+");
    char str[SIM_MESSAGE_BUFFER_SIZE] = {0};
    int i = 0;
    for (; i <= length; i++)
    {
        str[i] = buf[i];
    }
    str[++i] = '\0';
    fprintf(logfile, "%d,%d,%s\n", id, length, str);
    fclose(logfile);

}
/**
 * Reads the oldest unread entry in the SIM TX buffer for the specified ID.
 * Sends new packets once, triggered based on the 'packetBuffersNewFlag' flag
 * @param data The array you want to fill with the contents of a SIM packet. Minimum size PACKET_BUFFER_SIZE
 * @param id the SIM id of the buffer you want to read - e.g. radio or a specific sensor

 */
static void sendPackets()
{

    for (int id = 0; id < SIM_MAX_PACKET_IDS; id++)
    {
        if (uxSemaphoreGetCount(packetBuffersNewFlag[TX][id]))
        {
            uint8_t buf[512];
            char length = readFromBuf(TX, buf, id);
            putPacket(id, buf, length);
        }
    }
}

/*
 * Grabs a char from stdin. Does not grab empty chars. Blocking.
 */
static uint8_t getCinForce()
{
    uint8_t c;
    for (;;)
    {
        if (scanf(" %c", &c) != 1)
        {
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
static uint8_t getFilteredCin()
{
    uint8_t msb = getCinForce() - 'A';
    uint8_t lsb = getCinForce() - 'A';

    return (msb << 4) | lsb;
}




/*
 * outputs a single char to standard out and logs it in SIMlog.txt
 */
static void output(char c)
{
    printf("%c", c);
    FILE *logfile = fopen("SIMtxlog.txt", "a+"); //This file manipulation code is a little suspect
    fprintf(logfile, "%c", c);
    fclose(logfile);
}

/*
 * outputs a char to stdio in the SIM format, splitting it up into two chars
 * due to the reduced ascii space.
 */
static void synOut(char c)
{
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
 */
static void putPacket(const uint8_t id, const uint8_t *c, char const length)
{
    synOut(id);

    synOut((char)(length >> 8));
    synOut((char)(length & 0xFF));

    xSemaphoreTake(simOutSemaphore, portMAX_DELAY);

    for (uint8_t const *end = c + length; c != end; c++)
    {
        synOut(*c);
    }

    fflush(stdout);

    xSemaphoreGive(simOutSemaphore);
}

/*
 * Sends the SIM config packet, consisting of an int and float.
 */
static void putConfigPacket()
{
    uint8_t id = 0x01;
    uint32_t int_test = 0x04030201;
    float float_test = -2.0; // 0xC000 0000;

    uint8_t buf[8];
    memmove(buf, &int_test, 4);
    memmove(buf + 4, &float_test, 4);
    putPacket(id, buf, 8);
}

