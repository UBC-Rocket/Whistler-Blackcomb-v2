#ifndef _PREDICTION_H_
#define _PREDICTION_H_

#include <math.h>

#define MATRIX_SIZE 2

// Quaternion struct definition
typedef struct quaternions {
	double re;
	double i;
	double j;
	double k;
} quaternion;

quaternion qUnit();

quaternion qMult(quaternion qA, quaternion qB);

quaternion qSum(quaternion qA, quaternion qB);

quaternion qNorm(quaternion q);

quaternion qConjugate(quaternion q);

quaternion getOrientationOrder1(double deltaT, quaternion qPrev, double gx,
		double gy, double gz);

quaternion getOrientation(double deltaT, quaternion qPrev, double gx, double gy,
		double gz);

void matMult(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]);

void transpose(double mat[][MATRIX_SIZE]);

void matVecMult(double mat[][MATRIX_SIZE], double vec[], double result[]);

void scalMult(double vec[], double scal, double result[]);

void addMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]);

void addVec(double vec1[], double vec2[], double result[]);

void subtractMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]);

void subtractVec(double vec1[], double vec2[], double result[]);

void getCofactor(double A[][MATRIX_SIZE], double temp[][MATRIX_SIZE], int p,
		int q, int n);

double determinant(double A[][MATRIX_SIZE], int n);

void adjoint(double A[][MATRIX_SIZE], double adj[][MATRIX_SIZE]);

int inverse(double A[][MATRIX_SIZE], double inverse[][MATRIX_SIZE]);

void predictFilter(double deltaT, double position[], double velocity[],
		double acceleration[], double stateCovariance[][2][2],
		double processCovariance[2][2]);

void updateFilter(double position[], double velocity[], double gpsPos[],
		double gpsVel[], double stateCovariance[][2][2],
		double observationCovariance[2][2]);

#endif
