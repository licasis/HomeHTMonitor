  
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
	
	
	sensor.deviceID =0x25;
	data.deviceID = sensor.deviceID;
	if(init_GPIO() < 0)
	{
		printf("GPIO InIt Error\n");
		return 1;
	}

	init_LCD_4bit_mode();
	
	init_HT_10_Sensor(&sensor);
	
	
	
	
	while(1)
	{	
		
		read_HT_data(&sensor,&data);
		write_data(0x00,0x00,0x01);
		
		
		sprintf(message,"ID 0x%02x [%03d]",data.deviceID,data.deviceID);
		printf("%s\n",message);
		i=0;
		while(1)
		{	
			if(message[i] == '\0')
				break;
			write_data(0x01,0x00,(unsigned char)message[i]);
			i++;
		}
		
		sprintf(message,"%4.2fC   %4.2f%%",data.temperature,data.humidity);
		printf("%s\n",message);
		i=0;
		write_data(0x00,0x00,0xC0);
		while(1)
		{	
			if(message[i] == '\0')
				break;
			write_data(0x01,0x00,(unsigned char)message[i]);
			i++;
		}
		sleep(2);
		
	}
	return 0;
}
int do_clock()
{
	digitalWrite(PIN_E,1);
	usleep(2000);
	digitalWrite(PIN_E,0);
	return 0;
}


void init_LCD_4bit_mode()
{
	write_data_higher(0x00,0x00,0x30);
	usleep(1000);
	write_data_higher(0x00,0x00,0x30);
	usleep(500);
	write_data_higher(0x00,0x00,0x30);
	write_data_higher(0x00,0x00,0x20); // 4Bit Bus
	
	write_data(0x00,0x00,0x28); // Function set : 4Bit bus, 2Line  , 5X8 Dot 
	write_data(0x00,0x00,0x0C); // Display On, Cursor Off, Blinking Off
	write_data(0x00,0x00,0x04); // AC increase , No shift
	write_data(0x00,0x00,0x01); // Go to Home position

}

int init_GPIO()
{
	int index = 0;
	if(wiringPiSetup() == -1)
	{
		printf("Fail to init Wiring\n");
		return -1;
	}
	for(index=0;index<8;index++)
		 pinMode(index, OUTPUT); 
	digitalWrite(PIN_E,0);
	digitalWrite(PIN_RS,0);
	digitalWrite(PIN_RW,0);
	return 0;

}

int write_data_higher(unsigned char RS, unsigned char RW,unsigned char data)
{
	//it use lower 4 nimble
	unsigned char the_bit = 0x00;
	int index = 0;
	//printf("write_data_higher %d , %d : %0x\n",RS,RW,data);
	digitalWrite(PIN_RW,RW);
	digitalWrite(PIN_RS,RS);
	for (index=0;index<4;index++)
		digitalWrite((PIN_4+index),((data>>(index+4))&0x01) );
	do_clock();
	return 0;
	
}

int write_data(unsigned char RS,unsigned char RW,unsigned char data)
{
	unsigned char temp;

	temp = data &0xF0 ;
	write_data_higher(RS,RW,temp);
	temp = (data &0x0F)<<4 ;
	write_data_higher(RS,RW,temp);
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


