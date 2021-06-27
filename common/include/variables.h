#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h" 
#include "state_machine.h"


stateInput_t stateTransitonInput = {0};



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

//HP Tank PT
void setPT_HP_T_001Value(float value);
//HP Tank PT
float getPT_HP_T_001Value(void);

//HP Tank Outlet TC
void setTC_HP_OUT_001Value(float value);
//HP Tank Outlet TC
float getTC_HP_OUT_001Value(void);

//HP Press Valve
void setV_HP_P_001Value(uint8_t value);
//HP Press Valve
uint8_t getV_HP_P_001Value(void);

//Fuel Press Valve
void setV_F_PR_001Value(uint8_t value);
//Fuel Press Valve
uint8_t getV_F_PR_001Value(void);

//Fuel Vent Valve
void setV_F_V_001Value(uint8_t value);
//Fuel Vent Valve
uint8_t getV_F_V_001Value(void);

//Fuel Tank PT
void setPT_F_T_001Value(float value);
//Fuel Tank PT
float getPT_F_T_001Value(void);

//Fuel Fill Valve
void setV_F_F_001Value(uint8_t value);
//Fuel Fill Valve
uint8_t getV_F_F_001Value(void);

//Fuel Injector PT
void setPT_F_INJ_001Value(float value);
//Fuel Injector PT
float getPT_F_INJ_001Value(void);

//Main Fuel Valve
void setV_F_MFV_001Value(uint8_t value);
//Main Fuel Valve
uint8_t getV_F_MFV_001Value(void);

//LOX Press Valve
void setV_L_PR_001Value(uint8_t value);
//LOX Press Valve
uint8_t getV_L_PR_001Value(void);

//LOX Tank PT
void setPT_L_T_001Value(float value);
//LOX Tank PT
float getPT_L_T_001Value(void);

//LOX Vent Valve
void setV_L_V_001Value(uint8_t value);
//LOX Vent Valve
uint8_t getV_L_V_001Value(void);

//LOX Fill TC
void setTC_L_F_001Value(float value);
//LOX Fill TC
float getTC_L_F_001Value(void);

//LOX Fill Valve
void setV_L_F_001Value(uint8_t value);
//LOX Fill Valve
uint8_t getV_L_F_001Value(void);

//LOX Injector PT
void setPT_L_INJ_001Value(float value);
//LOX Injector PT
float getPT_L_INJ_001Value(void);

//Main Oxidizer Valve
void setV_L_MOV_001Value(uint8_t value);
//Main Oxidizer Valve
uint8_t getV_L_MOV_001Value(void);

//LOX Bleed Valve
void setV_L_BLD_001Value(uint8_t value);
//LOX Bleed Valve
uint8_t getV_L_BLD_001Value(void);

//LOX Bleed TC
void setTC_L_BLD_001Value(float value);
//LOX Bleed TC
float getTC_L_BLD_001Value(void);


#endif