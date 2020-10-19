
#include "hal_uart.h"

int uartInit(hal_uart_handle_t *handle) {
	return 1;
}

int uartDeinit(hal_uart_handle_t *handle){
	return 1;
}

void uartConfig(hal_uart_handle_t *handle, UART_Type *base, uint32_t baudrate) {

}

int uartSend(hal_uart_handle_t *handle, const uint8_t *buffer, uint32_t length){
	return 1;
}

int uartReceive(hal_uart_handle_t *handle, uint8_t *buffer, uint32_t length,
		size_t *received){
	return 1;
}
