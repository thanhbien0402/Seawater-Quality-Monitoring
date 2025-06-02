#include "./include/mqtt.h"
#include "./include/internal_mqtt.h"
#include "./ESSensor/include/es_sensor.h"
#include "./Global/include/global.h"
#include "./OTA/include/ota.h"
#include "./Model/include/model.h"
#include "./GPS/include/gps.h"
#include "./Cron/include/cronjob.h"
#include "cJSON.h"

// // #include <HTTPClient.h>
// // #include "Update.h"
// // #include <mbedtls/md.h> // SHA256

static const char *MQTT_TAG = "MQTT";
static const char *TIMER_TAG = "TIMER";

WiFiClient espClient;
PubSubClient client(espClient);

TaskHandle_t mqttTaskHandle;
TaskHandle_t sensor_task_handle;
TaskHandle_t publish_executed_cronjob_handle = NULL;

static QueueHandle_t control_queue = xQueueCreate(4, 200 * sizeof(char));

EventGroupHandle_t mqtt_event_group = NULL;

ControlDef control_def = {0};
static const queue_map_t queue_table[] = {
    {"control", &control_queue},
};

static TaskHandler subscribe_task_list[100];
static uint8_t number_subscribe_task = 0;
static esp_timer_handle_t periodic_timer;
// static uint32_t g_unix_get_timestamp = 0;
uint64_t measurement_time = 60000;      // Read sensor data every 60 seconds

volatile bool otaUpdateTriggered = false;
char receivedFwTitle[32] = {0}, receivedFwVersion[16] = {0};

// MQTT topics
const char* pubTopic = "v1/devices/me/telemetry";
const char* subTopic = "v1/devices/me/rpc/request/+";
const char* otaTopic = "v1/devices/me/attributes";

static void save_measurement_time_to_nvs(uint64_t time) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("nvs_data", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Failed to open NVS for saving measurement_time: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_u64(nvs_handle, "meas_time", time);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Failed to save measurement_time to NVS: %s", esp_err_to_name(err));
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(MQTT_TAG, "Failed to commit measurement_time to NVS: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(MQTT_TAG, "Saved measurement_time to NVS: %lld seconds", time / 1000);
        }
    }

    nvs_close(nvs_handle);
}

