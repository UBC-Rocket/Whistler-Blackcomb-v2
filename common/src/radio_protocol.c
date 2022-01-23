#include "radio_protocol.h"
#include <stdio.h>

//these lengths inlcude the 1 byte ID and the 4 byte timestamp
#define PING_PACKET_LENGTH 10
#define MESSAGE_MIN_PACKET_LENGTH 6
#define EVENT_PACKET_LENGTH 7
#define CONFIG_PACKET_LENGTH 47
#define GPS_PACKET_LENGTH 17
#define STATE_PACKET_LENGTH 7
#define ORIENT_PACKET_LENGTH 21
#define ACCEL_PACKET_LENGTH 17
#define SINGLE_FLOAT_PACKET_LENGTH 9
#define SINGLE_INT_PACKET_LENGTH 6
#define DATA_DUMP_PACKET_LENGTH 86
#define WB_DEVICE_ID 5

#define MAX_RADIO_WAIT_MS 10000

#define MAX_PACKET_SIZE 243

#define CRITICAL_WATERSHED 0x40
#define MAX_SENSOR_ID 0x2F

//I am not sure this is the best way to do this, but it seems workable
#ifdef COMPILE_BOARD
#define SIM_ACTIVE 0
#elif defined(COMPILE_x86)
#define SIM_ACTIVE 1
#endif

xbee_dev_t radio; //These might have more descriptive names in the future.

xbee_serial_t serial;

//Declarations
static void inputLoop(void *pv);
static void outputLoop(void *pv);
uint32_t getTimestamp();
void modBitFieldSensorStatus(uint8_t sensorID, uint8_t offset, uint8_t *generalFailMode, uint32_t *bitfield);

void dealWithLowMessages(void *pv);
void dealWithCritMessages(void *pv);

cbufHandle_t radioBufTX;
cbufHandle_t radioBufRXCrit;
cbufHandle_t radioBufRXLow;

/***** I/O management Loops *****/

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
            if (message > CRITICAL_WATERSHED)
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
    uint8_t *message = pvPortMalloc(MAX_PACKET_SIZE * sizeof(uint8_t));
    uint8_t length = 0;
    uint8_t *individMessage = pvPortMalloc(MAX_PACKET_SIZE * sizeof(uint8_t));
    uint8_t individLength;
    for (;;)
    {
        if (!cbufCheckEmpty(radioBufTX))
        {
            individLength = cbufGet(radioBufTX, individMessage);
            if (length + individLength < MAX_PACKET_SIZE)
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
    }
}

static void mandatoryOutput(void *pv)
{
    TickType_t maxRadioWaitTicks = pdMS_TO_TICKS(MAX_RADIO_WAIT_MS);

    for (;;)
    {
        vTaskDelay(maxRadioWaitTicks);

        if (cbufCheckEmpty(radioBufTX))
        {
            uint8_t *message = pvPortMalloc(MAX_PACKET_SIZE * sizeof(uint8_t));
            uint8_t length;

            radioPrepPing();

            length = cbufGet(radioBufTX, message);

            radioTxRequest(&radio, message, length);
        }
    }
}

//init function

