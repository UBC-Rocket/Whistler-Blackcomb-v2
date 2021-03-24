#include <unity.h>
#include <IMU_interpret.h>
#include <stdio.h>

/*******************************************************************************
 * Declarations
 ******************************************************************************/

int crc32_test_datagrams_from_path(char[], int, int);

/*******************************************************************************
 * Tests
 ******************************************************************************/

//Note: Correct CRC32 values are from https://crccalc.com/

void statusbyte_test(void){
    IMU_1 IMU;
    configImu(&IMU);
    TEST_ASSERT_EQUAL_INT(0x93,IMU.datagramID);
}

void crc32_test1(void){
    crc32_initTable();
    byte input[] = {0x00}; 
    uint output = crc32(input, 1, 0);
    uint expected = 0x4E08BFB4;

    TEST_ASSERT_EQUAL_INT(expected, output);
}

void crc32_test2(void){
    crc32_initTable();
    byte input[] = "123456789"; //Common string to use for checking CRC32 algorithms
    uint output = crc32(input, 9, 0);
    uint expected = 0x0376E6E7;

    TEST_ASSERT_EQUAL_INT(expected, output);
}

void crc32_test_datagram1(void) {
    //The first datagram copied from imu_uart_data_2.txt
    int result = crc32_test_datagrams_from_path("../../data/imu/imu_datagram_test1.txt", 34, 2);
    TEST_ASSERT_EQUAL_INT(1, result);
}

void crc32_test_uart2(void) {
    int result = crc32_test_datagrams_from_path("../../data/imu/imu_uart_data_2.txt", 34, 2); 
    TEST_ASSERT_EQUAL_INT(1, result);
}

void crc32_test_uart3(void) {
    int result = crc32_test_datagrams_from_path("../../data/imu/imu_uart_data_3.txt", 34, 2); 
    TEST_ASSERT_EQUAL_INT(1, result);
}

void crc32_test_error(void) {
    int result = crc32_test_datagrams_from_path("../../data/imu/imu_datagram_error.txt", 34, 2); 
    TEST_ASSERT_EQUAL_INT(0, result); //i've edited this text file so the the result should be false
}

//For identifier 0x93 (which all the messages we have so far are), there are 34 bytes preceding the CRC, so the CRC starts at byte # 34,
// and we need 2 dummy bytes (for message of length 40, so divisible by 4 bytes or 32 bits)
int crc32_test_datagrams_from_path(char path[], int crcStart, int dummyBytes){
    crc32_initTable();
    byte datagram[64];
    FILE *f = fopen(path, "r");
    
    int reachedEOF = 0; //flag checking if we've reached the end of file
    int errorsFound = 0; //counter holding number of errors detected by crc check
    int numDatagrams = 0; //counter holding number of datagrams we've run the crc check on
    int datagramLength = crcStart + 4 + 2; //length is the (# of bytes preceding crc) + (4 bytes for crc) + (2 bytes for <CR> and <LF>)

    while(!reachedEOF) {
        //parse datagram
        for(int i = 0; i < datagramLength; i++) {
            int curByte = 0;

            if(fscanf(f, "%d ", &curByte) == EOF) {
                reachedEOF = 1;
                break;
            }

            datagram[i] = curByte;
        }

        if(!reachedEOF) {
            uint msgCrc = (datagram[crcStart] << 24) + (datagram[crcStart + 1] << 16) +  
            (datagram[crcStart + 2] << 8) + datagram[crcStart + 3];

            //For identifier 0x93 we add 2 dummy bytes to get a multiple of 4 bytes or 32 bits
            uint expectedCrc = crc32(datagram, crcStart, 2);
            // printf("Message CRC: %x, Expected CRC: %x\n", msgCrc, expectedCrc);

            numDatagrams++;
            if(msgCrc != expectedCrc) {
                errorsFound++;
            }
        }
    }

    // printf("ERRORS: %d, TESTS: %d\n", errorsFound, numDatagrams);
    return !errorsFound;
}

//We can use this function as a template to write other interpretImuData test cases. 
//(doesn't actually test anything yet)
//I'm not too familiar with this method so I'll leave this as just a template for now.
void interpret_datagram_template(void){
    IMU_1 IMU;
    configImu(&IMU);
    crc32_initTable();

    char path[] = "../../data/imu/imu_datagram_test1.txt"; //The first datagram copied from imu_uart_data_2.txt 
    FILE *f = fopen(path, "r");
    int dgLen = 40; //Datagrams of identifier 0x93 are 40 bytes in length

    // char index2DataType[][32] = {"datagram", 
    // "Gyro X1", "Gyro X2", "Gyro X3", "Gyro Y1", "Gyro Y2", "Gyro Y3",  "Gyro Z1", "Gyro Z2", "Gyro Z3", "Gyro Status",
    // "Accel X1", "Accel X2", "Accel X3", "Accel Y1", "Accel Y2", "Accel Y3",  "Accel Z1", "Accel Z2", "Accel Z3", "Accel Status",
    // "Incl X1", "Incl X2", "Incl X3", "Incl Y1", "Incl Y2", "Incl Y3",  "Incl Z1", "Incl Z2", "Incl Z3", "Incl Status",
    // "Counter", "Latency 1", "Latench 2",
    // "CRC 1", "CRC 2", "CRC 3", "CRC 4",
    // "CR", "LF"
    // }; //Used exclusively for debugging, delete if you don't need

    for(int i = 0; i < dgLen; i++) {
        int curByte = 0;
        fscanf(f, "%d ", &curByte);
        IMU.datagram[i] = curByte;
        // printf("%s: %x\n", index2DataType[i], curByte); //Also debugging
    }

    int result = interpretImuData(&IMU);
}

void setUp(void){}
void tearDown(void){}
int main (void){
    UNITY_BEGIN();
    RUN_TEST(statusbyte_test);
    RUN_TEST(crc32_test1);
    RUN_TEST(crc32_test2);
    RUN_TEST(crc32_test_datagram1);
    RUN_TEST(crc32_test_uart2);
    RUN_TEST(crc32_test_uart3);
    RUN_TEST(crc32_test_error);

    return UNITY_END();
}