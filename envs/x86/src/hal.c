
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal.h"
#include "stdio_controller.h"

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void initHal(){
	stdioInit();
}

void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority){
    
}