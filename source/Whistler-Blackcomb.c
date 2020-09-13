/*
 * Main flight computer code
 * Right now this is pretty much just a blink sketch but more will be added
 */
 
/**
 * @file    Whistler-Blackcomb.c
 * @brief   Application entry point.
 */

// Includes
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
// Not sure why this wasn't required in blink sketch
#include "fsl_gpio.h"


// Definitions
#define BOARD_LED_GPIO     BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_INITPINS_LED_BUILTIN_PIN


// Variables
volatile uint32_t g_systickCounter;

// Functions
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /* Set systick reload value to generate 1ms interrupt */
	if (SysTick_Config(SystemCoreClock / 1000U))
	{
		while (1)
		{
		}
	}

	while (1)
	{
		/* Delay 1000 ms */
		SysTick_DelayTicks(1000U);
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
	}

//    printf("Hello World\n");
//
//    /* Force the counter to be placed into memory. */
//    volatile static int i = 0 ;
//    /* Enter an infinite loop, just incrementing a counter. */
//    while(1) {
//        i++ ;
//        /* 'Dummy' NOP to allow source level single stepping of
//            tight while() loop */
//        __asm volatile ("nop");
//    }
//    return 0 ;
}
