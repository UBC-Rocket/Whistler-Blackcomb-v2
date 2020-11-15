#ifndef _RADIO_H_
#define _RADIO_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "xbee/wpan.h"
#include "xbee/atcmd.h"
#include "xbee/device.h"

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Writes tx request frame (id 0x10) to radio
 * @param radio radio handle to send to
 * @param serial serial port to send through
 * @param buffer the buffer of data to send
 * @param length the length of the data to send (buffer)
 */
void radioTxRequest(xbee_dev_t * radio, xbee_serial_t * serial, 
        uint8_t * buffer, unsigned int length);
    

#endif