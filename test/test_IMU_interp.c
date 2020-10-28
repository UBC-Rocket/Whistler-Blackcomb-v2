#include <unity.h>
#include <IMU_interpret.h>

void statusbyte_test(void){
    IMU_1 IMU;
    configImu(&IMU);
    TEST_ASSERT_EQUAL_INT(0x93,IMU.datagramID);
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(statusbyte_test);
    return UNITY_END();
}