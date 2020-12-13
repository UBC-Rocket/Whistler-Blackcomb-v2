#ifndef _HAL_UART_H_
#define _HAL_UART_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#ifdef COMPILE_BOARD
#include "fsl_uart_freertos.h"
#elif defined(COMPILE_x86)
#include "MK66F18.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include <stddef.h>
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * The way that this works is that the hal_uart_handle_t is defined differently
 * for both environments. This means the same handle represents different things
 * in each environment, so don't edit their properties directly (instead make
 * a new modifier function). 
 */

#ifdef COMPILE_BOARD
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

#elif defined(COMPILE_x86)

#define HAL_UART_COMPLETE 0b00000001
#define HAL_UART_RING_BUFFER_OVERRUN 0b00000010
#define HAL_UART_HARDWARE_BUFFER_OVERRUN 0b00000100

/* Struct for uart creation with rtos */
typedef struct _hal_uart_handle_t {
	UART_Type* base;
	uint32_t baudrate;
	uint8_t buffer[200];
	uint32_t buffer_size;
	uint32_t cur_buffer_size;

	/* FreeRTOS stuff. Most of this was taken from uart_freertos handle 
	 * implementation
	 */
	SemaphoreHandle_t rxSemaphore;
	EventGroupHandle_t rxEvent;
    EventGroupHandle_t txEvent;
} hal_uart_handle_t;

enum{
	kStatus_UART_RxRingBufferOverrun = 1, 
	kStatus_UART_RxHardwareOverrun = 2,
};
#endif

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

/**
 * Gets the number of bytes in the rx buffer. 
 * @param handle a pointer to the uart instance struct
 * @return the number of bytes in the rx buffer at the moment
 */
size_t uartRxUsed(hal_uart_handle_t *handle);

/**
 * Gets the number of bytes in the tx buffer. 
 * @param handle a pointer to the uart instance struct
 * @return the number of bytes in the tx buffer at the moment
 */
size_t uartTxUsed(hal_uart_handle_t *handle);

/**
 * Sets the baud rate for the given handle. Must be called before uartInit(). 
 * @param handle pointer to the handle in question
 * @param baudrate the baudrate to set the configuration to
 */
void uartSetBaudrate(hal_uart_handle_t *handle, uint32_t baudrate);

#endif
