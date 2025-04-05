#include "./include/ota.h"
#include "./Global/include/global.h"

static const char *OTA_TAG = "OTA"; 

void performOTAUpdate(const char* fwTitle, const char* fwVersion) {
    // https://demo.thingsboard.io/api/v1/$ACCESS_TOKEN/firmware?title=$TITLE&version=$VERSION
    std::string otaURL = "https://demo.thingsboard.io/api/v1/";
    otaURL += MQTT_ACCESS_TOKEN;
    otaURL += "/firmware?title=";
    otaURL += fwTitle;
    otaURL += "&version=";
    otaURL += fwVersion;
    ESP_LOGI(OTA_TAG, "OTA URL: %s", otaURL.c_str());

    // create client HTTPS (for demo use setInsecure, production need setCACert)

    t_httpUpdate_return ret = ESPhttpUpdate.update(otaURL.c_str()); // HTTP
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("OTA Update thất bại (%d): %s\n",
                          ESPhttpUpdate.getLastError(),
                          ESPhttpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("Không có cập nhật OTA mới.");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("OTA Update thành công.");
            break;
    }

    // HTTPClient http;
    // http.begin(otaURL.c_str());
    // int httpCode = http.GET();

    // if (httpCode == HTTP_CODE_OK) {
    //     WiFiClient *stream = http.getStreamPtr();
    //     size_t contentLength = http.getSize();

    //     if (contentLength > 0) {
    //         uint8_t buffer[128];
    //         size_t written = 0;

    //         while (http.connected() && (written < contentLength)) {
    //             int len = stream->readBytes(buffer, sizeof(buffer));
    //             written += Update.write(buffer, len);

    //             ESP_LOGI(OTA_TAG, "Writing at 0x%08lX... (%d %%)", written, (written * 100) / contentLength);
    //         }

    //         if (Update.end(true)) {
    //             if (Update.isFinished()) {
    //                 ESP_LOGI(OTA_TAG, "OTA update finished successfully.");
    //             } else {
    //                 ESP_LOGE(OTA_TAG, "OTA update failed: %d", Update.getError());
    //             }
    //         } else {
    //             ESP_LOGE(OTA_TAG, "Update.end() failed: %d", Update.getError());
    //         }
    //     } else {
    //         ESP_LOGE(OTA_TAG, "Content length is zero or negative.");
    //     } 
    // } else {
    //     ESP_LOGE(OTA_TAG, "HTTP GET failed, error: %d", httpCode);
    // }

    // http.end();
}

void handleOTAUpdate() {
    if (otaUpdateTriggered) {
        ESP_LOGI(OTA_TAG, "OTA update triggered.");
        performOTAUpdate(receivedFwTitle, receivedFwVersion);
        otaUpdateTriggered = false;
    }
}

void otaUpdateTask(void *pvParameters) {
    while (true) {
        // Wait for the OTA update to be triggered
        if (otaUpdateTriggered) {
            handleOTAUpdate();
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Delay to avoid busy-waiting
    }
}