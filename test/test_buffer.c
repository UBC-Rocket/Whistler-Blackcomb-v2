#include <unity.h>
#include <buffer.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void baseTest(void){
    TEST_ASSERT_EQUAL_HEX8(2,2);
}

void initBufTest(void){
    cbufHandle_t testbuf=cbufInit(100);
    cbufFree(testbuf);
}

// void placeAndRecieveTest(void){
//     uint8_t testData1[4]={1,2,3,4};
//     uint8_t testData2=255;
//     uint8_t testData3[10]={100,6,7,8,9,10,11,12,13,14};
    
//     cbufHandle_t testbuf=cbufInit(100);
//     cbufPut(testbuf,4,testData1);
//     cbufPut(testbuf,1,&testData2);
    
//     uint8_t recievedData1[4]={0,0,0,0};
//     uint8_t recievedData2;
//     uint8_t recievedData3[10];
   
//     uint8_t length=cbufGet(testbuf,recievedData1);

//     cbufPut(testbuf,10,testData3);

//     cbufGet(testbuf,&recievedData2);

//     length=cbufGet(testbuf,recievedData3);


//     cbufFree(testbuf);

//     TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1,recievedData1,4);
//     TEST_ASSERT_EQUAL_INT8(testData2,recievedData2);
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(testData3,recievedData3,10);
// }

// //need to rewrite this test, behaviour is as expected but the test results are 
// //for a different kind of test, also it's a bit messy
// void bufferWraparoundTest(void){
//     uint8_t testData1[8]={1,2,3,4,5,6,7,8};
//     uint8_t testData2=255;
//     uint8_t testData3[10]={100,101,102,103,104,105,106,107,108,109};
//     cbufHandle_t testbuf=cbufInit(2);
//     cbufPut(testbuf,8,testData1);
//     cbufPut(testbuf,1,&testData2);
//     cbufPut(testbuf,10,testData3);

//     uint8_t recievedData1[8];
//     uint8_t recievedData2;
//     uint8_t recievedData3[10];

//     uint8_t length=cbufGet(testbuf,recievedData3);
//     printf("length: %d\n",length);
//     for(int i=0;i<length;i++){
//         printf("recieved Data 1-%d: %d\n",i,recievedData3[i]);
//     }

    

//     //cbufGet(testbuf,&recievedData2);
//     //printf("recieved Data 2: %d\n",recievedData2);

//     length=cbufGet(testbuf,recievedData3);
//     printf("length: %d\n",length);
//     for(int i=0;i<length;i++){
//         printf("recieved Data 3-%d: %d\n",i,recievedData3[i]);
//     }

//     length=cbufGet(testbuf,recievedData3);
//     printf("length: %d\n",length);

//     cbufFree(testbuf);

//     //TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1,recievedData1,4);
//     //TEST_ASSERT_EQUAL_INT8(testData2,recievedData2);
//     //TEST_ASSERT_EQUAL_UINT8_ARRAY(testData3,recievedData3,10);
// }


// //this isn't going to be an actual test... Just needed a place to scratch some
// //stuff out, and this was already setup.
// void test_Test(void){
//     cbufHandle_t radioBufRX;
//     radioBufRX=cbufInit(8);

//         uint8_t length=6;
//         uint8_t maxMessage[256] = {1,2,3,4,5,6,7};
//         uint8_t *briefMessage = pvPortMalloc(length*sizeof(uint8_t));
//         for(int i=0;i<length+1;i++){
//             briefMessage[i]=maxMessage[i];
//         }
//         cbufPut(radioBufRX,length,briefMessage);
//         vPortFree(briefMessage);
//     uint8_t outmessage[256];
//     cbufGet(radioBufRX,outmessage);

//     for(int i=0;i<length+1;i++){
//         printf("%d\n",outmessage[i]);
//     }

// }



void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(baseTest);
    RUN_TEST(initBufTest);
    // RUN_TEST(placeAndRecieveTest);
    // RUN_TEST(bufferWraparoundTest);
   // RUN_TEST(test_Test);
    return UNITY_END();
}