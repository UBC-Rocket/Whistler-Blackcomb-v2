/*
 * Main flight computer code
 */

/**
 * @file    Whistler-Blackcomb.c
 * @brief   Application entry point.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Pin definitions */
#include "board.h"
#include "pin_mux.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "variables.h"

/* Includes common between MCU and x86 */
#include "state_machine.h"
#include "IMU_interpret.h"
#include "prediction.h"

/* Includes specific to MCU or x86 */
#include "hal_time.h"
#include "hal.h"
#include "hal_io.h"
#include "hal_uart.h"
#include "hal_sd.h"

/* Radio Stuff */
#include "xbee/wpan.h"
#include "radio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Constants */
/* TODO: figure out where this is defined properly */
#define PI acos(-1)
#define EVER ;;

/* Task priorities. */
#define debug_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define imu_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define radio_task_PRIORITY (configMAX_PRIORITIES - 1)
#define log_task_PRIORITY (configMAX_PRIORITIES - 1)
#define state_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ReadImuTask(void *pvParameters);
static void BlinkTask(void *pv);
static void RadioTask(void *pv);
static void LogTask(void *pv);
static void StateTask(void *pv);

/*******************************************************************************
 * UART Variables
 ******************************************************************************/
const char *debug_intro_message = "";
const char *send_ring_overrun = "\r\nRing buffer overrun!\r\n";
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
IMU_1 IMU;

/*******************************************************************************
 * State Transition Variables
 ******************************************************************************/


/*******************************************************************************
 * Main
 ******************************************************************************/
int main(void)
{
	initHal();
	initTimers();

	halNvicSetPriority(DEBUG_UART_RX_TX_IRQn, 5);
	halNvicSetPriority(IMU_UART_RX_TX_IRQn, 5);
	halNvicSetPriority(RADIO_UART_RX_TX_IRQn, 5);

	BaseType_t error;

	/* Copy the following to create a new task */
	if (error = xTaskCreate( /* create task */
	BlinkTask, /* pointer to the task */
	"Blink Task", /* task name for kernel awareness debugging */
	200 / sizeof(StackType_t), /* task stack size */
	(void*) NULL, /* optional task startup argument */
	tskIDLE_PRIORITY + 2, /* initial priority */
	(TaskHandle_t*) NULL /* optional task handle_debug to create */
	) != pdPASS) {
		printf("Task init failed: %d\n", error);
		for (;;)
			; /* error! probably out of memory */
	}

	if (error = xTaskCreate(ReadImuTask, "IMU Task",
	configMINIMAL_STACK_SIZE + 300,
	NULL,
	debug_uart_task_PRIORITY,
	NULL) != pdPASS) {
		printf("Task init failed: %d\n", error);
		for (;;)
			;
	}

	if (error = xTaskCreate(RadioTask, "Radio Task",
	configMINIMAL_STACK_SIZE + 1000,
	NULL,
	radio_task_PRIORITY,
	NULL) != pdPASS) {
		printf("Task init failed: %d\n", error);
		for (;;)
			;
	}

	if (error = xTaskCreate(LogTask, "Log Task",
	configMINIMAL_STACK_SIZE + 500,
	NULL,
	log_task_PRIORITY,
	NULL) != pdPASS) {
		printf("Task init failed: %d\n", error);
		for (;;)
			;
	}
	if (error = xTaskCreate(StateTask, "State Task",
	configMINIMAL_STACK_SIZE + 500,
	NULL,
	state_task_PRIORITY,
	NULL) != pdPASS) {
		printf("Task init failed: %d\n", error);
		for (;;)
			;
	}

	vTaskStartScheduler();

	for (;;)
		;
}

/*******************************************************************************
 * Tasks
 ******************************************************************************/

/*!
 * @brief Simple blink Task as sanity check for program working
 */
