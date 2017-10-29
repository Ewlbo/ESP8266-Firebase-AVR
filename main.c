/*
	ESP8266-01 (ESP8266_NONOS_SDK) Firebase by Ewlbo
	Date: 29.10.2017
	https://github.com/Ewlbo/ESP8266-Firebase-AVR-ThingSpeak
	
	Program sends data to Google Firebase using ThingSpeak ThingHTTP App. All the links/guides are provided in GitHub
	
	UART communication is used between ESP8266 and ATmega328P 
	and it uses STDIO UART made by Mika Tuupola (https://github.com/tuupola/avr_demo/tree/master/blog/simple_usart)
*/

#define F_CPU 16000000UL 

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "ESP8266_Firebase.h"

unsigned int getDummyData();

int main (void)
{
	_delay_ms(1000);						// Delay on startup
	
	int data;

	UARTinit();								// Setup UART and STDIO
	ESPinit();								// Setup ESP8266
	printf("Setup complete, running program\n");
	while(1)
	{
		data = getDummyData();				// Get dummy data
		addData("data1",data);
		addData("data2",data);
		pushData();							// Push data to Firebase
	}
}

unsigned int getDummyData(void)
{
	unsigned int data;
	data = rand() % 42;						// Random number from 0 to 42
	return data;
}
