
#ifndef _STDIO_CONTROLLER_H_
#define _STDIO_CONTROLLER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_uart.h"

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SIM_PACKET_BUFFER_SIZE 8
#define SIM_MESSAGE_BUFFER_SIZE 512
#define SIM_MAX_PACKET_IDS 0x74
#define RX 0
#define TX 1

#define FLAGGED 1
#define UNFLAGGED 0


/*******************************************************************************
 * Variables
 ******************************************************************************/


extern uint8_t packetBuffers[2][SIM_MAX_PACKET_IDS][SIM_PACKET_BUFFER_SIZE][SIM_MESSAGE_BUFFER_SIZE];
extern int packetBuffersWriteIndex[2][SIM_MAX_PACKET_IDS];
extern int packetBuffersReadIndex[2][SIM_MAX_PACKET_IDS];
extern int packetBuffersReadLowerLimit[2][SIM_MAX_PACKET_IDS]; //needs to be initialized in the init function
//the upper packet limit is the lower limit + the packet buffer size. It's the
//range of allowable readable values
// extern int packetBuffersNewFlag[2][SIM_MAX_PACKET_IDS];


/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initalizes the io thread for communication. Called in HAL init. 
 */
void stdioInit();

/**
 * Assigns given uart handle to uart id for data spoofing. Automatically detects
 * what uart line to assign to based on base parameter of handle. 
 * @param handle a pointer to the handle to setd
 */
void stdioAssignUart(hal_uart_handle_t *handle);

/**
 * Reads a packet from either the sim TX or RX buffer. 
 * used by sendpackets() to send packets out or to recieve data from the sim RX buffer
 * @param mode specify TX or RX buffer
 * @param data where you will store the data you are recieveing. Will never be more than
 * MESSAGE_BUFFER_SIZE
 * @param id sim ID of what you want to read, in range 0-MAX_PACKET_IDS
 * @return the length of packet, i.e. the length of what's just been placed in data
 */
uint8_t readFromBuf(int mode, uint8_t data[], uint8_t id);

/**
 * Writes a packet to the sim buffer, used internally by extractPackets() or to 
 * add a packet to be sent out
 * 
 * @param mode specify TX ot RX
 * @param data the data you want to write to the buffer, i.e. the packet body
 * @param id sim ID of packet, in range 0-MAX_PACKET_IDS
 * @param length length of the packet, no more than MESSAGE_BUFFER_SIZE
 */
void writeToBuf(int mode, uint8_t data[], uint8_t id, uint16_t length);

#endif
