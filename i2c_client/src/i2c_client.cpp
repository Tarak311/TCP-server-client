#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>
#define GPIO11ON 0x54;
#define GPIO12ON 0x55;
#define ALLOFF 0x56
#define GPIO11OFF 0x44;
#define GPIO12OFF 0x45;
using namespace std;
uint8_t cmd=0x00;
struct status {
	uint8_t gpio11;
	uint8_t gpio12;
	uint8_t rgb;
	uint8_t max_temp1;
	uint8_t max_temp2;
};
struct scratchpad {
	uint8_t temp0;
	uint8_t temp1;
	uint8_t temphigh;
	uint8_t templow;
	uint8_t configuration;
	uint8_t crc;
	struct status state;
};


int main(int argc, char **argv)
{
	if(argc==1){cout<<"need arguments"<<endl;}
	struct  scratchpad scratch;
	uint8_t temprature;
	int network_socket = 0;

	struct sockaddr_in server_address;

	if((network_socket = socket(AF_INET, SOCK_STREAM, 0))< 0)
	{
		printf("Socket not created \n");
		return 1;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(2017);
	server_address.sin_addr.s_addr = inet_addr("192.168.43.188");
	if(connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0)
	{
		printf("Connection failed!! Raspberry Pis is offline or not in same network (^_^) \n");
		return 1;
	}

	cout<<argv[1]<<endl;
	string v =(string)argv[1];
	if(v=="relayonx"){cout<<"x:turning on relay"<<endl;cmd=0x54;}
	if(v=="relayoffx"){cout<<"x:turning off relay"<<endl;cmd=0x55;}
	if(v=="relayony"){cout<<"y:turning on relay"<<endl;cmd=0x44;}
	if(v=="relayoffy"){cout<<"y:turning off relay"<<endl;cmd=0x45;}
	if(v=="tempmodeon"){cout<<"y:turning on temp"<<endl;cmd=0x77;}
	if(v=="tempmodeoff"){cout<<"y:turning off temp"<<endl;cmd=0x88;}
	send(network_socket, &cmd, 1,0);
	read(network_socket, &scratch, sizeof(scratch));
	uint8_t y=scratch.temp1<<4;
	temprature=y|scratch.temp0>>4;
	printf("temp is: %02d\n",temprature);
	return 0;
}
