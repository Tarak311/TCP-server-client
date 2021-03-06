
#include <stdio.h> // standard input and output library
#include <stdlib.h> // this includes functions regarding memory allocation
#include <string.h> // contains string functions
#include <errno.h> //It defines macros for reporting and retrieving error conditions through error codes
#include <time.h> //contains various functions for manipulating date and time
#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses
#include <sys/select.h>
#include <fstream>
#include <fcntl.h>
#include <iostream>


int readBytes;


uint8_t scratchpad[18]={0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t cmd=0x00;

/*STATUS: gpio: BYTE [7]: status(0x05 is by user,0x07 due to temp logic BYTE[0-6]:reason*/

using namespace std;

int main()
{

	int i,server_socket = 0, server_socket1 = 0,clientfd = 0,clientfd1 =0 ;

	struct sockaddr_in server_address,server_address1 ;



	server_socket = socket(AF_INET, SOCK_STREAM, 0); // creating socket
	server_socket1 = socket(AF_INET, SOCK_STREAM, 0);
	memset(&server_address, '0', sizeof(server_address));
	memset(&server_address1, '0', sizeof(server_address1));
	memset(scratchpad, '0', sizeof(scratchpad));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(2018);         // this is the port number of running server
	server_address1.sin_family = AF_INET;
	server_address1.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address1.sin_port = htons(2017);         // this is the port number of running server

	bind(server_socket, (struct sockaddr*)&server_address , sizeof(server_address));
	listen(server_socket , 20);
	bind(server_socket1, (struct sockaddr*)&server_address1 , sizeof(server_address1));
	listen(server_socket1 , 20);
	while(1)
	{
		if(fork()==0)
		{
			while(1)
			{
				clientfd = accept(server_socket, (struct sockaddr*)NULL, NULL);
				cout<<"PID1:Some client requested data"<<endl;
				recv(clientfd, &cmd, sizeof(cmd),0);
				write(clientfd, scratchpad, sizeof(scratchpad));
				cout<<"value from client"<<scratchpad[0]<<endl;
				sleep(10);

			}

		}
		else
		{
			while(1)
			{
				clientfd1 = accept(server_socket1, (struct sockaddr*)NULL, NULL);
				cout<<"PID2:Some client sent data"<<endl;
				recv(clientfd1, scratchpad, sizeof(scratchpad),0);
				cout<<"value from client"<<scratchpad[0]<<endl;
				write(clientfd1, &cmd, sizeof(cmd));
				sleep(10);

			}

		}
	}
	close(clientfd);
	close(clientfd1);
	return 0;
}



