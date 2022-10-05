#ifndef _HAL_RTT_H_
#define _HAL_RTT_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#ifdef COMPILE_BOARD
#include "SEGGER_RTT.h"
#elif defined(COMPILE_x86)
#include <stdio.h>
#endif


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Sets up RTT
 * 
 * @return >= 0 - OK, < 0 - Error
 */
int rttSetup(void);


/**
* Stores string in SEGGER RTT control block. This data is read by the host. On
* x86, merely prints it to stderr. ONLY FOR DEBUGGING AND DEV.
*
* @param BufferIndex Index of "Up"-buffer to be used (e.g. 0 for "Terminal").
* @param s Pointer to string.

 */
int rttWriteString(unsigned bufferIndex, const char* s);

#endif