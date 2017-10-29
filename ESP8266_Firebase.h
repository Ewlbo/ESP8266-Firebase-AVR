#ifndef ESP8266_FIREBASE_H_
#define ESP8266_FIREBASE_H_

// Function prototypes
void ESPinit(void);
void waitFor(char* response);
void addData(char* name, int data);
void pushData(void);
void ping(void);
void tweet(char* message);

#endif /* ESP8266-FIREBASE_H_ */