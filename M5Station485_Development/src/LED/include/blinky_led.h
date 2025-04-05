#ifndef _BLINKY_LED_H_
#define _BLINKY_LED_H_

#include <Arduino.h>
#include <FastLED.h>

#define NUM_LED 7
#define DATA_PIN 4

extern CRGB led[NUM_LED];

extern unsigned long previousPointOfTime;
extern unsigned long currentPointOfTime;
extern const long intervalLedBlinky;
extern bool led_1_state;
extern bool led_2_state;
extern bool led_3_state;
extern bool led_4_state;
extern bool led_5_state;
extern bool led_6_state;
extern bool led_7_state;

void setupBlinkyLed();
void handleBlinkyLed(void *pvParameters);

#endif