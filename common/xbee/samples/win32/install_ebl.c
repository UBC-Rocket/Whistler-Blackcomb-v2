/*
 * Copyright (c) 2010-2019 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc., 9350 Excelsior Blvd., Suite 700, Hopkins, MN 55343
 * ===========================================================================
 */

/*
    Install firmware updates on XBee modules that use .EBL/.GBL firmware files.
*/

#include <stdio.h>
#include <stdlib.h>

#include "xbee/device.h"
#include "xbee/firmware.h"

#include "xbee/atcmd.h"         // for XBEE_FRAME_HANDLE_LOCAL_AT
const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
{
    XBEE_FRAME_HANDLE_LOCAL_AT,
    XBEE_FRAME_TABLE_END
};

#include "parse_serial_args.h"
#include "win32_select_file.h"

/*
    Sample code to read firmware from a file, used to demonstrate
    new non-blocking firmware update API.

    Note that the S2 doesn't actually need a seek function.  Maybe that should
    just be removed (set to NULL)?  We may need it in the future, so leave it in?
    There are benefits to not having it though -- can install updates straight
    from an HTTP download.
*/

xbee_dev_t my_xbee;

int fw_seek( void FAR *context, uint32_t offset)
{
    return fseek( (FILE FAR *)context, offset, SEEK_SET);
}

int fw_read( void FAR *context, void FAR *buffer, int16_t bytes)
{
    return fread( buffer, 1, bytes, (FILE FAR *)context);
}

void myxbee_reset( xbee_dev_t *xbee, int reset)
{
    static int state = 0;           // assume released
    char buffer[10];

    // standard API for an XBee reset handler, but there's only one XBee
    // in this sample, so no need to use 'xbee' parameter
    XBEE_UNUSED_PARAMETER( xbee);

    if (state != reset)
    {
        printf( "%s the XBee's reset button, press ENTER.\n",
            reset ? "Press and hold" : "Release");
        fgets( buffer, 10, stdin);

        state = reset;
    }
}

int main( int argc, char *argv[])
{
    xbee_fw_source_t fw = { 0 };
    char *firmware;
    FILE *file = NULL;
    char buffer[80];
    uint32_t t;
    int result;
    unsigned int last_state;
    xbee_serial_t XBEE_SERPORT;

    parse_serial_arguments( argc, argv, &XBEE_SERPORT);

    if (argc > 1 && memcmp( argv[argc - 1], "COM", 3) != 0)
    {
        file = fopen( argv[argc - 1], "rb");
    }
    if (! file)
    {
        // Prompt user to select a .ebl/.gbl file.
        printf( "Select firmware image (*.ebl or *.gbl) from file dialog box.\n");
        firmware = win32_select_file("Select Firmware Image",
            "XBee Firmware (*.ebl, *.gbl)\0*.ebl;*.gbl\0All Files (*.*)\0*.*\0");
        if (firmware == NULL)
        {
            printf("Dialog canceled.\n");
            exit( 0);
        }
        printf("Firmware set to\n  %s\n", firmware);
        file = fopen( firmware, "rb");
        if (! file)
        {
            printf( "Error: couldn't open %s\n", firmware);
            exit( -1);
        }
    }

    fw.context = file;
    fw.seek = fw_seek;
    fw.read = fw_read;

    if (xbee_dev_init( &my_xbee, &XBEE_SERPORT, NULL, myxbee_reset))
    {
        printf( "Failed to initialize device.\n");
        return 0;
    }

    xbee_fw_install_init( &my_xbee, NULL, &fw);
    do
    {
        t = xbee_millisecond_timer();
        last_state = xbee_fw_install_ebl_state( &fw);
        result = xbee_fw_install_ebl_tick( &fw);
        t = xbee_millisecond_timer() - t;
#ifdef BLOCKING_WARNING
        if (t > 50)
        {
            printf( "!!! blocked for %ums in state 0x%04X (now state 0x%04X)\n",
                t, last_state, xbee_fw_install_ebl_state( &fw));
        }
#endif
        if (last_state != xbee_fw_install_ebl_state( &fw))
        {
            // print new status
            printf( " %" PRIsFAR "                          \r",
                xbee_fw_status_ebl( &fw, buffer));
            fflush( stdout);
        }
    } while (result == 0);
    if (result == 1)
    {
        printf( "firmware update completed successfully\n");
    }
    else
    {
        printf("firmware update failed with error %d\n", result);
        // dump possible error message from bootloader
        printf("Remaining output from serial port:\n");
        t = xbee_millisecond_timer();
        do {
            result = xbee_ser_getchar(&my_xbee.serport);
            if (result > 0) {
                putchar(result);
            }
        } while (result != '>' && xbee_millisecond_timer() - t < 2000);
        putchar('\n');
    }

    fclose( file);

    xbee_ser_close( &my_xbee.serport);

    // Note that the module now has default settings, including baud rate,
    // flow control and even API mode.

    return 0;
}

