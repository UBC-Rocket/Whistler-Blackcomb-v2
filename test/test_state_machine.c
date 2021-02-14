#include <unity.h>
#include <state_machine.h>

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}
void defaultStateTest(void){
    state_t state = getState();
    TEST_ASSERT_EQUAL(state,stateFueling);
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(defaultStateTest);
    return UNITY_END();
}