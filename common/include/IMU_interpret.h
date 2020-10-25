#ifndef _IMU_INTERPRET_
#define _IMU_INTERPRET_

// Error/success codes
#define DATAGRAM_PARSE_SUCCESS						0
#define DATAGRAM_PARSE_ID_MISMATCH					1
#define DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK		2


// InterpretImuData takes an imu_config_t and interprets into usable double precision values. Output is mostly in .rate[], .accel[], .incl[] with index 0, 1, and 2 holding x, y, and z.


typedef struct imu_config {
	unsigned char	interpAccel;
	unsigned char	interpIncl;
	unsigned char	interpTemp;
	unsigned char	interpAux;
	unsigned int	gyroOutUnit;
	/*	1 = [° / s] - ANGULAR RATE
		2 =
		3 =*/
	unsigned int	AccelOutUnit;
	unsigned int	inclOutUnit;
	unsigned int	sampleRate;
	unsigned int	bitRate;

	unsigned char	datagram[64];

	//data out
	//all arrays of 3 are XYZ
	double rate[3];
	unsigned char rateStatus[8];

	double accel[3];
	unsigned char accelStatus[8];

	double incl[3];
	unsigned char inclStatus[8];

	double temp[9];
		/*	
		0-2 gyro  x,y,z
		3-5 accel x,y,z
		6-8 incl  x,y,z
		*/
	unsigned char tempStatus[24];
	/*
	byte 1 gyro temp status
	byte 2 accel temp status
	byte 3 incl temp status
	*/

	double aux;

	int latency;
	unsigned char auxStatus[8];

} imu_config_t;

/**
 * Configures IMU struct with parameters. To change these parameters change
 * implementation of this function. This is a pretty ugly way to do it, might
 * want to make it more elegant somehow. 
 * @param imu the imu struct to configure
 */
void configImu(imu_config_t *imu);

int interpretImuData(imu_config_t *imu);

//int get_ID(struct imu_config_t *imu);
#endif