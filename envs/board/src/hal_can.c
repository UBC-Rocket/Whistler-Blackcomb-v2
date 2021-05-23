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
uint32_t rxFifoFilter[] = {
//		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x321, 0, 0),
//		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x321, 1, 0),
		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x123, 0, 0),
//		FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(0x123, 1, 0)
};

/*******************************************************************************
 * Implementations
 ******************************************************************************/

static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle,
		status_t status, uint32_t result, void *userData) {
	hal_can_handle_t* hal_handle = (hal_can_handle_t*)userData;
	switch (status) {
	/* Process FlexCAN Rx event. */
        case kStatus_FLEXCAN_RxIdle:
//            if (RX_MESSAGE_BUFFER_NUM == result)
//            {
//                rxComplete = true;
//            }
            break;

        /* Process FlexCAN Tx event. */
        case kStatus_FLEXCAN_TxIdle:
//            if (TX_MESSAGE_BUFFER_NUM == result)
//            {
//                txComplete = true;
//            }
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

int canInit(hal_can_handle_t *handle, CAN_Type *base) {
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
//	FLEXCAN_SetTxMbConfig(handle->base, 8 + FLEXCAN_GetInstance(handle->base),
//			true);


	FLEXCAN_TransferCreateHandle(handle->base, &(handle->transfer_handle),
			flexcan_callback, handle);

	handle->rxSem = xSemaphoreCreateBinary();
	handle->txSem = xSemaphoreCreateBinary();
	xSemaphoreGive(handle->rxSem);
	xSemaphoreGive(handle->txSem);
}

int canReceive(hal_can_handle_t *handle, flexcan_frame_t *rxFrame) {
	if(xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE){
		handle->fifo_transfer.frame = rxFrame;
		if(FLEXCAN_TransferReceiveFifoNonBlocking(handle->base, &(handle->transfer_handle), &(handle->fifo_transfer))!=kStatus_Success)
			printf("CAN Receive Failed!\n");
	}
	if(xSemaphoreTake(handle->rxSem, portMAX_DELAY) == pdTRUE){
		rxFrame = handle->fifo_transfer.frame;
	}
	xSemaphoreGive(handle->rxSem);
}

static flexcan_frame_t txFrame;
int canSend(hal_can_handle_t *handle, uint32_t id, const uint8_t *buffer,
		uint32_t length) {
	/* Doesn't just accept this directly because macros are unavailable in
	 * x86 */
	txFrame.format = (uint8_t) kFLEXCAN_FrameFormatStandard;
	txFrame.type = (uint8_t) kFLEXCAN_FrameTypeData;
	txFrame.id = FLEXCAN_ID_STD(id);
	txFrame.length = (uint8_t) length;
	txFrame.dataWord0 =
			CAN_WORD0_DATA_BYTE_0(
					buffer[0]) | CAN_WORD0_DATA_BYTE_1(buffer[1]) | CAN_WORD0_DATA_BYTE_2(buffer[2]) |
					CAN_WORD0_DATA_BYTE_3(buffer[3]);
	txFrame.dataWord1 =
			CAN_WORD1_DATA_BYTE_4(
					buffer[4]) | CAN_WORD1_DATA_BYTE_5(buffer[5]) | CAN_WORD1_DATA_BYTE_6(buffer[6]) |
					CAN_WORD1_DATA_BYTE_7(buffer[7]);

	handle->txXfer.mbIdx = (uint8_t) (8+FLEXCAN_GetInstance(handle->base));
	handle->txXfer.frame = &txFrame;

	if(FLEXCAN_TransferSendNonBlocking(handle->base,
			&(handle->transfer_handle), &(handle->txXfer))!=kStatus_Success)
		printf("CAN Send Failed!\n");
}
