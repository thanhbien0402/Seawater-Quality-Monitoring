#include "./include/gps.h"
#include "./Global/include/global.h"
#include <M5Station.h>

const char* GPS_TAG = "GPS";

TinyGPSPlus gps;
HardwareSerial GPSSerial(GPS_UART_NUM);

double latitude = 10.773281;
double longitude = 106.660605;

void displayGPSInfo()
{
    static double lastLat = 0.0, lastLng = 0.0;
    static uint8_t lastHour = -1, lastMinute = -1, lastSecond = -1;
    static uint8_t lastDay = -1, lastMonth = -1;
    static uint16_t lastYear = -1;
    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    int labelX = 10;
    int valueX = 100;
    int baseY = 10;
    int lineSpacing = 25;

    int yLat  = baseY + lineSpacing * 0;
    int yLng  = baseY + lineSpacing * 1;
    int yDate = baseY + lineSpacing * 2;
    int yTime = baseY + lineSpacing * 3;
    int yMac  = baseY + lineSpacing * 4;

    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();

        if (latitude != lastLat || longitude != lastLng) {
            lastLat = latitude;
            lastLng = longitude;

            M5.Lcd.fillRect(valueX, yLat, 220, 15, BLACK);
            M5.Lcd.fillRect(valueX, yLng, 220, 15, BLACK);

            M5.Lcd.setTextColor(YELLOW, BLACK);
            M5.Lcd.setCursor(labelX, yLat);
            M5.Lcd.print("Lat: ");
            M5.Lcd.setCursor(labelX, yLng);
            M5.Lcd.print("Lng: ");

            // In giá trị
            M5.Lcd.setTextColor(CYAN, BLACK);
            M5.Lcd.setCursor(valueX, yLat);
            M5.Lcd.printf("%.6f", latitude);
            M5.Lcd.setCursor(valueX, yLng);
            M5.Lcd.printf("%.6f", longitude);
        }
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    uint8_t hourInVietNam = timeinfo.tm_hour;
    uint8_t minute = timeinfo.tm_min;
    uint8_t second = timeinfo.tm_sec;
    uint16_t year = timeinfo.tm_year + 1900;
    uint8_t month = timeinfo.tm_mon + 1;
    uint8_t day = timeinfo.tm_mday;

    if (year != lastYear || month != lastMonth || day != lastDay) {
        lastYear = year;
        lastMonth = month;
        lastDay = day;

        M5.Lcd.fillRect(valueX, yDate, 220, 15, BLACK);

        M5.Lcd.setTextColor(YELLOW, BLACK);
        M5.Lcd.setCursor(labelX, yDate);
        M5.Lcd.print("Date: ");

        M5.Lcd.setTextColor(CYAN, BLACK);
        M5.Lcd.setCursor(valueX, yDate);
        M5.Lcd.printf("%02d/%02d/%04d", day, month, year);
    }

    if (hourInVietNam != lastHour || minute != lastMinute || second != lastSecond) {
        lastHour = hourInVietNam;
        lastMinute = minute;
        lastSecond = second;

        M5.Lcd.fillRect(valueX, yTime, 220, 15, BLACK);

        M5.Lcd.setTextColor(YELLOW, BLACK);
        M5.Lcd.setCursor(labelX, yTime);
        M5.Lcd.print("Time: ");

        M5.Lcd.setTextColor(CYAN, BLACK);
        M5.Lcd.setCursor(valueX, yTime);
        M5.Lcd.printf("%02d:%02d:%02d", hourInVietNam, minute, second);
    }

    static bool macDisplayed = false;

    if (!macDisplayed && mac[0] != 0) {
        macDisplayed = true;

        M5.Lcd.setTextColor(YELLOW, BLACK);
        M5.Lcd.setCursor(10, yMac);
        M5.Lcd.printf("MAC: ");
        M5.Lcd.setTextColor(CYAN, BLACK);
        M5.Lcd.printf("%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
}

void updateGPSPosition(void *pvParameters)
{
    M5.begin();
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(2);
  
    GPSSerial.begin(115200, SERIAL_8N1, 13, 14); 
  
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.println("Starting GPS...");

    volatile bool hasValidFix = false;

    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t delay = 1000 / portTICK_PERIOD_MS;

    while (true) {
        while (GPSSerial.available() > 0) {
            if (gps.encode(GPSSerial.read())) {
                if (!hasValidFix && gps.location.isValid()) {
                    hasValidFix = true;
                    M5.Lcd.fillRect(10, 30, 240, 20, BLACK);
                }

                displayGPSInfo();
            }
        }

        if (GPSSerial.available() < 0 && gps.charsProcessed() < 10) {
            M5.Lcd.println("No GPS detected");
        }

        vTaskDelayUntil(&lastWakeTime, delay);
    }
    vTaskDelete(NULL);
}