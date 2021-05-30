#include "radio_protocol.h"
#include "board.h"

#define CONFIG_PACKET_LENGTH 43
#define STATE_CHANGE_WATERSHED 0x40


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
    // radioBufTX = cbufInit(2048); //this would fit 8 full packets (but they might not be full!)
    // radioBufRXCrit = cbufInit(256);
    // radioBufRXLow = cbufInit(256);

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


void radioPrepPing(void){
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




static void radioPrepGPS(void);

static void radioPrepState(void);

static void radioPrepOrientation(void);

static void radioPrepAccel(void);

//HP Tank PT
static void radioPrepPT_HP_T_001(void); 

//HP Tank Out Valve TC
static void radioPrepTC_HP_OUT_001(void); 

//HP Press Valve
static void radioPrepV_HP_P_001(void); 

//Fuel Press Valve
static void radioPrepV_F_PR_001(void); 

//Fuel Vent Valve
static void radioPrepV_F_V_001(void); 

//Fuel Tank PT
static void radioPrepPT_F_T_001(void); 

//Fuel Fill Valve
static void radioPrepV_F_F_001(void); 

//Fuel Injector PT
static void radioPrepPT_F_INJ_001(void); 

//Main Fuel Valve
static void radioPrepV_F_MFV_001(void); 

//LOX Press Valve
static void radioPrepV_L_PR_001(void);

//LOX Tank PT
static void radioPrepPT_L_T_001(void);

//LOX Vent Valve
static void radioPrepV_L_V_001(void);

//LOX Fill TC
static void radioPrepTC_L_F_001(void);

//LOX Fill Valve
static void radioPrepV_L_F_001(void);

//LOX Injector PT
static void radioPrepPT_L_INJ_001(void);

//Main Oxidizer Valve
static void radioPrepV_L_MOV_001(void);

//LOX Bleed Valve
static void radioPrepV_L_BLD_001(void);

//LOX Bleed TC
static void radioPrepTC_L_BLD_001(void);

//Data Dump
static void radioPrepDataDump(void);


void dealWithLowMessages(void){
    uint8_t message = 0;
    for(;;){
        if(!cbufGet(radioBufRXLow,&message)){
            switch(message){
                case 0x04:
                    radioPrepGps();
                    break;
                case 0x05:
                    radioPrepState();
                    break;
                case 0x06:
                    radioPrepOrientation();
                    break;
                case 0x07:
                    radioPrepAcc();
                    break;
                case 0x1E:
                    radioPrepPT_HP_T_001();
                    break;
                case 0x1F:
                    radioPrepTC_HP_OUT_001();
                    break;
                case 0x20:
                    radioPrepV_HP_P_001();
                    break;
                case 0x21:
                    radioPrepV_F_PR_001();
                    break;
                case 0x22:
                    radioPrepPT_F_T_001();
                    break;
                case 0x23:
                    radioPrepPT_F_INJ_001();
                    break;
                case 0x24:
                    radioPrepV_F_F_001();
                    break;
                case 0x25:
                    radioPrepPT_F_INJ_001();
                    break;
                case 0x26:
                    radioPrepV_F_MFV_001();
                    break;
                case 0x27:
                    radioPrepV_L_PR_001();
                    break;
                case 0x28:
                    radioPrepPT_L_T_001();
                    break;
                case 0x29:
                    radioPrepV_L_V_001();
                    break;
                case 0x2A:
                    radioPrepTC_L_F_001();
                    break;
                case 0x2B:
                    radioPrepV_L_F_001();
                    break;
                case 0x2C:
                    radioPrepPT_L_INJ_001();
                    break;
                case 0x2D:
                    radioPrepV_L_MOV_001();
                    break;
                case 0x2E:
                    radioPrepV_L_BLD_001();
                    break;
                case 0x2F:
                    radioPrepTC_L_BLD_001();
                    break;
                case 0x30:
                    radioPrepDataDump();
                    break;
            }
        }

    }
}