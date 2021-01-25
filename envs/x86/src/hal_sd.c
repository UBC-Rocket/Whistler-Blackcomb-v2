/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "hal_sd.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Card semaphore  */
static uint32_t s_taskSleepTicks = portMAX_DELAY;
static SemaphoreHandle_t sfileAccessSemaphore = NULL;

static const char *prePath = "../data/logs";

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void sdInit(void) {
	sfileAccessSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(sfileAccessSemaphore);
}

void sdOpen(HALFILE *file, const char *file_name) {
	const char* path = malloc(strlen(file_name) + strlen(prePath) + 1);
	strcpy(path, prePath);
	strcat(path, file_name);
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
        *file = fopen(path, "w");
	}
    xSemaphoreGive(sfileAccessSemaphore);
}

size_t sdWrite(HALFILE *file, const char *data) {
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
        fprintf(*file, "%s", data);
	}
	xSemaphoreGive(sfileAccessSemaphore);
}

void sdMkDir(const char *dir_name){
	const char* path = malloc(strlen(dir_name) + strlen(prePath) + 1);
	strcpy(path, prePath);
	strcat(path, dir_name);
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
        struct stat st = {0};
        if (stat(path, &st) == -1) {
            int error = mkdir(path, 0700);
			if(error){
				printf("Error making new directory\n");
			}
        }
	}
	xSemaphoreGive(sfileAccessSemaphore);
}

void sdClose(HALFILE *file) {
	if (xSemaphoreTake(sfileAccessSemaphore, s_taskSleepTicks) == pdTRUE) {
        fclose(*file);
		xSemaphoreGive(sfileAccessSemaphore);
	}
}
