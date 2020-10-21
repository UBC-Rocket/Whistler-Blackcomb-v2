/*
 * Main flight computer code
 * Right now this is pretty much just a blink sketch but more will be added
 * UPDATE 2020/10/20: Now it's a blink sketch that prints hello world and you might be able to plug an IMU into.
 */

/**
 * @file    Whistler-Blackcomb.c
 * @brief   Application entry point.
 */

#include "env_config.h"

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef x86_compilation
#endif

// Pin definitions
#include "board.h"
#include "pin_mux.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Includes common between MCU and x86 */
#include "IMU_interpret.h"
#include "prediction.h"

/* Includes specific to MCU or x86 */
#include <hal_time.h>
#include "hal.h"
#include "hal_io.h"
#include "hal_uart.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Constants */
#define PI acos(-1)

/* Blink */
#define BOARD_LED_GPIO     BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_INITPINS_LED_BUILTIN_PIN

/* FreeRTOS UART Debug */
#define DEBUG_UART            UART1
#define DEBUG_UART_CLKSRC     SYS_CLK
#define DEBUG_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define DEBUG_UART_RX_TX_IRQn UART1_RX_TX_IRQn

/* FreeRTOS UART IMU */
#define IMU_UART            UART0
#define IMU_UART_CLKSRC     SYS_CLK
#define IMU_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define IMU_UART_RX_TX_IRQn UART0_RX_TX_IRQn


/* Task priorities. */
#define debug_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define imu_uart_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ReadImuTask(void *pvParameters);
static void BlinkTask(void *pv);

/*******************************************************************************
 * UART Variables
 ******************************************************************************/
const char *debug_intro_message               = "Debug session started\n";
const char *send_ring_overrun     = "\r\nRing buffer overrun!\r\n";
const char *send_hardware_overrun = "\r\nHardware buffer overrun!\r\n";
char toPrint[100];
uint8_t background_buffer_debug[200];
uint8_t background_buffer_imu[200];

hal_uart_handle_t hal_uart_debug;
hal_uart_handle_t hal_uart_imu;

/*******************************************************************************
 * Positioning Variables
 ******************************************************************************/

double position[] = {0, 0, 0};
double velocity[] = {0, 0, 0};
double accel[] = {0, 0, 0};

/*******************************************************************************
 * Main
 ******************************************************************************/
/*
 * @brief   Application entry point.
 */
int main(void) {

	initHal();
    initTimers();

    halNvicSetPriority(DEBUG_UART_RX_TX_IRQn, 5);
    halNvicSetPriority(IMU_UART_RX_TX_IRQn, 5);

    /* Copy the following to create a new task */
    if (xTaskCreate(  /* create task */
			BlinkTask,  /* pointer to the task */
			"Blink Task", /* task name for kernel awareness debugging */
			200/sizeof(StackType_t), /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY+2,  /* initial priority */
			(TaskHandle_t*)NULL /* optional task handle_debug to create */
			) != pdPASS) {
    	for(;;); /* error! probably out of memory */
    }

    if (xTaskCreate(
    		ReadImuTask,
			"UART Task",
			configMINIMAL_STACK_SIZE + 100,
			NULL,
			debug_uart_task_PRIORITY,
			NULL
			) != pdPASS){
    	for(;;);
	}

     vTaskStartScheduler();

	for(;;);
}

/*******************************************************************************
 * Tasks
 ******************************************************************************/

/*!
 * @brief Simple blink Task as sanity check for program working
 */
