/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "bms.h"

/*******************************************************************************
 * Declarations
 ******************************************************************************/

uint8_t masterReceiveBuffer[TRANSFER_SIZE] = {0};
uint8_t masterSendBuffer[TRANSFER_SIZE]    = {0};

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void SpiInit(){
		dspi_transfer_t masterXfer;
	    dspi_rtos_handle_t master_rtos_handle;
	    dspi_master_config_t masterConfig;
	    uint32_t sourceClock;
	    status_t status;

	    DSPI_MasterGetDefaultConfig(&masterConfig);

	    masterConfig.whichPcs           = kDSPI_Pcs1;
	    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
	    masterConfig.whichCtar               = kDSPI_Ctar1;
	    masterConfig.ctarConfig.baudRate     = 7000;

	    sourceClock = DSPI_MASTER_CLK_FREQ;
	    status      = DSPI_RTOS_Init(&master_rtos_handle, EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, sourceClock);

	    if (status != kStatus_Success)
	    {
	        printf("DSPI master: error during initialization. \r\n");
	        vTaskSuspend(NULL);
	    }

	    /*Start master transfer*/
	    masterSendBuffer[0] = 0x03;
	    masterSendBuffer[1] = 0x60;
	    masterSendBuffer[2] = 0xF4;
	    masterSendBuffer[3] =  0x6C;

	    masterXfer.txData      = masterSendBuffer;
	    masterXfer.rxData      = masterReceiveBuffer;
	    masterXfer.dataSize    = TRANSFER_SIZE;
	    // The _MasterCtar# and _MasterPcs# flags should match the masterConfig flags that are set above.
	    masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;

	    status = DSPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);

	    if (status == kStatus_Success)
	    {
	        printf("DSPI master transfer completed successfully. \r\n\r\n");
	    }
	    else
	    {
	        printf("DSPI master transfer completed with error. \r\n\r\n");
	    }
	    for (int i = 0; i < TRANSFER_SIZE; i++) {
		    printf("RX: %hx ", masterReceiveBuffer[i]);
	    }
	    printf("\n");

	   while (true) {
		    masterSendBuffer[0] = 0x03;
		    masterSendBuffer[1] = 0x60;
		    masterSendBuffer[2] = 0xF4;
		    masterSendBuffer[3] = 0x6C;
		   for (int i = 0; i < TRANSFER_SIZE; i++) {
			   printf("TX: %hx ", masterXfer.txData[i]);
		   }
		   status = DSPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);
		   if (status == kStatus_Success) {
			   for (int i = 0; i < TRANSFER_SIZE; i++) {
				   printf("RX: %hx ", masterXfer.rxData[i]);
			   }
		   }
		   else {
			   printf("Transfer failed, retrying");
		   }
		   printf("\n");
		   vTaskDelay(pdMS_TO_TICKS(2000));
	   }
}
