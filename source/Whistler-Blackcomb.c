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
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
/* Not sure why this wasn't required in blink sketch */
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


/*******************************************************************************
 * Definitions
 ******************************************************************************/
// Blink
#define BOARD_LED_GPIO     BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_INITPINS_LED_BUILTIN_PIN

// Debug FreeRTOS UART
#define DEBUG_UART            UART0
#define DEBUG_UART_CLKSRC     SYS_CLK
#define DEBUG_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define DEBUG_UART_RX_TX_IRQn UART0_RX_TX_IRQn

// IMU FreeRTOS UART
#define IMU_UART            UART0
#define IMU_UART_CLKSRC     SYS_CLK
#define IMU_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define IMU_UART_RX_TX_IRQn UART0_RX_TX_IRQn


/* Task priorities. */
#define read_imu_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void read_imu_task(void *pvParameters);
static void BlinkTask(void *pv);

/*******************************************************************************
 * Variables
 ******************************************************************************/
const char *to_send               ="Debug session starting...";
const char *send_ring_overrun     = "\r\nRing buffer overrun!\r\n";
const char *send_hardware_overrun = "\r\nHardware buffer overrun!\r\n";
uint8_t background_buffer[200];
uint8_t recv_buffer[40];

uart_rtos_handle_t handle_debug;
struct _uart_handle t_handle_debug;

uart_rtos_handle_t handle_imu;
struct _uart_handle t_handle_imu;

uart_rtos_config_t debug_uart_config = {
    .baudrate    = 921600,
    .parity      = kUART_ParityDisabled,
    .stopbits    = kUART_OneStopBit,
    .buffer      = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

uart_rtos_config_t imu_uart_config = {
    .baudrate    = 921600,
    .parity      = kUART_ParityDisabled,
    .stopbits    = kUART_OneStopBit,
    .buffer      = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

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

    NVIC_SetPriority(DEBUG_UART_RX_TX_IRQn, 5);
//    NVIC_SetPriority(IMU_UART_RX_TX_IRQn, 5);

    // Copy the following to create a new task
    if (xTaskCreate(  /* create task */
			BlinkTask,  /* pointer to the task */
			"App", /* task name for kernel awareness debugging */
			200/sizeof(StackType_t), /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY+2,  /* initial priority */
			(TaskHandle_t*)NULL /* optional task handle to create */
			) != pdPASS) {
    	for(;;); /* error! probably out of memory */
    }

    if (xTaskCreate(
    		read_imu_task,
			"Uart_task",
			configMINIMAL_STACK_SIZE + 100,
			NULL,
			read_imu_task_PRIORITY,
			NULL
			) != pdPASS){
    	for(;;);
	}

     vTaskStartScheduler();

	while (1)
	{

	}
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
 * @brief Task responsible for UART read and write
 * Note: was taken from freertos_uart example
 */
static void read_imu_task(void *pvParameters)
{
    int error;
    size_t n = 0;

    debug_uart_config.srcclk = DEBUG_UART_CLK_FREQ;
    debug_uart_config.base   = DEBUG_UART;

    imu_uart_config.srcclk = IMU_UART_CLK_FREQ;
	imu_uart_config.base   = IMU_UART;

//    if (kStatus_Success != UART_RTOS_Init(&handle_imu, &t_handle_imu, &imu_uart_config))
//    {
//        vTaskSuspend(NULL);
//    }

    if (kStatus_Success != UART_RTOS_Init(&handle_debug, &t_handle_debug, &debug_uart_config))
	{
		vTaskSuspend(NULL);
	}

    /* Send introduction message. */
    if (kStatus_Success != UART_RTOS_Send(&handle_debug, (uint8_t *)to_send, strlen(to_send)))
    {
        vTaskSuspend(NULL);
    }

    /* Receive user input and send it back to terminal. */
    do
    {
        error = UART_RTOS_Receive(&handle_debug, recv_buffer, sizeof(recv_buffer), &n);
        if (error == kStatus_UART_RxHardwareOverrun)
        {
            /* Notify about hardware buffer overrun */
            if (kStatus_Success !=
                UART_RTOS_Send(&handle_debug, (uint8_t *)send_hardware_overrun, strlen(send_hardware_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (error == kStatus_UART_RxRingBufferOverrun)
        {
            /* Notify about ring buffer overrun */
            if (kStatus_Success != UART_RTOS_Send(&handle_debug, (uint8_t *)send_ring_overrun, strlen(send_ring_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (n > 0)
        {
            /* send back the received data */
            UART_RTOS_Send(&handle_debug, (uint8_t *)recv_buffer, n);
        }
    } while (kStatus_Success == error);

    UART_RTOS_Deinit(&handle_debug);
    UART_RTOS_Deinit(&handle_imu);
    vTaskSuspend(NULL);
}



