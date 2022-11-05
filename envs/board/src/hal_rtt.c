/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_rtt.h"

/*******************************************************************************
 * Implementations
 ******************************************************************************/

int rttSetup(void){
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
}

int rttWriteString(unsigned bufferIndex, const char* s){
	return SEGGER_RTT_WriteString(bufferIndex,s);
}