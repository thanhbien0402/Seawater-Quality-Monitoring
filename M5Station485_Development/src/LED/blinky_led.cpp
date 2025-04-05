#include "./include/blinky_led.h"

CRGB led[NUM_LED];

unsigned long previousPointOfTime = 0;
const long intervalLedBlinky = 1000;
bool led_1_state = false;
bool led_2_state = false;
bool led_3_state = false;
bool led_4_state = false;
bool led_5_state = false;
bool led_6_state = false;
bool led_7_state = false;

void setupBlinkyLed()
{
    FastLED.addLeds<SK6812, DATA_PIN, RGB>(led, NUM_LED);
    FastLED.setBrightness(255);
}

void handleBlinkyLed(void *pvParameters)
{
    setupBlinkyLed();
    while (1)
    {
        unsigned long currentPointOfTime = millis();

        if(currentPointOfTime - previousPointOfTime >= intervalLedBlinky)
        {
            previousPointOfTime = currentPointOfTime;

            if(led_1_state == false)
            {
                led[0] = CRGB(5, 235, 5); // GRB ordering is typical
            }
            else led[0] = CRGB(100, 235, 5);
            led_1_state = !led_1_state;

            if(led_2_state == false)
            {
                led[1] = CRGB(100, 235, 5);
            }
            else led[1] = CRGB(235, 231, 5);
            led_2_state = !led_2_state;

            if(led_3_state == false)
            {
                led[2] = CRGB(235, 231, 5);
            }
            else led[2] = CRGB(235, 24, 5);
            led_3_state = !led_3_state;

            if(led_4_state == false)
            {
                led[3] = CRGB(235, 24, 5);
            }
            else led[3] = CRGB(218, 12, 245);
            led_4_state = !led_4_state;

            if(led_5_state == false)
            {
                led[4] = CRGB(218, 12, 245);
            }
            else led[4] = CRGB(5, 235, 116);
            led_5_state = !led_5_state;

            if(led_6_state == false)
            {
                led[5] = CRGB(5, 235, 116);
            }
            else led[5] = CRGB(255, 255, 255);
            led_6_state = !led_6_state;

            if(led_7_state == false)
            {
                led[6] = CRGB(255, 255, 255);
            }
            else led[6] = CRGB(5, 235, 5);
            led_7_state = !led_7_state;
        }

        FastLED.show();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}