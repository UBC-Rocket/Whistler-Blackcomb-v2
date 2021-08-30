
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
	double recipNorm = sqrt(q.re * q.re + q.i * q.i + q.j * q.j + q.k * q.k);
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
quaternion getOrientationOrder1(double deltaT, quaternion qPrev, double gx,
		double gy, double gz) {
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
quaternion getOrientation(double deltaT, quaternion qPrev, double gx, double gy,
		double gz) {
	quaternion q;
	double norm = deltaT * sqrt(gx * gx + gy * gy + gz * gz);
	q.re = cos(norm / 2);
	double sinNorm = sin(norm / 2); // Variable to avoid repeated sin computation
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

// Matrix multiplication, assumes both matrices will be square
void matMult(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			result[i][j] = 0;
			for (int k = 0; k < MATRIX_SIZE; ++k) {
				result[i][j] += mat1[i][k] * mat2[k][j];
			}
		}
	}
}

// Matrix multiplication, assumes matrices have proper rows and columns
void newMatMult(float mat1[], float mat2[], float mat3[], int row_a, int column_a, int column_b) {
	mul(mat1, mat2, mat3, row_a, column_a, column_b);
}


// Transpose of matrix
void transpose(double mat[][MATRIX_SIZE]) {
	double temp;
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = i + 1; j < MATRIX_SIZE; ++j) {
			temp = mat[i][j];
			mat[i][j] = mat[j][i];
			mat[j][i] = temp;
		}
	}
}

void newTranspose(float mat[], int row, int column) {
	tran(mat, row, column);
}

//TODO: Replace with matMult
// Matrix and vetical vector multiplication
void matVecMult(double mat[][MATRIX_SIZE], double vec[], double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		result[i] = 0;
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			result[i] += mat[i][j] * vec[j];
		}
	}
}

// Scalar multiplication between vector and scalar
void scalMult(double vec[], double scal, double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		result[i] = vec[i] * scal;
	}
}

void newScalMult(float vec[], float scal, float result[], int size) {
	for (int i = 0; i < size; ++i) {
		result[i] = vec[i] * scal;
	}
}

// Matrix addition
void addMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			result[i][j] = mat1[i][j] + mat2[i][j];
		}
	}
}

//assumes matrices have same dimensions
void newAddMat(float mat1[], float mat2[], float result[], int mat1_row, int mat1_col) {
	for (int i = 0; i < mat1_row * mat1_col; i++) {
		result[i] = mat1[i] + mat2[i];
	}
}

//TODO: remove
// Vector addition
void addVec(double vec1[], double vec2[], double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		result[i] = vec1[i] + vec2[i];
	}
}

//TODO: remove
// Matrix subtraction
void subtractMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			result[i][j] = mat1[i][j] - mat2[i][j];
		}
	}
}

//assumes matrices have same dimensions
void newSubtractMat(float mat1[], float mat2[], float result[], int mat1_row, int mat1_col) {
	for (int i = 0; i < mat1_row * mat1_col; ++i) {
		result[i] = mat1[i] - mat2[i];
	}
}

//TODO: remove
// Vector subtraction
void subtractVec(double vec1[], double vec2[], double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		result[i] = vec1[i] - vec2[i];
	}
}

// Note: I got lazy, instead of coding inverse calculation by hand I copied it from here and then changed it a bit to handle doubles instead of ints:
// https://www.geeksforgeeks.org/adjoint-inverse-matrix/

// Function to get cofactor of A[p][q] in temp[][]. n is current 
// dimension of A[][] 
void getCofactor(double A[][MATRIX_SIZE], double temp[][MATRIX_SIZE], int p,
		int q, int n) {
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			//  Copying into temporary matrix only those element
			//  which are not in given row and column
			if (row != p && col != q) {
				temp[i][j++] = A[row][col];

				// Row is filled, so increase row index and
				// reset col index
				if (j == n - 1) {
					j = 0;
					i++;
				}
			}
		}
	}
}

/* Recursive function for finding determinant of matrix. 
 n is current dimension of A[][]. */
