#ifndef _RADIO_PROTOCOL_H_
#define _RADIO_PROTOCOL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
//#include "include/FreeRTOS.h"
#include "buffer.h"
#include "radio.h"

#define STATE_CHANGE_WATERSHED 0x40

cbufHandle_t radioBufTX;

cbufHandle_t radioBufRXCrit;

cbufHandle_t radioBufRXLow;

xbee_dev_t radio; //These might have more descriptive names in the future.

xbee_serial_t serial;

//typedef enum componentResponse_t{

//}

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/*prepates a config packet and puts it in the send buffer*/



#endif