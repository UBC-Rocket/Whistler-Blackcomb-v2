
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_uart.h"
#include "hal.h"
#include "stdio_controller.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "board.h"
#include "projdefs.h"

#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * Implementations
 ******************************************************************************/

int uartInit(hal_uart_handle_t *handle) {
	handle->rxSemaphore = xSemaphoreCreateMutex();
	handle->rxEvent = xEventGroupCreate();
	handle->txEvent = xEventGroupCreate();
	stdioAssignUart(handle);
	return kStatus_Success;
}

int uartDeinit(hal_uart_handle_t *handle){
	return 0;
}

void uartConfig(hal_uart_handle_t *handle, UART_Type *base, uint32_t baudrate) {
	handle->base = base;
	handle->baudrate = baudrate;
	handle->buffer_size = sizeof(handle->buffer);
	handle->cur_buffer_size = 0;
}

int uartSend(hal_uart_handle_t *handle, const uint8_t *buffer, uint32_t length){
	for(unsigned int i = 0; i < length; ++i){
		// printf("%02hhX ", buffer[i]);
		printf("%c", buffer[i]);
	}
	// printf("\n");
	return kStatus_Success;
}

int uartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length,
		size_t *received){
	if(handle->cur_buffer_size < length)
		xEventGroupWaitBits(handle->rxEvent, 
				HAL_UART_COMPLETE | HAL_UART_RING_BUFFER_OVERRUN 
				| HAL_UART_HARDWARE_BUFFER_OVERRUN,
				pdTRUE, pdFALSE, portMAX_DELAY);

	memcpy(buffer, &(handle->buffer[0]), length);
	memcpy(&(handle->buffer[0]), &(handle->buffer[length]), 
			handle->buffer_size - length);
	handle->cur_buffer_size -= length;
	*received = length;

	return kStatus_Success;
}

size_t uartRxUsed(hal_uart_handle_t *handle){
	return handle->cur_buffer_size;
}

size_t uartTxUsed(hal_uart_handle_t *handle){
	/* Since this serial port is only being emulated it just returns 0 */
	return 0;
}

void uartSetBaudrate(hal_uart_handle_t *handle, uint32_t baudrate){
	handle->baudrate = baudrate;
}
