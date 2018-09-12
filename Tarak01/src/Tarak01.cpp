//============================================================================
// Name        : Tarak01.cpp
// Author      : Tarak
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <cstring>
#include <sys/socket.h>
#include <stropts.h>
#include <stddef.h>

#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <fstream>
int deviceHandle;
int readBytes;
uint8_t buffer[9]={0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
int server_socket;
int deviceI2CAddress = 0x5d;
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	server_socket = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_address;
	memset(&server_address, '0', sizeof(server_address));

	server_address.sin_family=AF_INET;
	server_address.sin_port= htons(8829);
	server_address.sin_addr.s_addr=htonl(INADDR_ANY);

	bind(server_socket,(struct sockaddr*) &server_address, sizeof(server_address));

	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0)
	{
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	if (ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0)
	{
		printf("Error: Couldn't find device on address!\n");

	}

	//readBytes = write(deviceHandle, buffer, 1);
	readBytes = read(deviceHandle, buffer, 9);
	if (readBytes != 9)
	{
		printf("Error: Received no data!");
	}
	else
	{
		printf(" ----------------------------\n");
		printf("|  DS1482 scratchpad values: |\n");
		printf(" ---------------------------\n");
		printf("|temperature is:      | %02x |\n",buffer[1] );
		printf(" ---------------------------\n");
		printf("|temperature is:      | %02x |\n",buffer[0] );
		printf(" ---------------------------\n");
		printf("|High temp reg	      | %02x |\n",buffer[2] );
		printf(" ---------------------------\n");
		printf("|Low temp reg	      | %02x |\n",buffer[3] );
		printf(" ---------------------------\n");
		printf("|Config register      | %02x |\n",buffer[4] );
		printf(" ---------------------------\n");
		printf("|reserved register    | %02x |\n",buffer[5] );
		printf(" ---------------------------\n");
		printf("|reserved register    | %02x |\n",buffer[6] );
		printf(" ---------------------------\n");
		printf("|reserved register    | %02x |\n",buffer[7] );
		printf(" ---------------------------\n");
		printf("|CRC:                 | %02x |\n",buffer[8] );
		printf(" ---------------------------\n");
	}

	int client_socket;
	listen(server_socket,4);

	cout<< "listening"<<endl;
	client_socket=accept(server_socket,(struct sockaddr*) NULL,NULL);
	cout<<"connection established"<<endl;
	send(client_socket,buffer,sizeof(buffer),0);
	close(server_socket);
	close(deviceHandle);
	return 0;
}