void GSRadioInit(void)
{
    //printf("GS radio init started");
    serial.baudrate = 9600;
    radioBufTX=cbufInit(512);
    radioBufRXCrit=cbufInit(512);
    radioBufRXLow=cbufInit(512);

    initVariables();

    uartConfig(&(serial.uart_handle), RADIO_UART, 9600);

    xbee_dev_init(&radio, &serial, NULL, NULL);


    /* Add this for x86 testing */
    //memcpy(&radio.serport.uart_handle.buffer, radioPacket, sizeof(radioPacket));
    // radio.serport.uart_handle.cur_buffer_size = sizeof(radioPacket);


    if (xTaskCreate(
            outputLoop,
            "radio out controller",
            2500 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        printf("Radio out failed initialization");
        for (;;)
            ;
    }


    if (xTaskCreate(
            mandatoryOutput,
            "radio mandatory out controller",
            2500 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        printf("Radio mandatory out failed initialization");
        for (;;)
            ;
    }
    if (xTaskCreate(
            inputLoop,
            "radio in controller",
            2500 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        printf("Radio in failed initialization");
        for (;;)
            ;
    }
    if (xTaskCreate(
            dealWithLowMessages,
            "deal with low messages",
            2500 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        printf("Low message failed initialization");
        for (;;)
            ;
    }
    if (xTaskCreate(
            dealWithCritMessages,
            "deal with crit messages",
            1000 / sizeof(StackType_t),
            (void *)NULL,
            tskIDLE_PRIORITY + 2,
            (TaskHandle_t *)NULL) != pdPASS)
    {
        printf("Crit messages failed initialization");
        for (;;)
            ;
    }
    
    //immediately force send that config packet
    uint8_t *message = pvPortMalloc(MAX_PACKET_SIZE * sizeof(uint8_t));
    uint8_t length;
    radioPrepConfig();
    length = cbufGet(radioBufTX, message);
    radioTxRequest(&radio, message, length);
}

uint32_t getTimestamp()
{
    return (uint32_t)(timeSinceStartup() / 1000);
}

void modBitFieldSensorStatus(uint8_t sensorID, uint8_t offset,
                             uint8_t *generalFailMode, uint32_t *bitfield)
{
    *bitfield |= 0; //dereference the pointers (?)
    *generalFailMode |= 0;
    uint8_t status = getSensorStatus(sensorID);
    if (status > 0)
    {

        *bitfield = (*bitfield | (0x01 << offset));
        if (status > 1)
        {
            *generalFailMode = 0x03;
        }
        else
        {
            *generalFailMode = 0x01;
        }
    }
}

void radioPrepPing(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * PING_PACKET_LENGTH); //TODO: adjust length
    uint8_t ID = 0x00;
    uint32_t timestamp = getTimestamp();

    uint8_t generalFailMode = 0;

    uint32_t senStatusBitFieldA = 0x00; //these 4 bits represent sensor status,
                                        //and a few other statuses, like camera and SD

    //this is very explicit and not very elegant but I don't like assuming that
    //you can always enumerate through the sensors.

    //This might be something to look at in the future for improvement

    modBitFieldSensorStatus(0x04, 31, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x05, 30, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x06, 29, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x07, 28, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x1E, 27, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x1F, 26, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x20, 25, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x21, 24, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x22, 23, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x23, 22, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x24, 21, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x25, 20, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x26, 19, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x27, 18, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x28, 17, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x29, 16, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x2A, 15, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x2B, 14, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x2C, 13, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x2D, 12, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x2E, 11, &generalFailMode, &senStatusBitFieldA);
    modBitFieldSensorStatus(0x2F, 10, &generalFailMode, &senStatusBitFieldA);

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    message[5] = generalFailMode;
    memcpy(&message[6], &senStatusBitFieldA, 4);

    cbufPut(radioBufTX, PING_PACKET_LENGTH, message);

    vPortFree(message);
}

void radioPrepMessage(char *messageStr, uint8_t lengthBytes)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * (MESSAGE_MIN_PACKET_LENGTH + lengthBytes));
    uint8_t ID = 0x01;
    uint32_t timestamp = getTimestamp();

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    message[5] = lengthBytes;

    memcpy(&message[6], messageStr, lengthBytes);

    cbufPut(radioBufTX, (MESSAGE_MIN_PACKET_LENGTH + lengthBytes), message);

    vPortFree(message);
}

void radioPrepEvent(uint16_t eventCode)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * EVENT_PACKET_LENGTH);
    uint8_t ID = 0x02;
    uint32_t timestamp = getTimestamp();

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], &eventCode, 2);

    cbufPut(radioBufTX, PING_PACKET_LENGTH, message);

    vPortFree(message);
}

void radioPrepConfig(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * CONFIG_PACKET_LENGTH);
    uint8_t ID = 0x03;
    uint32_t timestamp = getTimestamp();
    uint8_t SIMstatus = SIM_ACTIVE;
    uint8_t deviceID = WB_DEVICE_ID;        
    uint8_t verString[40] = {0}; //actually setting the version thing would be cool.

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    message[5] = SIMstatus;
    message[6] = deviceID;
    for (int index = 0; index < 40; index++)
    {
        message[index + 7] = verString[index];
    }
    cbufPut(radioBufTX, CONFIG_PACKET_LENGTH, message);

    vPortFree(message);
}

void radioPrepGPS(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * GPS_PACKET_LENGTH); //TODO: adjust length
    uint8_t ID = 0x04;
    uint32_t timestamp = getTimestamp();
    float curGPS[3];
    getGPSValue(curGPS);
    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], &curGPS[0], 4);
    memcpy(&message[9], &curGPS[1], 4);
    memcpy(&message[16], &curGPS[2], 4);
    cbufPut(radioBufTX, GPS_PACKET_LENGTH, message);

    vPortFree(message);
}

