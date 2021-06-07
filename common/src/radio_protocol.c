#include "radio_protocol.h"


#define CONFIG_PACKET_LENGTH 43
#define GPS_PACKET_LENGTH 13
#define STATE_PACKET_LENGTH 3
#define ORIENT_PACKET_LENGTH 17
#define ACCEL_PACKET_LENGTH 13
#define SINGLE_FLOAT_PACKET_LENGTH 5
#define SINGLE_INT_PACKET_LENGTH 2

#define MAX_RADIO_WAIT_MS 120000

#define STATE_CHANGE_WATERSHED 0x40
#define MAX_SENSOR_ID 0x2F

#ifdef COMPILE_BOARD
#define SIM_ACTIVE 0
#elif defined(COMPILE_x86)
#define SIM_ACTIVE 1
#endif

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

    //TODO: Make this work if the messages come slowly, i.e. don't force wait until buffer full
    TickType_t maxRadioWaitTicks = pdMS_TO_TICKS(MAX_RADIO_WAIT_MS);
    TickType_t ticksAtLastSend = xTaskGetTickCount();
    //we shouldn't have overflow problems here
    //will overflow after about 50 days.

    uint8_t *message = pvPortMalloc(256 * sizeof(uint8_t));
    uint8_t length = 0;
    uint8_t *individMessage = pvPortMalloc(256 * sizeof(uint8_t));
    uint8_t individLength;
    for (;;)
    {
        if (!cbufCheckEmpty(radioBufTX))
        {
            individLength = cbufGet(radioBufTX, individMessage);
            if (length + individLength < 256)
            {
                for (int index = 0; index++; index < individLength)
                {
                    message[length + index] = individMessage[index];
                }
                length = length + individLength;
            }
            else
            {
                radioTxRequest(&radio, message, length);
                length = 0;
                for (int index = 0; index++; index < individLength)
                {
                    message[index] = individMessage[index];
                }
                length = individLength;
            }
        }

        if ((xTaskGetTickCount() - ticksAtLastSend) >= maxRadioWaitTicks)
        {
            radioTxRequest(&radio, message, length);
            length = 0;
            ticksAtLastSend = xTaskGetTickCount();
            /* This feels like it isn't the most efficient way to do this but
            at least it keeps it all in one task
            
            Need to do something to prevent empty messages? */
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

uint32_t getTimestamp(){
    return (uint32_t)(timeSinceStartup()/1000);
}

void modBitFieldSensorStatus(uint8_t sensorID,uint8_t offset,
uint8_t *generalFailMode, uint32_t *bitfield){
    *bitfield |= 0; //dereference the pointers (?)
    *generalFailMode |= 0;
    uint8_t status=getSensorStatus[sensorID];
    if(status>0){
        
        *bitfield = (*bitfield|(0x01<<offset));
        if(status>1){
            *generalFailMode = 0x03;
        }
        else{
            *generalFailMode = 0x01;
        }
    }
}

void radioPrepPing(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * 8); //TODO: adjust length
    uint8_t ID = 0x00;
    //timestamp
    
    uint8_t generalFailMode=0;
    //if status nominal
        message[1] = 0x00;
    //if non-critical failure
        message[1] = 0x01; 
    //if critical failure
        message[1]=0x03;

    uint32_t senStatusBitFieldA = 0x00;
    uint32_t senStatusBitFieldB = 0x00;
    

    //this is very explicit and not very elegant but I don't like assuming that 
    //you can always enumerate through the sensors.

    modBitFieldSensorStatus(0x04,31,&generalFailMode,&senStatusBitFieldA);
    //if sensor status 1 bad

    senStatusBitField = (senStatusBitField|(0x01<<31));

    //etc for status n


    //need to flesh out specs
}

void radioPrepConfig(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * CONFIG_PACKET_LENGTH); //TODO: adjust length
    uint8_t ID = 0x03;
    uint8_t SIMstatus = SIM_ACTIVE;
    uint8_t deviceID = 5; //magig number bad
    uint8_t verString[40] = {0}; //in future will be better to remove this step?

    message[0] = ID;
    message[1] = SIMstatus;
    message[2] = deviceID;
    for (int index = 0; index < 40; index++)
    {
        message[index] = verString[index + 3];
    }

    cbufPut(radioBufTX, CONFIG_PACKET_LENGTH, message);

    vPortFree(message);
}

