#include <unity.h>
#include <state_machine.h>

#define True 1
#define False 0

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}

/*test that it starts in the right state*/
void defaultStateTest(void){
    state_t state = getState();
    TEST_ASSERT_EQUAL(state,stateStartup);
}

/*test that it changes state on HMI triggers*/
void examplePathTest(void){ //how to name these?

    int testInputsExpectedOutputs[22][5]={
    /*  groundAbort Standby Fueling Ignition    Expected*/
        {False,     False,  False,  False,      stateStartup    }, //Stay in startup
        {False,     False,  True,   False,      stateFueling    }, //Move to Fueling
        {False,     False,  False,  False,      stateFueling    }, //Stay in Fueling
        {True,      False,  False,  False,      stateGroundAbort}, //Move to Ground Abort
        {False,     False,  False,  False,      stateGroundAbort}, //Stay in Ground Abort
        {False,     False,  True,   False,      stateFueling    }, //Move to Fueling
        {False,     False,  False,  False,      stateFueling    }, //Stay in Fueling
        {False,     True,   False,  False,      stateStandby    }, //Move to Standby
        {False,     False,  False,  False,      stateStandby    }, //Stay in Standby
        {False,     False,  True,   False,      stateFueling    }, //Revert to Fueling
        {False,     False,  False,  False,      stateFueling    }, //Stay in Fueling
        {False,     True,   False,  False,      stateStandby    }, //Move to Standby
        {False,     False,  False,  False,      stateStandby    }, //Stay in Standby
        {True,      False,  False,  False,      stateGroundAbort}, //Move to Ground Abort
        {False,     False,  False,  False,      stateGroundAbort}, //Stay in Ground Abort
        {False,     False,  True,   False,      stateFueling    }, //Move to Fueling
        {False,     False,  False,  False,      stateFueling    }, //Stay in Fueling
        {False,     True,   False,  False,      stateStandby    }, //Move to Standby
        {False,     False,  False,  False,      stateStandby    }, //Stay in Standby
        {False,     False,  False,  True,       stateIgnition   }, //Move to Ignition
        {False,     False,  False,  False,      stateIgnition   }, //Stay in Ignition
        {True,      False,  False,  False,      stateGroundAbort}  //Move to Ground Abort

    };

    for(int i=0;i<22;i++){
        
        stateInput_t testStateInput ={0};
        testStateInput.HMI_triggerGroundAbort = testInputsExpectedOutputs[i][0];
        testStateInput.HMI_triggerStandby = testInputsExpectedOutputs[i][1];
        testStateInput.HMI_triggerFueling = testInputsExpectedOutputs[i][2];
        testStateInput.GSE_triggerIgnition = testInputsExpectedOutputs[i][3];
        state_t state = setNextState(&testStateInput);
        TEST_ASSERT_EQUAL(state,testInputsExpectedOutputs[i][4]);
        printf("%d\n",i);
    }




}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(defaultStateTest);
    RUN_TEST(examplePathTest);
    return UNITY_END();
}