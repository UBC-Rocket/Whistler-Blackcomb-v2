#include "variables.h"

/* I feel bad about this code. It is bad and sad and makes me mad. Sorry.*/

struct sensorStatus_s{
    uint8_t status[0x2F]; 
    SemaphoreHandle_t semaphore;
}sensorStatus;

void setSensorStatus(int sensorID, int status){
    xSemaphoreTake(sensorStatus.semaphore,portMAX_DELAY);
    sensorStatus.status[sensorID] = status;
    xSemaphoreGive(sensorStatus.semaphore);
}

int getSensorStatus(int sensorID){
    xSemaphoreTake(sensorStatus.semaphore,portMAX_DELAY);
    int status = sensorStatus.status[sensorID];
    xSemaphoreGive(sensorStatus.semaphore);
    return status;
}


//GPS
struct GPSdata_s{
    float lat;
    float lon;
    float alt; 
    SemaphoreHandle_t semaphore;
} GPSdata;
void setGPSValue(float value[3]){
    xSemaphoreTake(GPSdata.semaphore,portMAX_DELAY);
    GPSdata.lat=value[0];
    GPSdata.lon=value[1];
    GPSdata.alt=value[2];
    xSemaphoreGive(GPSdata.semaphore);
}
void getGPSValue(float value[3]){
    xSemaphoreTake(GPSdata.semaphore,portMAX_DELAY);
    value[0]=GPSdata.lat;
    value[1]=GPSdata.lon;
    value[2]=GPSdata.alt;
    xSemaphoreGive(GPSdata.semaphore);
}

//Orientation
struct orientData_s{
    float x;
    float y;
    float z;
    float w;
    SemaphoreHandle_t semaphore;
} orientData;
void getOrientationValue(float value[4]){
    xSemaphoreTake(orientData.semaphore,portMAX_DELAY);
    orientData.x=value[0];
    orientData.y=value[1];
    orientData.z=value[2];
    orientData.w=value[3];
    xSemaphoreGive(orientData.semaphore);
}
void setOrientationValue(float value[4]){
    xSemaphoreTake(orientData.semaphore,portMAX_DELAY);
    value[0]=orientData.x;
    value[1]=orientData.y;
    value[2]=orientData.z;
    value[3]=orientData.w;
    xSemaphoreGive(orientData.semaphore);
}

//Acceleration
struct accelData_s{
    float x;
    float y;
    float z;
    SemaphoreHandle_t semaphore;
} accelData;
void getAccelValue(float value[3]){
    xSemaphoreTake(accelData.semaphore,portMAX_DELAY);
    accelData.x=value[0];
    accelData.y=value[1];
    accelData.z=value[2];
    xSemaphoreGive(accelData.semaphore);
}
void setAccelValue(float value[3]){
    xSemaphoreTake(accelData.semaphore,portMAX_DELAY);
    value[0]=accelData.x;
    value[1]=accelData.y;
    value[2]=accelData.z;
    xSemaphoreGive(accelData.semaphore);
}

//Single Values

//Single Float Values
typedef struct singleFloatSensor_s{
    float value;
    SemaphoreHandle_t semaphore;
}singleFloatSensor_s;

//single uint8_t Values 
typedef struct singleIntSensor_s{
    uint8_t value;
    SemaphoreHandle_t semaphore;
}singleIntSensor_s;


singleFloatSensor_s floatSensors[SIZE_FLOAT_SENSORS];
singleIntSensor_s   intSensors[SIZE_INT_SENSORS];

int getSensorDataInt(intSensor_t sensor){
    xSemaphoreTake(intSensors[sensor].semaphore,portMAX_DELAY);
    int value=intSensors[sensor].value;
    xSemaphoreGive(intSensors[sensor].semaphore);
    return value;
}

float getSensorDataFloat(floatSensor_t sensor){
    xSemaphoreTake(floatSensors[sensor].semaphore,portMAX_DELAY);
    float value=floatSensors[sensor].value;
    xSemaphoreGive(floatSensors[sensor].semaphore);
    return value;
}

void setSensorDataInt(intSensor_t sensor, int value){
    xSemaphoreTake(intSensors[sensor].semaphore,portMAX_DELAY);
    intSensors[sensor].value=value; //wow how descriptive, this is the epitome of self-documenting code
    xSemaphoreGive(intSensors[sensor].semaphore);
}

void setSensorDataFloat(floatSensor_t sensor, float value){
    xSemaphoreTake(floatSensors[sensor].semaphore,portMAX_DELAY);
    floatSensors[sensor].value=value;
    xSemaphoreGive(floatSensors[sensor].semaphore);
}

void initVariables(void){
    stateTransitionInput.semaphore =    xSemaphoreCreateMutex();
    sensorStatus.semaphore =            xSemaphoreCreateMutex();
    GPSdata.semaphore =                 xSemaphoreCreateMutex();
    orientData.semaphore =              xSemaphoreCreateMutex();
    accelData.semaphore =               xSemaphoreCreateMutex();
    
    for(int i = 0; i<SIZE_FLOAT_SENSORS; i++){
        floatSensors[i].semaphore = xSemaphoreCreateMutex();
    }

    for(int i = 0; i<SIZE_INT_SENSORS; i++){
        intSensors[i].semaphore = xSemaphoreCreateMutex();
    }
    
}