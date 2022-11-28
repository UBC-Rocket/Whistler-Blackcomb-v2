
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_uart.h"
#include "fsl_uart_freertos.h"

/*******************************************************************************
 * Implementations
 ******************************************************************************/

int uartInit(hal_uart_handle_t *handle) {
	return UART_RTOS_Init(&handle->rtos_handle, &handle->uart_handle,
			&handle->rtos_config);
}

int uartDeinit(hal_uart_handle_t *handle){
	return UART_RTOS_Deinit(&handle->rtos_handle);
}

void uartConfig(hal_uart_handle_t *handle, UART_Type *base, uint32_t baudrate) {
	(handle->rtos_config).baudrate = baudrate;
	(handle->rtos_config).base = base;
	(handle->rtos_config).parity = kUART_ParityDisabled;
	(handle->rtos_config).stopbits = kUART_OneStopBit;
	(handle->rtos_config).buffer = handle->buffer;
	(handle->rtos_config).buffer_size = sizeof(handle->buffer);
	(handle->rtos_config).srcclk = UART_CLK_FREQ;
}

int uartSend(hal_uart_handle_t *handle, const uint8_t *buffer, uint32_t length){
	return UART_RTOS_Send(&handle->rtos_handle, buffer, length);
}

int uartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length,
		size_t *received){
	return UART_RTOS_Receive(&handle->rtos_handle, buffer, length, received);
}

int gpsUartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length, size_t *received) {
	return UART_RTOS_GPS_Receive(&handle->rtos_handle, buffer, length, received);
}


size_t uartRxUsed(hal_uart_handle_t *handle){
	return UART_TransferGetRxRingBufferLength(&handle->uart_handle);
}

size_t uartTxUsed(hal_uart_handle_t *handle){
	return (handle->uart_handle).txDataSizeAll;
}

void uartSetBaudrate(hal_uart_handle_t *handle, uint32_t baudrate){
	(handle->rtos_config).baudrate = baudrate;
}
