#include <unity.h>
#include <buffer.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void baseTest(void *pv){
    TEST_ASSERT_EQUAL_HEX8(2,2);
    vTaskEndScheduler();
}

void baseTest_RTOS(void){
    BaseType_t error;
    if (error = xTaskCreate( /* create task */
	    baseTest, /* pointer to the task */
	    "Base Test", /* task name for kernel awareness debugging */
	    200 / sizeof(StackType_t), /* task stack size */
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

void initBufTest(void *pv){
    cbufHandle_t testbuf=cbufInit(100);
    cbufFree(testbuf);
    vTaskEndScheduler();
}

void initBufTest_RTOS(void){
    BaseType_t error;
    if (error = xTaskCreate( /* create task */
	    initBufTest, /* pointer to the task */
	    "Init Buf Test", /* task name for kernel awareness debugging */
	    200 / sizeof(StackType_t), /* task stack size */
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

void placeAndRecieveTest(void *pv){
    uint8_t testData1[4]={1,2,3,4};
    uint8_t testData2=255;
    uint8_t testData3[10]={100,6,7,8,9,10,11,12,13,14};
    cbufHandle_t testbuf=cbufInit(100);
    cbufPut(testbuf,4,testData1);
    cbufPut(testbuf,1,&testData2);
    
    uint8_t recievedData1[4]={0,0,0,0};
    uint8_t recievedData2;
    uint8_t recievedData3[10];
   
    uint8_t length=cbufGet(testbuf,recievedData1);

    cbufPut(testbuf,10,testData3);

    cbufGet(testbuf,&recievedData2);

    length=cbufGet(testbuf,recievedData3);

    cbufFree(testbuf);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1,recievedData1,4);
    TEST_ASSERT_EQUAL_INT8(testData2,recievedData2);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData3,recievedData3,10);

    vTaskEndScheduler();
}

void placeAndRecieveTest_RTOS(void){
    BaseType_t error;
    if (error = xTaskCreate( /* create task */
	    placeAndRecieveTest, /* pointer to the task */
	    "Place and Recieve Test", /* task name for kernel awareness debugging */
	    200 / sizeof(StackType_t), /* task stack size */
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

//need to rewrite this test, behaviour is as expected but the test results are 
//for a different kind of test, also it's a bit messy
void bufferWraparoundTest(void *pv){
    uint8_t testData1[8]={1,2,3,4,5,6,7,8};
    uint8_t testData2=255;
    uint8_t testData3[10]={100,101,102,103,104,105,106,107,108,109};
    cbufHandle_t testbuf=cbufInit(2);
    cbufPut(testbuf,8,testData1);
    cbufPut(testbuf,1,&testData2);
    cbufPut(testbuf,10,testData3);
    uint8_t recievedData1[8];
    uint8_t recievedData2;
    uint8_t recievedData3[10];
    uint8_t length=cbufGet(testbuf,recievedData3);
    printf("length: %d\n",length);
    for(int i=0;i<length;i++){
        printf("recieved Data 1-%d: %d\n",i,recievedData3[i]);
    }

    

    //cbufGet(testbuf,&recievedData2);
    //printf("recieved Data 2: %d\n",recievedData2);

    length=cbufGet(testbuf,recievedData3);
    printf("length: %d\n",length);
    for(int i=0;i<length;i++){
        printf("recieved Data 3-%d: %d\n",i,recievedData3[i]);
    }

    length=cbufGet(testbuf,recievedData3);
    printf("length: %d\n",length);
    cbufFree(testbuf);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1,recievedData1,4);
    TEST_ASSERT_EQUAL_INT8(testData2,recievedData2);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData3,recievedData3,10);

    vTaskEndScheduler();
}

void bufferWraparoundTest_RTOS(void){
    BaseType_t error;
    if (error = xTaskCreate( /* create task */
	    bufferWraparoundTest, /* pointer to the task */
	    "Buffer Wraparound Test", /* task name for kernel awareness debugging */
	    200 / sizeof(StackType_t), /* task stack size */
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
    RUN_TEST(initBufTest_RTOS);
    RUN_TEST(placeAndRecieveTest_RTOS);
    RUN_TEST(bufferWraparoundTest_RTOS);
    return UNITY_END();
}