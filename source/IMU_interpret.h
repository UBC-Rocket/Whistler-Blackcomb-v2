//so that VS doesn't throw a fit about some of the i/o functios
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

//defining some error/success codes
#define SUCCESS						0
#define ID_MISMATCH					1
#define ANY_STATUS_BYTE_NOT_OK		2

int to_2C(int value);

int interpretImuData(unsigned char datagram[], int identifier, double dataout[], unsigned char statusBytes[]);
//interpretImuData takes a single datagram and interperates into usabe double prescision values, stored in whatever is passed to dataout[]
