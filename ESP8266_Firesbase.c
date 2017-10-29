/*
	ESP8266-01 (ESP8266_NONOS_SDK) Firebase by Ewlbo
	Date: 29.10.2017
	https://github.com/Ewlbo/ESP8266-Firebase-AVR-ThingSpeak
	
	Program sends data to Google Firebase using ThingSpeak ThingHTTP App. All the links/guides are provided in GitHub
	
	UART communication is used between ESP8266 and ATmega328P 
	and it uses STDIO UART library made by Mika Tuupola (https://github.com/tuupola/avr_demo/tree/master/blog/simple_usart)
	
	Optional: link your Twitter account to ThingTweet app and tweet from ESP8266 using tweet("Message"); function
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "uart.h"
#include "ESP8266_Firebase.h"

#define _OK "OK"
#define _CONNECT "CONNECT"
#define _CLOSED "CLOSED"

char ssid[] = "SSID";
char psk[] = "Password";						// If no password leave it like this = "";
char api_key[] = "XXXXXXXXXXXXXXXX";			// Api key from ThingSpeak ThingHTTP app
char api_key_twitter[] = "XXXXXXXXXXXXXXXX";	// Api key from ThingSpeak ThingTweet app (optional)
int port = 80;									// Port for TCP connection
char buffer [64];								// Placeholder for handling data
char HTTP[512];									// HTTP request array placeholder
int dataCount = 0;

void ESPinit(void)
{
	// Reset ESP8266
	printf("AT+RST\r\n");
	_delay_ms(100);
	ping();					// Check if ready after reset
	
	// Connect to Wi-Fi
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,psk);
	waitFor("OK");
	
	// Client mode
	printf("AT+CWMODE=1\r\n");
	waitFor("OK");
	
	// Enable multiple connections
	printf("AT+CIPMUX=1\r\n");
	waitFor("OK");
	
	_delay_ms(500);
}

void waitFor(char* response)
{
	memset(buffer,0,strlen(buffer));		// Reset array
	scanf("\n%s", buffer);					// Get message from ESP8266
	if (strstr(buffer,response) == NULL)	// Finding specific ESP8266 response (returns NULL if not found)
	{
		waitFor(response);
	}
}

void addData(char* name, int data)
{
	if (dataCount == 0)
	{
		// HTTP GET request base
		strcat(HTTP,"GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=");
		// Add api_key to HTTP request array
		strcat(HTTP,api_key);
	}
	// Add the data to be sent
	strcat(HTTP,"&");
	strcat(HTTP,name);
	strcat(HTTP,"=");
	memset(buffer,0,strlen(buffer));
	itoa(data,buffer,10);				// Transform integer to string
	strcat(HTTP,buffer);
	dataCount++;
}

void pushData(void)
{
	// Check if ESP8266 is ready to send
	ping();
	
	// Start TCP connection with ThingSpeak on port 80 channel 0 (ESP8266 can handle 4 channels)
	printf("AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",%d\r\n",port);
	waitFor("OK");
	
	// Use channel 0 and define length of message
	printf("AT+CIPSEND=0,%d\r\n",strlen(HTTP)+34);	// 34 is the length of "HTTP/1.1 Host: api.thingspeak.com" added later on
	waitFor(_OK);
	
	// Send HTTP request with data
	printf("%s\nHTTP/1.1 Host: api.thingspeak.com\r\n",HTTP);

	// Wait for connection to get closed
	waitFor(_CLOSED);
	printf("Data sent\n");
	
	// Reset HTTP array and close connection to be sure
	printf("AT+CIPCLOSE=0\r\n");
	memset(HTTP,0,strlen(HTTP));
	dataCount=0;
}

void ping(void)
{
	// Check if ESP8266 is responding to AT with OK
	printf("AT\r\n");
	memset(buffer,0,strlen(buffer));		// Reset array
	scanf("\n%s", buffer);					// Get message from ESP8266
	if (strstr(buffer,"OK") == NULL)		// Finding specific response (returns NULL if not found)
	{
		ping();
		_delay_ms(100);						// Default interval: 100ms
	}
}

void tweet(char* message)
{
	// Note - Twitter wont allow to tweet message with the same text
	// Reset HTTP array
	memset(HTTP,0,strlen(HTTP));			
	// HTTP GET request base
	strcat(HTTP,"GET https://api.thingspeak.com/apps/thingtweet/1/statuses/update?api_key=");
	// Add api_key_twitter to HTTP request array
	strcat(HTTP,api_key_twitter);
	
	// Add the message to send
	strcat(HTTP,"&status=");
	strcat(HTTP,message);
	
	// Check if ESP8266 is ready to send
	ping();
	
	// Start TCP connection with ThingSpeak on port 80 channel 1 (ESP8266 can handle 4 channels)
	printf("AT+CIPSTART=1,\"TCP\",\"api.thingspeak.com\",%d\r\n",port);
	waitFor("OK");
	
	// Use channel 1 and define length of message
	printf("AT+CIPSEND=1,%d\r\n",strlen(HTTP)+34);	// 34 is the length of "HTTP/1.1 Host: api.thingspeak.com" added later on
	waitFor(_OK);
	
	// Send HTTP request with data
	printf("%s\nHTTP/1.1 Host: api.thingspeak.com\r\n",HTTP);

	// Wait for connection to get closed
	waitFor(_CLOSED);
	printf("Tweet sent\n");
	
	// Close connection to be sure
	printf("AT+CIPCLOSE=1\r\n");
}
