#ifndef _IMU_INTERPRET_
#define _IMU_INTERPRET_

#include <math.h>
#include <stdio.h>

// Error/success codes
#define DATAGRAM_PARSE_SUCCESS						0
#define DATAGRAM_PARSE_ID_MISMATCH					1
#define DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK		2

// InterpretImuData takes a single datagram and interprets into usable double precision values, stored in whatever is passed to dataout[]
int interpretImuData(unsigned char datagram[], int identifier, double dataout[],
		unsigned char statusBytes[]);

#endif
