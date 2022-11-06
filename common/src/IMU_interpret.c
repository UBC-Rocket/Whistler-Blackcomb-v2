
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <IMU_interpret.h>
#include <math.h>
#include <stdio.h>


/*******************************************************************************
 * Declarations
 ******************************************************************************/

int to_2C(int value);

int get_ID(IMU_1 *imu);

int get_dummyBytes(IMU_1 *imu);

uint crc32_lookup[256];

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void configImu(IMU_1 *imu){
	
	imu->interpGyro 	= 1;
	imu->interpAccel 	= 1;
	imu->interpIncl 	= 1;
	imu->interpTemp 	= 0;
	imu->interpAux 		= 0;
	imu->datagramID		=get_ID(imu);
	imu->dummyBytes     =get_dummyBytes(imu);
	//these guys don't do anything right now.
	imu->sampleRate 	= 125;
	imu->bitRate 		= 374400;
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

int get_ID(IMU_1 *imu) {
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

int get_dummyBytes(IMU_1 *imu) {
	//copy of the table in the STIM300 datasheet, table 6-21, pg. 36
	//kind of a gross way to implement this might clean it up later

	if(imu->datagramID == 0xB1 || imu->datagramID == 0xB3 || imu->datagramID == 0xB5 || imu->datagramID == 0xB7 
	|| imu->datagramID == 0xD1 || imu->datagramID == 0xD2 || imu->datagramID == 0x91 || imu->datagramID == 0x92 
	|| imu->datagramID == 0x99 || imu->datagramID == 0x9A) {
		return 0;
	} 
	else if(imu->datagramID == 0xA7 || imu->datagramID == 0xAF) {
		return 1;
	}
	else if(imu->datagramID == 0xBC || imu->datagramID == 0xBD || imu->datagramID == 0x90 || imu->datagramID == 0x93
	|| imu->datagramID == 0xA5 || imu->datagramID == 0xA6 || imu->datagramID == 0x98 || imu->datagramID == 0x9B ||
	imu->datagramID == 0xAD || imu->datagramID == 0xAE) {
		return 2;
	}
	else if(imu->datagramID == 0xBE || imu->datagramID == 0xBF || imu->datagramID == 0x94 || imu->datagramID == 0x9C) {
		return 3;
	}

	return 0;
}

int interpretImuData(IMU_1 *imu) {
	//IMPORTANT: This is not well made, so you better make sure that the length of the arrays you pass are right...

	//these variables keep track of where the program is in the datagram[], dataout[], and statusBytes[] arrays, respectivly
	int datagramCount = 0;

	//we'll use this to see if any of the status bytes are in non-OK states
	int statusFlag = 0;

	//we'll use this to for storing the result of our crc test
	int crcFlag = 0;

	//quick check to make sure the identifier from the packet = identifier it expects.
	if (imu->datagram[datagramCount] != imu->datagramID) {
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
		const int ACCEL_DIVISOR = 18;
		for (int i = 0; i < 3; i++) {
			signed int bitrep = (imu->datagram[datagramCount] << 16)
				+ (imu->datagram[datagramCount + 1] << 8)
				+ imu->datagram[datagramCount + 2];
			imu->accel[i] = to_2C(bitrep) / 1.0 / (1 << ACCEL_DIVISOR);
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
	unsigned int msgCrc = (imu->datagram[datagramCount] << 24) + (imu->datagram[datagramCount + 1] << 16) +  
		(imu->datagram[datagramCount + 2] << 8) + imu->datagram[datagramCount + 3];

	unsigned int expectedCrc = crc32(imu->datagram, datagramCount, imu->dummyBytes); 

	//if the 2 crcs dont' match, there's been an error so trigger the flag
	if(msgCrc != expectedCrc) {
		crcFlag = 1;
	}

	datagramCount += 4;

	//evaluate if any of the status bytes triggered the flag
	if (statusFlag) {
		//one or more of the status bytes had some kin
		return DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK;
	} else if(crcFlag) {
		//incorrect crc has been calculated
		return DATAGRAM_INCORRECT_CRC;
	} else {
		//if it made it to this point, as far as the program knows the packet was successfully interpereated with no status byte issues
		return DATAGRAM_PARSE_SUCCESS;
	}
}

void crc32_initTable(void) {
    for(int input = 0; input < 256; input++) {
        uint crc = (uint)(input << 24);

        for(int bit = 0; bit < 8; bit++) {
            if (crc & (1 << 31)) { 
                crc <<= 1;
                crc ^= POLYNOMIAL;
            }
            else {
                crc <<= 1;
            }
        }

        crc32_lookup[input] = crc;
    }
}

uint crc32(byte message[], int msgLength, int dummyBytes) {
    uint crc = SEED;

    for(int i = 0; i < msgLength + dummyBytes; i++) {
        byte curByte = (i < msgLength) ? message[i] : DUMMYBYTE; //anything after the length of our message is a dummy byte

        byte pos = (byte)((crc ^ (curByte << 24)) >> 24); 
        crc = (crc << 8) ^ (uint)crc32_lookup[pos];
    }

    return crc;
}