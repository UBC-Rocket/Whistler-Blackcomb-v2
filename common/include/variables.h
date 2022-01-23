#ifndef _VARIABLES_H_
#define _VARIABLES_H_


#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h" 
#include "state_machine.h"

/**
 * All generic sensors on the rocket
 * NOTE: No assigning values or making holes or anything screwed up like that
 */
typedef enum intSensor_t
{
    sensorV_HP_P_001,
    sensorV_F_PR_001,
    sensorV_F_V_001,
    sensorV_F_F_001,
    sensorV_F_MFV_001,
    sensorV_L_PR_001,
    sensorV_L_V_001,
    sensorV_L_F_001,
    sensorV_L_MOV_001,
    sensorV_L_BLD_001,
    //add new sensors here
    SIZE_INT_SENSORS
} intSensor_t;

typedef enum floatSensor_t
{
    sensorPT_HP_T_001,
    sensorTC_HP_OUT_001,
    sensorPT_F_T_001,
    sensorPT_F_INJ_001,
    sensorPT_L_T_001,
    sensorTC_L_F_001,
    sensorPT_L_INJ_001,
    sensorTC_L_BLD_001,
    //add new sensors here
    SIZE_FLOAT_SENSORS
} floatSensor_t;

/***** For all generic int and float sensors *****/
int getSensorDataInt(intSensor_t sensor);

float getSensorDataFloat(floatSensor_t sensor);

void setSensorDataInt(intSensor_t sensor, int value);

void setSensorDataFloat(floatSensor_t sensor, float value);

void initVariables(void);





extern stateInput_t stateTransitionInput;

//Sensor Status, in sensor order - see docs
void setSensorStatus(int sensorID, int status);

int getSensorStatus(int sensorID);

//GPS, [lat,lon,Alt]
void setGPSValue(float value[3]);
//GPS, [lat,lon,Alt]
void getGPSValue(float value[3]);

//state??

//Orientation, [x,y,z,w]
void setOrientationValue(float value[4]);
//Orientation, [x,y,z,w]
void getOrientationValue(float value[4]);

//Acceleration, [x,y,z]
void getAccelValue(float value[3]);
//Acceleration, [x,y,z]
void setAccelValue(float value[3]);

#endif
