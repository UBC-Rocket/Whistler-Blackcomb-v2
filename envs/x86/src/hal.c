
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal.h"
#include "stdio_controller.h"
#include <stdio.h>

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void initHal(){
	stdioInit();
	setbuf(stdout, NULL);
}

void halNvicSetPriority(IRQn_Type IRQn, uint32_t priority){
    
}