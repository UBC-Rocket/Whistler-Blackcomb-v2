
#ifndef _PREDICTION_H_
#define _PREDICTION_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MATRIX_SIZE 2
#define PI acos(-1)


// Quaternion struct definition
typedef struct quaternions {
	float re;
	float i;
	float j;
	float k;
} quaternion;

/*******************************************************************************
 * Declarations
 ******************************************************************************/

quaternion qUnit();

quaternion qMult(quaternion qA, quaternion qB);

quaternion qSum(quaternion qA, quaternion qB);

quaternion qNorm(quaternion q);

quaternion qConjugate(quaternion q);

quaternion getOrientationOrder1(float deltaT, quaternion qPrev, float gx,
		float gy, float gz);

/**
 * produces a quaternion representing orientation based on a previous orientation
 * and a 3d angular velocity vector
 * 
 * based on https://math.stackexchange.com/questions/39553/how-do-i-apply-an-angular-velocity-vector3-to-a-unit-quaternion-orientation
 *
 * @param deltaT time interval
 * @param qPrev quaternion representing the previous orientation
 * @param gx x component of angular velocity (rads/sec)
 * @param gy y component of angular velocity (rads/sec)
 * @param gz z component of angular velocity (rads/sec)
 * @return new quaternion representing orientation 
 */
quaternion getOrientation(float deltaT, quaternion qPrev, float gx, float gy,
		float gz);

void matMult(float mat1[], float mat2[], float mat3[], int row_a, 
	int column_a, int column_b);

void transpose(float mat[], int row, int column);

void scalMult(float vec[], float scal, float result[], int size);

void addMat(float mat1[], float mat2[], float result[], int mat1_row, int mat1_col);

void subtractMat(float mat1[], float mat2[], float result[],
	int mat1_row, int mat1_col);

float determinant(float A[], int row);

int inverse(float A[], float inverse[], int row);

void predictFilter(float deltaT, float position[], float velocity[],
		float acceleration[], float stateCovariance[][MATRIX_SIZE * MATRIX_SIZE],
		float processCovariance[MATRIX_SIZE * MATRIX_SIZE]);

void updateFilter(float position[], float velocity[], float gpsPos[],
		float gpsVel[], float stateCovariance[][MATRIX_SIZE * MATRIX_SIZE],
		float observationCovariance[MATRIX_SIZE * MATRIX_SIZE]);

#endif
