
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "peripherals.h"

/*******************************************************************************
 * Implementations
 ******************************************************************************/


void initHal(){
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
}

void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority){
	NVIC_SetPriority(IRQn, priority);
}