void radioPrepState(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * STATE_PACKET_LENGTH);
    uint8_t ID = 0x05;
    uint32_t timestamp = getTimestamp();
    uint16_t curState = getState();

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], &curState, 2);
    cbufPut(radioBufTX, STATE_PACKET_LENGTH, message);

    vPortFree(message);
}

void radioPrepOrientation(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * ORIENT_PACKET_LENGTH);
    uint8_t ID = 0x06;
    uint32_t timestamp = getTimestamp();
    float curOrient[4];
    getOrientationValue(curOrient);

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], &curOrient[0], 16);
    cbufPut(radioBufTX, ORIENT_PACKET_LENGTH, message);
    vPortFree(message);
}

void radioPrepAccel(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * ACCEL_PACKET_LENGTH);
    uint8_t ID = 0x07;
    uint32_t timestamp = getTimestamp();
    float curAccel[3];
    getAccelValue(curAccel);

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], &curAccel[0], 16);
    cbufPut(radioBufTX, ACCEL_PACKET_LENGTH, message);
    vPortFree(message);
}


/***** Generic Functions for single float and int sensors *****/

void radioPrepSingleFloat(uint8_t ID, float value){
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_FLOAT_PACKET_LENGTH);
    uint32_t timestamp = getTimestamp();

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], &value, 4);
    cbufPut(radioBufTX, SINGLE_FLOAT_PACKET_LENGTH, message);
    vPortFree(message);
}

void radioPrepSingleInt(uint8_t ID, int value){
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * SINGLE_INT_PACKET_LENGTH);
    uint32_t timestamp = getTimestamp();

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    message[5] = value;
    cbufPut(radioBufTX, SINGLE_INT_PACKET_LENGTH, message);
    vPortFree(message);
}

//Data Dump
void radioPrepDataDump(void)
{
    uint8_t *message = pvPortMalloc(sizeof(uint8_t) * DATA_DUMP_PACKET_LENGTH);
    uint8_t ID = 0x30; //verify
    uint32_t timestamp = getTimestamp();

    float GPSValue[3];
    getGPSValue(GPSValue);

    float orientValue[4];
    getOrientationValue(orientValue);

    float accelValue[3];
    getAccelValue(accelValue);

    float PT_HP_T_001Value =    getSensorDataFloat(sensorPT_HP_T_001);
    float TC_HP_OUT_001Value =  getSensorDataFloat(sensorTC_HP_OUT_001);
    uint8_t V_HP_P_001Value =   getSensorDataInt(sensorV_HP_P_001);
    uint8_t V_F_PR_001Value =   getSensorDataInt(sensorV_F_PR_001);
    uint8_t V_F_V_001Value =    getSensorDataInt(sensorV_F_V_001);
    float PT_F_T_001Value =     getSensorDataFloat(sensorPT_F_T_001);
    uint8_t V_F_F_001Value =    getSensorDataInt(sensorV_F_F_001);
    float PT_F_INJ_001Value =   getSensorDataFloat(sensorPT_F_INJ_001);
    uint8_t V_F_MFV_001Value =  getSensorDataInt(sensorV_F_MFV_001);
    uint8_t V_L_PR_001Value =   getSensorDataInt(sensorV_L_PR_001);
    float PT_L_T_001Value =     getSensorDataFloat(sensorPT_L_T_001);
    uint8_t V_L_V_001Value =    getSensorDataInt(sensorV_L_V_001);
    float TC_L_F_001Value =     getSensorDataFloat(sensorTC_L_F_001);
    uint8_t V_L_F_001Value =    getSensorDataInt(sensorV_L_F_001);
    float PT_L_INJ_001Value =   getSensorDataFloat(sensorPT_L_INJ_001);
    uint8_t V_L_MOV_001Value =  getSensorDataInt(sensorV_L_MOV_001);
    uint8_t V_L_BLD_001Value =  getSensorDataInt(sensorV_L_BLD_001);
    float TC_L_BLD_001Value =   getSensorDataFloat(sensorTC_L_BLD_001);

    message[0] = ID;
    memcpy(&message[1], &timestamp, 4);
    memcpy(&message[5], GPSValue, 12);
    memcpy(&message[17], orientValue, 16);
    memcpy(&message[33], accelValue, 12);
    memcpy(&message[45], &PT_HP_T_001Value, 4);
    memcpy(&message[49], &TC_HP_OUT_001Value, 4);
    message[53] = V_HP_P_001Value;
    message[54] = V_F_PR_001Value;
    message[55] = V_F_V_001Value;
    memcpy(&message[56], &PT_F_T_001Value, 4);
    message[60] = V_F_F_001Value;
    memcpy(&message[61], &PT_F_INJ_001Value, 4);
    message[65] = V_F_MFV_001Value;
    message[66] = V_L_PR_001Value;
    memcpy(&message[67], &PT_L_T_001Value, 4);
    message[71] = V_L_V_001Value;
    memcpy(&message[72], &TC_L_F_001Value, 4);
    message[76] = V_L_F_001Value;
    memcpy(&message[77], &PT_L_INJ_001Value, 4);
    message[81] = V_L_MOV_001Value;
    message[82] = V_L_BLD_001Value;
    memcpy(&message[83], &TC_L_BLD_001Value, 4);

    cbufPut(radioBufTX, DATA_DUMP_PACKET_LENGTH, message);
    vPortFree(message);
}

