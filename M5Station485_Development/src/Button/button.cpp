#include "./include/button.h"
#include "esp_log.h"

static const char *BUTTON_TAG = "BUTTON";

void handleButtonA(void *pvParameters)
{
    while (1)
    {
        if(M5.BtnA.wasPressed())
        {
            M5.Lcd.setCursor(60, 30);
            M5.Lcd.println("Button A is pressed");
            ESP_LOGI(BUTTON_TAG, "Button A is pressed");
        }
        if(M5.BtnA.pressedFor(2000))
        {
            M5.Lcd.fillRect(60, 30, 300, 15, TFT_WHITE);
            ESP_LOGI(BUTTON_TAG, "Clear text of button A");
            M5.Lcd.setCursor(60, 30);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void handleButtonB(void *pvParameters)
{
    while (1)
    {
        if(M5.BtnB.wasPressed())
        {
            M5.Lcd.setCursor(60, 55);
            M5.Lcd.println("Button B is pressed");
            ESP_LOGI(BUTTON_TAG, "Button B is pressed");
        }
        if(M5.BtnB.pressedFor(2000))
        {
            M5.Lcd.fillRect(60, 55, 300, 15, TFT_WHITE);
            ESP_LOGI(BUTTON_TAG, "Clear text of button B");
            M5.Lcd.setCursor(60, 55);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void handleButtonC(void *pvParameters)
{
    while (1)
    {
        if(M5.BtnC.wasPressed())
        {
            M5.Lcd.setCursor(60, 80);
            M5.Lcd.println("Button C is pressed");
            ESP_LOGI(BUTTON_TAG, "Button C is pressed");
        }
        if(M5.BtnC.pressedFor(2000))
        {
            M5.Lcd.fillRect(60, 80, 300, 15, TFT_WHITE);
            ESP_LOGI(BUTTON_TAG, "Clear text of button C");
            M5.Lcd.setCursor(60, 80);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}