#ifndef OTA_H
#define OTA_H

#include <ESP32httpUpdate.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

// const char* currentFirmwareTitle   = "MyFirmware";
// const char* currentFirmwareVersion = "1.0.0";

extern volatile bool otaUpdateTriggered;
extern char receivedFwTitle[32];
extern char receivedFwVersion[16];
extern QueueHandle_t ota_queue;
extern TaskHandle_t otaUpdateTask_handle;

void performOTAUpdate(const char* fwTitle, const char* fwVersion);
void handleOTAUpdate();
void otaUpdateTask(void *pvParameters);

#endif // OTA_H