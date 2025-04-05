#include "./LED/include/blinky_led.h"
#include "./Button/include/button.h"
#include "./WiFi/include/wifiprovision.h"
#include "./MQTT/include/mqtt.h"

#include "./ESSensor/include/es_sensor.h"
#include "./NVS/include/nvs_store.h"
#include "./Global/include/global.h"
#include "./OTA/include/ota.h"

static const char * MAIN_TAG = "MAIN";

void system_init(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret == ESP_OK)
    {
        ESP_LOGI(MAIN_TAG, "NVS flash init success");
    }
    else
    {
        ESP_LOGE(MAIN_TAG, "NVS flash init failed");
    }

    ret = nvs_flash_init_partition("nvs_data");
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase_partition("nvs_data"));
        ret = nvs_flash_init_partition("nvs_data");
    }

    if (ret == ESP_OK)
    {
        ESP_LOGI(MAIN_TAG, "NVS data flash init success");
    }
    else
    {
        ESP_LOGE(MAIN_TAG, "NVS data flash init failed");
    }
    nvs_init();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // esp_netif_create_default_wifi_sta();
}

void task_M5_setup(void *pvParameters)
{
    M5.begin();

    M5.Lcd.fillScreen(TFT_WHITE);
    M5.Lcd.setTextColor(TFT_BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50, 5);
    M5.Lcd.println("Hello M5Stack!");
    Serial.println("Hello M5Stack!");
    M5.Lcd.qrcode("https://docs.m5stack.com/en/core/station_485", 0, 0, 45);
    Serial.println("+--------------------------------------++++++++++---------------------------------------+");
    while (1)
    {
        M5.update();
        M5.Lcd.setTextSize(1);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  system_init();
  initEventGroup();

  initRS485(UART_NUM_2, BAUDRATE, RXD, TXD);
  xTaskCreate(connectWiFi, "connectWiFi", 4096, NULL, 10, NULL);
  xTaskCreate(task_M5_setup, "task_M5_setup", 4096, NULL, 5, NULL);

  xTaskCreate(handleBlinkyLed, "blinky_led", 4096, NULL, 5, NULL);   
  xTaskCreate(handleButtonA, "buttonA", 4096, NULL, 5, NULL);
  xTaskCreate(handleButtonB, "buttonB", 4096, NULL, 5, NULL);
  xTaskCreate(handleButtonC, "buttonC", 4096, NULL, 5, NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  
  xTaskCreate(mqttTask, "mqttTask", 4096, NULL, 5, NULL);
  xTaskCreate(publisherTask, "publisherTask", 4096, NULL, 5, NULL);
  xTaskCreate(otaUpdateTask, "otaUpdateTask", 8192, NULL, 10, NULL);
  // xTaskCreate(checkPumpStatusTask, "checkPumpStatusTask", 4096, NULL, 5, NULL);
}

void loop() {
    Serial.println("FW 2");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>
// #include <Update.h>
// #include "esp_ota_ops.h"
// #include "esp_system.h"
// // WiFi & MQTT Broker Information
// const char* ssid = "AP123";
// const char* password = "12345678";
// const char* mqtt_server = "demo.thingsboard.io";
// const char* access_token = "ypdqonvmc720mgvleb8j";  // Access Token của thiết bị

// void download_firmware(String title, String version);  // Forward declaration for download_firmware

// WiFiClient espClient;
// PubSubClient client(espClient);

// // Biến lưu firmware từ MQTT
// String fw_title;
// String fw_version;

// // Hàm kết nối WiFi
// void setup_wifi() {
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("\nWiFi connected!");
// }

// // Callback khi nhận dữ liệu MQTT
// void callback(char* topic, byte* payload, unsigned int length) {
//     Serial.print("Received MQTT message on topic: ");
//     Serial.println(topic);
    
//     String message;
//     for (int i = 0; i < length; i++) {
//         message += (char)payload[i];
//     }
//     Serial.println("Message: " + message);

//     // Kiểm tra nếu nhận được thông tin firmware
//     if (String(topic) == "v1/devices/me/attributes") {
//         // Giả sử payload nhận được là JSON chứa thông tin firmware
//         DynamicJsonDocument doc(512);
//         deserializeJson(doc, message);
        
//         if (doc.containsKey("fw_title") && doc.containsKey("fw_version")) {
//             fw_title = doc["fw_title"].as<String>();
//             fw_version = doc["fw_version"].as<String>();

//             Serial.println("Firmware info received:");
//             Serial.println("Title: " + fw_title);
//             Serial.println("Version: " + fw_version);

//             // Gọi hàm tải firmware qua HTTP
//             download_firmware(fw_title, fw_version);
//         }
//     }
// }

// // Hàm tải firmware từ ThingsBoard qua HTTP
// void download_firmware(String title, String version) {
//     String url = "https://demo.thingsboard.io/api/v1/" + String(access_token) +
//                  "/firmware?title=" + title + "&version=" + version;

//     Serial.println("Downloading firmware from: " + url);
    
//     HTTPClient http;
//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode == HTTP_CODE_OK) {
//         int contentLength = http.getSize(); 
//         WiFiClient * stream = http.getStreamPtr();

//         if (Update.begin(contentLength)) {
//             Serial.println("Start writing firmware...");

//             size_t written = Update.writeStream(*stream);
//             if (written == contentLength) {
//                 Serial.println("Firmware written successfully!");
//                 if (Update.end(true)) {
//                     Serial.println("OTA update successful! Restarting...");
//                     ESP.restart(); 
//                 } else {
//                     Serial.println("Update.end() failed!");
//                 }
//             } else {
//                 Serial.println("Firmware write failed!");
//             }
//         } else {
//             Serial.println("Not enough space for OTA update!");
//         }
//     } else {
//         Serial.println("Firmware download failed, HTTP code: " + String(httpCode));
//     }
//     http.end();
// }


// void setup() {
//     Serial.begin(115200);
//     setup_wifi();
//     // // Ép boot từ factory
//     // const esp_partition_t* factory_partition = esp_partition_find_first(
//     //     ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
//     // if (factory_partition) {
//     //     Serial.println("Forcing boot to Factory firmware...");
//     //     esp_ota_set_boot_partition(factory_partition);
//     //     esp_restart();
//     // }
    
//     client.setServer(mqtt_server, 1883);
//     client.setCallback(callback);
// }
