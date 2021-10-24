
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <prediction.h>
#include <math.h>
#include "Functions.h"

/*******************************************************************************
 * Implementations
 ******************************************************************************/

// Unit quaternion
quaternion qUnit() {
	quaternion ret;
	ret.re = 1;
	ret.i = 0;
	ret.j = 0;
	ret.k = 0;
	return ret;
}

// Quaternion multiplication
quaternion qMult(quaternion qA, quaternion qB) {
	quaternion qC;
	qC.re = qA.re * qB.re - qA.i * qB.i - qA.j * qB.j - qA.k * qB.k;
	qC.i = qA.re * qB.i + qA.i * qB.re + qA.j * qB.k - qA.k * qB.j;
	qC.j = qA.re * qB.j - qA.i * qB.k + qA.j * qB.re + qA.k * qB.i;
	qC.k = qA.re * qB.k + qA.i * qB.j - qA.j * qB.i + qA.k * qB.re;
	return qC;
}

// Quaternion summation
quaternion qSum(quaternion qA, quaternion qB) {
	quaternion qC;
	qC.re = qA.re + qB.re;
	qC.i = qA.i + qB.i;
	qC.j = qA.j + qB.j;
	qC.k = qA.k + qB.k;
	return qC;
}

// Returns a normalized quaternion
quaternion qNorm(quaternion q) {
	quaternion qReturn;
	float recipNorm = sqrt(q.re * q.re + q.i * q.i + q.j * q.j + q.k * q.k);
	qReturn.re = q.re / recipNorm;
	qReturn.i = q.i / recipNorm;
	qReturn.j = q.j / recipNorm;
	qReturn.k = q.k / recipNorm;
	return qReturn;
}

quaternion qConjugate(quaternion q) {
	q.i *= -1;
	q.j *= -1;
	q.k *= -1;
	return q;
}

// Gets the orientation of quaternion from 1st order polynomial approximation for quaternions exponentiation. See here for details: 
// https://www.ashwinnarayan.com/post/how-to-integrate-quaternions/
// Is computationally cheaper, so if performance is an issue we could use this 
quaternion getOrientationOrder1(float deltaT, quaternion qPrev, float gx,
		float gy, float gz) {
	gx *= deltaT / 2;
	gy *= deltaT / 2;
	gz *= deltaT / 2;
	quaternion omega;
	omega.re = 0;
	omega.i = gx;
	omega.j = gy;
	omega.k = gz;
	return qNorm(qSum(qPrev, qMult(omega, qPrev)));
}

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
		float gz) {
	quaternion q;
	float norm = deltaT * sqrt(gx * gx + gy * gy + gz * gz);
	q.re = cos(norm / 2);
	float sinNorm = sin(norm / 2); // Variable to avoid repeated sin computation
	/* Check to make sure we don't get a divide by 0 error. 
	   Note that in the case that norm is 0 we want the vector component
	   of q to be zeros because this represents no rotation (i.e. multiplication
	   by unity)*/
	if (sinNorm != 0) {
		q.i = deltaT * gx / norm * sinNorm;
		q.j = deltaT * gy / norm * sinNorm;
		q.k = deltaT * gz / norm * sinNorm;
	} else {
		q.i = 0;
		q.j = 0;
		q.k = 0;
	}
	// Adjust our previous estimate of what the rotation quaternion is
	return qMult(q, qPrev);
}

// Matrix multiplication, assumes matrices have proper rows and columns
void matMult(float mat1[], float mat2[], float mat3[], int row_a, int column_a, int column_b) {
	mul(mat1, mat2, mat3, row_a, column_a, column_b);
}

void transpose(float mat[], int row, int column) {
	tran(mat, row, column);
}

void scalMult(float vec[], float scal, float result[], int size) {
	for (int i = 0; i < size; ++i) {
		result[i] = vec[i] * scal;
	}
}

//assumes matrices have same dimensions
void addMat(float mat1[], float mat2[], float result[], int mat1_row, int mat1_col) {
	for (int i = 0; i < mat1_row * mat1_col; i++) {
		result[i] = mat1[i] + mat2[i];
	}
}

//assumes matrices have same dimensions
void subtractMat(float mat1[], float mat2[], float result[], int mat1_row, int mat1_col) {
	for (int i = 0; i < mat1_row * mat1_col; ++i) {
		result[i] = mat1[i] - mat2[i];
	}
}

float determinant(float A[], int row) {
	return det(A, row);
}

int inverse(float A[], float inverse[], int row) {
	if (fabs(det(A, row)) < FLT_EPSILON) {
		return 0;
	}

	memcpy(inverse, A, row * row * sizeof(float));
	int status = inv(inverse, row);
	return status;
}

