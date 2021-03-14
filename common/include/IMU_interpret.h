#ifndef _IMU_INTERPRET_
#define _IMU_INTERPRET_

// Error/success codes
#define DATAGRAM_PARSE_SUCCESS						0
#define DATAGRAM_PARSE_ID_MISMATCH					1
#define DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK		2
#define DATAGRAM_INCORRECT_CRC						3

//CRC32 calculation constants
#define POLYNOMIAL 0x04C11DB7
#define SEED 0xFFFFFFFF
#define DUMMYBYTE 0x00
typedef unsigned char byte;
typedef unsigned int uint;

/**
 * This struct holds all the data for the IMU, including configuartion, inputs,
 * and outputs.
 */
typedef struct IMU {
	/**
	 * the ID byte of the datagram. Generally assigned with config_IMU()
	 */
	unsigned char datagramID;

	/** 
	 *0 = Setting this to zero will definitly break something. 
	 *Rate is always included in datagram.
	 *1 = [° / s] - Angular Rate or Avg. Angular rate. [DEFAULT]
	 *2 = [°/sample] - Incremental Angle or Integrated angle
	*/
	unsigned char 	interpGyro;

	/**
	 * accelerometer
	 *	0 = not included in datagram
	 *
	 *	1 = [g] - Acceleration or Avg. Acceleration, 5g range
	 *	2 = [g] - Acceleration or Avg. Acceleration, 10g range
	 *	3 = [g] - Acceleration or Avg. Acceleration, 30g range
	 *	4 = [g] - Acceleration or Avg. Acceleration, 80g range
	 *
	 *	5 = [m/s/sample] - Incremented or Integrated Velocity, 5g range
	 *	6 = [m/s/sample] - Incremented or Integrated Velocity, 10g range
	 *	7 = [m/s/sample] - Incremented or Integrated Velocity, 30g range
	 *	8 = [m/s/sample] - Incremented or Integrated Velocity, 80g range	
	 */
	unsigned char	interpAccel;

	/**
	 *Inclinometer
	 *0 = not included in datagram
	 *1 = [g] - Acceleration or. Avg. Acceleration
	 *2 = [m/s/sample] - Incremental or Integrated Velocity
	*/
	unsigned char	interpIncl;

	/**	
	 * temperature
	 *0 = not included in datagram
 	 *1 = [°C]
	 */
	unsigned char	interpTemp;

	/**	
	 * auxillary 
	 *	0 = not included in datagram
	 *	1 = [V]
	*/
	unsigned char	interpAux;
	
	/**
	 * the sample rate of the IMU. Generally set with config_IMU but doesn't do
	 * anything right now
	 */
	unsigned int	sampleRate;

	/**
	 * the bitrate of the IMU. Generally set with config_IMU but doesn't do
	 * anything right now
	 */
	unsigned int	bitRate;

	/**
	 * The datagram to interpret.
	 */
	unsigned char	datagram[64];

	//data out
	//all arrays of 3 are XYZ
	/**
	 * rate data from IMU. Index 0,1,2 holds x, y, and z, respectively.
	 */
	double rate[3];
	unsigned char rateStatus[8];

	/**
	 * accelerometer data from IMU. Index 0,1,2 holds x, y, and z, respectively.
	 */
	double accel[3];
	unsigned char accelStatus[8];

	/**
	 * inclinometer data from IMU. Index 0,1,2 holds x, y, and z, respectively.
	 */
	double incl[3];
	unsigned char inclStatus[8];

	/**
	 * temperature data from IMU. Index 0,1,2 holds gyro temp x, y, and z, 
	 * respectively. 3-5 is accelerometer xyz, and 6-8 is inclinometer xyz.
	 */
	double temp[9];

	unsigned char tempStatus[24];
	/*
	byte 1 gyro temp status
	byte 2 accel temp status
	byte 3 incl temp status
	*/

	/**
	 *The aux output of the IMU. 
	 */
	double aux;

	unsigned char auxStatus[8];

	/**
	 * IMU latency
	 */
	int latency;

	/**
	 * The number of dummy bytes to append to the end of the datagram when calculating CRC.
	*/
	int dummyBytes;
} IMU_1;

/**
 * Configures IMU struct with parameters. To change these parameters change
 * implementation of this function. This is a pretty ugly way to do it, might
 * want to make it more elegant somehow. 
 * @param imu the imu struct to configure
 */
void configImu(IMU_1 *imu);

/**
 * Takes data from the IMU and turns it into usable double values. 
 * @param imu the imu struct to take data from.
 * @return Either DATAGRAM_PARSE_SUCCESS, DATAGRAM_PARSE_ID_MISMATCH, or 
 * DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK. The real data -- acceleration, 
 * temperature, etc, -- is all in the struct. rate[], accel[], incl[] with index 
 * 0, 1, and 2 hold x, y, and z values, respectively. temp[] 0-2 holds gyro temps 
 * x,y,z; 3-5 holds accelerometer temps x,y,z; and 6-8 holds inclinometer temps 
 * x,y,z. aux holds the auxillary output, and latency holds the latency.
 */
int interpretImuData(IMU_1 *imu);

/**
 * The 32-bit Cyclic Redundancy Checksum (CRC32) is calculated byte-by-byte which means we can precompute solutions 
 * in a 256-length lookup table (which is what we do here).
 * 
 * The CRC checksum is calculated crc = message % polynomial. Message and polynomial (binary numbers) are 
 * interpeted as polynomials (ex. 0b1101 -> x^3 + x^2 + 1) and "polynomial division" is peformed. 
 * (with some rule changes) This is intuitively pretty confusing, but at least the implementation isn't that complicated.
 * The remainder is our checksum. For more info about implementation, this article is probably the best: 
 * http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
 */
void crc32_initTable(void);

/*
 * Calculate CRC32 checkum by accessing our lookup table.
 * @param message The message we calculate the checksum for
 * @param msgLength The length of our message in bytes
 * @param dummyBytes As per the datasheet, the CRC32 algorithm requires a message that is an a multiple of  32 bits (4 bytes).
 * Some datagrams don't match this requirement, so for an easy fix 1-3 dummy bytes (content = 0x00) are appended to the end of 
 * the message. See datasheet pg. 35 for more info.
*/
uint crc32(byte[], int, int);

#endif