static void BlinkTask(void *pv) {
    while (1){
		digitalToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		printf( "Hello world\n" );
		// Very important: Don't use normal delays in RTOS tasks, things will break
		vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*!
 * @brief Task responsible for IMU UART read and parse
 * Note: much of this was based from freertos_uart example
 */
static void ReadImuTask(void *pv)
{
	struct IMUStruct IMU;

    int uart_error;
    size_t n = 0;

    int parse_error;

	/* TODO: change sizes to match IMU config automatically */
	uint8_t * imu_datagram = (uint8_t*) pvPortMalloc(100 * sizeof(uint8_t));
	//double * parsed_imu_data = (double*) pvPortMalloc(13 * sizeof(double));
	//unsigned char * statusBytes = (unsigned char*) pvPortMalloc(3 * sizeof(unsigned char));

	quaternion orientation = qUnit();
	quaternion acceleration;

	double * gravityAccel = (double*) pvPortMalloc(3 * sizeof(double));
	double * vecOrientation = (double*) pvPortMalloc(3 * sizeof(double));

	// Dummy covariance matrices for now since these are yet to be determined
//	double stateCovariance[][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
//	double processCovariance[2][2] = {{0, 0}, {0, 0}};

	double * stateCovariance = (double*) pvPortMalloc(3 * 2 * 2 * sizeof(double));
	double * processCovariance = (double*) pvPortMalloc(2 * 2 * sizeof(double));

	vecOrientation[0] = 0;
	vecOrientation[1] = 0;
	vecOrientation[2] = 1;


	uartConfig(&hal_uart_debug, DEBUG_UART, 374400);
	uartConfig(&hal_uart_imu, IMU_UART, 374400);

	unsigned long imu_last_time = 0;

	/* Initialize UART interfaces. */
    if (kStatus_Success != uartInit(&hal_uart_debug))
    {
        vTaskSuspend(NULL);
    }

    if (kStatus_Success != uartInit(&hal_uart_imu))
	{
		vTaskSuspend(NULL);
	}

    /* Send introduction message. */
    if (kStatus_Success != uartSend(&hal_uart_debug, (uint8_t *)debug_intro_message, strlen(debug_intro_message)))//UART_RTOS_Send(&handle_debug, (uint8_t *)debug_intro_message, strlen(debug_intro_message)))
    {
        vTaskSuspend(NULL);
    }

    /* Receive input from imu and parse it. */
    do
    {
        uart_error = uartReceive(&hal_uart_imu, IMU.datagram, 40, &n);//UART_RTOS_Receive(&handle_imu, imu_datagram, 40, &n);
        if (uart_error == kStatus_UART_RxHardwareOverrun)
        {
            /* Notify about hardware buffer overrun */
            if (kStatus_Success !=
                uartSend(&hal_uart_debug, (uint8_t *)send_hardware_overrun, strlen(send_hardware_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (uart_error == kStatus_UART_RxRingBufferOverrun)
        {
            /* Notify about ring buffer overrun */
            if (kStatus_Success != uartSend(&hal_uart_debug, (uint8_t *)send_ring_overrun, strlen(send_ring_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (n > 0 && IMU.datagram[0] == 0x93)
        {
        	// Parse Datagram
			parse_error = interpretImuData(&IMU);

			int len;
			if(parse_error == DATAGRAM_PARSE_ID_MISMATCH)
				len = sprintf(toPrint, "ID_MISMATCH\n");
			else if(parse_error == DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK)
				len = sprintf(toPrint, "STATUS_BYTE_FAIL\n");
			else if(parse_error == DATAGRAM_PARSE_SUCCESS){

				double gx = IMU.rate[0] * PI / 180;
				double gy = IMU.rate[1] * PI / 180;
				double gz = IMU.rate[2] * PI / 180;


				int cur_time = timeSinceStartup();

				// Get calculated orientation quaternion
				orientation = getOrientation((cur_time - imu_last_time) / 1000000.0, orientation, gx, gy, gz);

				quaternion o;
				o.re = 0;
				o.i = vecOrientation[0];
				o.j = vecOrientation[1];
				o.k = vecOrientation[2];

				o = qMult(orientation, qMult(o, qConjugate(orientation)));

				len = sprintf(toPrint, "x: %3d, y: %3d, z: %3d, r: %3d\n",
						(int)(o.i*100),
						(int)(o.j*100),
						(int)(o.k*100),
						(int)(100*sqrt(o.i*o.i+o.j*o.j+o.k*o.k)));

				imu_last_time = cur_time;
			}

			uartSend(&hal_uart_debug, (uint8_t *)toPrint, len);

        }
    } while (kStatus_Success == uart_error);

    uartDeinit(&hal_uart_debug);
    uartDeinit(&hal_uart_imu);
    vTaskSuspend(NULL);
}



