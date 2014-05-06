  
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
// wiringPi Pin Order 17,18,27,22,23,24,25,4

#define PIN_E (7)
#define PIN_RS (1)
#define PIN_RW (0)



#define PIN_4 (3)
#define PIN_5 (4)
#define PIN_6 (5)
#define PIN_7 (6)

#define LED (2)

typedef struct 
{
	int deviceID;
	float humidity;
	float temperature;
	
}HumiTemp;

typedef struct 
{
	int fd;
	int deviceID;
}HTSensor;

int init_GPIO();
int init_HT_10_Sensor(HTSensor *);
int do_clock();
void init_LCD_4bit_mode();
int write_data_higher(unsigned char , unsigned char ,unsigned char);
int write_data(unsigned char ,unsigned char ,unsigned char);
void read_HT_data(HTSensor *,HumiTemp * );

int main(int argc, char **argv)
{
	
	char message[100] = "PassWord You?";
	char message2[] = "****";
	int i;
	HTSensor sensor;
	HumiTemp data;
	
	
	sensor.deviceID =0x23;
	data.deviceID = sensor.deviceID;
	init_HT_10_Sensor(&sensor);
	
	while(1)
	{	
		read_HT_data(&sensor,&data);
		sprintf(message,"%4.2fC   %4.2f%%",data.temperature,data.humidity);
		printf("%s\n",message);
		i=0;

		sleep(2);
		
	}
	return 0; 
}

int init_HT_10_Sensor(HTSensor *sensor)
{
	
	int fd;														// File descrition
	char *fileName = "/dev/i2c-1";								// Name of the port we will be using
																// Address of TPA81 shifted right 1 bit
	unsigned int  address = sensor->deviceID;										// Address of TPA81 shifted right 1 bit
	int term =1 ;
	unsigned char buf[10];										// Buffer for data being read/ written on the i2c bus
	
	if ((fd = open(fileName, O_RDWR)) < 0) {					// Open port for reading and writing
		printf("Failed to open i2c port\n");
		
		exit(1);
	}
	
	if (ioctl(fd, I2C_SLAVE, address) < 0) {					// Set the port options and set the address of the device we wish to speak to
		printf("Unable to get bus access to talk to slave\n\tAre you sure that the device is exist?\n");
		exit(1);
	}
	sensor->fd = fd;
	return 0;
}
void read_HT_data(HTSensor *sensor,HumiTemp * buffer)
{
	unsigned char buf[10]={0,};
	
	if ((write(sensor->fd, buf, 0)) != 0) {								// Send register to read from
		printf("Error writing to i2c slave\n\tAre you sure that the device is exist?\n");
		exit(1);
	}

	usleep(1000*200);

	if ( read(sensor->fd, buf, 4) != 4 ) {								// Read back data into buf[]
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
			buffer->humidity = humidity;
			buffer->temperature = temperature;
		}
		else{
			buffer->humidity = -1.0f;
			buffer->temperature = -1.0f;

		}
		
	}
	
}

 
