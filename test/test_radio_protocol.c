#include <unity.h>
#include <radio_protocol.h>

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}

void 

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    return UNITY_END();
}
