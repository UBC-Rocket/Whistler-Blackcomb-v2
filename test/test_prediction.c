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
    double testMat1[][MATRIX_SIZE] = {{1,2},{3,4}};
    double testMat2[][MATRIX_SIZE] = {{5,6},{7,8}};
    double expectedResult[][MATRIX_SIZE] = {{19,22},{43,50}};
    double result[MATRIX_SIZE][MATRIX_SIZE];
    
    matMult(testMat1,testMat2,result);

    compareSquareMatrix(result,expectedResult);
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
    return UNITY_END();
}