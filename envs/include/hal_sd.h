#ifndef _HAL_SD_H_
#define _HAL_SD_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#ifdef COMPILE_BOARD
#include "ff.h"
#endif

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes SD card
 */
void sdInit(void);

/**
 * Opens file in write mode
 *
 * @param file blank file object
 * @param file_name the name of the file to open
 */
void sdOpen(HIL file, const char *file_name);

/**
 * Writes data to a file
 *
 * @param file the file object to write to
 * @param data the data to write
 */
void sdWrite(HIL file, const char *data);

/**
 * Closes file
 *
 * @param file the file to close
 */
void sdClose(HIL file);

#endif
