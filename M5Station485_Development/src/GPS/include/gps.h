#ifndef _GPS_H_
#define _GPS_H_

#include <Arduino.h>
#include <TinyGPSPlus.h>

#define GPS_UART_NUM          1
#define GPS_RXD               13
#define GPS_TXD               14
#define GPS_BAUDRATE          115200

extern const char* GPS_TAG;
extern unsigned long GPSPreviousMillis;
extern const long GPSInterval;
extern unsigned long GPSCurrentMillis;
extern unsigned long startGPS;
extern bool GPSIsReady;

extern TinyGPSPlus gps;
extern HardwareSerial GPSSerial;

extern double latitude;
extern double longitude;

// void initGPS();
void displayGPSInfo();
void updateGPSPosition(void *pvParameters);

#endif