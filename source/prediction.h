
#ifdef __linux__
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#else
//#include <Arduino.h>
#include <math.h>

#endif

#define MATRIX_SIZE 2

// Quaternion struct definition
typedef struct quaternions {
    float q0;
    float q1;
    float q2;
    float q3;
} quaternion;

void printMat(float mat[][MATRIX_SIZE]);

void printVec(float vec[], int size);

void printQuaternion(quaternion q);

quaternion qMult(quaternion qA, quaternion qB);

quaternion qSum(quaternion qA, quaternion qB);

quaternion qNorm(quaternion q);

quaternion qConjugate(quaternion q);

quaternion getOrientationOrder1(float deltaT, quaternion qPrev, float gx, float gy, float gz);

quaternion getOrientation(float deltaT, quaternion qPrev, float gx, float gy, float gz);

void matMult(float mat1[][MATRIX_SIZE], float mat2[][MATRIX_SIZE], float result[][MATRIX_SIZE]);

void transpose(float mat[][MATRIX_SIZE]);

void matVecMult(float mat[][MATRIX_SIZE], float vec[], float result[]);

void scalMult(float vec[], float scal, float result[]);

void addMat(float mat1[][MATRIX_SIZE], float mat2[][MATRIX_SIZE], float result[][MATRIX_SIZE]);

void addVec(float vec1[], float vec2[], float result[]);

void subtractMat(float mat1[][MATRIX_SIZE], float mat2[][MATRIX_SIZE], float result[][MATRIX_SIZE]);

void subtractVec(float vec1[], float vec2[], float result[]);

void getCofactor(float A[][MATRIX_SIZE], float temp[][MATRIX_SIZE], int p, int q, int n);

float determinant(float A[][MATRIX_SIZE], int n);

void adjoint(float A[][MATRIX_SIZE],float adj[][MATRIX_SIZE]);

int inverse(float A[][MATRIX_SIZE], float inverse[][MATRIX_SIZE]);

void predictFilter(float deltaT, float position[], float velocity[], float acceleration[], float stateCovariance[][2][2], float processCovariance[2][2]);

void updateFilter(float position[], float velocity[], float gpsPos[], float gpsVel[], float stateCovariance[][2][2], float observationCovariance[2][2]);

