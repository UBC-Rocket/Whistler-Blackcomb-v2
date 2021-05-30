/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_can.h"
#include "fsl_flexcan.h"
#include "board.h"
#include "stdio.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* TODO: Implement id scheme for DAQ board and flight computer */
uint32_t rxFifoFilter[] = {
//		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x321, 0, 0),
//		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x321, 1, 0),
		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x123, 0, 0),
//		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x123, 1, 0)
		};

/*******************************************************************************
 * Implementations
 ******************************************************************************/

/**
 * Callback function for status change, documentation for most parameters in
 * fsl_flexcan.h
 *
 * userData is the hal_can_handle_t for can instance calling this function
 */
static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle,
		status_t status, uint32_t result, void *userData) {
	hal_can_handle_t *hal_handle = (hal_can_handle_t*) userData;
	/* Note: it's important to use FromISR versions of semaphore taking, since
	 * normal versions won't work inside a interrupt */
	switch (status) {
	case kStatus_FLEXCAN_RxIdle:
		break;

	case kStatus_FLEXCAN_TxIdle:
		xSemaphoreGiveFromISR(hal_handle->rxSem, NULL);
		break;
	case kStatus_FLEXCAN_RxFifoIdle:
		xSemaphoreGiveFromISR(hal_handle->rxSem, NULL);
		break;
	case kStatus_FLEXCAN_ErrorStatus:
		break;
	default:
		break;
	}
}

void canInit(hal_can_handle_t *handle, CAN_Type *base) {
	handle->base = base;
	FLEXCAN_GetDefaultConfig(&handle->config);
	/* Change configuration settings here before init */
	handle->config.clkSrc = CAN_CLK_SOURCE;
	handle->config.disableSelfReception = true;
	FLEXCAN_Init(handle->base, &handle->config, CAN_CLK_FREQ);

	/* Sets up receive FIFO, got from here: https://community.nxp.com/t5/Kinetis-Microcontrollers/FlexCAN-Fifo-SDK-2-0-example/m-p/718276 */
	handle->fifo_config.idFilterTable = rxFifoFilter;
	handle->fifo_config.idFilterType = kFLEXCAN_RxFifoFilterTypeA;
	handle->fifo_config.idFilterNum = sizeof(rxFifoFilter)
			/ sizeof(rxFifoFilter[0]);
	handle->fifo_config.priority = kFLEXCAN_RxFifoPrioHigh;
	FLEXCAN_SetRxFifoConfig(handle->base, &handle->fifo_config, true);

	/* Sets up TX buffer. 8 offset because of FIFO buffer */
	FLEXCAN_SetTxMbConfig(handle->base, 8 + FLEXCAN_GetInstance(handle->base),
	true);

	FLEXCAN_TransferCreateHandle(handle->base, &(handle->transfer_handle),
			flexcan_callback, handle);

	/* Semaphores are used to make functions blocking */
	handle->rxSem = xSemaphoreCreateBinary();
	handle->txSem = xSemaphoreCreateBinary();
	xSemaphoreGive(handle->rxSem);
	xSemaphoreGive(handle->txSem);
}

void canReceive(hal_can_handle_t *handle, flexcan_frame_t *rxFrame) {
	if (xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE) {
		handle->fifo_transfer.frame = rxFrame;
		FLEXCAN_TransferReceiveFifoNonBlocking(handle->base,
				&(handle->transfer_handle), &(handle->fifo_transfer));
	}
	if (xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE) {
		rxFrame = handle->fifo_transfer.frame;
	}
	xSemaphoreGive(handle->rxSem);
}

void canSend(hal_can_handle_t *handle, uint32_t id, hal_can_packet_t packet,
		uint32_t length) {
	if (xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE) {
		/* Doesn't just accept this directly because macros are unavailable in
		 * x86
		 * Instead sets up transmit packet in this function*/
		flexcan_frame_t txFrame;
		txFrame.format = (uint8_t) kFLEXCAN_FrameFormatStandard;
		txFrame.type = (uint8_t) kFLEXCAN_FrameTypeData;
		txFrame.id = FLEXCAN_ID_STD(id);
		txFrame.length = (uint8_t) length;
		uint8_t *buffer = packet.c;
		txFrame.dataWord0 =
				CAN_WORD0_DATA_BYTE_0(
						buffer[0]) | CAN_WORD0_DATA_BYTE_1(buffer[1]) | CAN_WORD0_DATA_BYTE_2(buffer[2]) |
						CAN_WORD0_DATA_BYTE_3(buffer[3]);
		txFrame.dataWord1 =
				CAN_WORD1_DATA_BYTE_4(
						buffer[4]) | CAN_WORD1_DATA_BYTE_5(buffer[5]) | CAN_WORD1_DATA_BYTE_6(buffer[6]) |
						CAN_WORD1_DATA_BYTE_7(buffer[7]);

		handle->txXfer.mbIdx =
				(uint8_t) (8 + FLEXCAN_GetInstance(handle->base));
		handle->txXfer.frame = &txFrame;

		FLEXCAN_TransferSendNonBlocking(handle->base,
				&(handle->transfer_handle), &(handle->txXfer));
	}
	/* Wait until message sent before returning */
	xSemaphoreTake(handle->rxSem, portMAX_DELAY);
	xSemaphoreGive(handle->rxSem);
}

hal_can_packet_id_t canGetId(flexcan_frame_t *rxFrame) {
	return rxFrame->dataByte0;
}

void canSetId(hal_can_packet_t *packet, hal_can_packet_id_t id) {
	packet->c[0] = id;
}

int canGetSensorId(flexcan_frame_t *rxFrame) {
	return rxFrame->dataByte1;
}

void canSetSensorId(hal_can_packet_t *packet, int id) {
	packet->c[1] = id;
}

float canGetSensorValue(flexcan_frame_t *rxFrame) {
	float pressure;
	char *c = (char*) &pressure;
	c[0] = rxFrame->dataByte2;
	c[1] = rxFrame->dataByte3;
	c[2] = rxFrame->dataByte4;
	c[3] = rxFrame->dataByte5;
	return pressure;
}

void canSetSensorValue(hal_can_packet_t *packet, float value) {
	char *c = (char*) &value;
	packet->c[2] = c[0];
	packet->c[3] = c[1];
	packet->c[4] = c[2];
	packet->c[5] = c[3];
}

