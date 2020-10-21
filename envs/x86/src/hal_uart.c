
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_uart.h"
#include "hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "board.h"

#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * Implementations
 ******************************************************************************/

int uartInit(hal_uart_handle_t *handle) {
	handle->mutex = xSemaphoreCreateMutex();
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
	if(handle->base == DEBUG_UART)
		printf("%s", buffer);
	return kStatus_Success;
}

int uartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length,
		size_t *received){
	while(handle->cur_buffer_size <= length)
		vTaskDelay(pdMS_TO_TICKS(1));

	memcpy(buffer, &(handle->buffer[0]), length);
	memcpy(&(handle->buffer[0]), &(handle->buffer[length+1]), handle->buffer_size - length);
	handle->cur_buffer_size -= length;

	return kStatus_Success;
}