static void load_measurement_time_from_nvs() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("nvs_data", NVS_READONLY, &nvs_handle);

    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Failed to open NVS for loading measurement_time, using default: %s", esp_err_to_name(err));
        return;
    }

    uint64_t saved_time = 0;
    err = nvs_get_u64(nvs_handle, "meas_time", &saved_time);

    if (err == ESP_OK && saved_time >= 10000) { // Ensure the saved time is reasonable
        measurement_time = saved_time;
        ESP_LOGI(MQTT_TAG, "Loaded measurement_time from NVS: %lld seconds", measurement_time / 1000);
    } else {
        ESP_LOGE(MQTT_TAG, "No saved measurement_time in NVS, using default: %s", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

static void periodic_timer_callback(void *arg) {
    // Notify the sensor task
    xTaskNotifyGive(sensor_task_handle);
}

void start_esp_timer() {
    load_measurement_time_from_nvs();  // Load measurement time from NVS

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

    ESP_LOGI(TIMER_TAG, "Start Periodic Timer successfully: %lld seconds!!!", measurement_time / 1000);
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
    
    if (strncmp(topic, subTopic, strlen(subTopic) - 1) == 0) {
        const char* requestID = topic + strlen(subTopic) - 1;
        ESP_LOGI(MQTT_TAG, "Request ID: %s", requestID);

        handle_mqtt_command(message.c_str(), requestID);
    } else {
        ESP_LOGE(MQTT_TAG, "Unknown topic: %s", topic);
        return;
    }
}

void mqttTask(void* parameter) {
    while (!WiFi.isConnected()) {
        ESP_LOGW(MQTT_TAG, "MQTT Task: Waiting for WiFi connection...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(mqttCallback);
    init_mqtt_token();
    ESP_LOGI(MQTT_TAG, "MQTT Access Token: %s", MQTT_ACCESS_TOKEN);

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
        ulTaskNotifyTake(pdTRUE, (TickType_t)portMAX_DELAY);                               // Wait for the notification from the timer callback
        xEventGroupClearBits(mqtt_event_group, MQTT_TASK_DONE_BIT);
        
        if (client.connected()) {
            // Read sensor data, uncomment the following lines to read sensor data
            // float tur_value = read_sensor_data(data_tur, sizeof(data_tur), "TUR");      // Read TUR sensor data (NTU)
            // float ph_value = read_sensor_data(data_ph, sizeof(data_ph), "pH");          // Read pH sensor data
            // float ec_value = read_sensor_data(data_ec, sizeof(data_ec), "EC");          // Read EC sensor data (μS/cm) 
            // float orp_value = read_sensor_data(data_orp, sizeof(data_orp), "ORP");      // Read ORP sensor data (mV)          

            // Random sensor data for testing
            float ph_value = getRandomVal(ph_min, ph_max, gen);
            float orp_value = getRandomVal(orp_min, orp_max, gen);
            float ec_value = getRandomVal(ec_min, ec_max, gen);
            float tur_value = getRandomVal(tur_min, tur_max, gen);

            float predicted_ph_value = 0.0;
            float predicted_orp_value = 0.0;
            float predicted_ec_value = 0.0;
            float predicted_tur_value = 0.0;

            std::vector<float> prediction = processModel(ph_value, orp_value, ec_value, tur_value);
            
            if (prediction.size() == 4) {
                predicted_ph_value = prediction[0];
                predicted_orp_value = prediction[1];
                predicted_ec_value = prediction[2];
                predicted_tur_value = prediction[3];
            }

            char payload[256];
            sprintf(payload, "{\"pH\": %.2f, \"predicted_pH\": %.2f, \"ORP\": %.2f, \"predicted_ORP\": %.2f, \"EC\": %.2f, \"predicted_EC\": %.2f, \"TUR\": %.2f, \"predicted_TUR\": %.2f, \"long\": %.6f, \"lat\": %.6f}", 
                    ph_value, predicted_ph_value,
                    orp_value, predicted_orp_value,
                    ec_value, predicted_ec_value,
                    tur_value, predicted_tur_value,
                    longitude, latitude);
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

void publishExecutedCronTask(void* parameter) {
    // Task that waits for notifications from execute_cronjob
    ESP_LOGI(MQTT_TAG, "Cron job publisher task started");
    
    while (true) {
        // Wait for notification from execute_cronjob
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        if (executed_cronjob && client.connected()) {
            // Create payload with executed cron job information including the cron expression and executed status
            char payload[128];
            snprintf(payload, sizeof(payload), 
                    "{\"executed_cronjob\": {\"id\": %u, \"cron\": \"%s\", \"executed\": true}, \"pumpStatus\": %d}",
                    id_executed_cronjob, executed_cron_expr, pumpStatus_executed_cronjob);
                    
            // Publish to telemetry topic
            if (client.publish(pubTopic, payload)) {
                ESP_LOGI(MQTT_TAG, "Published executed cron job: ID %u, Cron: %s; Pump Status: %s",
                        id_executed_cronjob, executed_cron_expr, pumpStatus_executed_cronjob ? "ON" : "OFF");
            } else {
                ESP_LOGE(MQTT_TAG, "Failed to publish executed cron job");
            }
            
            // Reset the flag
            executed_cronjob = false;
        } else if (!client.connected()) {
            ESP_LOGW(MQTT_TAG, "MQTT client not connected. Cannot publish executed cron job.");
        }
    }
}

void checkPumpStatusTask(void* parameter) {
    bool success = false;

    while (1) {
        if (client.connected()) {
            // int pumpStatus = readPumpStatus() ? 1 : 0;
            char payload[128];
            snprintf(payload, sizeof(payload), "{\"current_fw_version\": %s, \"monitoring_time\": %lld}", FIRMWARE_VERSION, measurement_time);
            client.publish(pubTopic, payload);
            // ESP_LOGI(MQTT_TAG, "Pump status: %s", pumpStatus ? "ON" : "OFF");
            ESP_LOGI(MQTT_TAG, "Current firmware version: %s", FIRMWARE_VERSION);
            success = true; 
        } else {
            ESP_LOGW(MQTT_TAG, "MQTT client not connected. Cannot publish pump status.");
        }
        
        if (success) vTaskDelete(NULL); // Delete the task after publishing
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void handle_pump_relay(char *cmd, ControlDef *def) {
    if (cmd == NULL) {
        ESP_LOGE(MQTT_TAG, "Invalid control pump command! Just: control_pump: ON/OFF");
        return;
    }
    
    bool success = false;
    bool pumpStatus = false;
    
    if (strcmp(cmd, "ON") == 0) {
        ESP_LOGI(MQTT_TAG, "Turning pump relay ON");
        turnPumpRelay(true);
        success = true;
        pumpStatus = true;
    } else if (strcmp(cmd, "OFF") == 0) {
        ESP_LOGI(MQTT_TAG, "Turning pump relay OFF");
        turnPumpRelay(false);
        success = true;
        pumpStatus = false;
    } else {
        ESP_LOGE(MQTT_TAG, "Invalid control pump command! Just: control_pump: ON/OFF");
        return;
    }
    
    // Publish pump status based on the command if control was successful
    if (success && client.connected()) {
        char payload[128];
        snprintf(payload, sizeof(payload), "{\"pumpStatus\": %d}", pumpStatus ? 1 : 0);
        
        if (client.publish(pubTopic, payload)) {
            ESP_LOGI(MQTT_TAG, "Published pump status: %s", pumpStatus ? "ON" : "OFF");
        } else {
            ESP_LOGE(MQTT_TAG, "Failed to publish pump status");
        }
    }
}

static void handle_change_wifi(char *cmd, ControlDef *def) {
    if (cmd == NULL) {
        ESP_LOGE(MQTT_TAG, "Invalid change Wi-Fi command! Use: change_wifi:SSID:PASSWORD");
        return;
    }

    char *ssid = strtok(cmd, ":");
    char *password = strtok(NULL, "");

    if (ssid == NULL || password == NULL) {
        ESP_LOGE(MQTT_TAG, "Invalid change Wi-Fi command! Use: change_wifi:SSID:PASSWORD");
        return;
    }

    ESP_LOGI(MQTT_TAG, "Changing Wi-Fi to SSID: %s, Password: %s", ssid, password);

    // Save new Wi-Fi credentials to NVS
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_stored", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_str(nvs_handle, "ssid", ssid);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Error saving SSID to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    err = nvs_set_str(nvs_handle, "password", password);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Error saving password to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Error committing changes to NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(MQTT_TAG, "Wi-Fi credentials saved to NVS successfully");
    }

    nvs_close(nvs_handle);

    // Disconnect and reconnect to the new Wi-Fi
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    ESP_LOGI(MQTT_TAG, "Connecting to new Wi-Fi...");
    for (int i = 0; i < 10; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            ESP_LOGI(MQTT_TAG, "Connected to new Wi-Fi: SSID=%s", ssid);
            return;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGE(MQTT_TAG, "Failed to connect to new Wi-Fi: SSID=%s", ssid);
}

static void handle_measurement_time(char *cmd, ControlDef *def) {
    uint64_t new_measurement_time = (uint64_t)atol(cmd);

    if (new_measurement_time < 10000) {
        ESP_LOGE(MQTT_TAG, "Invalid measurement time! Must be at least 10 seconds.");
        new_measurement_time = 10000; // Set to minimum value
    }

    stop_esp_timer();
    measurement_time = new_measurement_time;  // Update measurement time
    save_measurement_time_to_nvs(measurement_time);  // Save to NVS

    ESP_LOGI(MQTT_TAG, "Measurement time changed to: %lld seconds", measurement_time / 1000);
    start_esp_timer();

    if (client.connected()) {
        char payload[128];
        snprintf(payload, sizeof(payload), "{\"monitoring_time\": %lld}", measurement_time);
        
        if (client.publish(pubTopic, payload)) {
            ESP_LOGI(MQTT_TAG, "Published measurement time: %lld", measurement_time / 1000);
        } else {
            ESP_LOGE(MQTT_TAG, "Failed to publish measurement time!");
        }
    }
} 

// static void handle_factory_reset(char *cmd, ControlDef *def) {
//     ESP_LOGI(MQTT_TAG, "Factory reset command received");
    
//     // Notify the user before reset
//     if (client.connected()) {
//         const char *reset_payload = "{\"status\": \"Factory reset initiated\", \"message\": \"Device will reboot to factory partition\"}";
//         client.publish(pubTopic, reset_payload);
//         // Give time for the MQTT message to be sent before reset
//         delay(1000);
//     }
    
//     // Call the rollback function from OTA module
//     rollbackToFactory();
// }

typedef void (*command_handler_t)(char *cmd, ControlDef *def);
typedef struct
{
    const char *command;
    command_handler_t handler;
} command_t;

static const command_t command_table[] = {
    {"pump_relay", handle_pump_relay},       // Command to turn the pump relay on/off: control/pump_relay:ON/OFF
    {"change_wifi", handle_change_wifi},        // Command to change WiFi: control/change_wifi:SSID:PASSWORD
    {"measurement_time", handle_measurement_time},
    // {"factory_reset", handle_factory_reset},    // Command to trigger factory reset: control/factory_reset:confirm
};

void controlTask(void *parameter)
{
    ControlDef *def = (ControlDef *)parameter;
    BaseType_t ret;
    char command[200];
    while (1)
    {
        ret = xQueueReceive(control_queue, command, (TickType_t)500); // Wait for a command from the queue
        if (ret == pdTRUE)
        {
            ESP_LOGI(MQTT_TAG, "Received command data from queue: %s", command);       
            char *cmd_name = strtok(command, ":");
            char *cmd_value = strtok(NULL, "");
            
            uint8_t command_found_flag = 0;
            for (size_t i = 0; i < sizeof(command_table) / sizeof(command_table[0]); i++) {
                if (strcmp(cmd_name, command_table[i].command) == 0) {
                    command_table[i].handler(cmd_value, def);
                    command_found_flag = 1;
                    break;
                }
            }
            if (!command_found_flag) {
                ESP_LOGE(MQTT_TAG, "Unknown command: %s", cmd_name);
            }
        }
    }
}

// Handle OTA update commands
bool handle_ota_command(cJSON *params) {
    const char *ota_key = "ota/fw_title";
    cJSON *ota_item = cJSON_GetObjectItem(params, ota_key);
    
    if (!ota_item) {
        return false;
    }
    
    // Extract firmware title from the JSON value
    if (cJSON_IsString(ota_item)) {
        strlcpy(receivedFwTitle, ota_item->valuestring, sizeof(receivedFwTitle));
    } else {
        ESP_LOGE(MQTT_TAG, "Invalid firmware title format");
        return false;
    }

    cJSON *fw_version_item = cJSON_GetObjectItem(params, "fw_version");
    if (fw_version_item && cJSON_IsString(fw_version_item)) {
        strlcpy(receivedFwVersion, fw_version_item->valuestring, sizeof(receivedFwVersion));
        ESP_LOGI(MQTT_TAG, "Received Firmware title: %s, version: %s", receivedFwTitle, receivedFwVersion);
        otaUpdateTriggered = true;

        if (otaUpdateTask_handle != NULL) {
            xTaskNotifyGive(otaUpdateTask_handle);
        } else {
            ESP_LOGE(MQTT_TAG, "OTA update task handle is NULL");
        }
        return true;
    } else {
        ESP_LOGE(MQTT_TAG, "Invalid OTA command: missing or invalid fw_version");
        return false;
    }
}

// Handle schedule commands
bool handle_schedule_command(cJSON *params) {
    const char *schedule_actions[] = {"schedule/add", "schedule/edit", "schedule/delete"};
    const char *current_action = NULL;
    cJSON *schedule_item = NULL;

    // Find which schedule action is present in the command
    for (int i = 0; i < sizeof(schedule_actions) / sizeof(schedule_actions[0]); i++) {
        schedule_item = cJSON_GetObjectItem(params, schedule_actions[i]);
        if (schedule_item) {
            current_action = schedule_actions[i];
            break;
        }
    }

    if (!current_action || !schedule_item) {
        return false; // No schedule command found
    }

    CronJob cronjob = {0};
    char action[32] = {0};

    // Extract action type (add, edit, delete)
    sscanf(current_action, "schedule/%31s", action);
    strlcpy(cronjob.action, action, sizeof(cronjob.action));

    // Get target relay
    if (cJSON_IsString(schedule_item)) {
        strlcpy(cronjob.relay, schedule_item->valuestring, sizeof(cronjob.relay));
    } else {
        ESP_LOGE(MQTT_TAG, "Invalid target device for %s", current_action);
        return false;
    }

    // Get job ID
    cJSON *id_item = cJSON_GetObjectItem(params, "id");
    if (id_item && cJSON_IsNumber(id_item)) {
        cronjob.id = id_item->valueint;
    } else {
        ESP_LOGE(MQTT_TAG, "Invalid ID for %s", current_action);
        return false;
    }

    // For add/edit actions, get cron expression and state
    if (strcmp(action, "add") == 0 || strcmp(action, "edit") == 0) {
        cJSON *cron_item = cJSON_GetObjectItem(params, "cron");
        if (cron_item && cJSON_IsString(cron_item)) {
            strlcpy(cronjob.cron, cron_item->valuestring, sizeof(cronjob.cron));
        } else {
            ESP_LOGE(MQTT_TAG, "Invalid cron expression for %s", current_action);
            return false;
        }

        cJSON *state_item = cJSON_GetObjectItem(params, "state");
        cronjob.state = state_item ? cJSON_IsTrue(state_item) : false;

        ESP_LOGI(MQTT_TAG, "Action %s: relay=%s, id=%u, cron=%s, state=%s", 
                 current_action, cronjob.relay, cronjob.id, cronjob.cron, 
                 cronjob.state ? "On" : "Off");
        save_cronjob_to_nvs(&cronjob);
    } 
    // For delete action
    else if (strcmp(action, "delete") == 0) {
        ESP_LOGI(MQTT_TAG, "Deleting cron job: relay=%s, id=%d", cronjob.relay, cronjob.id);
        delete_cronjob_from_nvs(cronjob.id);
    }
    
    return true;
}

// Handle other command types
void handle_other_commands(cJSON *params) {
    cJSON *item = params->child;
    while (item) {
        if (cJSON_IsString(item)) {
            char prefix[64] = {0}, command[64] = {0};

            if (sscanf(item->string, "%63[^/]/%63s", prefix, command) == 2) {
                QueueHandle_t target_queue = NULL;
    
                for (size_t i = 0; i < sizeof(queue_table) / sizeof(queue_table[0]); i++) {
                    if (strcmp(prefix, queue_table[i].prefix) == 0) {
                        target_queue = *queue_table[i].queue;
                        break;
                    }
                }
    
                if (target_queue != NULL) {
                    char msg[128];
                    snprintf(msg, sizeof(msg), "%s:%s", command, item->valuestring);
                    xQueueSend(target_queue, msg, (TickType_t)100);
                    ESP_LOGI(MQTT_TAG, "Pushed command '%s' to queue %s", msg, prefix);
                } else {
                    ESP_LOGE(MQTT_TAG, "No queue mapped for prefix: %s", prefix);
                }
            }
        }

        item = item->next;
    }
}

void handle_mqtt_command(const char *json_str, const char *requestID) {
    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        ESP_LOGE(MQTT_TAG, "Failed to parse JSON");
        return;
    }

    cJSON *params = cJSON_GetObjectItem(root, "params");
    if (!params || !cJSON_IsObject(params)) {
        ESP_LOGE(MQTT_TAG, "Invalid params in JSON");
        cJSON_Delete(root);
        return;
    }

    bool command_handled = false;

    // Handle OTA updates
    if (handle_ota_command(params)) {
        command_handled = true;
    }
    // Handle schedule commands
    else if (handle_schedule_command(params)) {
        command_handled = true;
    }
    // Handle other commands
    else {
        handle_other_commands(params);
        command_handled = true;
    }

    cJSON_Delete(root);
    
    // Send response
    if (command_handled) {
        char response_topic[128];
        snprintf(response_topic, sizeof(response_topic), "v1/devices/me/rpc/response/%s", requestID);
        
        const char *response_payload = "{\"status\": \"success\"}";
        client.publish(response_topic, response_payload);
        ESP_LOGI(MQTT_TAG, "Published response to topic: %s", response_topic);
    }
}