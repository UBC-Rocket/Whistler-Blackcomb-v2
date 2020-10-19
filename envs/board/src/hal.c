#include "hal.h"


void initHal(){
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
}

void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority){
	NVIC_SetPriority(IRQn, priority);
}
