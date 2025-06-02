#include "./include/ota.h"
#include "./Global/include/global.h"

TaskHandle_t otaUpdateTask_handle = NULL;

static const char *OTA_TAG = "OTA"; 


std::string getOTAURL(const char* fwTitle, const char* fwVersion) {
    // https://demo.thingsboard.io/api/v1/$ACCESS_TOKEN/firmware?title=$TITLE&version=$VERSION
    std::string otaURL = "https://";
    otaURL += MQTT_BROKER;
    otaURL += "/api/v1/";
    otaURL += MQTT_ACCESS_TOKEN;
    otaURL += "/firmware?title=";
    otaURL += fwTitle;
    otaURL += "&version=";
    otaURL += fwVersion;
    return otaURL;
}

void performOTAUpdate(const char* fwTitle, const char* fwVersion) {
    // https://demo.thingsboard.io/api/v1/$ACCESS_TOKEN/firmware?title=$TITLE&version=$VERSION
    std::string otaURL = getOTAURL(fwTitle, fwVersion);
    ESP_LOGI(OTA_TAG, "OTA URL: %s", otaURL.c_str());

    // Get current running partition
    const esp_partition_t* running = esp_ota_get_running_partition();
    const esp_partition_t* nextOTA = esp_ota_get_next_update_partition(running);
    
    if (running && nextOTA) {
        ESP_LOGI(OTA_TAG, "Current running partition: %s", running->label);
        ESP_LOGI(OTA_TAG, "Next OTA partition: %s", nextOTA->label);
    } else {
        ESP_LOGE(OTA_TAG, "Failed to get partition info");
        return;
    }

    // Create client HTTPS (for demo use setInsecure, production need setCACert)
    // ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

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
}

void handleOTAUpdate() {
    if (otaUpdateTriggered) {
        performOTAUpdate(receivedFwTitle, receivedFwVersion);
        otaUpdateTriggered = false;
    }
}

void otaUpdateTask(void *pvParameters) {
    while (true) {
        // Wait for the OTA update trigger
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        ESP_LOGI(OTA_TAG, "OTA update triggered!");
        handleOTAUpdate();
    }
}