#ifndef _TIME_H_
#define _TIME_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_lptmr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Timer */
#define STARTUP_LPTMR_BASE   LPTMR0
#define STARTUP_LPTMR_IRQn   LPTMR0_IRQn
#define LPTMR_LED_HANDLER LPTMR0_IRQHandler
/* Get source clock for LPTMR driver */
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_LpoClk)
/* Define LPTMR microseconds counts value */
#define LPTMR_USEC_COUNT 1000000U

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes timers. Right now just initializes startup timer, if more are
 * added they will be initialized here. Only call once at startup.
 */
void initTimers();

/**
 * Gets time since startup. Requires initTimers() to be called at startup.
 * @return time since startup in microseconds.
 */
int timeSinceStartup();

#endif
