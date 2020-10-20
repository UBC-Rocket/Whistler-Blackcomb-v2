
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

#endif
