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
 * Writes tx request frame (id 0x10) to radio. 
 * @param radio radio handle to send to
 * @param serial serial port to send through
 * @param buffer the buffer of data to send
 * @param length the length of the data to send (buffer)
 */
void radioTxRequest(xbee_dev_t * radio, const uint8_t * buffer, 
        unsigned int length);

/**
 * Receives data from the radio. Block until data is received. 
 * @param radio pointer to handle of radio to receive from
 * @param packet the packet to receive data into. Must be pointing to at least
 * 256 bytes of free space, as taht is maximum packet size. 
 * @return number of bytes read, i.e. how many bytes are in packet
 */
int radioReceive(xbee_dev_t * radio, uint8_t * packet);
    

#endif