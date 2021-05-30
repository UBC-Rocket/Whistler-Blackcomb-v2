#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h" 

//GPS, [lat,lon,Alt]
void setGPS(float value[3]);
//GPS, [lat,lon,Alt]
void getGPS(float value[3]);

//state??

//Orientation, [x,y,z,w]
void setOrientation(float value[4]);
//Orientation, [x,y,z,w]
void getOrientation(float value[4]);

//Acceleration, [x,y,z]
void getAccel(float value[3]);
//Acceleration, [x,y,z]
void setAccel(float value[3]);

//HP Tank PT
void setPT_HP_T_001(float value);
//HP Tank PT
float getPT_HP_T_001(void);

//HP Tank Outlet TC
void setTC_HP_OUT_001(float value);
//HP Tank Outlet TC
float getTC_HP_OUT_001(void);

//HP Press Valve
void setV_HP_P_001(uint8_t value);
//HP Press Valve
uint8_t getV_HP_P_001(void);

//Fuel Press Valve
void setV_F_PR_001(uint8_t value);
//Fuel Press Valve
uint8_t getV_F_PR_001(void);

//Fuel Vent Valve
void setV_F_V_001(uint8_t value);
//Fuel Vent Valve
uint8_t getV_F_V_001(void);

//Fuel Tank PT
void setPT_F_T_001(float value);
//Fuel Tank PT
float getPT_F_T_001(void);

//Fuel Fill Valve
void setV_F_F_001(uint8_t value);
//Fuel Fill Valve
uint8_t getV_F_F_001(void);

//Fuel Injector PT
void setPT_F_INJ_001(float value);
//Fuel Injector PT
float getPT_F_INJ_001(void);

//Main Fuel Valve
void setV_F_MFV_001(uint8_t value);
//Main Fuel Valve
uint8_t getV_F_MFV_001(void);

//LOX Press Valve
void setV_L_PR_001(uint8_t value);
//LOX Press Valve
uint8_t getV_L_PR_001(void);

//LOX Tank PT
void setPT_L_T_001(float value);
//LOX Tank PT
float getPT_L_T_001(void);

//LOX Vent Valve
void setV_L_V_001(uint8_t value);
//LOX Vent Valve
uint8_t getV_L_V_001(void);

//LOX Fill TC
void setTC_L_F_001(float value);
//LOX Fill TC
float getTC_L_F_001(void);

//LOX Fill Valve
void setV_L_F_001(uint8_t value);
//LOX Fill Valve
uint8_t getV_L_F_001(void);

//LOX Injector PT
void setPT_L_INJ_001(float value);
//LOX Injector PT
float getPT_L_INJ_001(void);

//Main Oxidizer Valve
void setV_L_MOV_001(uint8_t value);
//Main Oxidizer Valve
uint8_t getV_L_MOV_001(void);

//LOX Bleed Valve
void setV_L_BLD_001(uint8_t value);
//LOX Bleed Valve
uint8_t getV_L_BLD_001(void);

//LOX Bleed TC
void setTC_L_BLD_001(float value);
//LOX Bleed TC
float getTC_L_BLD_001(void);


#endif