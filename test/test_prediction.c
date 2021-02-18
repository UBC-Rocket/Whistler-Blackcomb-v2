#include <unity.h>
#include <prediction.h>

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}
void scalarMultTest(void){
    void ScalarMultTest(void){
    int matrix1[3] = {1,1,1};
    int scalar = 2;
    int expectedResult[3] = {2,2,2};
    int result;

    scalMult(matrix1,scalar,result);

    TEST_ASSERT_EQUAL(result,expectedResult);
    }
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(scalarMultTest);
    return UNITY_END();
}