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

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"


// Definitions
#define BOARD_LED_GPIO     BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_INITPINS_LED_BUILTIN_PIN

/* simple task */
static void MyTask(void *pv) {
    while (1){
   	 GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
   	 // Very important: Don't use normal delays in RTOS tasks, things will break
   	 vTaskDelay(pdMS_TO_TICKS(1000));
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

    // Copy the following to create a new task
    if (xTaskCreate(  /* create task */
        MyTask,  /* pointer to the task */
        "App", /* task name for kernel awareness debugging */
        200/sizeof(StackType_t), /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY+2,  /* initial priority */
        (TaskHandle_t*)NULL /* optional task handle to create */
      ) != pdPASS) {
       for(;;){} /* error! probably out of memory */
    }

     vTaskStartScheduler();

	while (1)
	{

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
