///*******************************************************************************
// * Includes
// ******************************************************************************/
//#include "hal_sd.h"
//
///*******************************************************************************
// * Variables
// ******************************************************************************/
//static FATFS g_fileSystem; /* File system object */
//static FIL g_fileObject; /* File object */
//static uint32_t s_taskSleepTicks = portMAX_DELAY;
//static const uint8_t s_buffer1[] = { 'T', 'A', 'S', 'K', '1', '\r', '\n' };
//static const uint8_t s_buffer2[] = { 'T', 'A', 'S', 'K', '2', '\r', '\n' };
///*! @brief SD card detect flag  */
//static volatile bool s_cardInserted = false;
//static volatile bool s_cardInsertStatus = false;
///*! @brief Card semaphore  */
//static SemaphoreHandle_t s_fileAccessSemaphore = NULL;
//static SemaphoreHandle_t s_CardDetectSemaphore = NULL;
//
///*******************************************************************************
// * Declarations
// ******************************************************************************/
//static void BOARD_SD_Config(void *card, sd_cd_t cd, uint32_t hostIRQPriority,
//		void *userData);
//
///*******************************************************************************
// * Implementations
// ******************************************************************************/
//
//static void BOARD_SD_Config(void *card, sd_cd_t cd, uint32_t hostIRQPriority,
//		void *userData) {
//	assert(card);
//
//	s_host.dmaDesBuffer = s_sdmmcHostDmaBuffer;
//	s_host.dmaDesBufferWordsNum = BOARD_SDMMC_HOST_DMA_DESCRIPTOR_BUFFER_SIZE;
//	((sd_card_t*) card)->host = &s_host;
//	((sd_card_t*) card)->host->hostController.base =
//			BOARD_SDMMC_SD_HOST_BASEADDR;
//	((sd_card_t*) card)->host->hostController.sourceClock_Hz = CLOCK_GetFreq(
//			kCLOCK_CoreSysClk);
//
//	((sd_card_t*) card)->host->hostEvent = &s_event;
//	((sd_card_t*) card)->usrParam.cd = &s_cd;
//
//	BOARD_SDCardDetectInit(cd, userData);
//
//	NVIC_SetPriority(BOARD_SDMMC_SD_HOST_IRQ, hostIRQPriority);
//}
//
//void sdInit(void);
//
//void sdOpen(HIL file, const char *file_name);
//
//void sdWrite(HIL file, const char *data);
//
//void sdClose(HIL file);
