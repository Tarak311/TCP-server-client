
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
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/select.h>
#include <fstream>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
int deviceHandle;
int readBytes;


uint8_t scratchpad[18]={0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t cmd=0x00;
struct status {
	uint8_t gpio11;
	uint8_t gpio12;
	uint8_t rgb;
	uint8_t max_temp1;
	uint8_t max_temp2;
};

int deviceI2CAddress = 0x5d;
struct scratchpad {
	uint8_t temp0;
	uint8_t temp1;
	uint8_t temphigh;
	uint8_t templow;
	uint8_t configuration;
	uint8_t crc;
	struct status state;
};
/*STATUS: gpio: BYTE [7]: status(0x05 is by user,0x07 due to temp logic BYTE[0-6]:reason*/

using namespace std;
int readi2c(int& devhndl,struct scratchpad&,struct scratchpad&);
int printscratchpad(struct scratchpad& scrth);
struct status checkcmd(uint8_t &cmd);
int main()
{

	int server_socket = 0, clientfd = 0, network_socket_client = 0,i=0,k=0;
	struct sockaddr_in server_address,client_address;
	struct  scratchpad scratch,scratch2;

	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0)
	{
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	if (ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0)
	{
		printf("Error: Couldn't find device on address!\n");

	}
	memset(&client_address, '0', sizeof(client_address));
	memset(&server_address, '0', sizeof(server_address));
	memset(scratchpad, '0', sizeof(scratchpad));

	/********Set Client sockets parmeter*******/

	network_socket_client = socket(AF_INET, SOCK_STREAM, 0);
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(2017);
	client_address.sin_addr.s_addr = inet_addr("34.227.171.194");


	/******************************************************************************************/

	server_socket = socket(AF_INET, SOCK_STREAM, 0); // creating socket
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(2018);         // this is the port number of running server
	if(connect(network_socket_client, (struct sockaddr *)&client_address, sizeof(client_address))<0)
	{
		printf("Connection failed!! EC2 server is offline(^-.-^) \n");
		k=1;
		return 1;
	}



	bind(server_socket, (struct sockaddr*)&server_address , sizeof(server_address));
	listen(server_socket , 20);
	int j=0;

	while(1)
	{

		//clientfd = accept(server_socket, (struct sockaddr*)NULL, NULL);
		cout<<"Some client requested data"<<endl;

		int i=readi2c(deviceHandle,scratch,scratch2);
		if (i<0){cout<<"Data not recived"<<endl;}
		j++;
		if(k==1)
		{
			recv(clientfd, &cmd, sizeof(cmd),0);
			write(clientfd, scratchpad, sizeof(scratchpad));
		}
		else
		{
			write(network_socket_client, scratchpad, sizeof(scratchpad));
			recv(network_socket_client, &cmd, sizeof(cmd),0);

		}
		//scratch.state=checkcmd(cmd);

		close(network_socket_client);
		readBytes = write(deviceHandle, &cmd, 1);
		sleep(1);
	}
	close(clientfd);

	return 0;
}
int readi2c(int& devhndl,struct scratchpad& sp,struct scratchpad& sp2)
{
	readBytes = read(devhndl, scratchpad, 18);
	if (readBytes != 18)
	{
		printf("Error: Received no data!");
		return -1;
	}
	else
	{
		sp.temp0=scratchpad[1];
		sp.temp1=scratchpad[0];
		sp.temphigh=scratchpad[2];
		sp.templow=scratchpad[3];
		sp.configuration=scratchpad[4];
		sp.crc=scratchpad[5];
		printscratchpad(sp);
		sp2.temp0=scratchpad[9];
		sp2.temp1=scratchpad[10];
		sp2.temphigh=scratchpad[11];
		sp2.templow=scratchpad[12];
		sp2.configuration=scratchpad[13];
		sp2.crc=scratchpad[14];
		printscratchpad(sp2);
	}

	return 0;
}
int printscratchpad(struct scratchpad& sc)

{
	uint8_t temp_dec = (sc.temp1&&0xFF)>>4||(sc.temp0&&0xFF)<<4;
	float temp_frac = sc.temp1&0x0F;
	double temperature = temp_dec +temp_frac;
	cout<<temp_dec<<endl;
	printf(" ---------------------------\n");
	printf("|  DS1482 scratchpad values: |\n");
	printf(" ---------------------------\n");
	printf("|temperature is:      | %02x |\n",sc.temp0 );
	printf(" ---------------------------\n");
	printf("|temperature is:      | %02x |\n",sc.temp1 );
	printf(" ---------------------------\n");
	printf("|High temp reg	      | %02x |\n",sc.temphigh );
	printf(" ---------------------------\n");
	printf("|Low temp reg	      | %02x |\n",sc.templow );
	printf(" ---------------------------\n");
	printf("|Config register      | %02x |\n",sc.configuration );
	printf(" ---------------------------\n");
	printf("|reserved register    | %02x |\n",scratchpad[5] );
	printf(" ---------------------------\n");
	printf("|reserved register    | %02x |\n",scratchpad[6] );
	printf(" ---------------------------\n");
	printf("|reserved register    | %02x |\n",scratchpad[7] );
	printf(" ---------------------------\n");
	printf("|CRC:                 | %02x |\n",sc.crc );
	printf(" ---------------------------\n");
	return 0;
}
struct status checkcmd(uint8_t &cmd){
	struct status st;
	switch(cmd){
	case 0x44:
		cout<<"Setting Pin 11 high(off)"<<endl;
		break;
	case 0x45:
		cout<<"Setting Pin 12 high(off)"<<endl;
		break;
	case 0x46:
		cout<<"Setting ALL pin high(off)"<<endl;
		break;
	case 0x54:
		cout<<"Setting Pin 11 low(on)"<<endl;
		break;
	case 0x55:
		cout<<"Setting Pin 12 low(on)"<<endl;
		break;
	case 0x56:
		cout<<"Setting ALL Pin low(on)"<<endl;
		break;
	}
	return st;
}
