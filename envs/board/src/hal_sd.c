/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "sdmmc_config.h"
#include "fsl_sd.h"
#include "ff.h"
#include "diskio.h"
#include "stdio.h"
#include "fsl_sd_disk.h"
#include "fsl_sysmpu.h"

#include "hal_sd.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

static FATFS g_fileSystem; /* File system object */
static uint32_t s_taskSleepTicks = portMAX_DELAY;
/*! @brief SD card detect flag  */
static volatile bool s_cardInserted = false;
static volatile bool s_cardInsertStatus = false;
/*! @brief Card semaphore  */
static SemaphoreHandle_t sfileAccessSemaphore = NULL;

const TCHAR driverNumberBuffer[3U] = { SDDISK + '0', ':', '/' };

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void sdInit(void) {
	FRESULT error;

	SYSMPU_Enable(SYSMPU, false);

	sfileAccessSemaphore = xSemaphoreCreateBinary();
	BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

	if (SD_HostInit(&g_sd) != kStatus_Success)
		printf("SD Host init failed.\n");

	/* Not nec */
//	SD_SetCardPower(&g_sd, false);
//	printf("Resetting chip...\n");
//	SD_SetCardPower(&g_sd, true);

	if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
		printf("Mount volume failed.\r\n");
	}

#if (FF_FS_RPATH >= 2U)
	error = f_chdrive((char const*) &driverNumberBuffer[0U]);
	if (error) {
		printf("Change drive failed.\r\n");
	}
#endif

	xSemaphoreGive(sfileAccessSemaphore);
}

void sdOpen(HALFILE *file, const char *file_name) {
	FRESULT error;
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
		error = f_open(file, file_name, FA_WRITE);
		if (error == FR_OK) {
			printf("File opened");
		} else if (error == FR_NO_FILE) {
			if (f_open(file, file_name, (FA_WRITE | FA_CREATE_NEW)) != FR_OK) {
				printf("Create file failed.\r\n");
			}
		} else {
			printf("Open file failed.\r\n");
		}
	}
	xSemaphoreGive(sfileAccessSemaphore);
}

size_t sdWrite(HALFILE *file, const char *data) {
	FRESULT error;
	UINT bytesWritten = 0U;
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
		error = f_lseek(file, file->obj.objsize);
		if (error != FR_OK) {
			printf("lseek file failed.\r\n");
			xSemaphoreGive(sfileAccessSemaphore);
			return -1;
		}

		error = f_write(file, data, strlen(data), &bytesWritten);
		if ((error) || (bytesWritten != strlen(data))) {
			printf("Write file failed.\r\n");
			xSemaphoreGive(sfileAccessSemaphore);
			return -1;
		}
	}
	xSemaphoreGive(sfileAccessSemaphore);
	return bytesWritten;
}

void sdMkDir(const char *dir_name){
	FRESULT error;
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
		error = f_mkdir(dir_name);
		if (error) {
			if (error == FR_EXIST) {
				printf("Directory exists.\r\n");
			} else {
				printf("Make directory failed.\r\n");
			}
		}
	}
	xSemaphoreGive(sfileAccessSemaphore);
}

void sdClose(HALFILE *file) {
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
		f_close(file);
		xSemaphoreGive(sfileAccessSemaphore);
	}
}
