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
    radioPrepEvent(15); //this is literally a random number. It could be anything you want!
	radioPrepConfig();
	radioPrepGPS();
	radioPrepState();
	radioPrepOrientation();
	radioPrepAccel();
	radioPrepSingleFloat(0x1E, getSensorDataFloat(sensorPT_HP_T_001));
    radioPrepSingleFloat(0x1F,getSensorDataFloat(sensorTC_HP_OUT_001));
    radioPrepSingleInt(0x20,getSensorDataInt(sensorV_HP_P_001));
    radioPrepSingleInt(0x21,getSensorDataInt(sensorV_F_PR_001));
    radioPrepSingleFloat(0x22,getSensorDataFloat(sensorPT_F_T_001));
    radioPrepSingleFloat(0x23,getSensorDataFloat(sensorPT_F_INJ_001));
    radioPrepSingleInt(0x24,getSensorDataInt(sensorV_F_F_001));
    radioPrepSingleFloat(0x25,getSensorDataFloat(sensorPT_F_INJ_001));
    radioPrepSingleInt(0x26,getSensorDataInt(sensorV_F_MFV_001));
    radioPrepSingleInt(0x27,getSensorDataInt(sensorV_L_PR_001));
    radioPrepSingleFloat(0x28,getSensorDataFloat(sensorPT_L_T_001));
    radioPrepSingleInt(0x29,getSensorDataInt(sensorV_L_V_001));
    radioPrepSingleFloat(0x2A,getSensorDataFloat(sensorTC_L_F_001));
    radioPrepSingleInt(0x2B,getSensorDataInt(sensorV_L_F_001));
    radioPrepSingleFloat(0x2C,getSensorDataFloat(sensorPT_L_INJ_001));
    radioPrepSingleInt(0x2D,getSensorDataInt(sensorV_L_MOV_001));
    radioPrepSingleInt(0x2E,getSensorDataInt(sensorV_L_BLD_001));
    radioPrepSingleFloat(0x2F,getSensorDataFloat(sensorTC_L_BLD_001));
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
