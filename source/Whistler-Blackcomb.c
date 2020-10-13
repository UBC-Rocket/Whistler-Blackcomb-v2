/*
 * Main flight computer code
 * Right now this is pretty much just a blink sketch but more will be added
 */

/**
 * @file    Whistler-Blackcomb.c
 * @brief   Application entry point.
 */

/* Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_gpio.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_uart_freertos.h"
#include "fsl_uart.h"
#include "fsl_lptmr.h"


#include "IMU_interpret.h"
#include "prediction.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Constants */
#define PI acos(-1)

/* Blink */
#define BOARD_LED_GPIO     BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_INITPINS_LED_BUILTIN_PIN

/* Timer */
#define STARTUP_LPTMR_BASE   LPTMR0
#define STARTUP_LPTMR_IRQn   LPTMR0_IRQn
#define LPTMR_LED_HANDLER LPTMR0_IRQHandler
/* Get source clock for LPTMR driver */
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_LpoClk)
/* Define LPTMR microseconds counts value */
#define LPTMR_USEC_COUNT 1000000U

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

uart_rtos_handle_t handle_debug;
struct _uart_handle t_handle_debug;

uart_rtos_handle_t handle_imu;
struct _uart_handle t_handle_imu;

uart_rtos_config_t debug_uart_config = {
    .baudrate    = 374400,
    .parity      = kUART_ParityDisabled,
    .stopbits    = kUART_OneStopBit,
    .buffer      = background_buffer_debug,
    .buffer_size = sizeof(background_buffer_debug),
};

uart_rtos_config_t imu_uart_config = {
    .baudrate    = 374400,
    .parity      = kUART_ParityDisabled,
    .stopbits    = kUART_OneStopBit,
    .buffer      = background_buffer_imu,
    .buffer_size = sizeof(background_buffer_imu),
};

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
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /* Configure LPTMR */
    lptmr_config_t lptmrConfig;
    LPTMR_GetDefaultConfig(&lptmrConfig);
	/* Initialize the LPTMR */
	LPTMR_Init(STARTUP_LPTMR_BASE, &lptmrConfig);

	/* Sets the timer length to extremely high to not expire */
	LPTMR_SetTimerPeriod(STARTUP_LPTMR_BASE, USEC_TO_COUNT((100000000), LPTMR_SOURCE_CLOCK));
	LPTMR_StartTimer(STARTUP_LPTMR_BASE);

    NVIC_SetPriority(DEBUG_UART_RX_TX_IRQn, 5);
    NVIC_SetPriority(IMU_UART_RX_TX_IRQn, 5);

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
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
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
    int uart_error;
    size_t n = 0;

    int parse_error;

	/* TODO: change sizes to match IMU config automatically */
	uint8_t * imu_datagram = (uint8_t*) pvPortMalloc(100 * sizeof(uint8_t));
	double * parsed_imu_data = (double*) pvPortMalloc(13 * sizeof(double));
	unsigned char * statusBytes = (unsigned char*) pvPortMalloc(3 * sizeof(unsigned char));

	quaternion orientation = qUnit();;
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

    debug_uart_config.srcclk = DEBUG_UART_CLK_FREQ;
    debug_uart_config.base   = DEBUG_UART;

    imu_uart_config.srcclk = IMU_UART_CLK_FREQ;
	imu_uart_config.base   = IMU_UART;

	unsigned long imu_last_time = 0;

	/* Initialize UART interfaces. */
    if (kStatus_Success != UART_RTOS_Init(&handle_debug, &t_handle_debug, &debug_uart_config))
    {
        vTaskSuspend(NULL);
    }

    if (kStatus_Success != UART_RTOS_Init(&handle_imu, &t_handle_imu, &imu_uart_config))
	{
		vTaskSuspend(NULL);
	}

    /* Send introduction message. */
    if (kStatus_Success != UART_RTOS_Send(&handle_debug, (uint8_t *)debug_intro_message, strlen(debug_intro_message)))
    {
        vTaskSuspend(NULL);
    }

    /* Receive input from imu and parse it. */
    do
    {
        uart_error = UART_RTOS_Receive(&handle_imu, imu_datagram, 40, &n);
        if (uart_error == kStatus_UART_RxHardwareOverrun)
        {
            /* Notify about hardware buffer overrun */
            if (kStatus_Success !=
                UART_RTOS_Send(&handle_debug, (uint8_t *)send_hardware_overrun, strlen(send_hardware_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (uart_error == kStatus_UART_RxRingBufferOverrun)
        {
            /* Notify about ring buffer overrun */
            if (kStatus_Success != UART_RTOS_Send(&handle_debug, (uint8_t *)send_ring_overrun, strlen(send_ring_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (n > 0 && imu_datagram[0] == 0x93)
        {
        	// Parse Datagram
			parse_error = interpretImuData(imu_datagram, 0x93, parsed_imu_data, statusBytes);

			int len;
			if(parse_error == DATAGRAM_PARSE_ID_MISMATCH)
				len = sprintf(toPrint, "ID_MISMATCH\n");
			else if(parse_error == DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK)
				len = sprintf(toPrint, "STATUS_BYTE_FAIL\n");
			else if(parse_error == DATAGRAM_PARSE_SUCCESS){

				double gx = parsed_imu_data[0] * PI / 180;
				double gy = parsed_imu_data[1] * PI / 180;
				double gz = parsed_imu_data[2] * PI / 180;


				int cur_time = COUNT_TO_USEC(LPTMR_GetCurrentTimerCount(STARTUP_LPTMR_BASE), LPTMR_SOURCE_CLOCK);

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

			UART_RTOS_Send(&handle_debug, (uint8_t *)toPrint, len);

        }
    } while (kStatus_Success == uart_error);

    UART_RTOS_Deinit(&handle_debug);
    vTaskSuspend(NULL);
}



