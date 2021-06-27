#ifndef _RADIO_PROTOCOL_H_
#define _RADIO_PROTOCOL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
//#include "include/FreeRTOS.h"
#include "buffer.h"
#include "radio.h"
#include "hal_time.h"
#include "board.h"
#include "variables.h"
#include "state_machine.h"

xbee_dev_t radio; //These might have more descriptive names in the future.

xbee_serial_t serial;



/*******************************************************************************
 * Declarations
 ******************************************************************************/
/**
 * Initialize the GS radio protocol functions - input loops and output loops
 */
void GSRadioInit(void);

/**
 * Load a ping packet into the "to transmit" buffer
 */
void radioPrepPing(void);

/**
 * Load a message packet into the "to transmit" buffer
 * @param messageStr Pointer to char array of the message to send
 * @param lengthbytes Length in bytes of the message
 */
void radioPrepMessage(char *messageStr, uint8_t lengthBytes);

/**
 * Load an event packet into the "to transmit" buffer
 * @param eventCode the eventcode of the event that has occured
 */
void radioPrepEvent(uint16_t eventCode);

/**
 * Load a config packet into the "to transmit" buffer
 */
void radioPrepConfig(void);

/**
 * Load a GPS packet into the "to transmit" buffer
 */
static void radioPrepGPS(void);

/**
 * Load a state packet into the "to transmit" buffer
 */
static void radioPrepState(void);

/**
 * Load an orientation packet into the "to transmit" buffer
 */
static void radioPrepOrientation(void);

/**
 * Load an acceleration packet into the "to transmit" buffer
 */
static void radioPrepAccel(void);

/**
 * Load an HP Tank PT packet into the "to transmit" buffer
 */
static void radioPrepPT_HP_T_001(void);

/**
 * Load an HP Tank Outlet TC packet into the "to transmit" buffer
 */
static void radioPrepTC_HP_OUT_001(void);

/**
 * Load an HP Press Valve packet into the "to transmit" buffer
 */
static void radioPrepV_HP_P_001(void);

/**
 * Load a Fuel Press Valve packet into the "to transmit" buffer
 */
static void radioPrepV_F_PR_001(void);

/**
 * Load a Fuel Vent Valve packet into the "to transmit" buffer
 */
static void radioPrepV_F_V_001(void);

/**
 * Load a Fuel Tank PT packet into the "to transmit" buffer
 */
static void radioPrepPT_F_T_001(void);

/**
 * Load a Fuel Fill Valve packet into the "to transmit" buffer
 */
static void radioPrepV_F_F_001(void);

/**
 * Load a Fuel Injector PT into the "to transmit" buffer
 */
static void radioPrepPT_F_INJ_001(void);

/**
 * Load a Main Fuel Valve packet into the "to transmit" buffer
 */
static void radioPrepV_F_MFV_001(void);

/**
 * Load a LOX Press Valve packet into the "to transmit" buffer
 */
static void radioPrepV_L_PR_001(void);

/**
 * Load a LOX Tank PT packet into the "to transmit" buffer
 */
static void radioPrepPT_L_T_001(void);

/**
 * Load a LOX Vent Valve packet into the "to transmit" buffer
 */
static void radioPrepV_L_V_001(void);

/**
 * Load a LOX Fill TC packet into the "to transmit" buffer
 */
static void radioPrepTC_L_F_001(void);

/**
 * Load a LOX Fill Valve packet into the "to transmit" buffer
 */
static void radioPrepV_L_F_001(void);

/**
 * Load a LOX Injector PT packet into the "to transmit" buffer
 */
static void radioPrepPT_L_INJ_001(void);

/**
 * Load a Main Oxidizer Valve packet into the "to transmit" buffer
 */
static void radioPrepV_L_MOV_001(void);

/**
 * Load a LOX Bleed Valve packet into the "to transmit" buffer
 */
static void radioPrepV_L_BLD_001(void);

/**
 * Load a LOX Bleed TC packet into the "to transmit" buffer
 */
static void radioPrepTC_L_BLD_001(void);

/**
 * Load a data dump packet into the "to transmit" buffer
 */
static void radioPrepDataDump(void);



#endif