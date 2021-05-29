#include "radio_protocol.h"
#include "board.h"

#define CONFIG_PACKET_LENGTH 43

//static SemaphoreHandle_t sRadioBufRXCritSemaphore = NULL;


//loops

static void inputLoop(void *pv)
{
    //get a packet from radio (blocking)
    //put it into the buffer
    //this separates the 'big' packet from the radio into it's constituent packets
    uint8_t length;
    uint8_t radioPacket[256];
    uint8_t messageReadHead;

    uint8_t message;


    for (;;)
    {
        length = radioReceive(&radio, radioPacket);

        /*there are essentially two types of messages that can be recieved:
             - HMI-triggered state changes
             - Data requests
            We evaluate them separately, to make sure the HMI input ones are as
            fast as possible
        */

        messageReadHead = 0;
        for (; messageReadHead < length; messageReadHead++)
        {
            message = radioPacket[messageReadHead];
            if (message > STATE_CHANGE_WATERSHED)
            {
                cbufPut(radioBufRXCrit, 1, &message);
            }
            else
            {
                cbufPut(radioBufRXLow, 1, &message);
            }
        }
    }
}

static void outputLoop(void *pv)
{
    //this loop should send all messages in the send buffer

    uint8_t *message=pvPortMalloc(256*sizeof(uint8_t));
    uint8_t length=0;
    uint8_t *individMessage=pvPortMalloc(256*sizeof(uint8_t));
    uint8_t individLength;
    for(;;){
        if(!cbufCheckEmpty(radioBufTX)){
            individLength=cbufGet(radioBufTX,individMessage);
            if(length+individLength<256){
                for(int index=0;index++;index<individLength){
                    message[length+index]=individMessage[index];
                }
                length=length+individLength;
            }
            else{
                radioTxRequest(&radio,message,length);
                length=0;
                for(int index=0;index++;index<individLength){
                    message[index]=individMessage[index];
                }
                length=individLength;
            }
        }
    }

}



//init function

void GSRadioInit(void)
{
    radioBufTX = cbufInit(2048); //this would fit 8 full packets (but they might not be full!)
    radioBufRXCrit = cbufInit(256);
    radioBufRXLow = cbufInit(256);

    //init the two tasks

    serial.baudrate = 9600;

    uartConfig(&(serial.uart_handle), RADIO_UART, 9600);

    xbee_dev_init(&radio, &serial, NULL, NULL);

    /* Add this for x86 testing */
    // memcpy(&radio.serport.uart_handle.buffer, radioPacket, sizeof(radioPacket));
    // radio.serport.uart_handle.cur_buffer_size = sizeof(radioPacket);

    if (xTaskCreate(
            outputLoop,
            "radio out controller",
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
            "radio in controller",
            1000 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        for (;;)
            ;
    }
}



//command response helper functions

void radioSendPing(void){
    uint8_t *message=pvPortMalloc(sizeof(uint8_t)*8);//TODO: adjust length
    uint8_t ID=0x00;
    //need to flesh out specs


}

void radioPrepConfig(void){
    uint8_t *message=pvPortMalloc(sizeof(uint8_t)*CONFIG_PACKET_LENGTH);//TODO: adjust length
    uint8_t ID=0x03;
    uint8_t SIMstatus=0; //need to figure out how to get this
    uint8_t deviceID=5;
    uint8_t verString[40]={0};//in future will be better to remove this step?

    message[0]=ID;
    message[1]=SIMstatus;
    message[2]=deviceID;
    for(int index=0;index<40;index++){
        message[index]=verString[index+3];
    }

    cbufPut(radioBufTX,CONFIG_PACKET_LENGTH,message);
}

//declarations for all the single component radio return preps

static void radioPrepNothing(void);

static void radioPrepAcc(void);

//implementations
static void radioPrepNothing(void){
    return;
}

static void radioPrepAccX(void){

}