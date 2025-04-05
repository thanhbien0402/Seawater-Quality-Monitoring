#include "./include/mqtt.h"
#include "./include/internal_mqtt.h"
#include "./ESSensor/include/es_sensor.h"
#include "./Global/include/global.h"
#include "./OTA/include/ota.h"

// // #include <HTTPClient.h>
// // #include "Update.h"
// // #include <mbedtls/md.h> // SHA256

// // #define CURRENT_FW_VERSION "1.0" 
static const char *MQTT_TAG = "MQTT";
static const char *TIMER_TAG = "TIMER";

WiFiClient espClient;
PubSubClient client(espClient);

TaskHandle_t mqttTaskHandle;
TaskHandle_t publisherTaskHandle;
TaskHandle_t sensor_task_handle;
// static QueueHandle_t control_queue;
EventGroupHandle_t mqtt_event_group;

static esp_timer_handle_t periodic_timer;
static uint32_t g_unix_get_timestamp = 0;
uint64_t measurement_time = 60000;      // Read sensor data every 60 seconds

volatile bool otaUpdateTriggered = false;
char receivedFwTitle[32] = {0}, receivedFwVersion[16] = {0};

// MQTT topics
const char* pubTopic = "v1/devices/me/telemetry";
const char* subTopic = "v1/devices/me/rpc/request/+";
// // const char* otaTopic = "v1/devices/me/attributes";

static void periodic_timer_callback(void *arg)
{
    // Notify the sensor task
    xTaskNotifyGive(sensor_task_handle);
}

void start_esp_timer()
{
    esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .name = "periodic_sensors_task_timer"
    };

    esp_err_t err = esp_timer_create(&periodic_timer_args, &periodic_timer);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "Failed to create periodic timer!!!");
        return;
    }
    
    err = esp_timer_start_periodic(periodic_timer, measurement_time * 1000);  // Convert to microseconds
    if (err != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "Failed to start periodic timer!!!");
        esp_timer_delete(periodic_timer);
        return;
    }

    ESP_LOGI(TIMER_TAG, "Start Periodic Timer successfully: %lld seconds!!!", measurement_time);
}

void stop_esp_timer()
{
    esp_timer_stop(periodic_timer);
    esp_timer_delete(periodic_timer);
    periodic_timer = NULL;
    ESP_LOGI(TIMER_TAG, "Stop and delete periodic timer!!!");
}

