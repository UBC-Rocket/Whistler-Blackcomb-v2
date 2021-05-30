#include "variables.h"


//GPS

struct GPSdata_s{
    float lat;
    float lon;
    float alt; 
    SemaphoreHandle_t semaphore;
} GPSdata;

void setGPS(float value[3]){
    xSemaphoreTake(GPSdata.semaphore,portMAX_DELAY);
    GPSdata.lat=value[0];
    GPSdata.lon=value[1];
    GPSdata.alt=value[2];
    xSemaphoreGive(GPSdata.semaphore);
}

void getGPS(float value[3]){
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

void getOrientation(float value[4]){
    xSemaphoreTake(orientData.semaphore,portMAX_DELAY);
    orientData.x=value[0];
    orientData.y=value[1];
    orientData.z=value[2];
    orientData.w=value[3];
    xSemaphoreGive(orientData.semaphore);
}

void setOrientation(float value[4]){
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

void getAccel(float value[3]){
    xSemaphoreTake(accelData.semaphore,portMAX_DELAY);
    accelData.x=value[0];
    accelData.y=value[1];
    accelData.z=value[2];
    xSemaphoreGive(accelData.semaphore);
}

void setAccel(float value[3]){
    xSemaphoreTake(accelData.semaphore,portMAX_DELAY);
    value[0]=accelData.x;
    value[1]=accelData.y;
    value[2]=accelData.z;
    xSemaphoreGive(accelData.semaphore);
}



//Single Values

//Single Float Values
struct singleFloatSemphr_s{
    float value;
    SemaphoreHandle_t semaphore;
};

//single uint8_t Values 
struct singleIntSemphr_s{
    uint8_t value;
    SemaphoreHandle_t semaphore;
};



//HP Tank PT
struct singleFloatSemphr_s PT_HP_T_001data;
void setPT_HP_T_001(float value){
    xSemaphoreTake(PT_HP_T_001data.semaphore,portMAX_DELAY);
    PT_HP_T_001data.value=value; //wow how descriptive, this is the epitome of self-documenting code
    xSemaphoreGive(PT_HP_T_001data.semaphore);
}
float getPT_HP_T_001(void){
    xSemaphoreTake(PT_HP_T_001data.semaphore,portMAX_DELAY);
    float value=PT_HP_T_001data.value;
    xSemaphoreGive(PT_HP_T_001data.semaphore);
    return value;
}

//HP Tank Outlet TC
struct singleFloatSemphr_s TC_HP_OUT_001data;
void setTC_HP_OUT_001(float value){
    xSemaphoreTake(TC_HP_OUT_001data.semaphore,portMAX_DELAY);
    TC_HP_OUT_001data.value=value;
    xSemaphoreGive(TC_HP_OUT_001data.semaphore);
}
float getTC_HP_OUT_001(void){
    xSemaphoreTake(TC_HP_OUT_001data.semaphore,portMAX_DELAY);
    float value=TC_HP_OUT_001data.value;
    xSemaphoreGive(TC_HP_OUT_001data.semaphore);
    return value;
}

//HP Press Valve
struct singleIntSemphr_s V_HP_P_001data;
void setV_HP_P_001(uint8_t value){
    xSemaphoreTake(V_HP_P_001data.semaphore,portMAX_DELAY);
    V_HP_P_001data.value=value;
    xSemaphoreGive(V_HP_P_001data.semaphore);
}
uint8_t getV_HP_P_001(void){
    xSemaphoreTake(V_HP_P_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_HP_P_001data.value;
    xSemaphoreGive(V_HP_P_001data.semaphore);
    return value;
}

//Fuel Press Valve
struct singleIntSemphr_s V_F_PR_001data;
void setV_F_PR_001(uint8_t value){
    xSemaphoreTake(V_F_PR_001data.semaphore,portMAX_DELAY);
    V_F_PR_001data.value=value;
    xSemaphoreGive(V_F_PR_001data.semaphore);
}
uint8_t getV_F_PR_001(void){
    xSemaphoreTake(V_F_PR_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_F_PR_001data.value;
    xSemaphoreGive(V_F_PR_001data.semaphore);
    return value;
}

//Fuel Vent Valve
struct singleIntSemphr_s V_F_V_001data;
void setV_F_V_001(uint8_t value){
    xSemaphoreTake(V_F_V_001data.semaphore,portMAX_DELAY);
    V_F_V_001data.value=value;
    xSemaphoreGive(V_F_V_001data.semaphore);
}
uint8_t getV_F_V_001(void){
    xSemaphoreTake(V_F_V_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_F_V_001data.value;
    xSemaphoreGive(V_F_V_001data.semaphore);
    return value;
}

//Fuel Tank PT
struct singleFloatSemphr_s PT_F_T_001data;
void setPT_F_T_001(float value){
    xSemaphoreTake(PT_F_T_001data.semaphore,portMAX_DELAY);
    PT_F_T_001data.value=value;
    xSemaphoreGive(PT_F_T_001data.semaphore);
}
float getPT_F_T_001(void){
    xSemaphoreTake(PT_F_T_001data.semaphore,portMAX_DELAY);
    float value=PT_F_T_001data.value;
    xSemaphoreGive(PT_F_T_001data.semaphore);
    return value;
}

//Fuel Fill Valve
struct singleIntSemphr_s V_F_F_001data;
void setV_F_F_001(uint8_t value){
    xSemaphoreTake(V_F_F_001data.semaphore,portMAX_DELAY);
    V_F_F_001data.value=value;
    xSemaphoreGive(V_F_F_001data.semaphore);
}
uint8_t getV_F_F_001(void){
    xSemaphoreTake(V_F_F_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_F_F_001data.value;
    xSemaphoreGive(V_F_F_001data.semaphore);
    return value;
}

//Fuel Injector PT
struct singleFloatSemphr_s PT_F_INJ_001data;
void setPT_F_INJ_001(float value){
    xSemaphoreTake(PT_F_INJ_001data.semaphore,portMAX_DELAY);
    PT_F_INJ_001data.value=value;
    xSemaphoreGive(PT_F_INJ_001data.semaphore);
}
float getPT_F_INJ_001(void){
    xSemaphoreTake(PT_F_INJ_001data.semaphore,portMAX_DELAY);
    float value=PT_F_INJ_001data.value;
    xSemaphoreGive(PT_F_INJ_001data.semaphore);
    return value;
}

//Main Fuel Valve
struct singleIntSemphr_s V_F_MFV_001data;
void setV_F_MFV_001(uint8_t value){
    xSemaphoreTake(V_F_MFV_001data.semaphore,portMAX_DELAY);
    V_F_MFV_001data.value=value;
    xSemaphoreGive(V_F_MFV_001data.semaphore);
}
uint8_t getV_F_MFV_001(void){
    xSemaphoreTake(V_F_MFV_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_F_MFV_001data.value;
    xSemaphoreGive(V_F_MFV_001data.semaphore);
    return value;
}

//LOX Press Valve
struct singleIntSemphr_s V_L_PR_001data;
void setV_L_PR_001(uint8_t value){
    xSemaphoreTake(V_L_PR_001data.semaphore,portMAX_DELAY);
    V_L_PR_001data.value=value;
    xSemaphoreGive(V_L_PR_001data.semaphore);
}
uint8_t getV_L_PR_001(void){
    xSemaphoreTake(V_L_PR_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_L_PR_001data.value;
    xSemaphoreGive(V_L_PR_001data.semaphore);
    return value;
}

//LOX Tank PT
struct singleFloatSemphr_s PT_L_T_001data;
void setPT_L_T_001(float value){
    xSemaphoreTake(PT_L_T_001data.semaphore,portMAX_DELAY);
    PT_L_T_001data.value=value;
    xSemaphoreGive(PT_L_T_001data.semaphore);
}
float getPT_L_T_001(void){
    xSemaphoreTake(PT_L_T_001data.semaphore,portMAX_DELAY);
    float value=PT_L_T_001data.value;
    xSemaphoreGive(PT_L_T_001data.semaphore);
    return value;
}

//LOX Vent Valve
struct singleIntSemphr_s V_L_V_001data;
void setV_L_V_001(uint8_t value){
    xSemaphoreTake(V_L_V_001data.semaphore,portMAX_DELAY);
    V_L_V_001data.value=value;
    xSemaphoreGive(V_L_V_001data.semaphore);
}
uint8_t getV_L_V_001(void){
    xSemaphoreTake(V_L_V_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_L_V_001data.value;
    xSemaphoreGive(V_L_V_001data.semaphore);
    return value;
}

//LOX Fill TC
struct singleFloatSemphr_s TC_L_F_001data;
void setTC_L_F_001(float value){
    xSemaphoreTake(TC_L_F_001data.semaphore,portMAX_DELAY);
    TC_L_F_001data.value=value;
    xSemaphoreGive(TC_L_F_001data.semaphore);
}
float getTC_L_F_001(void){
    xSemaphoreTake(TC_L_F_001data.semaphore,portMAX_DELAY);
    float value=TC_L_F_001data.value;
    xSemaphoreGive(TC_L_F_001data.semaphore);
    return value;
}

//LOX Fill Valve
struct singleIntSemphr_s V_L_F_001data;
void setV_L_F_001(uint8_t value){
    xSemaphoreTake(V_L_F_001data.semaphore,portMAX_DELAY);
    V_L_F_001data.value=value;
    xSemaphoreGive(V_L_F_001data.semaphore);
}
uint8_t getV_L_F_001(void){
    xSemaphoreTake(V_L_F_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_L_F_001data.value;
    xSemaphoreGive(V_L_F_001data.semaphore);
    return value;
}

//LOX Injector PT
struct singleFloatSemphr_s PT_L_INJ_001data;
void setPT_L_INJ_001(float value){
    xSemaphoreTake(PT_L_INJ_001data.semaphore,portMAX_DELAY);
    PT_L_INJ_001data.value=value;
    xSemaphoreGive(PT_L_INJ_001data.semaphore);
}
float getPT_L_INJ_001(void){
    xSemaphoreTake(PT_L_INJ_001data.semaphore,portMAX_DELAY);
    float value=PT_L_INJ_001data.value;
    xSemaphoreGive(PT_L_INJ_001data.semaphore);
    return value;
}

//Main Oxidizer Valve
struct singleIntSemphr_s V_L_MOV_001data;
void setV_L_MOV_001(uint8_t value){
    xSemaphoreTake(V_L_MOV_001data.semaphore,portMAX_DELAY);
    V_L_MOV_001data.value=value;
    xSemaphoreGive(V_L_MOV_001data.semaphore);
}
uint8_t getV_L_MOV_001(void){
    xSemaphoreTake(V_L_MOV_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_L_MOV_001data.value;
    xSemaphoreGive(V_L_MOV_001data.semaphore);
    return value;
}

//LOX Bleed Valve
struct singleIntSemphr_s V_L_BLD_001data;
void setV_L_BLD_001(uint8_t value){
    xSemaphoreTake(V_L_BLD_001data.semaphore,portMAX_DELAY);
    V_L_BLD_001data.value=value;
    xSemaphoreGive(V_L_BLD_001data.semaphore);
}
uint8_t getV_L_BLD_001(void){
    xSemaphoreTake(V_L_BLD_001data.semaphore,portMAX_DELAY);
    uint8_t value=V_L_BLD_001data.value;
    xSemaphoreGive(V_L_BLD_001data.semaphore);
    return value;
}

//LOX Bleed TC
struct singleFloatSemphr_s TC_L_BLD_001data;
void setTC_L_BLD_001(float value){
    xSemaphoreTake(TC_L_BLD_001data.semaphore,portMAX_DELAY);
    TC_L_BLD_001data.value=value;
    xSemaphoreGive(TC_L_BLD_001data.semaphore);
}
float getTC_L_BLD_001(void){
    xSemaphoreTake(TC_L_BLD_001data.semaphore,portMAX_DELAY);
    float value=TC_L_BLD_001data.value;
    xSemaphoreGive(TC_L_BLD_001data.semaphore);
    return value;
}