void dealWithCritMessages(void *pv)
{
    uint8_t message = 0;
    for (;;)
    {
        if (!cbufGet(radioBufRXCrit, &message))
        {
            switch (message)
            {
            case 0x50:
                radioPrepPing();
                break;
            case 0x43:
                radioPrepConfig();
                break;
            case 0x41:
                xSemaphoreTake(stateTransitionInput.semaphore,portMAX_DELAY);
                stateTransitionInput.HMI_triggerAbort = STATE_MACHINE_TRIGGERED;
                xSemaphoreGive(stateTransitionInput.semaphore);
                break;
            case 0x46:
                xSemaphoreTake(stateTransitionInput.semaphore,portMAX_DELAY);
                stateTransitionInput.HMI_triggerFueling = STATE_MACHINE_TRIGGERED;
                xSemaphoreGive(stateTransitionInput.semaphore);
                break;
            case 0x53:
                xSemaphoreTake(stateTransitionInput.semaphore,portMAX_DELAY);
                stateTransitionInput.HMI_triggerStandby = STATE_MACHINE_TRIGGERED;
                xSemaphoreGive(stateTransitionInput.semaphore);
                break;
            }
        }
    }
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
                radioPrepSingleFloat(0x1E, getSensorDataFloat(sensorPT_HP_T_001));
                break;
            case 0x1F:
                radioPrepSingleFloat(0x1F,getSensorDataFloat(sensorTC_HP_OUT_001));
                break;
            case 0x20:
                radioPrepSingleInt(0x20,getSensorDataInt(sensorV_HP_P_001));
                break;
            case 0x21:
                radioPrepSingleInt(0x21,getSensorDataInt(sensorV_F_PR_001));
                break;
            case 0x22:
                radioPrepSingleFloat(0x22,getSensorDataFloat(sensorPT_F_T_001));
                break;
            case 0x23:
                radioPrepSingleFloat(0x23,getSensorDataFloat(sensorPT_F_INJ_001));
                break;
            case 0x24:
                radioPrepSingleInt(0x24,getSensorDataInt(sensorV_F_F_001));
                break;
            case 0x25:
                radioPrepSingleFloat(0x25,getSensorDataFloat(sensorPT_F_INJ_001));
                break;
            case 0x26:
                radioPrepSingleInt(0x26,getSensorDataInt(sensorV_F_MFV_001));
                break;
            case 0x27:
                radioPrepSingleInt(0x27,getSensorDataInt(sensorV_L_PR_001));
                break;
            case 0x28:
                radioPrepSingleFloat(0x28,getSensorDataFloat(sensorPT_L_T_001));
                break;
            case 0x29:
                radioPrepSingleInt(0x29,getSensorDataInt(sensorV_L_V_001));
                break;
            case 0x2A:
                radioPrepSingleFloat(0x2A,getSensorDataFloat(sensorTC_L_F_001));
                break;
            case 0x2B:
                radioPrepSingleInt(0x2B,getSensorDataInt(sensorV_L_F_001));
                break;
            case 0x2C:
                radioPrepSingleFloat(0x2C,getSensorDataFloat(sensorPT_L_INJ_001));
                break;
            case 0x2D:
                radioPrepSingleInt(0x2D,getSensorDataInt(sensorV_L_MOV_001));
                break;
            case 0x2E:
                radioPrepSingleInt(0x2E,getSensorDataInt(sensorV_L_BLD_001));
                break;
            case 0x2F:
                radioPrepSingleFloat(0x2F,getSensorDataFloat(sensorTC_L_BLD_001));
                break;
            case 0x30:
                radioPrepDataDump();
                break;
            }
        }
    }
}
