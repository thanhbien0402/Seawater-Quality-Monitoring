// #ifndef WIFI_PROVISION_H
// #define WIFI_PROVISION_H

// #include <WiFi.h>
// #include <esp_wifi.h>
// #include <WebServer.h>  // Web server for provisioning

// // Function prototypes
// void wifi_init_sta(const char* ssid, const char* password);
// void wifi_init_softap(const char* ap_ssid, const char* ap_password);
// void start_wifi_provisioning();
// bool is_wifi_connected();

// #endif // WIFI_PROVISION_H

#ifndef WIFI_PROVISION_H
#define WIFI_PROVISION_H

#include "WiFi.h"

void wifi_init();
void connectWiFi(void *pvParameters);

// #define MAX_WIFI_NETWORKS 5 // Store up to 5 networks
// #define MAX_SSID_LEN 32 // Max SSID length
// #define MAX_PASS_LEN 64 // Max password length

// extern const char *pop; // Proof of possession - otherwise called a PIN - string provided by the device, entered by the user in the phone app
// extern const char *service_name; // Name of your device (the Espressif apps expects by default device name starting with "Prov_")
// extern const char *service_key; // Password used for SofAP method (NULL = no password needed)
// extern bool reset_provisioned; // When true the library will automatically delete previously provisioned data.
// extern bool wifi_connected;

// // Function prototypes
// void saveWiFiCredentials(const char *ssid, const char *password);
// void loadWiFiCredentials(char ssid_list[MAX_WIFI_NETWORKS][MAX_SSID_LEN], char pass_list[MAX_WIFI_NETWORKS][MAX_PASS_LEN]);
// void wifiReconnectTask(void *pvParameters);
// void SysProvEvent(arduino_event_t *sys_event);
// void wifi1_init(void);

#endif // WIFI_PROVISION_H
