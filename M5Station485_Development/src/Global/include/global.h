#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp32-hal-log.h"

#include "esp_attr.h"
#include "esp_netif.h"

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"
#include "esp_sntp.h"
#include "esp_task_wdt.h"
#include "esp_mac.h"
#include "esp_timer.h"
#include "esp_tls.h"
#include "esp_pm.h"

#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "qrcode.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <wifi_provisioning/scheme_softap.h>
#include "qrcode.h"

// WiFi Credentials
#define WIFI_SSID "RD-SEAI_2.4G"
#define WIFI_PASSWORD ""

// MQTT Broker
#define MQTT_BROKER "demo.thingsboard.io"
#define MQTT_PORT 1883
#define MQTT_ACCESS_TOKEN "ypdqonvmc720mgvleb8j"
#define MQTT_PASSWORD ""

// #define MAX_WIFI_CONFIGS 5
// #define MAX_RETRIES   10
// extern EventGroupHandle_t g_manager_event;

// typedef struct {
//     char ssid[32];
//     char password[64];
// } WifiConfig;

// typedef enum
// {
//     eWIFI_CONFIG_NOT_INITIALIZED = 0,
//     eWIFI_CONFIG_INITIALIZED = 1,
//     eWIFI_CONFIG_ERROR = 2,
// }WIFI_CONFIG_STATE;

// typedef enum
// {
//     WIFI_CONFIG_NOT_INITIALIZED,
//     WIFI_CONFIG_INITIALIZED,
//     WIFI_CONFIG_ERROR,
//     WIFI_CONFIG_NO_WIFI_STORED,
//     WIFI_CONNECTED,
//     WIFI_DISCONNECTED
// } wifi_state_t;

// void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
// void wifi_stack_init(void);
// void wifi_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport);
// void get_device_service_name(char *service_name, size_t max);
// WIFI_CONFIG_STATE read_wifi_config(void);
// int list_wifi_stored(void);
// wifi_state_t connect_wifi(char *ssid, char *password, bool change);
// void add_newconfig(const char* ssid, const char* password, bool change);
// wifi_state_t checkwifi_state(void);
// void set_wifi_max_tx_power(void);
// void wifi_init();
// void wifi_task(void *arg);
// wifi_state_t scan_and_connect_wifi(bool connect_type);
// void sync_timestamp(EventGroupHandle_t event_manager);
// void set_timestamp(time_t unix_timestamp);
// void start_provisioning(bool no_wifi_stored);

// typedef struct {
//     EventGroupHandle_t EventManager;
//     QueueHandle_t      PublishQueue;
// } PublishDef;

// typedef struct
// {
//     char Cmd[200];
//     QueueHandle_t QueueTaskHandle;
// } TaskHandler;

// typedef struct
// {
//     EventGroupHandle_t EventManager;
//     TaskHandler        *List;
//     uint8_t            NumberOfLists;
// } subscribehDef;

// typedef enum
// {
//     eOK = 0,
//     eFAIL=1
// } MQTT_STATE;

// extern EventGroupHandle_t g_manager_event;

// void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
// // void parse_and_add_to_json(const char *input_string, cJSON *json_obj);
// MQTT_STATE data_to_mqtt(char *data, char *topic, int delay_time_ms, int qos);
// void mqtt_init(char *broker_uri, char *username, char *client_id, bool storage_use);
// void subscribe_to_topic(char *topic,int qos);
// void get_data_subscribe_topic( uint16_t *data);
// void mqtt_subscriber(esp_mqtt_event_handle_t event);
// void publish_task(void *arg);
// void get_data_subscribe_task(void *arg);
// void add_subscribe_task(const char *command_name,QueueHandle_t recv_queue);
// void subscribe_task(void *arg);
// // MQTT_STATE data_sensor_to_mqtt(char *data, char *topic, int delay_time_ms, int qos, DataStore store);
// // MQTT_STATE publish_action(char *data, char *topic, int qos, DataStore pub_store);
// void mqtt_disconnect(void);
// void mqtt_reconnect(void);
// void mqtt_set_id(char *broker_url, char *id);
// void mqtt_storage_connect_information(esp_mqtt_client_config_t *config);
// void publish_frequent_data(void *arg);