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
#include "hal_can.h"

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

/* Task priorities. Will update once more finalized */
#define debug_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define imu_uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define radio_task_PRIORITY (configMAX_PRIORITIES - 1)
#define log_task_PRIORITY (configMAX_PRIORITIES - 1)
#define state_task_PRIORITY (configMAX_PRIORITIES - 1)
#define can_task_PRIORITY (configMAX_PRIORITIES - 1)
#define gps_task_PRIORITY (configMAX_PRIORITIES - 1)

#define log(msg...) { printf(msg); printf("\n"); }

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ReadImuTask(void *pvParameters);
static void BlinkTask(void *pv);
static void RadioTask(void *pv);
static void LogTask(void *pv);
static void StateTask(void *pv);
static void CanTask(void *pv);
static void GpsTask(void *pv);

/*******************************************************************************
 * UART Variables
 ******************************************************************************/
const char *debug_intro_message = "";
const char *gps_message = "ECHO THISPORT ON\n";
const char *send_ring_overrun = "\r\nRing buffer overrun!\r\n";
const char *send_hardware_overrun = "\r\nHardware buffer overrun!\r\n";
char toPrint[100];
uint8_t background_buffer_debug[200];
uint8_t background_buffer_imu[200];

hal_uart_handle_t hal_uart_debug;
hal_uart_handle_t hal_uart_imu;
hal_uart_handle_t hal_uart_gps;

/*******************************************************************************
 * Positioning Variables
 ******************************************************************************/

double position[] = { 0, 0, 0 };
double velocity[] = { 0, 0, 0 };
double accel[] = { 0, 0, 0 };
IMU_1 IMU;

/*******************************************************************************
 * DAQ Variables
 ******************************************************************************/
char *tc_names[] = { "HP Tank Outlet TC", "LOX Fill TC", "LOX Bleed TC" };
char *pt_names[] = { "HP Tank PT", "Fuel Tank PT", "Fuel Injector PT",
		"LOX Tank PT", "LOX Injector PT", "Igniter Chamber PT",
		"IG GOX Tank PT", "IG Fuel Tank PT" };
/* Thermocouple data in C */
float tc_data[sizeof(tc_names)];
/* Pressure transducer data in kPa */
float pt_data[sizeof(pt_names)];

/*******************************************************************************
 * State Transition Variables
 ******************************************************************************/

stateInput_t stateTransitonInput = { 0 };

/*******************************************************************************
 * Main
 ******************************************************************************/
