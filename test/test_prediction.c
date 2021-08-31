#include <unity.h>
#include <prediction.h>

void compareSquareMatrix(float testArray[MATRIX_SIZE * MATRIX_SIZE],
    float expectedArray[MATRIX_SIZE * MATRIX_SIZE]) {
        for (int i=0;i<MATRIX_SIZE;i++) {
            TEST_ASSERT_EQUAL_FLOAT(expectedArray[i],testArray[i]);
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
    //testing CControl functions.

    int numMats = 4;
    float testMats[][MATRIX_SIZE * MATRIX_SIZE] = {
        {1,2,3,4},
        {-5,-6,-7,-8},
        {0,0,0,0},
        {pow(10,5),10,pow(10,-5),-10}
        };
    float expectedResults[][MATRIX_SIZE * MATRIX_SIZE] = {
        {-19,-22,-43,-50},
        {0,0,0,0},
        {5000000001/50000, -10, 7500000001/25000, -10},
        {0,0,0,0},
        {-25000000003/50000, 10, -8750000001/12500, 10},
        {0,0,0,0}
        };
    float result[MATRIX_SIZE * MATRIX_SIZE];
    
    int resultIndex=0;
    for(int i=0;i<numMats;i++){
        for(int j=i+1;j<numMats;j++){
            matMult(testMats[i],testMats[j], result, MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE);
            for (int k = 0; k < sizeof(result) / sizeof(result[0]); k++) {
                TEST_ASSERT_EQUAL_FLOAT(result[k], expectedResults[resultIndex][k]);
            }
            resultIndex++;
        }
    }
}

void transposeTest(void){
    float testMat[MATRIX_SIZE * MATRIX_SIZE] = {1,2,3,4};
    float expectedResult[MATRIX_SIZE * MATRIX_SIZE] = {1,3,2,4};

    transpose(testMat, MATRIX_SIZE, MATRIX_SIZE);

    compareSquareMatrix(testMat, expectedResult);
}

void matVecMultTest(void){
    float testMat[MATRIX_SIZE * MATRIX_SIZE] = {1,2,3,4};
    float testVec[MATRIX_SIZE] = {5,6};
    float resultVec[MATRIX_SIZE];

    float expectedVec[MATRIX_SIZE] = {17,39};

    matMult(testMat, testVec, resultVec, MATRIX_SIZE, MATRIX_SIZE, 1);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedVec,resultVec,MATRIX_SIZE);
}

void scalMultTest(void){
    float testVec[MATRIX_SIZE]={1,2};
    float testScal = 3;
    float resultVec[MATRIX_SIZE];

    float expectedVec[MATRIX_SIZE] = {3,6};

    scalMult(testVec,testScal,resultVec, MATRIX_SIZE);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedVec,resultVec,MATRIX_SIZE);
}

void addSubtractMatTest(void){
    float testMat1[MATRIX_SIZE * MATRIX_SIZE] = {1,2,3,4};
    float testMat2[MATRIX_SIZE * MATRIX_SIZE] = {5,6,7,8};
    float resultMat[MATRIX_SIZE * MATRIX_SIZE];

    float expectedAddMat[MATRIX_SIZE * MATRIX_SIZE] = {6,8,10,12};
    float expectedSubMat[MATRIX_SIZE * MATRIX_SIZE] = {-4,-4,-4,-4};

    addMat(testMat1,testMat2,resultMat, MATRIX_SIZE, MATRIX_SIZE);

    compareSquareMatrix(resultMat,expectedAddMat);

    subtractMat(testMat1,testMat2,resultMat, MATRIX_SIZE, MATRIX_SIZE);

    compareSquareMatrix(resultMat,expectedSubMat);
}

void addSubtractVecTest(void){
    float testVec1[MATRIX_SIZE] = {1,2};
    float testVec2[MATRIX_SIZE] = {3,4};
    float resultVec[MATRIX_SIZE];

    float expectedAddVec[MATRIX_SIZE] = {4,6};
    float expectedSubVec[MATRIX_SIZE] = {-2,-2};

    addMat(testVec1,testVec2,resultVec, MATRIX_SIZE, MATRIX_SIZE);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedAddVec,resultVec,MATRIX_SIZE);

    subtractMat(testVec1,testVec2,resultVec, MATRIX_SIZE, MATRIX_SIZE);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedSubVec,resultVec,MATRIX_SIZE);
}

void determinantTest(void){
    float testMat[MATRIX_SIZE * MATRIX_SIZE] = {1.0,2.0, 3.0,4.0};
    float expectedResult = -2.0;
    float testResult = determinant(testMat, MATRIX_SIZE);

    TEST_ASSERT_EQUAL_FLOAT(expectedResult, testResult);
}

