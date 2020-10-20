#ifndef _HAL_UART_H_
#define _HAL_UART_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "MK66F18.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Struct for uart creation with rtos */
typedef struct _hal_uart_handle_t {
	UART_Type* base;
	uint32_t baudrate;
	uint8_t buffer[200];
	uint32_t buffer_size;
	uint32_t cur_buffer_size;

	SemaphoreHandle_t mutex;
} hal_uart_handle_t;

enum{
	kStatus_UART_RxRingBufferOverrun = 1, 
	kStatus_UART_RxHardwareOverrun = 2,
};

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes uart in the RTOS.
 * @param handle a pointer to the handle of the new uart instance to be created
 * @return kStatus_Success if success, otherwise fail
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