void initEventGroup() {
    mqtt_event_group = xEventGroupCreate();
    if (mqtt_event_group == NULL) {
        ESP_LOGE(MQTT_TAG, "Failed to create event group");
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    ESP_LOGI(MQTT_TAG, "Message arrived in topic: %s", topic);

    std::string message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    ESP_LOGI(MQTT_TAG, "Message: %s", message.c_str());
    
    uint8_t topic_case = 0;
    if (strcmp(topic, subTopic) == 0) {
        ESP_LOGI(MQTT_TAG, "Received RPC request: %s", message.c_str());
        topic_case = 0;
    }
    else if (strcmp(topic, "v1/devices/me/attributes") == 0) {
        ESP_LOGI(MQTT_TAG, "Received attributes: %s", message.c_str());
        topic_case = 1;
    } else {
        ESP_LOGE(MQTT_TAG, "Unknown topic: %s", topic);
        // return;
    }

    switch (topic_case) {
        case 0:
            ESP_LOGI(MQTT_TAG, "Received command to turn pump relay ON");
            break;
        case 1:
            ESP_LOGI(MQTT_TAG, "Received command to turn pump relay OFF");
            break;
        default:
            ESP_LOGE(MQTT_TAG, "Unknown command received: %s", message.c_str());
            // return;
    }

    std::string param_value = extractParamValue(message, "control_pump");
    std::string fw_checksum = extractParamValue(message, "fw_checksum");
    std::string fw_algorithm = extractParamValue(message, "fw_checksum_algorithm");
    std::string fw_size = extractParamValue(message, "fw_size");
    std::string fw_title = extractParamValue(message, "fw_title");
    std::string fw_version = extractParamValue(message, "fw_version");

    ESP_LOGI(MQTT_TAG, "control_pump: %s", param_value.c_str());
    ESP_LOGI(MQTT_TAG, "fw_checksum: %s", fw_checksum.c_str());
    ESP_LOGI(MQTT_TAG, "fw_algorithm: %s", fw_algorithm.c_str());
    ESP_LOGI(MQTT_TAG, "fw_size: %s", fw_size.c_str());
    ESP_LOGI(MQTT_TAG, "fw_title: %s", fw_title.c_str());
    ESP_LOGI(MQTT_TAG, "fw_version: %s", fw_version.c_str());


    if (fw_checksum.length() > 0 && strcmp(fw_algorithm.c_str(), "SHA256") == 0 && fw_size.length() > 0) {
        ESP_LOGI(MQTT_TAG, "New firmware available for update.");
        ESP_LOGI(MQTT_TAG, "Firmware checksum: %s", fw_checksum.c_str());

        Serial.println("Có firmware mới. Trigger OTA update.");
        strncpy(receivedFwTitle, fw_title.c_str(), sizeof(receivedFwTitle)-1);
        strncpy(receivedFwVersion, fw_version.c_str(), sizeof(receivedFwVersion)-1);
        otaUpdateTriggered = true;

        // xTaskCreate(ota_update_task, "OTA Update Task", 8192, NULL, 4, NULL);
    } else {
        Serial.println("Không có bản cập nhật nào.");
    }

    if (strcmp(param_value.c_str(), "ON") == 0) {
        ESP_LOGI(MQTT_TAG, "Turning pump relay ON");
    
        // Call the function to turn the relay ON
        turnPumpRelay(true);    

        if (client.connected())
        {
            char payload[64];
            sprintf(payload, "{\"pumpStatus\": %d}", 1);
            ESP_LOGI(MQTT_TAG, "Publishing payload: %s", payload);

            if (client.publish(pubTopic, payload))
            {
                ESP_LOGI(MQTT_TAG, "Published successfully");
            }
            else
            {
                ESP_LOGE(MQTT_TAG, "Publish failed");
            }
        }
            
    }

    else if (strcmp(param_value.c_str(), "OFF") == 0) {
        ESP_LOGI(MQTT_TAG, "Turning pump relay OFF");
        
        // Call the function to turn the relay OFF
        turnPumpRelay(false);
        
        if (client.connected())
        {
            char payload[64];
            sprintf(payload, "{\"pumpStatus\": %d}", 0);
            ESP_LOGI(MQTT_TAG, "Publishing payload: %s", payload);

            if (client.publish(pubTopic, payload))
            {
                ESP_LOGI(MQTT_TAG, "Published successfully");
            }
            else
            {
                ESP_LOGE(MQTT_TAG, "Publish failed");
            }
        }

    }

    else {
        ESP_LOGE(MQTT_TAG, "Invalid control pump command!!! Just: control_pump: ON/OFF\n");
    }
}

void mqttTask(void* parameter) {
    while (!WiFi.isConnected()) {
        ESP_LOGW(MQTT_TAG, "MQTT Task: Waiting for WiFi connection...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(mqttCallback);

    while (1) 
    {
        if (!client.connected()) {
            ESP_LOGI(MQTT_TAG, "Connecting to MQTT broker...");
            while (!client.connected()) {
                if (client.connect("ESP32_Client", MQTT_ACCESS_TOKEN, MQTT_PASSWORD)) {
                    ESP_LOGI(MQTT_TAG, "Connected to MQTT broker");
                    client.subscribe(subTopic);
                    ESP_LOGI(MQTT_TAG, "Subscribed to topic: %s", subTopic);
                    client.subscribe("v1/devices/me/attributes"); // Subscribe to attributes topic
                    ESP_LOGI(MQTT_TAG, "Subscribed to topic: %s", "v1/devices/me/attributes");

                } else {
                    ESP_LOGE(MQTT_TAG, "Failed. Retrying in 5 seconds.");
                    ESP_LOGE(MQTT_TAG, "State: %d", client.state());
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                }
            }
        }
        client.loop();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void publisherTask(void* parameter) {
    sensor_task_handle = xTaskGetCurrentTaskHandle();
    start_esp_timer();

    while (1) 
    {
        ulTaskNotifyTake(pdTRUE, (TickType_t)portMAX_DELAY);    // Wait for the notification from the timer callback
        xEventGroupClearBits(mqtt_event_group, MQTT_TASK_DONE_BIT);
        
        if (client.connected()) {
            // Read sensor data, uncomment the following lines to read sensor data
            // float tur_value = read_sensor_data(data_tur, sizeof(data_tur), "TUR");      // Read TUR sensor data (NTU)
            // float ph_value = read_sensor_data(data_ph, sizeof(data_ph), "pH");          // Read pH sensor data
            // float ec_value = read_sensor_data(data_ec, sizeof(data_ec), "EC");          // Read EC sensor data (μS/cm) 
            // float orp_value = read_sensor_data(data_orp, sizeof(data_orp), "ORP");      // Read ORP sensor data (mV)          

            // Random sensor data for testing
            float ph_value = random(0, 14);         // pH
            float orp_value = random(-2000, 2000);  // mV
            float ec_value = random(0, 2000);       // μS/cm
            float tur_value = random(10, 4000);     // NTU
            
            char payload[256];
            sprintf(payload, "{\"pH\": %.2f, \"ORP\": %.2f, \"EC\": %.2f, \"TUR\": %.2f}", ph_value, orp_value, ec_value, tur_value);

            ESP_LOGI(MQTT_TAG, "Publishing payload: %s", payload);

            // Publish data to ThingsBoard
            if (client.publish(pubTopic, payload)) {
                ESP_LOGI(MQTT_TAG, "Published successfully");
            } else {
                ESP_LOGE(MQTT_TAG, "Publish failed");
            }
        }
        else 
        {
            ESP_LOGW(MQTT_TAG, "MQTT client not connected. Retrying in 5 seconds.");
        }
        // Don't need to use vTaskDelay() here because the timer callback will notify this task
        xEventGroupSetBits(mqtt_event_group, MQTT_TASK_DONE_BIT);
    }
}

// void checkPumpStatusTask(void* parameter)
// {
//     while (1)
//     {
//         vTaskDelay(5000 / portTICK_PERIOD_MS);
//     }
// }

// void handle_pump_relay(char* cmd) {
//     if (cmd == NULL) {
//         ESP_LOGE(MQTT_TAG, "Invalid control pump command!!! Just: control_pump: ON/OFF");
//         return;
//     }
//     char *relay_state = strtok(cmd, ":");
//     if (relay_state == "ON") {
//         ESP_LOGI(MQTT_TAG, "Turning pump relay ON");
//         //TODO: Control relay, call the function to turn the relay OFF

//     } 
//     else if (relay_state == "OFF") {
//         ESP_LOGI(MQTT_TAG, "Turning pump relay OFF");
//         //TODO: Control relay, call the function to turn the relay ON
//     }
//     else {
//         ESP_LOGE(MQTT_TAG, "Invalid control pump command!!! Just: control_pump: ON/OFF");
//     } 
// }

// typedef void (*command_handler_t)(char *);
// typedef struct
// {
//     const char *command;
//     command_handler_t handler;
// } command_t;
// static const command_t command_table[] = {
//     {"control_pump", handle_pump_relay},       // Command to turn the pump relay on/off: control/water_relay:1
// };

// void controlTask(void *arg)
// {
//     // control_queue = xQueueCreate(2, sizeof(char) * 200);
//     // BaseType_t ret;
//     char command[200];
//     while (1)
//     {
//         // ret = xQueueReceive(control_queue, command, (TickType_t)500);
//         // if (ret == pdTRUE)
//         // {
//         //     Serial.printf("Received command data from queue: %s\n", command);
//         //     char *cmd = strtok(command, ":");
//         //     char *args = strtok(NULL, "");

//         //     // Search the command table for a matching command
//         //     uint8_t command_found_flag = 0;
//         //     for (size_t i = 0; i < sizeof(command_table) / sizeof(command_table[0]); i++)
//         //     {
//         //         if (strcmp(cmd, command_table[i].command) == 0)
//         //         {
//         //             command_table[i].handler(args);
//         //             command_found_flag = 1;
//         //             break;
//         //         }
//         //     }
//         //     if (!command_found_flag)
//         //     {
//         //         Serial.printf("Unknown command: %s !!!\n", cmd);
//         //     }
//         // }

//         if (xQueueReceive(control_queue, command, (TickType_t)500) == pdTRUE) {
//             Serial.printf("Received command data from queue: %s\n", command);

//             char *cmd = strtok(command, ":");
//             char *args = strtok(NULL, "");

//             if (cmd == NULL || args == NULL) {
//                 Serial.println("Invalid command format.");
//                 continue;
//             }

//             uint8_t command_found_flag = 0;
//             for (size_t i = 0; i < sizeof(command_table) / sizeof(command_table[0]); i++) {
//                 if (strcmp(cmd, command_table[i].command) == 0) {
//                     command_table[i].handler(args);
//                     command_found_flag = 1;
//                     break;
//                 }
//             }

//             if (!command_found_flag) {
//                 Serial.printf("Unknown command: %s !!!\n", cmd);
//             }
//         }

//     }
// }

std::string extractParamValue(const std::string& message, const std::string& key) {
    std::string keyPattern = "\"" + key + "\":";
    size_t keyStart = message.find(keyPattern);
    if (keyStart == std::string::npos) {
        return "";
    }

    size_t valueStart = keyStart + keyPattern.length();
    while (valueStart < message.size() && isspace(message[valueStart])) {
        valueStart++;
    }

    if (valueStart >= message.size()) {
        return "";
    }

    // Xử lý giá trị
    if (message[valueStart] == '"') {  // Giá trị là chuỗi
        size_t valueEnd = message.find('"', valueStart + 1);
        if (valueEnd == std::string::npos) {
            return "";
        }
        return message.substr(valueStart + 1, valueEnd - valueStart - 1);
    } else { 
        size_t valueEnd = message.find_first_of(",}", valueStart);
        if (valueEnd == std::string::npos) {
            return "";
        }
        
        while (valueEnd > valueStart && isspace(message[valueEnd - 1])) {
            valueEnd--;
        }
        return message.substr(valueStart, valueEnd - valueStart);
    }
}

// // std::string fw_url = "https://demo.thingsboard.io/api/v1", fw_checksum, fw_algorithm, fw_title, fw_version;
// // int fw_size = 0;

// void getOTAURLUpdate(const char* access_token, const char* fw_title, const char* fw_version) {
//     std::string otaURL = "https://demo.thingsboard.io/api/v1/";
//     otaURL += access_token;
//     otaURL += "/firmware?title=";
//     otaURL += fw_title;
//     otaURL += "&version=";
// }

// // String extractValue(String json, String key) {
// //     int startIndex = json.indexOf("\"" + key + "\":\"");
// //     if (startIndex == -1) {
// //         return "";
// //     }

// //     startIndex = json.indexOf("\"", startIndex + key.length() + 3) + 1;
// //     int endIndex = json.indexOf("\"", startIndex);
// //     return json.substring(startIndex, endIndex);
// // }

// // void ota_update_task(void *pvParameters) {
// //     Serial.println("Starting OTA update task...");

// //     // HTTPClient http;
// //     // http.begin(fw_url);
// //     int httpCode = http.GET();
// //     if (httpCode != HTTP_CODE_OK) {
// //         Serial.printf("HTTP GET failed, error: %d\n", httpCode);
// //         http.end();
// //         vTaskDelete(NULL);
// //     }

// //     int contentLength = http.getSize();
// //     WiFiClient *stream = http.getStreamPtr();

// //     if (contentLength != fw_size) {
// //         Serial.printf("Content length mismatch: expected %d, got %d\n", fw_size, contentLength);
// //         http.end();
// //         vTaskDelete(NULL);
// //     }

// //     if (!Update.begin(contentLength)) {
// //         Serial.println("Not enough space to begin OTA update");
// //         http.end();
// //         vTaskDelete(NULL);
// //     }

// //     uint8_t sha256_calc[32];
// //     mbedtls_md_context_t ctx;
// //     mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
// //     mbedtls_md_init(&ctx);
// //     mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
// //     mbedtls_md_starts(&ctx);

// //     size_t written = 0;
// //     while (written < contentLength) {
// //         uint8_t buffer[512];
// //         int bytesRead = stream->readBytes(buffer, sizeof(buffer));
// //         if (bytesRead > 0) {
// //             Update.write(buffer, bytesRead);
// //             mbedtls_md_update(&ctx, buffer, bytesRead);
// //             written += bytesRead;
// //         }
// //     }

// //     mbedtls_md_finish(&ctx, sha256_calc);
// //     mbedtls_md_free(&ctx);

// //     char sha256_str[65];
// //     for (int i = 0; i < 32; i++) {
// //         sprintf(&sha256_str[i * 2], "%02x", sha256_calc[i]);
// //     }
// //     sha256_str[64] = '\0';

// //     if (String(sha256_str) != fw_checksum) {
// //         Serial.printf("SHA256 checksum mismatch: expected %s, got %s\n", fw_checksum.c_str(), sha256_str);
// //         Update.end();
// //         http.end();
// //         vTaskDelete(NULL);
// //     }

// //     if (Update.end()) {
// //         Serial.println("OTA update successful, restarting...");
// //         esp_restart();
// //     } else {
// //         Serial.println("OTA update failed");
// //     }

// //     http.end();
// //     vTaskDelete(NULL);
// // }