static void radioPrepGPS(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * GPS_PACKET_LENGTH); //TODO: adjust length
    uint8_t ID = 0x04;
    float curGPS[3];
    getGPS(curGPS);
    message[0] = ID;
    memcpy(&message[1], &curGPS[0], 4);
    memcpy(&message[5], &curGPS[1], 4);
    memcpy(&message[9], &curGPS[2], 4);
    cbufPut(radioBufTX, GPS_PACKET_LENGTH, message);

    vPortFree(message);
}

static void radioPrepState(void){
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * STATE_PACKET_LENGTH);
    uint8_t ID = 0x05;
    uint16_t curState = getState();

    message[0] = ID;
    memcpy(&message[1], &curState, 2);
    cbufPut(radioBufTX, STATE_PACKET_LENGTH, message);

    vPortFree(message);

}

static void radioPrepOrientation(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * ORIENT_PACKET_LENGTH);
    uint8_t ID = 0x06;
    float curOrient[4];
    getOrientation(curOrient);

    message[0] = ID;
    memcpy(&message[1], &curOrient[0], 16);
    cbufPut(radioBufTX, ORIENT_PACKET_LENGTH, message);
    vPortFree(message);
}

static void radioPrepAccel(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * ACCEL_PACKET_LENGTH);
    uint8_t ID = 0x07;
    float curAccel[3];
    getAccel(curAccel);

    message[0] = ID;
    memcpy(&message[1], &curAccel[0], 16);
    cbufPut(radioBufTX, ACCEL_PACKET_LENGTH, message);
    vPortFree(message);
}

//HP Tank PT
static void radioPrepPT_HP_T_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x1E;
    float value = getPT_HP_T_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//HP Tank Out Valve TC
static void radioPrepTC_HP_OUT_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x1F;
    float value = getTC_HP_OUT_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//HP Press Valve
static void radioPrepV_HP_P_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x20;
    uint8_t value = getV_HP_P_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Fuel Press Valve
static void radioPrepV_F_PR_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x21;
    uint8_t value = getV_F_PR_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Fuel Vent Valve
static void radioPrepV_F_V_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x22;
    uint8_t value = getV_F_V_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Fuel Tank PT
static void radioPrepPT_F_T_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x23;
    float value = getPT_F_T_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Fuel Fill Valve
static void radioPrepV_F_F_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x24;
    uint8_t value = getV_F_F_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Fuel Injector PT
static void radioPrepPT_F_INJ_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x25;
    float value = getPT_F_INJ_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Main Fuel Valve
static void radioPrepV_F_MFV_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x26;
    uint8_t value = getV_F_MFV_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Press Valve
static void radioPrepV_L_PR_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x27;
    uint8_t value = getV_L_PR_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Tank PT
static void radioPrepPT_L_T_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x28;
    float value = getPT_L_T_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Vent Valve
static void radioPrepV_L_V_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x29;
    uint8_t value = getV_L_V_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Fill TC
static void radioPrepTC_L_F_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x2A; // this is the gun rights packet.
    float value = getTC_L_F_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Fill Valve
static void radioPrepV_L_F_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x2B;
    uint8_t value = getV_L_F_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Injector PT
static void radioPrepPT_L_INJ_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x2C;
    float value = getPT_L_INJ_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Main Oxidizer Valve
static void radioPrepV_L_MOV_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x2D;
    uint8_t value = getV_L_MOV_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Bleed Valve
static void radioPrepV_L_BLD_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint8_t ID = 0x2E;
    uint8_t value = getV_L_BLD_001();

    message[0] = ID;
    message[1] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//LOX Bleed TC
static void radioPrepTC_L_BLD_001(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint8_t ID = 0x2F;
    float value = getTC_L_BLD_001();

    message[0] = ID;
    memcpy(&message[1], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Data Dump
static void radioPrepDataDump(void)
{

}

void dealWithLowMessages(void *pv)
{
    uint8_t message = 0;
    for (;;)
    {
        if (!cbufGet(radioBufRXLow, &message))
        {
            switch (message)
            {
            case 0x04:
                radioPrepGPS();
                break;
            case 0x05:
                radioPrepState();
                break;
            case 0x06:
                radioPrepOrientation();
                break;
            case 0x07:
                radioPrepAccel();
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