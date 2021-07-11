#include <unity.h>
#include <radio_protocol.h>

void baseTest(void *pv){
    TEST_ASSERT_EQUAL_HEX8(2,2);
    vTaskEndScheduler();
    
    
}

void baseTest_RTOS(void){
    BaseType_t error;
    if (error = xTaskCreate( /* create task */
	    baseTest, /* pointer to the task */
	    "Base Test", /* task name for kernel awareness debugging */
	    2000 / sizeof(StackType_t), /* task stack size */
	    (void*) NULL, /* optional task startup argument */
	    tskIDLE_PRIORITY + 2, /* initial priority */
	    (TaskHandle_t*) NULL /* optional task handle_debug to create */
	    ) != pdPASS) {
		    printf("Task init failed: %d\n", error);
		    for (;;)
			    ; /* error! probably out of memory */
	}
    vTaskStartScheduler();
}


void packetPlaceTest(void *pv){
	radioPrepPing();
	radioPrepMessage("Hello World!",12);
    radioPrepEvent(15); //this is literally a random number. It could be anything you want
	radioPrepConfig();
	radioPrepGPS();
	radioPrepState();
	radioPrepOrientation();
	radioPrepAccel();
	radioPrepPT_HP_T_001();
	radioPrepTC_HP_OUT_001();
	radioPrepV_HP_P_001();
	radioPrepV_F_PR_001();
	radioPrepV_F_V_001();
	radioPrepPT_F_T_001();
	radioPrepV_F_F_001();
	radioPrepPT_F_INJ_001();
	radioPrepV_F_MFV_001();
	radioPrepV_L_PR_001();
	radioPrepPT_L_T_001();
	radioPrepV_L_V_001();
	radioPrepTC_L_F_001();
	radioPrepV_L_F_001();
	radioPrepPT_L_INJ_001();
	radioPrepV_L_MOV_001();
	radioPrepV_L_BLD_001();
	radioPrepTC_L_BLD_001();
	radioPrepDataDump();
    vTaskEndScheduler();
    
    
}

void packetPlaceTest_RTOS(void){
    BaseType_t error;
    if (error = xTaskCreate( /* create task */
	    packetPlaceTest, /* pointer to the task */
	    "Base Test", /* task name for kernel awareness debugging */
	    2000 / sizeof(StackType_t), /* task stack size */
	    (void*) NULL, /* optional task startup argument */
	    tskIDLE_PRIORITY + 2, /* initial priority */
	    (TaskHandle_t*) NULL /* optional task handle_debug to create */
	    ) != pdPASS) {
		    printf("Task init failed: %d\n", error);
		    for (;;)
			    ; /* error! probably out of memory */
	}
    vTaskStartScheduler();
}



void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest_RTOS);
	RUN_TEST(packetPlaceTest_RTOS);
    return UNITY_END();
}
