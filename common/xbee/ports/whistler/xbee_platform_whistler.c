/*******************************************************************************
 * Time
 ******************************************************************************/

#include "hal_time.h"
#include <stdint.h>

uint32_t xbee_seconds_timer(){
    return timeSinceStartup()/1000;
}

uint32_t xbee_millisecond_timer(){
    return timeSinceStartup();
}

/*******************************************************************************
 * Serial
 * NOTE: Documentation for these functions can be found in xbee/serial.h.
 ******************************************************************************/

#include "xbee/serial.h"
#include "hal_uart.h"

#define XBEE_SER_CHECK(ptr) \
    do { if (xbee_ser_invalid(ptr)) return -EINVAL; } while (0)

/* For MCU build, don't remove */
#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

int xbee_ser_invalid( xbee_serial_t *serial){
    /* TODO: Do some actual checking here */
    return 0;
}

const char *xbee_ser_portname( xbee_serial_t *serial){
    /* TODO: Somehow get the actual port name */
    return "[Port Name Placeholder]";
}

int xbee_ser_write( xbee_serial_t *serial, const void FAR *buffer,
        int length){
    
    XBEE_SER_CHECK( serial);

    if (length < 0){
        return -EINVAL;
    }

    // uint8_t toSend = 17;

    uartSend(&(serial->uart_handle), buffer, length);
    // uartSend(&(serial->uart_handle), &toSend, 1);
    return uartTxUsed(&(serial->uart_handle));
}

int xbee_ser_read( xbee_serial_t *serial, void FAR *buffer, int bufsize){
    size_t received;

    XBEE_SER_CHECK( serial);

    if (! buffer || bufsize < 0){
        return -EINVAL;
    }

    if(xbee_ser_rx_used(serial) <= 0){
        return 0;
    }

    uartReceive(&(serial->uart_handle), buffer, bufsize, &received);

    return received;
}

/* Copied from POSIX port */
int xbee_ser_putchar( xbee_serial_t *serial, uint8_t ch){
    int retval;

    retval = xbee_ser_write( serial, &ch, 1);
    if (retval == 1)
    {
        return 0;
    }
    else if (retval == 0)
    {
        return -ENOSPC;
    }
    else
    {
        return retval;
    }
}

/* Copied from POSIX port */
int xbee_ser_getchar( xbee_serial_t *serial){
    uint8_t ch = 0;
    int retval;

    retval = xbee_ser_read( serial, &ch, 1);
    if (retval != 1)
    {
        return retval ? retval : -ENODATA;
    }

    return ch;
}

/* Copied from POSIX port */
int xbee_ser_tx_free( xbee_serial_t *serial){
    XBEE_SER_CHECK( serial);
    return INT_MAX;
}

int xbee_ser_tx_used( xbee_serial_t *serial){
    XBEE_SER_CHECK( serial);
    return uartTxUsed(&(serial->uart_handle));
}


int xbee_ser_tx_flush( xbee_serial_t *serial){
    /* TODO: Make this actually do something (or decide not to) */
    XBEE_SER_CHECK( serial);
    return 0;
}

/* Copied from POSIX port */
int xbee_ser_rx_free( xbee_serial_t *serial){
    XBEE_SER_CHECK( serial);
    return INT_MAX;
}

int xbee_ser_rx_used( xbee_serial_t *serial){
    XBEE_SER_CHECK( serial);
    return uartRxUsed(&(serial->uart_handle));
}

int xbee_ser_rx_flush( xbee_serial_t *serial){
    /* TODO: Make this actually do something (or decide not to) */
    XBEE_SER_CHECK( serial);
    return 0;
}

int xbee_ser_baudrate( xbee_serial_t *serial, uint32_t baudrate){
    XBEE_SER_CHECK( serial);
    serial->baudrate = baudrate;
    uartSetBaudrate(&(serial->uart_handle), baudrate);
    return 0;
}

int xbee_ser_open( xbee_serial_t *serial, uint32_t baudrate){
    int ret = xbee_ser_baudrate(serial, baudrate);
    uartInit(&(serial->uart_handle));
    return ret;
}

int xbee_ser_close( xbee_serial_t *serial){
    uartDeinit(&(serial->uart_handle));
    return 0;
}

int xbee_ser_break( xbee_serial_t *serial, int enabled){
    /* TODO: try and actually implement this */
    return 0;
}

/*
 * Note: we do not try and implement flow control
 * This might be something we want to look into, but for now I'm just leaving it
 */

int xbee_ser_flowcontrol( xbee_serial_t *serial, int enabled){
    return 0;
}


int xbee_ser_set_rts( xbee_serial_t *serial, int asserted){
    return 0;
}

int xbee_ser_get_cts( xbee_serial_t *serial){
    /* Always clear to send */
    return 1;
}