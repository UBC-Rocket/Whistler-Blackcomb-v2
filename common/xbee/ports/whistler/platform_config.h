/* 
 * Platform config for whistler-blackcomb project
 * 
 * 
 */

#ifndef __XBEE_PLATFORM_WHISTLER
#define __XBEE_PLATFORM_WHISTLER

#include <string.h>

#define strcmpi         strcasecmp
#define strncmpi        strncasecmp

// macros used to declare a packed structure (no alignment of elements)
#define PACKED_STRUCT       struct __attribute__ ((__packed__))
#define XBEE_PACKED(name, decl) PACKED_STRUCT name decl

#define _f_memcpy        memcpy
#define _f_memset        memset

#include <stdint.h>

typedef int bool_t;

#define FAR

#include "hal_uart.h"

typedef struct xbee_serial_t
{
    uint32_t baudrate;
	hal_uart_handle_t uart_handle;
} xbee_serial_t;

// We'll use 1/1/2000 as the epoch, to match ZigBee.
#define ZCL_TIME_EPOCH_DELTA    0

#define XBEE_MS_TIMER_RESOLUTION 1

#define XBEE_SERIAL_MAX_BAUDRATE 921600
   
#define XBEE_WIFI_ENABLED 0

#endif