static void BlinkTask(void *pv)
{
	while (1)
	{
		digitalToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		// Very important: Don't use normal delays in RTOS tasks, things will break
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

/*!
 * @brief Task responsible for IMU UART read and parse
 * Note: this whole function is a mess for now before we start actually
 * implementing library code. 
 */
static void ReadImuTask(void *pv)
{
	configImu(&IMU);
	crc32_initTable();

	int uart_error;
	size_t n = 0;

	int parse_error;
	//double * parsed_imu_data = (double*) pvPortMalloc(13 * sizeof(double));
	//unsigned char * statusBytes = (unsigned char*) pvPortMalloc(3 * sizeof(unsigned char));

	quaternion orientation = qUnit();
	// quaternion acceleration;

	// double * gravityAccel = (double*) pvPortMalloc(3 * sizeof(double));
	double *vecOrientation = (double *)pvPortMalloc(3 * sizeof(double));

	// Dummy covariance matrices for now since these are yet to be determined
	//	double stateCovariance[][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
	//	double processCovariance[2][2] = {{0, 0}, {0, 0}};

	// double * stateCovariance = (double*) pvPortMalloc(3 * 2 * 2 * sizeof(double));
	// double * processCovariance = (double*) pvPortMalloc(2 * 2 * sizeof(double));

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
	if (kStatus_Success != uartSend(&hal_uart_debug, (uint8_t *)debug_intro_message,
									strlen(debug_intro_message)))
	{
		vTaskSuspend(NULL);
	}

	/* Receive input from imu and parse it. */
	do
	{
		uart_error = uartReceive(&hal_uart_imu, IMU.datagram, 40, &n);
		if (uart_error == kStatus_UART_RxHardwareOverrun)
		{
			/* Notify about hardware buffer overrun */
			if (kStatus_Success != uartSend(&hal_uart_debug,
											(uint8_t *)send_hardware_overrun,
											strlen(send_hardware_overrun)))
			{
				vTaskSuspend(NULL);
			}
		}
		if (uart_error == kStatus_UART_RxRingBufferOverrun)
		{
			/* Notify about ring buffer overrun */
			if (kStatus_Success != uartSend(&hal_uart_debug, (uint8_t *)send_ring_overrun,
											strlen(send_ring_overrun)))
			{
				vTaskSuspend(NULL);
			}
		}
		if (n > 0 && IMU.datagram[0] == 0x93)
		{
			// Parse Datagram
			parse_error = interpretImuData(&IMU);

			int len;
			if (parse_error == DATAGRAM_PARSE_ID_MISMATCH)
				len = sprintf(toPrint, "ID_MISMATCH\n");
			else if (parse_error == DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK)
				len = sprintf(toPrint, "STATUS_BYTE_FAIL\n");
			else if (parse_error == DATAGRAM_INCORRECT_CRC)
				len = sprintf(toPrint, "CRC CHECK FAILED\n");
			else if (parse_error == DATAGRAM_PARSE_SUCCESS)
			{

				double gx = IMU.rate[0] * PI / 180;
				double gy = IMU.rate[1] * PI / 180;
				double gz = IMU.rate[2] * PI / 180;

				int cur_time = timeSinceStartup();

				// Get calculated orientation quaternion
				orientation = getOrientation(
					(cur_time - imu_last_time) / 1000000.0, orientation, gx,
					gy, gz);

				quaternion o;
				o.re = 0;
				o.i = vecOrientation[0];
				o.j = vecOrientation[1];
				o.k = vecOrientation[2];

				o = qMult(orientation, qMult(o, qConjugate(orientation)));

				len = sprintf(toPrint, "x: %3d, y: %3d, z: %3d, r: %3d\n",
							  (int)(o.i * 100), (int)(o.j * 100), (int)(o.k * 100),
							  (int)(100 * sqrt(o.i * o.i + o.j * o.j + o.k * o.k)));
				// len = sprintf(toPrint, "t = %d", cur_time - imu_last_time);

				imu_last_time = cur_time;
			}

			uartSend(&hal_uart_debug, (uint8_t *)toPrint, len);
		}
	} while (kStatus_Success == uart_error);

	uartDeinit(&hal_uart_debug);
	uartDeinit(&hal_uart_imu);
	vTaskSuspend(NULL);
}

/* Temporary, can be used to test right now before we get stdioController
 * working for radio */
//uint8_t radioPacket[] = { 0x7E, 0x00, 0x18, 0x90, 0x00, 0x13, 0xA2, 0x00, 0x41,
//		0x67, 0x8F, 0xC0, 0xFF, 0xFE, 0xC2, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20,
//		0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0xC7 };
/* Right now just echoes anything sent to the radio */
static void RadioTask(void *pv)
{

	xbee_dev_t radio;
	xbee_serial_t serial;

	uint8_t *packet = (uint8_t *)pvPortMalloc(256 * sizeof(uint8_t));

	serial.baudrate = 9600;
	uartConfig(&(serial.uart_handle), RADIO_UART, 9600);

	xbee_dev_init(&radio, &serial, NULL, NULL);
	/* Add this for x86 testing */
	// memcpy(&radio.serport.uart_handle.buffer, radioPacket, sizeof(radioPacket));
	// radio.serport.uart_handle.cur_buffer_size = sizeof(radioPacket);
	while (1) {
		int len = radioReceive(&radio, packet);

		if (len > 0)
		{
			radioTxRequest(&radio, packet, len);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void LogTask(void *pv) {
	HALFILE file;
	sdInit();
	for (EVER) {
		sdMkDir("/testdir");
		sdOpen(&file, "/testdir/testfile.txt");
		sdWrite(&file, "test data\n");
		sdClose(&file);
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

static void StateTask(void *pv) {	
	for(EVER){
		/*TODO: set all the state inputs...*/
		setNextState(&stateTransitonInput);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	

}
