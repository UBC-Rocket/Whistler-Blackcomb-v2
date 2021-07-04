
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

/**
 * Gets sim id from a uart handle
 * @param handle the handle to check the sim packet id for
 * @return the sim packet id
 */
static unsigned char getSimId(hal_uart_handle_t *handle){
	UART_Type* base = handle->base;
	unsigned char ret;
	/* TODO: Implement the rest once they get assigned a letter */
	if (base == RADIO_UART){
		ret = 'R';
	} else {
		printf("You're trying to read from a uart line that's not associated with a sim packet, please add it in hal_uart.c. \n");
		ret = 0;
	}
	return ret;
}

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
	writeToBuf(TX, buffer, getSimId(handle), length);
	return kStatus_Success;
}

int uartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length,
		size_t *received){

	xSemaphoreTake(handle->rxSemaphore, portMAX_DELAY);

	*received = 0;
	do{
		if(!handle->cur_buffer_size){
			char id = getSimId(handle);
			handle->cur_buffer_size = readFromBuf(RX, handle->buffer, id);
		}

		/* Gets minimum between buffer size and amount you want to copy */
		int copyAmount = length - *received > handle->cur_buffer_size ? handle->cur_buffer_size : length - *received;
		/* Shifts the buffer */
		memcpy(buffer + *received, &(handle->buffer[0]), copyAmount);
		memcpy(&(handle->buffer[0]), &(handle->buffer[copyAmount]), 
				handle->buffer_size - copyAmount);
		handle->cur_buffer_size -= copyAmount;
		*received += copyAmount;
	} while (*received < length);


	xSemaphoreGive(handle->rxSemaphore);

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
