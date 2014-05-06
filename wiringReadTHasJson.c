  
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_DEVICES (100)
char reader_version[100] = "1.0";
typedef struct 
{
	float humidity;
	float temperature;
}HumiTemp;

typedef struct 
{
	int fd;
	int deviceID;
	char devicePos[100];
	HumiTemp data;
}HTSensor;

int init_HT_10_Sensor(HTSensor *);
void read_HT_data(HTSensor *);
int print_HT_As_Json(HTSensor sensors  [] , int count);

int main(int argc, char **argv)
{
	
	char message[100] = "PassWord You?";
	char message2[] = "****";
	int devices_ID[MAX_DEVICES]={0x23,0,};
	char devices_position[MAX_DEVICES][100]={"LivingRoom Front","",};
	HTSensor sensors[MAX_DEVICES];
	int index = 0;
	HumiTemp data;
	
	
	for(index = 0;index<100;index++)
	{
		if(devices_ID[index] == 0 )
			devices_ID[index]=-1;
		else
		{
			sensors[index].deviceID =devices_ID[index];
			sprintf(sensors[index].devicePos,"%s",devices_position[index]);
		}
	}
	
	
	
	
	for(index = 0;index<100;index++)
	{
		if(devices_ID[index] > 0)
		{
			init_HT_10_Sensor(&sensors[index]);
			read_HT_data(&sensors[0]);
		}
	}

	print_HT_As_Json(sensors,1);
		
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
//void read_HT_data(HTSensor *sensor,HumiTemp * buffer)
void read_HT_data(HTSensor *sensor)
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
			sensor->data.humidity = humidity;
			sensor->data.temperature = temperature;
		}
		else{
			sensor->data.humidity = -1.0f;
			sensor->data.temperature = -1.0f;
		}
	}
}
int print_HT_As_Json(HTSensor sensors [] , int count)
{
	int i;
	char json[8000]={0,};
	char temp[300]={0,};
	sprintf(json,"{\n\t\"Version\":\"%s\",\n\n",reader_version);
	
	for (i=0;i<count-1;i++)
	{
			sprintf(temp,"\t\"0x%x\":{\"where\":\"%s\",\"Temperature\":\"%f\",\"Humidity\":\"%f\"},\n",sensors[i].deviceID,sensors[i].devicePos,sensors[i].data.temperature,sensors[i].data.humidity);
			strcat(json,temp);
	}
	sprintf(temp,"\t\"0x%x\":{\"where\":\"%s\",\"Temperature\":\"%f\",\"Humidity\":\"%f\"}",sensors[i].deviceID,sensors[i].devicePos,sensors[count-1].data.temperature,sensors[count-1].data.humidity);
	strcat(json,temp);
	strcat(json,"\n}\n");
	
	printf("%s",json);

}