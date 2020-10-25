
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
 * Definitions
 ******************************************************************************/

enum sensors {
    uart_index_imu = 0, 
    uart_index_debug = 1, 
};

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

    printf("Starting IMU loop");

    /* Convenience variable to reduce repetition */
    const int ind = uart_index_imu;

    FILE *fp = fopen("../data/imu/imu_uart_data_3.txt", "r");

    while(1){
        vTaskDelay(pdMS_TO_TICKS(8));
        int i = 0;
        xSemaphoreTake(uart_handles[ind]->rxSemaphore, portMAX_DELAY);
        for(int i = 0; i < 40; ++i){
            if(uart_handles[ind]->buffer_size - uart_handles[ind]->cur_buffer_size <= 40)
                continue;
            if(!fscanf(fp, "%hhu ", &(uart_handles[ind]->buffer[uart_handles[ind]->cur_buffer_size]))){
                return;
            }
            uart_handles[0]->cur_buffer_size++;
        }
        xSemaphoreGive(uart_handles[ind]->rxSemaphore);

        xEventGroupSetBits(uart_handles[0]->rxEvent, HAL_UART_COMPLETE);
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
}

void stdioAssignUart(hal_uart_handle_t *handle){
    if((void*)handle->base == (void*)IMU_UART){
		uart_handles[0] = handle;
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
}