void inverseTest(void){
    float testMat[MATRIX_SIZE * MATRIX_SIZE] = {1.0,2.0, 3.0,4.0};
    float resultMat[MATRIX_SIZE * MATRIX_SIZE];
    float expectedResultMat[MATRIX_SIZE * MATRIX_SIZE] = {-2.0,1.0, 3.0/2,-1.0/2};
    
    int status = inverse(testMat,resultMat, MATRIX_SIZE);
    
    compareSquareMatrix(resultMat,expectedResultMat);
    TEST_ASSERT_EQUAL_INT(1, status);
}

//bandaid fix on inverse function to make this properly fail.
void failInverseTest(void){
    float testMat[MATRIX_SIZE * MATRIX_SIZE] = {1.0,2.0, 1.0,2.0};
    float resultMat[MATRIX_SIZE * MATRIX_SIZE];
    
    int det = determinant(testMat, MATRIX_SIZE);
    int status = inverse(testMat, resultMat, MATRIX_SIZE);
    
    TEST_ASSERT_EQUAL_INT(0, det);
    TEST_ASSERT_EQUAL_INT(0, status);

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

void basicGetOrientationTest(void){
    /*some things we'll need for all the tests*/
    quaternion initalOrientation;
    quaternion finalOrientation;
    quaternion expectedOrientation;
    double testGx;
    double testGy;
    double testGz;

    //testing w/ nice, pleasant right angles

    /*Test1: rotate 90 degrees around x axis*/
    initalOrientation = qUnit();

    testGx=PI/2;
    testGy=0;
    testGz=0;

    expectedOrientation.re=0.707107;
    expectedOrientation.i=0.707107;
    expectedOrientation.j=0;
    expectedOrientation.k=0;

    finalOrientation=getOrientation(1,initalOrientation,testGx,testGy,testGz);

    compareQ(finalOrientation,expectedOrientation);

    /*Test2: rotate 90 degrees around y axis*/
    initalOrientation = qUnit();
    testGx=0;
    testGy=PI/2;
    testGz=0;

    expectedOrientation.re=0.707107;
    expectedOrientation.i=0;
    expectedOrientation.j=0.707107;
    expectedOrientation.k=0;

    finalOrientation=getOrientation(1,initalOrientation,testGx,testGy,testGz);

    compareQ(finalOrientation,expectedOrientation);

    /*Test3: rotate 90 degrees around z axis*/
    initalOrientation = qUnit();
    testGx=0;
    testGy=0;
    testGz=PI/2;

    expectedOrientation.re=0.707107;
    expectedOrientation.i=0;
    expectedOrientation.j=0;
    expectedOrientation.k=0.707107;

    finalOrientation=getOrientation(1,initalOrientation,testGx,testGy,testGz);

    compareQ(finalOrientation,expectedOrientation);
}

    void getOrientationTest(void){
    /*some things we'll need for all the tests*/
    quaternion initalOrientation;
    quaternion finalOrientation;
    quaternion expectedOrientation;
    double testGx;
    double testGy;
    double testGz;

    /*Test 1*/
    testGx=0;
    testGy=0;
    testGz=0;

    initalOrientation = qUnit();
    expectedOrientation  = qUnit();

    finalOrientation = getOrientation(1,initalOrientation,testGx,testGy,testGz);

    compareQ(finalOrientation,expectedOrientation);

    /*Test 2*/
    testGx=1;
    testGy=2;
    testGz=3;

    initalOrientation = qUnit();
    expectedOrientation.re=-0.295551127493;
    expectedOrientation.i=0.255321860045;
    expectedOrientation.j=0.510643720091;
    expectedOrientation.k=0.765965580136;

    finalOrientation = getOrientation(1,initalOrientation,testGx,testGy,testGz);

    compareQ(finalOrientation,expectedOrientation);

    /*Test 3 (this one multiplies by the result of test 2*/
    testGx=0;
    testGy=-2;
    testGz=2;

    initalOrientation = finalOrientation;

    expectedOrientation.re=-0.22442;
    expectedOrientation.i=-0.85184;
    expectedOrientation.j=0.464392;
    expectedOrientation.k=0.0913491;

    finalOrientation = getOrientation(1,initalOrientation,testGx,testGy,testGz);

    compareQ(finalOrientation,expectedOrientation);
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
    determinantTest,
    inverseTest,
    failInverseTest,
    qMultTest,
    qSumTest,
    qNormTest,
    qConjugateTest,
    basicGetOrientationTest,
    getOrientationTest,
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