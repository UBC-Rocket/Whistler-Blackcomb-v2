#include <unity.h>
#include <IMU_interpret.h>

void statusbyte_test(void){
    IMU_1 IMU;
    IMU.interpGyro=1;
    IMU.interpIncl =0;
    IMU.interpAux = 0;
    IMU.interpAccel=0;
    IMU.interpTemp=0;
    int ID= get_ID(IMU);
    TEST_ASSERT_EQUAL_INT(0x90,ID);
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(statusbyte_test);
    return UNITY_END();
}