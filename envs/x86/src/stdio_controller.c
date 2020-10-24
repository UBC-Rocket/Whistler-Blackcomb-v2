
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "stdio_controller.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "console.h"
#include "hal_uart.h"
#include "board.h"
#include "semphr.h"

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* TODO: update for all sensors */
static hal_uart_handle_t *uart_handles[1];

/*******************************************************************************
 * Declarations
 ******************************************************************************/

static void inputLoop(void * pv);
static void generateImuLoop(void * pv);

/*******************************************************************************
 * Implementations
 ******************************************************************************/

/**
 * Loop for interacting with ground station interface
 */
static void inputLoop(void *pv){
    for(;;){
        // console_print("stdio polling...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void generateImuLoop(void *pv){
    /* Useful for checking current working directory */
    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) 
    //     printf("Current working dir: %s\n", cwd);

    while(uart_handles[0]->base != IMU_UART)
        vTaskDelay(pdMS_TO_TICKS(1));

    FILE *fp = fopen("../data/imu/imu_uart_data_2.txt", "r");

    while(1){
        int i = 0;
        xSemaphoreTake(uart_handles[0]->rxSemaphore, portMAX_DELAY);
        while(i < 40){
            vTaskDelay(pdMS_TO_TICKS(10));
            if(uart_handles[0]->buffer_size - uart_handles[0]->cur_buffer_size <= 40)
                continue;
            if(!fscanf(fp, "%hhu", &(uart_handles[0]->buffer[uart_handles[0]->cur_buffer_size + i]))){
                return;
            }
            uart_handles[0]->cur_buffer_size++;
            i++;
        }
        xSemaphoreGive(uart_handles[0]->rxSemaphore);

        // printf("%hhu\n", uart_handles[0]->buffer[0]);
        // printf("fast");
    }
}

void stdioInit(){
    /* Thread safe output */
    console_init();

    /* For generation of normal threads if needed for testing */
    // pthread_t ioThread;
    // pthread_create( &ioThread, NULL, inputLoop, NULL);

    if (xTaskCreate( 
			inputLoop, 
			"stdio controller",
			200/sizeof(StackType_t),
			(void*)NULL,
			tskIDLE_PRIORITY+2,
			(TaskHandle_t*)NULL
			) != pdPASS) {
    	for(;;);
    }

    if (xTaskCreate( 
			generateImuLoop, 
			"imu generator controller",
			200/sizeof(StackType_t),
			(void*)NULL,
			tskIDLE_PRIORITY+2,
			(TaskHandle_t*)NULL
			) != pdPASS) {
    	for(;;);
    }
}

void stdioAssignUart(hal_uart_handle_t *handle){
    if((void*)handle->base == (void*)IMU_UART){
		uart_handles[0] = handle;
	}
}