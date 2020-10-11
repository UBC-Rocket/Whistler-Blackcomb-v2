#include "IMU_interpret.h"

int to_2C(int value) {
	const int MODULO = 1 << 24;
	const int MAX_VALUE = (1 << 23) - 1;
	if (value > MAX_VALUE) {
		value -= MODULO;
	}
	return value;
}

int interpretImuData(unsigned char datagram[], int identifier, double dataout[], unsigned char statusBytes[]) {
	//IMPORTANT: This is not well made, so you better make sure that the length of the arrays you pass are right...

	//these variables keep track of where the program is in the datagram[], dataout[], and statusBytes[] arrays, respectivly
	int datagramCount = 0;
	int dataOutCount = 0;
	int statusCount = 0;

	//we'll use this to see if any of the status bytes are in non-OK states
	int statusFlag = 0;

	//quick check to make sure the identifier from the packet = identifier it expects.
	if (datagram[datagramCount] != identifier) {
		//return error 
		return DATAGRAM_PARSE_ID_MISMATCH;
	}

	datagramCount += 1; // move 1 byte forward in the datagram

	//gyro parse loop
	//There is no option for a normal mode datapacket without gyro data, so we evaluate it every time.
	for (int i = dataOutCount; i < dataOutCount + 3; i++) {
		unsigned int bitrep = (datagram[datagramCount] << 16) + (datagram[datagramCount + 1] << 8) + datagram[datagramCount + 2];
		dataout[i] = to_2C(bitrep) / 1.0 / (1 << 14);
		datagramCount += 3;		//move 3 bytes forward in the datagram
	}
	dataOutCount += 3;

	//status byte output and engage flag
	if (datagram[datagramCount] != 0) {
		statusFlag = 1;
	}
	statusBytes[statusCount] = datagram[datagramCount];
	statusCount++;
	//move past status byte
	datagramCount += 1;

	//evaluate if datagram includes accelerometer data
	if ((identifier == 0x91) || (identifier == 0x93) || (identifier = 0xA5) || (identifier = 0xA7) || (identifier = 0x9B) || (identifier = 0x99) || (identifier = 0xAD) || (identifier = 0xAF)) {
		//accelerometer parse loop
		for (int i = dataOutCount; i < dataOutCount + 3; i++) {
			unsigned int bitrep = (datagram[datagramCount] << 16) + (datagram[datagramCount + 1] << 8) + datagram[datagramCount + 2];
			dataout[i] = to_2C(bitrep) / 1.0 / (1 << 20);
			datagramCount += 3;
			//this only works in the "5g" range
			//IDK if that's something you set on the IMU or something that would chagnge over the course of the flight.
		}
		dataOutCount += 3;

		//status byte output and engage flag
		if (datagram[datagramCount] != 0) {
			statusFlag = 1;
		}
		statusBytes[statusCount] = datagram[datagramCount];
		statusCount++;
		//move past status byte
		datagramCount += 1;
	}


	//evaluate if datagram includes accelerometer data
	if ((identifier == 0x92) || (identifier == 0x93) || (identifier == 0xA6) || (identifier == 0xA7) || (identifier == 0x9A) || (identifier == 0x9B) || (identifier == 0xAE) || (identifier == 0xAF)) {
		//inclinometer parse loop
		//assuming that the inclinometer output unit is accel
		for (int i = dataOutCount; i < dataOutCount + 3; i++) {
			unsigned int bitrep = (datagram[datagramCount] << 16) + (datagram[datagramCount + 1] << 8) + datagram[datagramCount + 2];
			dataout[i] = to_2C(bitrep) / 1.0 / (1 << 22);
			datagramCount += 3;
		}
		dataOutCount += 3;
		//status byte output and engage flag
		if (datagram[datagramCount] != 0) {
			statusFlag = 1;
		}
		statusBytes[statusCount] = datagram[datagramCount];
		statusCount++;
		//move past status byte
		datagramCount += 1;
	}
	//evaluate if datagram includes temp
	if ((identifier == 0x94) || (identifier == 0xA5) || (identifier == 0xA6) || (identifier == 0xA7) || (identifier == 0x9C) || (identifier == 0xAD) || (identifier == 0xAE) || (identifier == 0xAF)) {
		//there are 3 different temp readings, so we loop this 3x
		for (int ii = 0; ii <= 3; ii++) {
			//temp parse loop
			for (int i = dataOutCount; i < dataOutCount + 3; i++) {
				unsigned int bitrep = (datagram[datagramCount] << 8) + (datagram[datagramCount + 1]);
				dataout[i] = to_2C(bitrep) / 1.0 / (1 << 8);
				datagramCount += 2;
			}
			dataOutCount += 3;
			//status byte output and engage flag
			if (datagram[datagramCount] != 0) {
				statusFlag = 1;
			}
			statusBytes[statusCount] = datagram[datagramCount];
			statusCount++;
			//move past status byte
			datagramCount += 1;
		}
	}
	//evaluate if datagram includes aux
	if ((identifier == 0x98) || (identifier == 0x99) || (identifier == 0x9A) || (identifier == 0x9B) || (identifier == 0x9C) || (identifier == 0xAD) || (identifier == 0xAE) || (identifier == 0xAF)) {
		//aux parse loop
		for (int i = dataOutCount; i < dataOutCount + 3; i++) {
			unsigned int bitrep = (datagram[datagramCount] << 16) + (datagram[datagramCount + 1] << 8) + datagram[datagramCount + 2];
			dataout[i] = to_2C(bitrep) / 1.0 / (1 << 25) * 5;
			datagramCount += 3;
		}
		dataOutCount += 3;

		//status byte output and engage flag
		if (datagram[datagramCount] != 0) {
			statusFlag = 1;
		}
		statusBytes[statusCount] = datagram[datagramCount];
		statusCount++;
		//move past status byte
		datagramCount += 1;
	}

	//These things (counter, latency, crc, are at the end of every data packet), so we evaluate them every loop

	//evaluate counter
	//I am not sure what this is for right now so I am just moving past it
	datagramCount += 1;

	//evaluate latency
	unsigned int bitrep = (datagram[datagramCount] << 8) + (datagram[datagramCount + 1]);
	dataout[dataOutCount] = to_2C(bitrep) / 1.0;
	datagramCount += 2;
	dataOutCount += 1;

	//evaluate crc
	datagramCount += 3;

	//evaluate if any of the status bytes triggered the flag
	if (statusFlag) {
		//one or more of the status bytes had some kin
		return DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK;
	}
	else {
		//if it made it to this point, as far as the program knows the packet was successfully interpereated with no status byte issues
		return DATAGRAM_PARSE_SUCCESS;
	}

}
