#ifndef _IMU_INTERPRET_
#define _IMU_INTERPRET_

// Error/success codes
#define DATAGRAM_PARSE_SUCCESS						0
#define DATAGRAM_PARSE_ID_MISMATCH					1
#define DATAGRAM_PARSE_ANY_STATUS_BYTE_NOT_OK		2


// InterpretImuData takes an imu_config_t and interprets into usable double precision values. Output is mostly in .rate[], .accel[], .incl[] with index 0, 1, and 2 holding x, y, and z.


typedef struct imu_config {
	unsigned char 	interpGyro;
	/* 	0 = Setting this to zero will definitly break something. Rate is always included in datagram.
		1 = [° / s] - Angular Rate or Avg. Angular rate. [DEFAULT]
		2 = [°/sample] - Incremental Angle or Integrated angle
	*/
	unsigned char	interpAccel;
	/*	accelerometer
		0 = not included in datagram

		1 = [g] - Acceleration or Avg. Acceleration, 5g range
		2 = [g] - Acceleration or Avg. Acceleration, 10g range
		3 = [g] - Acceleration or Avg. Acceleration, 30g range
		4 = [g] - Acceleration or Avg. Acceleration, 80g range

		5 = [m/s/sample] - Incremented or Integrated Velocity, 5g range
		6 = [m/s/sample] - Incremented or Integrated Velocity, 10g range
		7 = [m/s/sample] - Incremented or Integrated Velocity, 30g range
		8 = [m/s/sample] - Incremented or Integrated Velocity, 80g range
	*/
	unsigned char	interpIncl;
	/*	inclinometer
		0 = not included in datagram
		1 = [g] - Acceleration or. Avg. Acceleration
		2 = [m/s/sample] - Incremental or Integrated Velocity
	*/
	unsigned char	interpTemp;
	/*	temperature
		0 = not included in datagram
		1 = [°C]
	*/
	unsigned char	interpAux;
	/*	auxillary 
		0 = not included in datagram
		1 = [V]
	*/

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