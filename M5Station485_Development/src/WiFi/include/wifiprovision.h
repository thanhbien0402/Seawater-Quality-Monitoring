#ifndef WIFI_PROVISION_H
#define WIFI_PROVISION_H

#include "WiFi.h"

typedef struct {
    char ssid[32];
    char password[64];
} my_wifi_config_t;

void wifi_init();
void connectWiFi(void *pvParameters);
void save_connected_wifi_to_nvs(my_wifi_config_t new_config);
void scanWiFi(void *pvParameters);
void start_ap_mode();

#endif // WIFI_PROVISION_H