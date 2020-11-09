

/*******************************************************************************
 * Time
 ******************************************************************************/

#include "hal_time.h"

uint32_t xbee_seconds_timer(){
    return timeSinceStartup()/1000;
}

uint32_t xbee_millsecond_timer(){
    return timeSinceStartup();
}

/*******************************************************************************
 * Serial
 ******************************************************************************/

#include "xbee/serial.h"

int xbee_ser_invalid( xbee_serial_t *serial){
    
}

