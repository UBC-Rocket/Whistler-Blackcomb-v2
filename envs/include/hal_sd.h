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

#ifdef COMPILE_BOARD
typedef FIL HALFILE;
#elif COMPILE_X86

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
void sdOpen(HALFILE *file, const char *file_name);

/**
 * Writes data to a file
 *
 * @param file the file object to write to
 * @param data the data to write
 *
 * @return the number of bytes written
 */
size_t sdWrite(HALFILE *file, const char *data);

/**
 * Makes a new directory
 *
 * @param dir_name the path of the new directory
 */
void sdMkDir(const char *dir_name);

/**
 * Closes file
 *
 * @param file the file to close
 */
void sdClose(HALFILE *file);

#endif
