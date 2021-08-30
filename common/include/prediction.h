
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
	double re;
	double i;
	double j;
	double k;
} quaternion;

/*******************************************************************************
 * Declarations
 ******************************************************************************/

quaternion qUnit();

quaternion qMult(quaternion qA, quaternion qB);

quaternion qSum(quaternion qA, quaternion qB);

quaternion qNorm(quaternion q);

quaternion qConjugate(quaternion q);

quaternion getOrientationOrder1(double deltaT, quaternion qPrev, double gx,
		double gy, double gz);

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
quaternion getOrientation(double deltaT, quaternion qPrev, double gx, double gy,
		double gz);

void matMult(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]);

void newMatMult(float mat1[], float mat2[], float mat3[], int row_a, 
	int column_a, int column_b);

void transpose(double mat[][MATRIX_SIZE]);

void newTranspose(float mat[], int row, int column);

void matVecMult(double mat[][MATRIX_SIZE], double vec[], double result[]);

void scalMult(double vec[], double scal, double result[]);

void newScalMult(float vec[], float scal, float result[], int size);

void addMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]);

void newAddMat(float mat1[], float mat2[], float result[], int mat1_row, int mat1_col);

void addVec(double vec1[], double vec2[], double result[]);

void subtractMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]);

void newSubtractMat(float mat1[], float mat2[], float result[],
	int mat1_row, int mat1_col);

void subtractVec(double vec1[], double vec2[], double result[]);

void getCofactor(double A[][MATRIX_SIZE], double temp[][MATRIX_SIZE], int p,
		int q, int n);

double determinant(double A[][MATRIX_SIZE], int n);

float newDeterminant(float A[], int row);

void adjoint(double A[][MATRIX_SIZE], double adj[][MATRIX_SIZE]);

int inverse(double A[][MATRIX_SIZE], double inverse[][MATRIX_SIZE]);

int newInverse(float A[], float inverse[], int row);

void predictFilter(float deltaT, float position[], float velocity[],
		float acceleration[], float stateCovariance[][MATRIX_SIZE * MATRIX_SIZE],
		float processCovariance[MATRIX_SIZE * MATRIX_SIZE]);

void updateFilter(float position[], float velocity[], float gpsPos[],
		float gpsVel[], float stateCovariance[][MATRIX_SIZE * MATRIX_SIZE],
		float observationCovariance[MATRIX_SIZE * MATRIX_SIZE]);

#endif
