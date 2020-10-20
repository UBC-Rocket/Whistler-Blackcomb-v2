#ifndef _HAL_IO_H_
#define _HAL_IO_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/*
 * Toggles pin pointed to by the base and mask
 * @param base GPIO peripheral base pointer (GPIOA, GPIOB, GPIOC, and so on.)
 * @param mask GPIO pin number macro
 */
void digitalToggle(GPIO_Type *base, uint32_t mask);


#endif
