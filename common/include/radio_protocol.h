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

extern xbee_dev_t radio; //These might have more descriptive names in the future.

extern xbee_serial_t serial;



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
void radioPrepGPS(void);

/**
 * Load a state packet into the "to transmit" buffer
 */
void radioPrepState(void);

/**
 * Load an orientation packet into the "to transmit" buffer
 */
void radioPrepOrientation(void);

/**
 * Load an acceleration packet into the "to transmit" buffer
 */
void radioPrepAccel(void);

/**
 * Load a generic single floating point number packet into the "to transmit" buffer
 */
void radioPrepSingleFloat(uint8_t ID, float value);

/**
 * Load a generic single integer number packet into the "to transmit" buffer
 */
void radioPrepSingleInt(uint8_t ID, int value);


/**
 * Load a data dump packet into the "to transmit" buffer
 */
void radioPrepDataDump(void);



#endif
