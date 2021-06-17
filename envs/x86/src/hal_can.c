/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "hal_can.h"
#include "stdio_controller.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Implementations
 ******************************************************************************/


void canInit(hal_can_handle_t *handle, CAN_Type *base) {
	handle->base = base;

	/* Semaphores are used to make functions blocking */
	handle->rxSem = xSemaphoreCreateBinary();
	handle->txSem = xSemaphoreCreateBinary();
	xSemaphoreGive(handle->rxSem);
	xSemaphoreGive(handle->txSem);
}

void canReceive(hal_can_handle_t *handle, flexcan_frame_t *rxFrame) {
	if (xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE) {
        uint8_t data[8];
		// TODO: enumerate all SIM packet IDs somewhere centralized
        readFromBuf(RX, data, 'c');
        rxFrame->dataByte0 = data[0];
        rxFrame->dataByte1 = data[1];
        rxFrame->dataByte2 = data[2];
        rxFrame->dataByte3 = data[3];
        rxFrame->dataByte4 = data[4];
        rxFrame->dataByte5 = data[5];
        rxFrame->dataByte6 = data[6];
        rxFrame->dataByte7 = data[7];
	}
	xSemaphoreGive(handle->rxSem);
}

void canSend(hal_can_handle_t *handle, uint32_t id, hal_can_packet_t packet,
		uint32_t length) {
	if (xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE) {
		writeToBuf(TX, packet.c, 'c', length);
	}
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

