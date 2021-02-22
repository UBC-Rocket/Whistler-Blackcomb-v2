#include <unity.h>
#include <state_machine.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}

/*test that it starts in the right state*/
void defaultStateTest(void){
    state_t state = getState();
    TEST_ASSERT_EQUAL(state,stateStartup);
}
void statesResetTest(void){
    stateInput_t input = {0};
    state_t state;

    //this should move it out of startup to fueling
    state = setNextState(&input);
    printf("%s\n",stateNames[state]); //for testing the testing

    input.HMI_triggerFueling = TRUE;
    state = setNextState(&input);
    TEST_ASSERT_EQUAL(input.HMI_triggerFueling,FALSE); //is it reset?
    printf("%s\n",stateNames[state]); //for testing the testing

    input.HMI_triggerStandby = TRUE;
    setNextState(&input);
    TEST_ASSERT_EQUAL(input.HMI_triggerStandby, FALSE); //is it reset?

    input.GSE_triggerIgnition = TRUE;
    setNextState(&input);
    TEST_ASSERT_EQUAL(input.GSE_triggerIgnition,FALSE); //is it reset?

    input.HMI_triggerGroundAbort = TRUE;
    setNextState(&input);
    TEST_ASSERT_EQUAL(input.HMI_triggerGroundAbort,FALSE); //is it reset?
}


/*test that it changes state on HMI triggers*/
void examplePathTest(void){ //how to name these tests?
    stateInput_t testStateInput ={0};
    int testInputsExpectedOutputs[22][5]={
    /*  groundAbort Standby Fueling Ignition    Expected*/
        {FALSE,     FALSE,  FALSE,  FALSE,      stateStartup    }, //Stay in startup
        {FALSE,     FALSE,  TRUE,   FALSE,      stateFueling    }, //Move to Fueling
        {FALSE,     FALSE,  FALSE,  FALSE,      stateFueling    }, //Stay in Fueling
        {TRUE,      FALSE,  FALSE,  FALSE,      stateGroundAbort}, //Move to Ground Abort
        {FALSE,     FALSE,  FALSE,  FALSE,      stateGroundAbort}, //Stay in Ground Abort
        {FALSE,     FALSE,  TRUE,   FALSE,      stateFueling    }, //Move to Fueling
        {FALSE,     FALSE,  FALSE,  FALSE,      stateFueling    }, //Stay in Fueling
        {FALSE,     TRUE,   FALSE,  FALSE,      stateStandby    }, //Move to Standby
        {FALSE,     FALSE,  FALSE,  FALSE,      stateStandby    }, //Stay in Standby
        {FALSE,     FALSE,  TRUE,   FALSE,      stateFueling    }, //Revert to Fueling
        {FALSE,     FALSE,  FALSE,  FALSE,      stateFueling    }, //Stay in Fueling
        {FALSE,     TRUE,   FALSE,  FALSE,      stateStandby    }, //Move to Standby
        {FALSE,     FALSE,  FALSE,  FALSE,      stateStandby    }, //Stay in Standby
        {TRUE,      FALSE,  FALSE,  FALSE,      stateGroundAbort}, //Move to Ground Abort
        {FALSE,     FALSE,  FALSE,  FALSE,      stateGroundAbort}, //Stay in Ground Abort
        {FALSE,     FALSE,  TRUE,   FALSE,      stateFueling    }, //Move to Fueling
        {FALSE,     FALSE,  FALSE,  FALSE,      stateFueling    }, //Stay in Fueling
        {FALSE,     TRUE,   FALSE,  FALSE,      stateStandby    }, //Move to Standby
        {FALSE,     FALSE,  FALSE,  FALSE,      stateStandby    }, //Stay in Standby
        {FALSE,     FALSE,  FALSE,  TRUE,       stateIgnition   }, //Move to Ignition
        {FALSE,     FALSE,  FALSE,  FALSE,      stateIgnition   }, //Stay in Ignition
        {TRUE,      FALSE,  FALSE,  FALSE,      stateGroundAbort}  //Move to Ground Abort

    };

    for(int i=0;i<22;i++){
        
        
        testStateInput.HMI_triggerGroundAbort = testInputsExpectedOutputs[i][0];
        testStateInput.HMI_triggerStandby = testInputsExpectedOutputs[i][1];
        testStateInput.HMI_triggerFueling = testInputsExpectedOutputs[i][2];
        testStateInput.GSE_triggerIgnition = testInputsExpectedOutputs[i][3];
        state_t state = setNextState(&testStateInput);
        char expectedState[32];
        strcpy(expectedState,stateNames[testInputsExpectedOutputs[i][4]]);
        char actualState[32];
        strcpy(actualState, stateNames[state]);
        printf("%d: expected %s got %s\n",i,expectedState,actualState);

        TEST_ASSERT_EQUAL(testInputsExpectedOutputs[i][4],state);
    }




}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(defaultStateTest);
    RUN_TEST(statesResetTest);
    RUN_TEST(examplePathTest);
    return UNITY_END();
}