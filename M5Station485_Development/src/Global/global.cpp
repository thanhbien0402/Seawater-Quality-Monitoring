#include "./Global/include/global.h"

char MQTT_ACCESS_TOKEN[13];  // 12 hex chars + 1 for null terminator

void init_mqtt_token() {
    uint8_t mac[6];
    esp_err_t result = esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (result != ESP_OK) {
        ESP_LOGE("GLOBAL", "Failed to read MAC address: %s", esp_err_to_name(result));
        // Set a default token in case of failure
        strcpy(MQTT_ACCESS_TOKEN, "000000000000");
        return;
    }
    
    // Format the MAC address as a hex string without separators
    snprintf(MQTT_ACCESS_TOKEN, sizeof(MQTT_ACCESS_TOKEN), "%02X%02X%02X%02X%02X%02X", 
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    ESP_LOGI("GLOBAL", "MQTT access token set to: %s", MQTT_ACCESS_TOKEN);
}