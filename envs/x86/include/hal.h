
#ifndef _HAL_H_
#define _HAL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "MK66F18.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define HAL_STATUS_SUCCESS kStatusSuccess

enum{
    kStatus_Success = 0,
};

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes hardware for board. Must be called at startup.
 */
void initHal();

void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority);

#endif
