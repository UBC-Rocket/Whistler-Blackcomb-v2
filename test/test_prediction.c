#include <unity.h>
#include <prediction.h>

void compareSquareMatrix(double testArray[MATRIX_SIZE][MATRIX_SIZE], 
    double expectedArray[MATRIX_SIZE][MATRIX_SIZE]){
     for(int i=0;i<MATRIX_SIZE;i++){
        for(int ii=0;ii<MATRIX_SIZE;ii++){
            TEST_ASSERT_EQUAL_FLOAT(expectedArray[i][ii],testArray[i][ii]);
        }
    }
}

void compareQ(quaternion testQ, quaternion expectedQ){
    TEST_ASSERT_EQUAL_FLOAT(expectedQ.re, testQ.re);
    TEST_ASSERT_EQUAL_FLOAT(expectedQ.i, testQ.i);
    TEST_ASSERT_EQUAL_FLOAT(expectedQ.j, testQ.j);
    TEST_ASSERT_EQUAL_FLOAT(expectedQ.k, testQ.k);
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

void inverseTest(void){
    double testMat[][MATRIX_SIZE] = {{1.0,2.0},{3.0,4.0}};
    double resultMat[MATRIX_SIZE][MATRIX_SIZE];
    double expectedResultMat[][MATRIX_SIZE] = {{-2.0,1.0},{3.0/2,-1.0/2}};

    inverse(testMat,resultMat);
    
    compareSquareMatrix(resultMat,expectedResultMat);
}

void qMultTest(void){
    quaternion testQ0;
    testQ0.re=0;
    testQ0.i=0;
    testQ0.j=0;
    testQ0.k=0;

    quaternion testQ1;
    testQ1.re=1;
    testQ1.i=2;
    testQ1.j=3;
    testQ1.k=4;

    quaternion testQ2;
    testQ2.re=-5;
    testQ2.i=-6;
    testQ2.j=-7;
    testQ2.k=-8;

    quaternion resultQ1m0;
    quaternion resultQ1m2;
    quaternion resultQ2m1;
    quaternion resultQ1m1;
    quaternion resultQ2m2;

    quaternion expectedResultQ1m0; //Q1 multiply Q0
    expectedResultQ1m0.re=0; 
    expectedResultQ1m0.i=0;
    expectedResultQ1m0.j=0;
    expectedResultQ1m0.k=0;

    quaternion expectedResultQ1m2;
    expectedResultQ1m2.re= 60;
    expectedResultQ1m2.i=-12;
    expectedResultQ1m2.j=-30;
    expectedResultQ1m2.k=-24;

    quaternion expectedResultQ2m1;
    expectedResultQ2m1.re= 60;
    expectedResultQ2m1.i=-20;
    expectedResultQ2m1.j=-14;
    expectedResultQ2m1.k=-32;

    quaternion expectedResultQ1m1;
    expectedResultQ1m1.re= -28;
    expectedResultQ1m1.i=4;
    expectedResultQ1m1.j=6;
    expectedResultQ1m1.k=8;

    quaternion expectedResultQ2m2;
    expectedResultQ2m2.re= -124;
    expectedResultQ2m2.i=60;
    expectedResultQ2m2.j=70;
    expectedResultQ2m2.k=80;

    resultQ1m0 = qMult(testQ1,testQ0);
    resultQ1m2=qMult(testQ1,testQ2);
    resultQ2m1=qMult(testQ2,testQ1);
    resultQ1m1 = qMult(testQ1,testQ1);
    resultQ2m2 = qMult(testQ2,testQ2);

    compareQ(resultQ1m0,expectedResultQ1m0);
    compareQ(resultQ1m2,expectedResultQ1m2);
    compareQ(resultQ2m1,expectedResultQ2m1);
    compareQ(resultQ1m1,expectedResultQ1m1);
    compareQ(resultQ2m2,expectedResultQ2m2);
}

void qSumTest(void){
    quaternion testQ0;
    testQ0.re=0;
    testQ0.i=0;
    testQ0.j=0;
    testQ0.k=0;

    quaternion testQ1;
    testQ1.re=1;
    testQ1.i=2;
    testQ1.j=3;
    testQ1.k=4;

    quaternion testQ2;
    testQ2.re=-5;
    testQ2.i=-6;
    testQ2.j=-7;
    testQ2.k=-8;

    quaternion resultQ1s0;
    quaternion resultQ1s2;
    quaternion resultQ2s1;
    quaternion resultQ1s1;
    quaternion resultQ2s2;

    quaternion expectedResultQ1s0; //Q1 sum Q0
    expectedResultQ1s0.re=1; 
    expectedResultQ1s0.i=2;
    expectedResultQ1s0.j=3;
    expectedResultQ1s0.k=4;

    quaternion expectedResultQ1s2;
    expectedResultQ1s2.re= -4;
    expectedResultQ1s2.i=-4;
    expectedResultQ1s2.j=-4;
    expectedResultQ1s2.k=-4;

    quaternion expectedResultQ2s1;
    expectedResultQ2s1.re= -4;
    expectedResultQ2s1.i=-4;
    expectedResultQ2s1.j=-4;
    expectedResultQ2s1.k=-4;

    quaternion expectedResultQ1s1;
    expectedResultQ1s1.re= 2;
    expectedResultQ1s1.i=4;
    expectedResultQ1s1.j=6;
    expectedResultQ1s1.k=8;

    quaternion expectedResultQ2s2;
    expectedResultQ2s2.re= -10;
    expectedResultQ2s2.i=-12;
    expectedResultQ2s2.j=-14;
    expectedResultQ2s2.k=-16;

    resultQ1s0 = qSum(testQ1,testQ0);
    resultQ1s2=qSum(testQ1,testQ2);
    resultQ2s1=qSum(testQ2,testQ1);
    resultQ1s1 = qSum(testQ1,testQ1);
    resultQ2s2 = qSum(testQ2,testQ2);

    compareQ(resultQ1s0,expectedResultQ1s0);
    compareQ(resultQ1s2,expectedResultQ1s2);
    compareQ(resultQ2s1,expectedResultQ2s1);
    compareQ(resultQ1s1,expectedResultQ1s1);
    compareQ(resultQ2s2,expectedResultQ2s2);
}

void qNormTest(void){

    quaternion testQ1;
    testQ1.re=1;
    testQ1.i=2;
    testQ1.j=3;
    testQ1.k=4;

    quaternion testQ2;
    testQ2.re=-5;
    testQ2.i=-6;
    testQ2.j=-7;
    testQ2.k=-8;

    quaternion resultQ1 = qNorm(testQ1);

    quaternion resultQ2 = qNorm(testQ2);

    quaternion expectedResultQ1;
    expectedResultQ1.re=1.0/sqrt(30);
    expectedResultQ1.i=2.0/sqrt(30);
    expectedResultQ1.j=3.0/sqrt(30);
    expectedResultQ1.k=4.0/sqrt(30);

    quaternion expectedResultQ2;
    expectedResultQ2.re= -5.0/sqrt(174);
    expectedResultQ2.i= -6.0/sqrt(174);
    expectedResultQ2.j= -7.0/sqrt(174);
    expectedResultQ2.k= -8.0/sqrt(174);

    compareQ(resultQ1,expectedResultQ1);
    compareQ(resultQ2,expectedResultQ2);
}

void qConjugateTest(void){

    quaternion testQ0;
    testQ0.re=0;
    testQ0.i=0;
    testQ0.j=0;
    testQ0.k=0;

    quaternion testQ1;
    testQ1.re=1;
    testQ1.i=2;
    testQ1.j=3;
    testQ1.k=4;

    quaternion testQ2;
    testQ2.re=-5;
    testQ2.i=-6;
    testQ2.j=-7;
    testQ2.k=-8;

    quaternion resultQ0 = qConjugate(testQ0);

    quaternion resultQ1 = qConjugate(testQ1);

    quaternion resultQ2 = qConjugate(testQ2);

    quaternion expectedResultQ0;
    expectedResultQ0.re=0;
    expectedResultQ0.i=0;
    expectedResultQ0.j=0;
    expectedResultQ0.k=0;
    
    quaternion expectedResultQ1;
    expectedResultQ1.re=1;
    expectedResultQ1.i=-2;
    expectedResultQ1.j=-3;
    expectedResultQ1.k=-4;

    quaternion expectedResultQ2;
    expectedResultQ2.re= -5;
    expectedResultQ2.i= 6;
    expectedResultQ2.j= 7;
    expectedResultQ2.k= 8;

    compareQ(resultQ0,expectedResultQ0);
    compareQ(resultQ1,expectedResultQ1);
    compareQ(resultQ2,expectedResultQ2);
}





/*helper array of all the tests, for simplifying main() and just to try it out*/
void (*predictionTests[])(void)={
    baseTest,
    matMultTest,
    transposeTest,
    matVecMultTest,
    scalMultTest,
    addSubtractMatTest,
    addSubtractVecTest,
    inverseTest,
    qMultTest,
    qSumTest,
    qNormTest,
    qConjugateTest,
};

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    int numTests = (sizeof(predictionTests) / sizeof(*(predictionTests)));
    for(int testNum=0;testNum<numTests;testNum++){
        RUN_TEST(predictionTests[testNum]);
    }
    return UNITY_END();
}