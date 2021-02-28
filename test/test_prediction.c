#include <unity.h>
#include <prediction.h>

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}
void matMultTest(void){
    double matrix1[2][2] = {{1,2},{3,4}};
    double matrix2[2][2] = {{5,6},{7,8}};
    double expectedResult[2][2] = {{19,22},{43,50}};
    double result[2][2];
    
    matMult(matrix1,matrix2,result);

    for(int i=0;i<2;i++){
        for(int ii=0;ii<2;ii++){
            TEST_ASSERT_EQUAL(result[i][ii],expectedResult[i][ii]);
        }
    }
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(matMultTest);
    return UNITY_END();
}