
#ifndef _HAL_H_
#define _HAL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "peripherals.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes hardware for board. Must be called at startup.
 */
void initHal();

/**
 * Sets priority for an interrupt. Really just a wrapper for NVIC_SetPriority
 * @param IRQn interrupt to set
 * @param priority what to set it to
 */
void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority);

#endif
