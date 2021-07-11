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
    radioPrepEvent(15);
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
    return UNITY_END();
}
