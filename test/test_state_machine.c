#include <unity.h>
#include <state_machine.h>

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}

/*test that it starts in the right state*/
void defaultStateTest(void){
    state_t state = getState();
    TEST_ASSERT_EQUAL(state,stateStartup);
}

/*test that it changes state on HMI triggers*/
void basicStateChangeTest(void){
    stateInput_t testStateInput;
    testStateInput.HMI_triggerFueling = 1; //bad magic number bad
    state_t state = setNextState(&testStateInput);
    TEST_ASSERT_EQUAL(state,stateFueling);

    testStateInput.HMI_triggerGroundAbort = 1;
    state = setNextState(&testStateInput);
    TEST_ASSERT_EQUAL(state,stateGroundAbort);
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(defaultStateTest);
    RUN_TEST(basicStateChangeTest);
    return UNITY_END();
}