//============================================================================
// Name        : Tarak01.cpp
// Author      : Tarak
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <stdint.h>
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

int readBytes;
uint8_t buffer[9];
int network_socket;
using namespace std;

int main() {
	network_socket = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(2017);
	server_address.sin_addr.s_addr = inet_addr("192.168.2.40");
	int connection_status = connect(network_socket, (struct sockaddr*) &server_address,sizeof(server_address));
	if (connection_status ==-1){
		 printf("looks like raspberry's server application is down or device is not on same network or not connected\n\n");
	}
	recv(network_socket,buffer,sizeof(buffer),0);
	close(network_socket);



}
