
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "IMU_interpret.h"
#include <math.h>
#include <stdio.h>


/*******************************************************************************
 * Declarations
 ******************************************************************************/

int to_2C(int value);

int get_ID(imu_config_t *imu);

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void configImu(imu_config_t *imu){
	imu->interpGyro 	= 1;
	imu->interpAccel 	= 1;
	imu->interpIncl 	= 1;
	imu->interpTemp 	= 0;
	imu->interpAux 		= 0;

	imu->sampleRate = 125;
	imu->bitRate = 374400;
}

//TODO: Better status byte stuff

int to_2C(int value) {
	const int MODULO = 1 << 24;
	const int MAX_VALUE = (1 << 23) - 1;
	if (value > MAX_VALUE) {
		value -= MODULO;
	}
	return value;
}

int get_ID(imu_config_t *imu) {
	if (imu->interpAux) {
		if (imu->interpTemp) {
			if (imu->interpAccel) {
				if (imu->interpIncl) {
					return 0xAF;
				}
				else {
					return 0xAD;
				}
			}
			else if (imu->interpIncl) {
				return 0xAE;
			}
			else {
				return 0x9C;
			}
		}
		else if (imu->interpAccel) {
			if (imu->interpIncl) {
				return 0x9B;
			}
			else {
				return 0x99;
			}
		}
		else if (imu->interpIncl) {
			return 0x9A;
		}
		else {
			return 0x98;
		}

	}
	else {
		if (imu->interpTemp) {
			if (imu->interpAccel) {
				if (imu->interpIncl) {
					return 0xA7;
				}
				else {
					return 0xA5;
				}
			}
			else if (imu->interpIncl) {
				return 0xA6;
			}
			else {
				return 0x94;
			}
		}
		else if (imu->interpAccel) {
			if (imu->interpIncl) {
				return 0x93;
			}
			else {
				return 0x91;
			}
		}
		else if (imu->interpIncl) {
			return 0x92;
		}
		else {
			return 0x90;
		}
	}
}


int interpretImuData(imu_config_t *imu) {
	//IMPORTANT: This is not well made, so you better make sure that the length of the arrays you pass are right...

	//these variables keep track of where the program is in the datagram[], dataout[], and statusBytes[] arrays, respectivly
	int datagramCount = 0;

	//we'll use this to see if any of the status bytes are in non-OK states
	int statusFlag = 0;

	//quick check to make sure the identifier from the packet = identifier it expects.
	if (imu->datagram[datagramCount] != get_ID(imu)) {
		//return error 
		return DATAGRAM_PARSE_ID_MISMATCH;
	}

	datagramCount += 1; // move 1 byte forward in the datagram

	//gyro parse loop
	//There is no option for a normal mode datapacket without gyro data, so we evaluate it every time.
	int gyro_divisor[2]={14,21};
	for (int i = 0; i < 3; i++) {
		unsigned int bitrep = (imu->datagram[datagramCount] << 16)
				+ (imu->datagram[datagramCount + 1] << 8)
				+ imu->datagram[datagramCount + 2];
		imu->rate[i] = to_2C(bitrep) / 1.0 / (1 << gyro_divisor[imu->interpGyro-1]);
		datagramCount += 3;		//move 3 bytes forward in the datagram
	}

	//status byte output and engage flag
	if (imu->datagram[datagramCount] != 0) {
		statusFlag = 1;
	}

	//move past status byte
	datagramCount += 1;

	//evaluate if datagram includes accelerometer data
	if (imu->interpAccel) {
		//accelerometer parse loop
		int accel_divisor[8]={20,19,18,16,23,22,21,19};
		for (int i = 0; i < 3; i++) {
			unsigned int bitrep = (imu->datagram[datagramCount] << 16)
					+ (imu->datagram[datagramCount + 1] << 8)
					+ imu->datagram[datagramCount + 2];
			imu->accel[i] = to_2C(bitrep) / 1.0 / (1 << accel_divisor[imu->interpAccel-1]);
			datagramCount += 3;
		}

		//status byte output and engage flag
		if (imu->datagram[datagramCount] != 0) {
			statusFlag = 1;
		}

		//move past status byte
		datagramCount += 1;
	}

	//evaluate if datagram includes inlc data
	if (imu->interpIncl) {
		//inclinometer parse loop
		int incl_divisor[2]= {22,25};
		for (int i =0; i < 3; i++) {
			unsigned int bitrep = (imu->datagram[datagramCount] << 16)
					+ (imu->datagram[datagramCount + 1] << 8)
					+ imu->datagram[datagramCount + 2];
			imu->incl[i] = to_2C(bitrep) / 1.0 / (1 << incl_divisor[imu->interpAccel-1]);
			datagramCount += 3;
		}
		//status byte output and engage flag
		if (imu->datagram[datagramCount] != 0) {
			statusFlag = 1;
		}

		//move past status byte
		datagramCount += 1;
	}
	//evaluate if datagram includes temp
	if (imu->interpTemp) {
		//there are 3 different temp readings, so we loop this 3x
		for (int ii = 0; ii <= 3; ii++) {
			//temp parse loop
			for (int i = ii*3; i < ii*3+3; i++) {
				unsigned int bitrep = (imu->datagram[datagramCount] << 8)
						+ (imu->datagram[datagramCount + 1]);
				imu->temp[i] = to_2C(bitrep) / 1.0 / (1 << 8);
				datagramCount += 2;
			}
			//status byte output and engage flag
			if (imu->datagram[datagramCount] != 0) {
				statusFlag = 1;
			}

			//move past status byte
			datagramCount += 1;
		}
	}
	//evaluate if datagram includes aux
	if (imu->interpAux) {
		//aux parse loop
		for (int i = 0; i < 3; i++) {
			unsigned int bitrep = (imu->datagram[datagramCount] << 16)
					+ (imu->datagram[datagramCount + 1] << 8)
					+ imu->datagram[datagramCount + 2];
			imu->aux = to_2C(bitrep) / 1.0 / (1 << 25) * 5;
			datagramCount += 3;
		}

		//status byte output and engage flag
		if (imu->datagram[datagramCount] != 0) {
			statusFlag = 1;
		}

		//move past status byte
		datagramCount += 1;
	}

	//These things (counter, latency, crc, are at the end of every data packet), so we evaluate them every loop

	//evaluate counter
	//I am not sure what this is for right now so I am just moving past it
	datagramCount += 1;

	//evaluate latency
	unsigned int bitrep = (imu->datagram[datagramCount] << 8)
			+ (imu->datagram[datagramCount + 1]);
	imu->latency = to_2C(bitrep);
	datagramCount += 2;

	//evaluate crc
	datagramCount += 3;

	//evaluate if any of the status bytes triggered the flag
	if (statusFlag) {
		//one or more of the status bytes had some kin
		return DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK;
	} else {
		//if it made it to this point, as far as the program knows the packet was successfully interpereated with no status byte issues
		return DATAGRAM_PARSE_SUCCESS;
	}

}
