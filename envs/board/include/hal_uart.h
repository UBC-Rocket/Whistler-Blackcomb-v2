#ifndef _HAL_UART_H_
#define _HAL_UART_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_uart_freertos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Struct for uart creation with rtos */
typedef struct _hal_uart_handle_t {
	uart_rtos_handle_t rtos_handle;
	uart_handle_t uart_handle;
	uart_rtos_config_t rtos_config;
	/* TODO: Figure out how big this buffer should actually be */
	/* Might need to adjust it in which case it shouldn't be stored here and
	 * instead should be allocated dynamically. */
	uint8_t buffer[200];
} hal_uart_handle_t;

#define UART_CLKSRC     SYS_CLK
#define UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)



/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes uart in the RTOS.
 * @param handle a pointer to the handle of the new uart instance to be created
 * @return kStatusSuccess if success, otherwise fail
 */
int uartInit(hal_uart_handle_t *handle);

/**
 * Deinitializes handle to uart
 * @param handle the handle to deinitialize
 * @return 0 on success, else fail
 */
int uartDeinit(hal_uart_handle_t *handle);

/**
 * Configures handle specified baud rate and base, with default configuration
 * for the rest. The default parameters for the rtos_confighandle are as
 * follows:
 *
 * parity = kUART_ParityDisabled
 * stopbits = kUART_OneStopBit
 * buffer = &handle->buffer
 * buffer_size = sizeof(handle->buffer)
 * srcclk = UART_CLK_FREQ
 *
 * @param handle a pointer to the handle of the uart config to change
 * @param base a pointer to the UART instance in question
 * @param baud rate to be set to
 */
void uartConfig(hal_uart_handle_t *handle, UART_Type *base, uint32_t baudrate);

/**
 * Sends data to uart instance.
 * @param handle a pointer to the uart instance struct
 * @param buffer array of bytes to send
 * @param length of the buffer to send
 * @return kStatus_Success on success, otherwise fail
 */
int uartSend(hal_uart_handle_t *handle, const uint8_t *buffer, uint32_t length);

/**
 * Receives data from uart instance
 * @param handle a pointer to the uart instance struct
 * @param buffer array of bytes receive to
 * @param number of bytes to receive (should be <= to size of buffer)
 * @param received pointer to where number of bytes read will be written
 * @return kStatus_Success on success, otherwise fail
 */
int uartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length,
		size_t *received);

#endif
