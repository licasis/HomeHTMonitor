// TPA81 example c code for the Raspberry pi.
//
// Reads the software revision of the TPA81 and all temperature data
// and then prints to the screen.
//
// By James Henderson 2012.

#include <stdio.h>
#include <wiringPiI2C.h>

/*
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
*/

void read_data(int,int);
int main(int argc, char **argv)
{
	int fd = -1;
	int data = 0; 
	int rtn = 0;
	fd = wiringPiI2CSetup(0x23) ;
	unsigned char reg[10];
	if (fd < 0)
	{
		printf("Error writing to i2c Setup \n");
		return -1;
	}
	printf("FD = %d\n",fd) ;
	rtn =  wiringPiI2CWriteReg8(fd,0x25,0);
	
	printf("RTN Write = %d\n",rtn) ;
	usleep(1000*500);
	data = wiringPiI2CReadReg8(fd,0);
	usleep(1000*500);
	printf("Data = %x\n",data);
	data = wiringPiI2CReadReg8(fd,1);
	printf("Data = %x\n",data);
	/*
	data = wiringPiI2CRead(fd);
	printf("Data = %x\n",data);
	data = wiringPiI2CRead(fd);
	printf("Data = %x\n",data);
	*/
	return 0;
}
/*
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
*/

