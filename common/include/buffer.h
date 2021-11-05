#ifndef _BUFFER_H_
#define _BUFFER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h" 
/*******************************************************************************
 * Definitions
 ******************************************************************************/

//opaque circ buf structure

typedef struct cbuf_t cbuf_t;

//handle for circular buffer

typedef cbuf_t* cbufHandle_t;

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Circular buffer initiaization
 * 
 * @param size number of uint8_t elements you want to hold
 * @return handle of created circular buffer
 */
cbufHandle_t cbufInit(size_t size);

/**
 * Frees buffer structure and underlying data buffer
 * @param cbuf handle of the buffer to free
 */
void cbufFree(cbufHandle_t cbuf);


/**
 * resets the buffer to empty, head == tail
 * @param cbuf handle of the buffer to free
 */
void cbufReset(cbufHandle_t cbuf);


/**
 * Put places new data into buffer into the buffer if there is space. It will
 * not overwrite existing data
 * @param cbuf handle of buffer to write too
 * @param data data added to buffer (uint8_t)
 * @return 0 on success, -1 on failure
 */
int cbufPut(cbufHandle_t cbuf, uint8_t length, uint8_t *data);

/**
 * Retreives a value from the buffer (oldest first)
 * @param cbuf handle of the buffer to read from
 * @param data where the data should be written to
 * @return length of message in buffer, 0 if no message
 */
int cbufGet(cbufHandle_t cbuf, uint8_t * data);


/**
 * Checks if buffer is empty - only for use within something already taking the semaphore!
 * @param cbuf handle of buffer to check
 * @return true if empty
 */
bool cbufCheckEmpty(cbufHandle_t cbuf);

/**
 * Checks if buffer is full
 * @param cbuf handle of buffer to check
 * @return true if full
 */bool cbufCheckFull(cbufHandle_t cbuf);

/// Returns the maximum capacity of the buffer

/**
 * Returns maxmimum buffer capacity (max number of uint8_t that could be stored)
 * @param cbuf handle of buffer of interest
 * @return size of buffer
 */
size_t cbufGetCapacity(cbufHandle_t cbuf);

/// Returns the current number of elements in the buffer
size_t cbufGetSize(cbufHandle_t cbuf);

#endif