// Predict phase of Kalman filter
void predictFilter(float deltaT, float position[], float velocity[],
		float acceleration[], float stateCovariance[][MATRIX_SIZE * MATRIX_SIZE],
		float processCovariance[MATRIX_SIZE * MATRIX_SIZE]) {
	// State Transition Matrix, derived from kinematics (to be multiplied by state vector of position and velocity)
	float F[MATRIX_SIZE * MATRIX_SIZE] = {1, deltaT, 0,1};
	// Control input matrix, derived from kinematics (to be multiplied by acceleration)
	float B[MATRIX_SIZE] = { 0.5f * deltaT * deltaT, deltaT };

	// Iterate through values in x, y, z order
	for (int axis = 0; axis < 3; axis++) {
		// Previous estimate of state
		float xPrev[] = { position[axis], velocity[axis] };
		// Predicted estimate of state after given observations
		float xPred[2] = { 0 };
		// Control vector
		float u = acceleration[axis];
		// Predicted covariance matrix
		float PPred[MATRIX_SIZE * MATRIX_SIZE] = { 0 };

		// Temp vectors to calculate next state
		float temp1[MATRIX_SIZE] = { 0 };
		float temp2[MATRIX_SIZE] = { 0 };
		float temp3[MATRIX_SIZE * MATRIX_SIZE] = { 0 };
		float temp4[MATRIX_SIZE * MATRIX_SIZE] = { 0 };

		// x_k = F*x_{k-1} + B*u
		matMult(F, xPrev, temp1, MATRIX_SIZE, MATRIX_SIZE, 1);
		scalMult(B, u, temp2, MATRIX_SIZE);
		addMat(temp1, temp2, xPred, MATRIX_SIZE, 1);

		// P_k = F*P_{k-1}*F^T + Q
		transpose(F, MATRIX_SIZE, MATRIX_SIZE);
		matMult(stateCovariance[axis], F, temp3, MATRIX_SIZE, 
			MATRIX_SIZE, MATRIX_SIZE);
		transpose(F, MATRIX_SIZE, MATRIX_SIZE);
		matMult(F, temp3, temp4, MATRIX_SIZE, MATRIX_SIZE,
			MATRIX_SIZE);
		addMat(temp4, processCovariance, PPred, MATRIX_SIZE, MATRIX_SIZE);

		// Write back predictions to original variables
		position[axis] = xPred[0];
		velocity[axis] = xPred[1];
		for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; ++i) {
			stateCovariance[axis][i] = PPred[i];
		}
	}
}

// Update phase of Kalman filter
void updateFilter(float position[], float velocity[], float gpsPos[],
		float gpsVel[], float stateCovariance[][MATRIX_SIZE * MATRIX_SIZE],
		float observationCovariance[MATRIX_SIZE * MATRIX_SIZE]) {

	// Iterate through values in x, y, z order
	for (int axis = 0; axis < 3; axis++) {
		// Measurement of true state, i.e. gps reading
		float z[2] = { gpsPos[axis], gpsVel[axis] };
		// Previous estimate of state
		float xPrev[2] = { position[axis], velocity[axis] };
		// Predicted estimate of state after given observations
		float xPred[2] = { 0 };
		// Predicted covariance matrix
		float PPred[MATRIX_SIZE * MATRIX_SIZE] = { 0 };
		// Innovation
		float y[2] = { 0 };
		// Inovation covariance
		float S[2 * 2] = { 0 };
		// Kalman gain
		float K[2 * 2] = { 0 };
		// Identity
		float I[2 * 2] = { 1, 0, 0, 1 };
		// Temp variables for equations
		float temp1[2];
		float temp2[2 * 2];

		// y = z - H*x_{k-1}, here H is the identity because we are directly measuring position and velocity through GPS
		subtractMat(z, xPrev, y, MATRIX_SIZE, 1);

		//S = H*P_{k-1}*H^T + R
		addMat(stateCovariance[axis], observationCovariance, S,
			MATRIX_SIZE, MATRIX_SIZE);

		// K = P_{k-1}*H^T*S^{-1}
		float SInverse[2 * 2] = { 0 };
		inverse(S, SInverse, MATRIX_SIZE);
		matMult(stateCovariance[axis], SInverse, K, MATRIX_SIZE,
			MATRIX_SIZE, MATRIX_SIZE);

		// x_k = x_{k-1} + K*y
		matMult(K, y, temp1, MATRIX_SIZE, MATRIX_SIZE, 1);
		addMat(xPrev, temp1, xPred, MATRIX_SIZE, 1);

		// P_k = (I - K*H)*P_{k-1}
		subtractMat(I, K, temp2, MATRIX_SIZE, MATRIX_SIZE);
		matMult(temp2, stateCovariance[axis], PPred, MATRIX_SIZE,
			MATRIX_SIZE, MATRIX_SIZE);

		// Write back predictions to original variables
		position[axis] = xPred[0];
		velocity[axis] = xPred[1];
		for (int i = 0; i < 4; ++i) {
			stateCovariance[axis][i] = PPred[i];
		}
	}
}

// int main(void){
//     // double pos[] = {1, 1, 1};
//     // double vel[] = {1, 1, 1};
//     double accel[] = {1, 0, 1};
//     // double deltaT = 0.5;
//     // double covariance[3][2][2] = {0};
//     // double gpsPos[] = {350, 350, 350};
//     // double gpsVel[] = {30, 30, 30};
//     // // Covariance of process (imu) and observation (gps) noise respectively. For now these are static, although they don't have to be and if we come up with a good way of 
//     // // dynamically estimating covariance we can change them for each time step
//     // double Q[][2] = {{0.01, 0},
//     //                 {0, 0.01}};
//     // double R[][2] = {{0.5, 0},
//     //                 {0, 0.5}};
//     // for(int i = 0; i < 50; ++i)
//     //     predictFilter(deltaT, pos, vel, accel, covariance, Q);
//     // updateFilter(pos, vel, gpsPos, gpsVel, covariance, R);
//     // printVec(pos, 3);
//     // printVec(vel, 3);
//     // printMat(covariance[0]);
//     quaternion qA, qB, qC;
//     qA.q0 = 1;
//     qA.q1 = 0;
//     qA.q2 = 0;
//     qA.q3 = 0;
//     qB.q0 = 0;
//     qB.q1 = accel[0];
//     qB.q2 = accel[1];
//     qB.q3 = accel[2];

//     // for(int i = 0; i < 50; ++i){
//     //     qA = getOrientationOrder1(1.0/50, qA, 1, 0, 0);
//     //     printQuaternion(qA);
//     // }
//     qA = getOrientation(1.0, qA, 1, 0, 0);
//     qB = qMult(qMult(qA, qB), qConjugate(qA));
//     printQuaternion(qB);

//     return 0;
// }
