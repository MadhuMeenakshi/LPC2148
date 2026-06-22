

/*PROGRAM TO MAKE THE LEDS CONNECTED FROM 0.0 TO 0.7 BLINK*/

#include<lpc21xx.h>

void delay(unsigned int time)
{ unsigned int i,j;
	for(i=0;i<time;i++)
	{
		for(j=0;j<7500;j++)
		{
			//generates an exact delay of 1 ms
		}
	}
	
}

int main()
{
	PINSEL0=0x0000;   //select ins 0-7 as gpios
	IODIR0=0xFF;     // configure port 0s pins as output pins
	while(1)
	{
		IOSET0=0xFF;//set the pins high
		delay(100);
		IOCLR0=0xFF;//clear the set pins
		delay(100);
	}
	
	return 0;
	
}