int main(void) {
	initHal();
	initTimers();

	halNvicSetPriority(DEBUG_UART_RX_TX_IRQn, 5);
	halNvicSetPriority(IMU_UART_RX_TX_IRQn, 5);
	halNvicSetPriority(GPS_UART_RX_TX_IRQn, 5);
	/* Have to set these or else semaphore functions in interrupts won't work */
	halNvicSetPriority(CAN0_ORed_Message_buffer_IRQn, 5);
	halNvicSetPriority(CAN1_ORed_Message_buffer_IRQn, 5);

	BaseType_t error;

	/* Copy the following to create a new task */
	if ((error = xTaskCreate( /* create task */
	BlinkTask, /* pointer to the task */
	"Blink Task", /* task name for kernel awareness debugging */
	200 / sizeof(StackType_t), /* task stack size */
	(void*) NULL, /* optional task startup argument */
	tskIDLE_PRIORITY + 2, /* initial priority */
	(TaskHandle_t*) NULL /* optional task handle_debug to create */
	)) != pdPASS) {
		printf("Task init failed: %ld\n", error);
		for (;;)
			; /* error! probably out of memory */
	}

	if ((error = xTaskCreate(ReadImuTask, "IMU Task",
	configMINIMAL_STACK_SIZE + 300,
	NULL,
	debug_uart_task_PRIORITY,
	NULL)) != pdPASS) {
		printf("Task init failed: %ld\n", error);
		for (;;)
			;
	}

	if ((error = xTaskCreate(RadioTask, "Radio Task",
	configMINIMAL_STACK_SIZE + 1000,
	NULL,
	radio_task_PRIORITY,
	NULL)) != pdPASS) {
		printf("Task init failed: %ld\n", error);
		for (;;)
			;
	}

	if ((error = xTaskCreate(LogTask, "Log Task",
	configMINIMAL_STACK_SIZE + 500,
	NULL,
	log_task_PRIORITY,
	NULL)) != pdPASS) {
		printf("Task init failed: %ld\n", error);
		for (;;)
			;
	}
	if ((error = xTaskCreate(StateTask, "State Task",
	configMINIMAL_STACK_SIZE + 500,
	NULL,
	state_task_PRIORITY,
	NULL)) != pdPASS) {
		printf("Task init failed: %ld\n", error);
		for (;;)
			;
	}
	if ((error = xTaskCreate(CanTask, "Can Task",
	configMINIMAL_STACK_SIZE + 500,
	NULL,
	can_task_PRIORITY,
	NULL)) != pdPASS) {
		printf("Task init failed: %ld\n", error);
		for (;;)
			;
	}
	if ((error = xTaskCreate(GpsTask, "GPS Task",
		configMINIMAL_STACK_SIZE + 500,
		NULL,
		gps_task_PRIORITY,
		NULL)) != pdPASS) {
			printf("Task init failed: %ld\n", error);
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
static void BlinkTask(void *pv) {
	while (1) {
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
static void ReadImuTask(void *pv) {
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
	double *vecOrientation = (double*) pvPortMalloc(3 * sizeof(double));

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
	if (kStatus_Success != uartInit(&hal_uart_debug)) {
		vTaskSuspend(NULL);
	}

	if (kStatus_Success != uartInit(&hal_uart_imu)) {
		vTaskSuspend(NULL);
	}

	/* Send introduction message. */
	if (kStatus_Success
			!= uartSend(&hal_uart_debug, (uint8_t*) debug_intro_message,
					strlen(debug_intro_message))) {
		vTaskSuspend(NULL);
	}

	// For testing UART
	// for(EVER){
	// 	unsigned char c[10];
	// 	size_t numRead;
	// 	uartReceive(&hal_uart_imu, c, 3, &numRead);
	// 	uartReceive(&hal_uart_imu, c, 7, &numRead);
	// 	for(int i = 0; i < 7; ++i){
	// 		printf("UART character: %c\n", c[i]);
	// 	}
	// 	uartSend(&hal_uart_imu, c, 5);
	// }

	/* Receive input from imu and parse it. */
	do {
		uart_error = uartReceive(&hal_uart_imu, IMU.datagram, 40, &n);
		if (uart_error == kStatus_UART_RxHardwareOverrun) {
			/* Notify about hardware buffer overrun */
			if (kStatus_Success
					!= uartSend(&hal_uart_debug,
							(uint8_t*) send_hardware_overrun,
							strlen(send_hardware_overrun))) {
				vTaskSuspend(NULL);
			}
		}
		if (uart_error == kStatus_UART_RxRingBufferOverrun) {
			/* Notify about ring buffer overrun */
			if (kStatus_Success
					!= uartSend(&hal_uart_debug, (uint8_t*) send_ring_overrun,
							strlen(send_ring_overrun))) {
				vTaskSuspend(NULL);
			}
		}
		if (n > 0 && IMU.datagram[0] == 0x93) {
			// Parse Datagram
			parse_error = interpretImuData(&IMU);

			int len;
			if (parse_error == DATAGRAM_PARSE_ID_MISMATCH)
				len = sprintf(toPrint, "ID_MISMATCH\n");
			else if (parse_error == DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK)
				len = sprintf(toPrint, "STATUS_BYTE_FAIL\n");
			else if (parse_error == DATAGRAM_INCORRECT_CRC)
				len = sprintf(toPrint, "CRC CHECK FAILED\n");
			else if (parse_error == DATAGRAM_PARSE_SUCCESS) {

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
						(int) (o.i * 100), (int) (o.j * 100), (int) (o.k * 100),
						(int) (100 * sqrt(o.i * o.i + o.j * o.j + o.k * o.k)));
				// len = sprintf(toPrint, "t = %d", cur_time - imu_last_time);

				imu_last_time = cur_time;
			}

			uartSend(&hal_uart_debug, (uint8_t*) toPrint, len);
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
static void RadioTask(void *pv) {

	xbee_dev_t radio;
	xbee_serial_t serial;

	uint8_t *packet = (uint8_t*) pvPortMalloc(256 * sizeof(uint8_t));

	serial.baudrate = 9600;
	uartConfig(&(serial.uart_handle), RADIO_UART, 9600);

	xbee_dev_init(&radio, &serial, NULL, NULL);
	/* Add this for x86 testing */
	// memcpy(&radio.serport.uart_handle.buffer, radioPacket, sizeof(radioPacket));
	// radio.serport.uart_handle.cur_buffer_size = sizeof(radioPacket);
	while (1) {
		int len = radioReceive(&radio, packet);

		if (len > 0) {
			radioTxRequest(&radio, packet, len);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void LogTask(void *pv) {
	//HALFILE file;
	sdInit();
	for (EVER) {
//		sdMkDir("/testdir");
//		sdOpen(&file, "/testdir/testfile.txt");
//		sdWrite(&file, "test data\n");
//		sdClose(&file);
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

static void StateTask(void *pv) {
	for (EVER) {
		/*TODO: set all the state inputs...*/
		setNextState(&stateTransitonInput);
		vTaskDelay(pdMS_TO_TICKS(10));
	}

}

hal_can_packet_t rxPacket;
hal_can_packet_t txPacket;
flexcan_frame_t rxFrame;
static void CanTask(void *pv) {
	hal_can_handle_t can_handle;
	canInit(&can_handle, CAN1);
	for (EVER) {
		canReceive(&can_handle, &rxFrame);
		printf("CAN received, b0: %d, b1: %d\n", rxFrame.dataByte0, rxFrame.dataByte1);
		canSend(&can_handle, 0x123, (hal_can_packet_t){{'a', 'b', 'c', 'd'}}, 4);

		hal_can_packet_id_t id = canGetId(&rxFrame);
		switch (id) {
		case can_packet_id_pt:{
			hal_can_pt_id_t sensor_id = canGetSensorId(&rxFrame);
			float pressure = canGetFloatValue(&rxFrame);
			pt_data[sensor_id] = pressure;
			break;
		}
		case can_packet_id_tc:{
			hal_can_tc_id_t sensor_id = canGetSensorId(&rxFrame);
			float temp = canGetFloatValue(&rxFrame);
			tc_data[sensor_id] = temp;
			break;
		}
		default:
			break;
		}


		/* For debugging sending side of can bus communication */
//		canSetId(&txPacket, can_packet_id_pt);
//		canSetSensorId(&txPacket, can_pt_1);
//		canSetSensorValue(&txPacket, 5.3);
//		canSend(&can_handle, 0x123, txPacket, 6);
//
//		canSetId(&txPacket, can_packet_id_tc);
//		canSetSensorId(&txPacket, can_tc_1);
//		canSetSensorValue(&txPacket, 45.7);
//		canSend(&can_handle, 0x123, txPacket, 6);
//		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

#define GPS_BUF_SIZE 512

/// @brief gets a pointer to the next string after the next comma and sets the comma to a null char
/// @param src the src string
/// @return pointer to the next string
static char *get_next_gps_str(char *src) {
	char *ret = strchr(src, ',');
	if (!ret) {
		return NULL;
	}
	*ret++ = '\0';
	return ret;
}

/// @brief gets a pointer to the nth string after the next comma and sets the commas to a null char
/// @param src the src string
/// @return pointer to the nth string
static char *get_next_gps_str_n(char *src, int n) {
	if (!n) {
		return;
	}
	return get_next_gps_str_n(get_next_gps_str(src), n--);
}

void process_gps(char *msg) {
	// check for both "#BESTPOSA" and "BESTPOSA" cases
	char *start = strchr(msg, '#');
	if (!start) {
		start = strchr(msg, 'B');
		if (!start) {
			log("did not get correct message from uart %s", msg);
			return;
		} else {
			if (strncmp(start, "BESTPOSA", 8)) {
				log("did not get correct message from uart %s", msg);
				return;
			}
		}
	} else {
		if (strncmp(start, "#BESTPOSA", 9)) {
			log("did not get correct message from uart %s", msg);
			return;
		}
	}
    

    // get a pointer to the data section of the message (after the header which ends with ';')
    char *sol_stat = strchr(start, ';') + 1;

    /*
    0 => sol stat
    1 => pos type
    2 => lat
    3 => lon
    4 => hgt
    5 => undulation
    6 => datum id#
    9 => lat sd
    10 => lon sd
    11 => hgt sd
    12 => stn id
    13 => diff age
    14 => #SVS
    15 => #solnSVs

    */
    char *values[16];
    values[0] = sol_stat;
    for (int i = 1; i < 16; i++) {
        values[i] = get_next_gps_str(values[i - 1]);
        if (!values[i]) {
			log("Failed to get value: %u", i);
        	return;
        }
    }

    for (int i = 0; i < 16; i++) {
        printf("%u: %s\n", i, values[i]);
    }
}

static void GpsTask(void *pv) {


	//See hal_uart.h for documentation on UART commands

	//See board.h for GPS_UART
	//Had some concerns about baudrate, not sure if this is actually an issue or not

	uartConfig(&hal_uart_gps, GPS_UART, 9600 * 2);

	int uart_error;
	if (kStatus_Success != uartInit(&hal_uart_gps)) {
		vTaskSuspend(NULL);
	}

	char gpsdata[GPS_BUF_SIZE + 1];
	// this is to prevent overflow
	gpsdata[GPS_BUF_SIZE] = '\0';
	memset(&gpsdata[0], '\0', GPS_BUF_SIZE);

	//Task must be in an infinite loop, not entirely sure which UART commands should be within the loop
	for (EVER) {
		vTaskDelay(pdMS_TO_TICKS(1000));

		//Sends message to the GPS.
		//See   https://docs.novatel.com/OEM7/Content/PDFs/PreviousVersions/OEM7_Commands_Logs_Manual_v9.pdf
		//for list of GPS commands and details on them
		// gps_message = "LOG THISPORT BESTPOSA ONCE 0 0 NOHOLD\r\n";
		// if (kStatus_Success != uartSend(&hal_uart_gps, (uint8_t*)gps_message, strlen(gps_message))) {
		// 	vTaskSuspend(NULL);
		// }

		gps_message = "LOG THISPORT BESTPOSA ONCE 0 0 HOLD\r\n";
		if (kStatus_Success != uartSend(&hal_uart_gps, (uint8_t*)gps_message, strlen(gps_message))) {
			vTaskSuspend(NULL);
		}
		//vTaskDelay(pdMS_TO_TICKS(1000));

		// With no antenna, sending:
		// #BESTPOSA,COM1,0,95.5,UNKNOWN,0,1189.000,02440020,b1f6,14307;INSUFFICIENT_OBS,NONE,0.00000000000,0.00000000000,0.0000,0.0000,WGS84,0.0000,0.0000,0.0000,"",0.000,0.000,0,0,0,0,00,00,00,00*76077f48
		size_t rcvdSize;
		//int stat = uartReceive(&hal_uart_gps, gpsdata, 197, &rcvdSize);
		int err = gpsUartReceive(&hal_uart_gps, gpsdata, GPS_BUF_SIZE, &rcvdSize);
		printf("got string: \"%s\", status: %u\n", gpsdata, err);
		printf("received: %u\n", rcvdSize);
		process_gps(gpsdata);


		memset(&gpsdata[0], '\0', GPS_BUF_SIZE);
	}

	//Final UART command
	uartDeinit(&hal_uart_gps);
	vTaskSuspend(NULL);

}
