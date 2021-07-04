#include <stdio.h>

void convert(unsigned char id, unsigned short length, const char* packet, char* ret){
	ret[0] = (id >> 4) + 'A';
	ret[1] = (id & 0xF) + 'A';
	ret[2] = ((length >> 8) >> 4) + 'A';
	ret[3] = ((length >> 8) & 0xF) + 'A';
	ret[4] = ((length & 0xFF) >> 4) + 'A';
	ret[5] = ((length & 0xFF) & 0xF) + 'A';
	for(int i = 0; i < length; ++i){
		char msb = (packet[i] >> 4) + 'A';
		char lsb = (packet[i] & 0xF) + 'A';
		ret[2*i+6] = msb;
		ret[2*i+7] = lsb;
	}
	ret[6+2*length] = 0;
}

int main(){
	unsigned int id;
	unsigned int length;
	char packet[100];
	char simpacket[200];
	printf("Enter id of packet, in decimal (i.e. radio packet would be 82): ");
	scanf(" %d", &id);
	printf("Enter length of packet not including the header, in decimal: ");
	scanf(" %d", &length);
	printf("Enter packet to convert, as a string: ");
	scanf(" %s", packet);
	convert(id, length, packet, simpacket);
	printf("%s", simpacket);
	
}
