#include <unity.h>

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}
void baseTest2(void){
    TEST_ASSERT_EQUAL_HEX8(1,2);
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(baseTest2);
    return UNITY_END();
}