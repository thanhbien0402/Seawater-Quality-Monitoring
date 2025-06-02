#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp32-hal-log.h"
#include "esp_attr.h"
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

// #include "wifi_provisioning/manager.h"
// #include "wifi_provisioning/scheme_ble.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "qrcode.h"

// WiFi Credentials
#define WIFI_SSID "RD-SEAI_2.4G"
#define WIFI_PASSWORD ""

// MQTT Broker
#define MQTT_BROKER "app.coreiot.io"
#define MQTT_PORT 1883
#define MQTT_PASSWORD ""
#define FIRMWARE_VERSION "2.2.4"

extern char MQTT_ACCESS_TOKEN[13];  // 12 hex chars + 1 for null terminator

void init_mqtt_token();