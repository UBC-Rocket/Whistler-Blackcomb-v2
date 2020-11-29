
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
#include <assert.h>

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
static void outputLoop(void * pv);
static void generateImuLoop(void * pv);

/*******************************************************************************
 * Implementations
 ******************************************************************************/

/**
 * Loop for interacting with ground station interface
 */
static void inputLoop(void *pv){
    /*handshake*/
    /*check for handshake acknowedgement*/
    char ack[4] = "ACK";
    char readChar;
    for (int i = 0; i < 3; i++) {
        scanf("%c",&readChar);
        assert(ack[i] == readChar);
    }
    FILE *logfile = fopen("log2.txt","w");
    fprintf(logfile,"input loop ran, ACK verified");
    fclose(logfile);
    
    //printf("Handshake Recieved!");
    for(;;){
        //console_print("stdio polling...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void outputLoop(void *pv){
    printf("SYN"); /*this has to be the first thing to go out, I think*/
    fflush(stdout);
    FILE *logfile = fopen("log.txt","w");
    fprintf(logfile,"output loop ran");
    fclose(logfile);
    for(;;){}
    
}

static void generateImuLoop(void *pv){
    /* Useful for checking current working directory */
    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) 
    //     printf("Current working dir: %s\n", cwd);

    //printf("Starting IMU loop");

    /* Convenience variable to reduce repetition */
    const int ind = uart_index_imu;

    FILE *fp = fopen("../data/imu/imu_uart_data_3.txt", "r");

    while(1){
        vTaskDelay(pdMS_TO_TICKS(8));
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
	  		outputLoop, 
	  		"stdio out controller",
	  		50000/sizeof(StackType_t),
	  		(void*)NULL,
	  		tskIDLE_PRIORITY+2,
	  		(TaskHandle_t*)NULL
	  		) != pdPASS) {
      	for(;;);
      }
     if (xTaskCreate( 
	 		inputLoop, 
	 	"stdio in controller",
		1000/sizeof(StackType_t),
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