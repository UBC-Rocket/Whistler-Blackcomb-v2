
#ifndef _HAL_H_
#define _HAL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#ifdef COMPILE_BOARD
#include "peripherals.h"
#elif defined(COMPILE_x86)
#include "MK66F18.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef COMPILE_x86
#define HAL_STATUS_SUCCESS kStatusSuccess

enum{
    kStatus_Success = 0,
};
#endif

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes hardware for board. Must be called at startup.
 */
void initHal();

void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority);

#endif
