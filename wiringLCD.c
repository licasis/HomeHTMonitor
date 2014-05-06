 
#include <stdio.h>
#include <wiringPi.h>

// wiringPi Pin Order 17,18,27,22,23,24,25,4

#define PIN_E (7)
#define PIN_RS (1)
#define PIN_RW (0)



#define PIN_4 (3)
#define PIN_5 (4)
#define PIN_6 (5)
#define PIN_7 (6)

#define LED (2)

int init_GPIO();
int do_clock();
void init_LCD_4bit_mode();
int write_data_higher(unsigned char , unsigned char ,unsigned char);
int write_data(unsigned char ,unsigned char ,unsigned char);
int main(int argc, char **argv)
{
	
	char message[] = "PassWord You?";
	char message2[] = "****";
	int i;
	 
	if(init_GPIO() < 0)
	{
		printf("GPIO InIt Error\n");
		return 1;
	}

	init_LCD_4bit_mode();
	i=0;
	//return 1;
	while(1)
	{	
		if(message[i] == '\0')
			break;
		write_data(0x01,0x00,(unsigned char)message[i]);
		i++;
	}
	printf("%s\n",message);
	i=0;
	write_data(0x00,0x00,0xC0);
	while(1)
	{	
		if(message2[i] == '\0')
			break;
		write_data(0x01,0x00,(unsigned char)message2[i]);
		i++;

	}
	printf("%s\n",message2);
	
	sleep(3);
	
	while(1)
	{
		int len1 = strlen(message);
		int len2 = strlen(message2);
		i=0;
		write_data(0x00,0x00,0x01);
		if(len1!=0)
			message[len1-1] = '\0';
		if(len2!=0)
			message2[len2-1] = '\0';
		if (len1 ==0 && len2==0)
			break;
		
		while(1)
		{	
			if(message[i] == '\0')
				break;
			write_data(0x01,0x00,(unsigned char)message[i]);
			i++;
		}
		i=0;
		write_data(0x00,0x00,0xC0);
		while(1)
		{	
			if(message2[i] == '\0')
				break;
			write_data(0x01,0x00,(unsigned char)message2[i]);
			i++;

		}
		sleep(1);
	
		
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


