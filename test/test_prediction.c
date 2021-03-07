#include <unity.h>
#include <prediction.h>

void compareSquareMatrix(double testArray[MATRIX_SIZE][MATRIX_SIZE], 
    double expectedArray[MATRIX_SIZE][MATRIX_SIZE]){
     for(int i=0;i<MATRIX_SIZE;i++){
        for(int ii=0;ii<MATRIX_SIZE;ii++){
            TEST_ASSERT_EQUAL(expectedArray[i][ii],testArray[i][ii]);
        }
    }
}

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}
void matMultTest(void){
    //for this one, I tried more tests. Hopefully all testing different cases here.

    int numMats = 4;
    double testMats[4][MATRIX_SIZE][MATRIX_SIZE] = {
        {{1,2},{3,4}},
        {{-5,-6},{-7,-8}},
        {{0,0},{0,0}},
        {{pow(10,5),10},{pow(10,-5),-10}}
        };
    double expectedResults[][MATRIX_SIZE][MATRIX_SIZE] = {
        {{-19,-22},{-43,-50}},
        {{0,0},{0,0}},
        {{5000000001/50000, -10}, {7500000001/25000, -10}},
        {{0,0},{0,0}},
        {{-25000000003/50000, 10}, {-8750000001/12500, 10}},
        {{0,0},{0,0}}
        };
    double result[MATRIX_SIZE][MATRIX_SIZE];
    
    int resultIndex=0;
    for(int i=0;i<numMats;i++){
        for(int ii=i+1;ii<numMats;ii++){
            matMult(testMats[i],testMats[ii],result);
            compareSquareMatrix(result,expectedResults[resultIndex]);
            resultIndex++;
        }
    }
}

void transposeTest(void){
    double testMat[2][2] = {{1,2},{3,4}};
    double expectedResult[MATRIX_SIZE][MATRIX_SIZE] = {{1,3},{2,4}};

    transpose(testMat);

    compareSquareMatrix(testMat,expectedResult);
}

void matVecMultTest(void){
    double testMat[][MATRIX_SIZE] = {{1,2},{3,4}};
    double testVec[MATRIX_SIZE] = {5,6};
    double resultVec[MATRIX_SIZE];

    double expectedVec[MATRIX_SIZE] = {17,39};

    matVecMult(testMat,testVec,resultVec);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedVec,resultVec,MATRIX_SIZE);
}

void scalMultTest(void){
    double testVec[MATRIX_SIZE]={1,2};
    double testScal = 3;
    double resultVec[MATRIX_SIZE];

    double expectedVec[MATRIX_SIZE] = {3,6};

    scalMult(testVec,testScal,resultVec);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedVec,resultVec,MATRIX_SIZE);
}

void addSubtractMatTest(void){
    double testMat1[][MATRIX_SIZE] = {{1,2},{3,4}};
    double testMat2[][MATRIX_SIZE] = {{5,6},{7,8}};
    double resultMat[MATRIX_SIZE][MATRIX_SIZE];

    double expectedAddMat[MATRIX_SIZE][MATRIX_SIZE] = {{6,8},{10,12}};
    double expectedSubMat[MATRIX_SIZE][MATRIX_SIZE] = {{-4,-4},{-4,-4}};

    addMat(testMat1,testMat2,resultMat);

    compareSquareMatrix(resultMat,expectedAddMat);

    subtractMat(testMat1,testMat2,resultMat);

    compareSquareMatrix(resultMat,expectedSubMat);
}

void addSubtractVecTest(void){
    double testVec1[MATRIX_SIZE] = {1,2};
    double testVec2[MATRIX_SIZE] = {3,4};
    double resultVec[MATRIX_SIZE];

    double expectedAddVec[MATRIX_SIZE] = {4,6};
    double expectedSubVec[MATRIX_SIZE] = {-2,-2};

    addVec(testVec1,testVec2,resultVec);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedAddVec,resultVec,MATRIX_SIZE);

    subtractVec(testVec1,testVec2,resultVec);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedSubVec,resultVec,MATRIX_SIZE);
}

void adjointTest(void){
    double testMat[][MATRIX_SIZE] = {{1,2},{3,4}};
    double resultMat[MATRIX_SIZE][MATRIX_SIZE];
    double expectedResultMat[][MATRIX_SIZE] = {{4,-2},{-3,1}};

    adjoint(testMat,resultMat);
    
    compareSquareMatrix(resultMat,expectedResultMat);
}

void inverseTest(void){
    double testMat[][MATRIX_SIZE] = {{1,2},{3,4}};
    double resultMat[MATRIX_SIZE][MATRIX_SIZE];
    double expectedResultMat[][MATRIX_SIZE] = {{-2,1},{3/2,-1/2}};

    inverse(testMat,resultMat);
    
    compareSquareMatrix(resultMat,expectedResultMat);
}
/**
 * Qs:
 * WTF is cofactor
 * What's n in determinant
 * what's adjoint

*/


void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(matMultTest);
    RUN_TEST(transposeTest);
    RUN_TEST(matVecMultTest);
    RUN_TEST(scalMultTest);
    RUN_TEST(addSubtractMatTest);
    RUN_TEST(addSubtractVecTest);
    RUN_TEST(adjointTest);
    RUN_TEST(inverseTest);
    return UNITY_END();
}