double determinant(double A[][MATRIX_SIZE], int n) {
	double D = 0; // Initialize result

	//  Base case : if matrix contains single element
	if (n == 1)
		return A[0][0];

	double temp[MATRIX_SIZE][MATRIX_SIZE]; // To store cofactors

	int sign = 1;  // To store sign multiplier

	// Iterate for each element of first row
	for (int f = 0; f < n; f++) {
		// Getting Cofactor of A[0][f]
		getCofactor(A, temp, 0, f, n);
		D += sign * A[0][f] * determinant(temp, n - 1);

		// terms are to be added with alternate sign
		sign = -sign;
	}

	return D;
}

float newDeterminant(float A[], int row) {
	return det(A, row);
}

// Function to get adjoint of A[N][N] in adj[N][N]. 
void adjoint(double A[][MATRIX_SIZE], double adj[][MATRIX_SIZE]) {
	if (MATRIX_SIZE == 1) {
		adj[0][0] = 1;
		return;
	}

	// temp is used to store cofactors of A[][]
	int sign = 1;
	double temp[MATRIX_SIZE][MATRIX_SIZE];

	for (int i = 0; i < MATRIX_SIZE; i++) {
		for (int j = 0; j < MATRIX_SIZE; j++) {
			// Get cofactor of A[i][j]
			getCofactor(A, temp, i, j, MATRIX_SIZE);

			// sign of adj[j][i] positive if sum of row
			// and column indexes is even.
			sign = ((i + j) % 2 == 0) ? 1 : -1;

			// Interchanging rows and columns to get the
			// transpose of the cofactor matrix
			adj[j][i] = (sign) * (determinant(temp, MATRIX_SIZE - 1));
		}
	}
}

// Function to calculate and store inverse, returns false if 
// matrix is singular 
int inverse(double A[][MATRIX_SIZE], double inverse[][MATRIX_SIZE]) {
	// Find determinant of A[][]
	double det = determinant(A, MATRIX_SIZE);
	if (det == 0) {
		return 0;
	}

	// Find adjoint
	double adj[MATRIX_SIZE][MATRIX_SIZE];
	adjoint(A, adj);

	// Find Inverse using formula "inverse(A) = adj(A)/det(A)"
	for (int i = 0; i < MATRIX_SIZE; i++)
		for (int j = 0; j < MATRIX_SIZE; j++)
			inverse[i][j] = adj[i][j] / (double) det;

	return 1;
}

int newInverse(float A[], float inverse[], int row) {
	memcpy(inverse, A, row * row * sizeof(float));
	return inv(inverse, row);
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
		newMatMult(F, xPrev, temp1, MATRIX_SIZE, MATRIX_SIZE, 1);
		newScalMult(B, u, temp2, MATRIX_SIZE);
		newAddMat(temp1, temp2, xPred, MATRIX_SIZE, 1);

		// P_k = F*P_{k-1}*F^T + Q
		newTranspose(F, MATRIX_SIZE, MATRIX_SIZE);
		newMatMult(stateCovariance[axis], F, temp3, MATRIX_SIZE, 
			MATRIX_SIZE, MATRIX_SIZE);
		newTranspose(F, MATRIX_SIZE, MATRIX_SIZE);
		newMatMult(F, temp3, temp4, MATRIX_SIZE, MATRIX_SIZE,
			MATRIX_SIZE);
		newAddMat(temp4, processCovariance, PPred, MATRIX_SIZE, MATRIX_SIZE);

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
		newSubtractMat(z, xPrev, y, MATRIX_SIZE, 1);

		//S = H*P_{k-1}*H^T + R
		newAddMat(stateCovariance[axis], observationCovariance, S,
			MATRIX_SIZE, MATRIX_SIZE);

		// K = P_{k-1}*H^T*S^{-1}
		float SInverse[2 * 2] = { 0 };
		newInverse(S, SInverse, MATRIX_SIZE);
		newMatMult(stateCovariance[axis], SInverse, K, MATRIX_SIZE,
			MATRIX_SIZE, MATRIX_SIZE);

		// x_k = x_{k-1} + K*y
		newMatMult(K, y, temp1, MATRIX_SIZE, MATRIX_SIZE, 1);
		newAddMat(xPrev, temp1, xPred, MATRIX_SIZE, 1);

		// P_k = (I - K*H)*P_{k-1}
		newSubtractMat(I, K, temp2, MATRIX_SIZE, MATRIX_SIZE);
		newMatMult(temp2, stateCovariance[axis], PPred, MATRIX_SIZE,
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
