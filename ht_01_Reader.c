// TPA81 example c code for the Raspberry pi.
//
// Reads the software revision of the TPA81 and all temperature data
// and then prints to the screen.
//
// By James Henderson 2012.

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
//#include <sys/stat.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <wiringPi.h>
#include <unistd.h>

void read_data(int,int);
int main(int argc, char **argv)
{
	
	int fd;														// File descrition
	char *fileName = "/dev/i2c-1";								// Name of the port we will be using
																// Address of TPA81 shifted right 1 bit
	unsigned int  address = 0x28;										// Address of TPA81 shifted right 1 bit
	int term =1 ;
	unsigned char buf[10];										// Buffer for data being read/ written on the i2c bus
	
	
	if (argc != 3)
	{
		printf("Usage : \n\t %s HT-01D address( in Decimal) Term(in Second)\n",argv[0]);
		exit(0xff);
	}
	
	address = atoi(argv[1]);
	term = atoi(argv[2]);
	if (address <0x20 || address > 0x40)
	{
		printf("Please check your device address \n\t Address %d is not in range %d - %d\n ",argv[1],0x20,0x40);
		exit(0xFE);
	}
	
	term = atoi(argv[2]);
	if (term <= 0 )
	{
		printf("Term data is not proper!! %d second",term);
		exit(0xFE);
	}
	
	if ((fd = open(fileName, O_RDWR)) < 0) {					// Open port for reading and writing
		printf("Failed to open i2c port\n");
		
		exit(1);
	}
	
	if (ioctl(fd, I2C_SLAVE, address) < 0) {					// Set the port options and set the address of the device we wish to speak to
		printf("Unable to get bus access to talk to slave\n\tAre you sure that the device is exist?\n");
		exit(1);
	}
	
	while(1)
	{
		read_data(fd,address);
		sleep(term);
	}
	
	return 0;
}
void read_data(int fd,int addr)
{
	unsigned char buf[10]={0,};
	
	if ((write(fd, buf, 0)) != 0) {								// Send register to read from
		printf("Error writing to i2c slave\n\tAre you sure that the device is exist?\n");
		exit(1);
	}

	usleep(1000*200);

	if ( read(fd, buf, 4) != 4 ) {								// Read back data into buf[]
		printf("Unable to read from slave\n"); 
		exit(1);
	}
	
	else {
		unsigned char status =0x00;
		float humidity =0.0f;
		unsigned int temp =0x00;
		float temperature =0x00000;
		
		status = (buf[0] & 0xC0) >> 6;
		if (status == 0x00)
		{
			temp = (( unsigned  int)(buf[0] & 0x3F))<<8 | buf[1];
			humidity = (float)temp;
			humidity = (float)(((float)(temp))/(0x4000))*100;
			temp = (( unsigned  int)(buf[2]<<6))|(buf[2]>>2);
			temperature = (float)(((float)(temp))/(0x4000))*164 - 40;
			printf("ID\t%d\tHumidity \t%8.3f\tTemperature\t%8.3f  \n",addr,humidity,temperature);
		}
		else{
			printf("Error\tStatus code is not 1\n");

		}
		
	}
	
}


