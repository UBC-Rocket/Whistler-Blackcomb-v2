
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "prediction.h"
#include <math.h>

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

// Gets orientation of quaternion the "proper" way
// This is the best explanation I could find of how it works: 
// https://math.stackexchange.com/questions/39553/how-do-i-apply-an-angular-velocity-vector3-to-a-unit-quaternion-orientation
quaternion getOrientation(double deltaT, quaternion qPrev, double gx, double gy,
		double gz) {
	quaternion q;
	double norm = deltaT * sqrt(gx * gx + gy * gy + gz * gz);
	q.re = cos(norm / 2);
	double sinNorm = sin(norm / 2); // Variable to avoid repeated sin computation
	// Check to make sure we don't get a divide by 0 error. Note that in the case that norm is 0 we want the vector component
	// of q to be zeros because this represents no rotation (i.e. multiplication by unity)
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

// Matrix and vector multiplication
void matVecMult(double mat[][MATRIX_SIZE], double vec[], double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		result[i] = 0;
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			result[i] += mat[i][j] * vec[j];
		}
	}
}

// Scalar multiplication betwee vector and scalar
void scalMult(double vec[], double scal, double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
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

// Vector addition
void addVec(double vec1[], double vec2[], double result[]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		result[i] = vec1[i] + vec2[i];
	}
}

// Matrix subtraction
void subtractMat(double mat1[][MATRIX_SIZE], double mat2[][MATRIX_SIZE],
		double result[][MATRIX_SIZE]) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			result[i][j] = mat1[i][j] - mat2[i][j];
		}
	}
}

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

// Predict phase of Kalman filter
void predictFilter(double deltaT, double position[], double velocity[],
		double acceleration[], double stateCovariance[][2][2],
		double processCovariance[2][2]) {
	// State Transition Matrix, derived from kinematics (to be multiplied by state vector of position and velocity)
	double F[][2] = { { 1, deltaT }, { 0, 1 } };
	// Control input matrix, derived from kinematics (to be multiplied by acceleration)
	double B[] = { 0.5f * deltaT * deltaT, deltaT };

	// Iterate through values in x, y, z order
	for (int axis = 0; axis < 3; axis++) {
		// Previous estimate of state
		double xPrev[] = { position[axis], velocity[axis] };
		// Predicted estimate of state after given observations
		double xPred[2] = { 0 };
		// Control vector
		double u = acceleration[axis];
		// Predicted covariance matrix
		double PPred[2][2] = { 0 };

		// Temp vectors to calculate next state
		double temp1[2] = { 0 };
		double temp2[2] = { 0 };
		double temp3[2][2] = { 0 };
		double temp4[2][2] = { 0 };

		// x_k = F*x_{k-1} + B*u
		matVecMult(F, xPrev, temp1);
		scalMult(B, u, temp2);
		addVec(temp1, temp2, xPred);

		// P_k = F*P_{k-1}*F^T + Q
		transpose(F);
		matMult(stateCovariance[axis], F, temp3);
		transpose(F);
		matMult(F, temp3, temp4);
		addMat(temp4, processCovariance, PPred);

		// Write back predictions to original variables
		position[axis] = xPred[0];
		velocity[axis] = xPred[1];
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				stateCovariance[axis][i][j] = PPred[i][j];
			}
		}
	}
}

// Update phase of Kalman filter
void updateFilter(double position[], double velocity[], double gpsPos[],
		double gpsVel[], double stateCovariance[][2][2],
		double observationCovariance[2][2]) {

	// Iterate through values in x, y, z order
	for (int axis = 0; axis < 3; axis++) {
		// Measurement of true state, i.e. gps reading
		double z[2] = { gpsPos[axis], gpsVel[axis] };
		// Previous estimate of state
		double xPrev[] = { position[axis], velocity[axis] };
		// Predicted estimate of state after given observations
		double xPred[2] = { 0 };
		// Predicted covariance matrix
		double PPred[2][2] = { 0 };
		// Innovation
		double y[2] = { 0 };
		// Inovation covariance
		double S[2][2] = { 0 };
		// Kalman gain
		double K[2][2] = { 0 };
		// Identity
		double I[2][2] = { { 1, 0 }, { 0, 1 } };
		// Temp variables for equations
		double temp1[2];
		double temp2[2][2];

		// y = z - H*x_{k-1}, here H is the identity because we are directly measuring position and velocity through GPS
		subtractVec(z, xPrev, y);

		//S = H*P_{k-1}*H^T + R
		addMat(stateCovariance[axis], observationCovariance, S);

		// K = P_{k-1}*H^T*S^{-1}
		double SInverse[2][2] = { 0 };
		inverse(S, SInverse);
		matMult(stateCovariance[axis], SInverse, K);

		// x_k = x_{k-1} + K*y
		matVecMult(K, y, temp1);
		addVec(xPrev, temp1, xPred);

		// P_k = (I - K*H)*P_{k-1}
		subtractMat(I, K, temp2);
		matMult(temp2, stateCovariance[axis], PPred);

		// Write back predictions to original variables
		position[axis] = xPred[0];
		velocity[axis] = xPred[1];
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				stateCovariance[axis][i][j] = PPred[i][j];
			}
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
