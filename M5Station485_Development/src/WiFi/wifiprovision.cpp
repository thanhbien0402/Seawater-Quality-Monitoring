#include "./include/wifiprovision.h"
#include "./Global/include/global.h"

static const char *WIFI_TAG = "WiFi";

void wifi_init() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    ESP_LOGW(WIFI_TAG, "Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGW(WIFI_TAG, ".");
    }
    ESP_LOGI(WIFI_TAG, "Wi-Fi connected successfully");
    ESP_LOGI(WIFI_TAG, "IP Address: %s", WiFi.localIP().toString().c_str());
    ESP_LOGI(WIFI_TAG, "Signal Strength: %d dBm", WiFi.RSSI());
}

void connectWiFi(void *pvParameters) {
    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            ESP_LOGW(WIFI_TAG, "Wi-Fi disconnected. Attempting to reconnect...");
            wifi_init();
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);  // Delay before checking Wi-Fi status again
    }
}

// // #define USE_SOFT_AP // Uncomment if you want to enforce using Soft AP method instead of BLE

// const char * pop = "1234"; // Proof of possession - otherwise called a PIN - string provided by the device, entered by user in the phone app
// const char * service_name = "PROV_123"; // Name of your device (the Espressif apps expects by default device name starting with "Prov_")
// const char * service_key = NULL; // Password used for SofAP method (NULL = no password needed)
// bool reset_provisioned = true; // When true the library will automatically delete previously provisioned data.
// bool wifi_connected = 0;

// nvs_handle_t nvs_handler;

// void loadWiFiCredentials(char ssid_list[MAX_WIFI_NETWORKS][MAX_SSID_LEN], char pass_list[MAX_WIFI_NETWORKS][MAX_PASS_LEN])
// {
//     size_t len;
//     for (int8_t i = 0; i < MAX_WIFI_NETWORKS; i++)
//     {
//         char key_ssid[10], key_pass[10];
//         snprintf(key_ssid, sizeof(key_ssid), "ssid%d", i);
//         snprintf(key_pass, sizeof(key_pass), "pass%d", i);

//         len = MAX_SSID_LEN;
//         nvs_get_str(nvs_handler, key_ssid, ssid_list[i], &len);

//         len = MAX_PASS_LEN;
//         nvs_get_str(nvs_handler, key_pass, pass_list[i], &len);
//     }
// }

// void saveWiFiCredentials(const char *ssid, const char *password)
// {
//     size_t ssid_len = strlen(ssid);
//     size_t pass_len = strlen(password);

//     // Temporarily store the credentials to shift them down
//     char prev_ssid[MAX_SSID_LEN], prev_pass[MAX_PASS_LEN];
//     size_t prev_ssid_len = MAX_SSID_LEN, prev_pass_len = MAX_PASS_LEN;

//     for (int8_t i = MAX_WIFI_NETWORKS - 1; i > 0; i--)
//     {
//         char key_ssid[10], key_pass[10];
//         snprintf(key_ssid, sizeof(key_ssid), "ssid%d", i - 1);
//         snprintf(key_pass, sizeof(key_pass), "pass%d", i - 1);

//         // Load existing credentials for the previous slot
//         nvs_get_str(nvs_handler, key_ssid, prev_ssid, &prev_ssid_len);
//         nvs_get_str(nvs_handler, key_pass, prev_pass, &prev_pass_len);

//         // Shift credentials down to make space for new entry
//         snprintf(key_ssid, sizeof(key_ssid), "ssid%d", i);
//         snprintf(key_pass, sizeof(key_pass), "pass%d", i);

//         nvs_set_str(nvs_handler, key_ssid, prev_ssid);
//         nvs_set_str(nvs_handler, key_pass, prev_pass);
//     }

//     // Store the latest SSID and password at index 0
//     nvs_set_str(nvs_handler, "ssid0", ssid);
//     nvs_set_str(nvs_handler, "pass0", password);

//     esp_err_t err = nvs_commit(nvs_handler);
//     if (err == ESP_OK)
//     {
//         Serial.println("Wi-Fi credentials saved successfully.");
//     }
//     else
//     {
//         Serial.printf("Failed to commit NVS changes: %s\n", esp_err_to_name(err));
//     }
// }

// void wifiReconnectTask(void *pvParameters)
// {
//     char ssid_list[MAX_WIFI_NETWORKS][MAX_SSID_LEN];
//     char pass_list[MAX_WIFI_NETWORKS][MAX_PASS_LEN];

//     while (1)
//     {
//         if (!wifi_connected)
//         {
//             Serial.println("Reconnecting to Wi-Fi...");
//             loadWiFiCredentials(ssid_list, pass_list);

//             for (int8_t i = 0; i < MAX_WIFI_NETWORKS; i++)
//             {
//                 if (strlen(ssid_list[i]) > 0)
//                 {
//                     Serial.printf("Trying to connect to SSID: %s\n", ssid_list[i]);
//                     WiFi.begin(ssid_list[i], pass_list[i]);

//                     if (WiFi.waitForConnectResult() == WL_CONNECTED)
//                     {
//                         Serial.printf("Connected to %s\n", ssid_list[i]);
//                         wifi_connected = true;
//                         break;
//                     }
//                 }
//             }

//             if (!wifi_connected)
//             {
//                 Serial.println("No saved Wi-Fi credentials found. Starting provisioning...");
//                 WiFi.onEvent(SysProvEvent);
//                 uint8_t uuid[16] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02};
//                 WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name, service_key, uuid, reset_provisioned);
//                 // log_d("ble qr");
//                 // WiFiProv.printQR(service_name, pop, "ble");
//             }
//         }
//         vTaskDelay(30000 / portTICK_PERIOD_MS);
//     }
// }

// void SysProvEvent(arduino_event_t *sys_event)
// {
//     switch (sys_event->event_id) 
//     {
//         case ARDUINO_EVENT_WIFI_STA_GOT_IP:
//             Serial.print("\nConnected IP address : ");
//             Serial.println(IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr));
//             wifi_connected = 1;
//             // Set Adafruit IO's root CA
//             // client.setCACert(adafruitio_root_ca);

//             // // Subscribe to the 'test' feed
//             // mqtt.subscribe(&test_subscribe);
//             // Save the SSID and RSSI of the connected network
//             // saveWiFiCredentials(WiFi.SSID().c_str(), WiFi.psk().c_str());
//             break;
//         case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
//             Serial.println("\nDisconnected. Will try to reconnect... ");
//             wifi_connected = 0;
//             break;
//         case ARDUINO_EVENT_PROV_START:
//             Serial.println("\nProvisioning started\nGive Credentials of your access point using smartphone app");
//             break;
//         case ARDUINO_EVENT_PROV_CRED_RECV: 
//             Serial.println("\nReceived Wi-Fi credentials");
//             Serial.print("\tSSID : ");
//             Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
//             Serial.print("\tPassword : ");
//             Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
//             break;
//         case ARDUINO_EVENT_PROV_CRED_FAIL: 
//             Serial.println("\nProvisioning failed!\nPlease reset to factory and retry provisioning\n");
//             if (sys_event->event_info.prov_fail_reason == WIFI_PROV_STA_AUTH_ERROR)
//                 Serial.println("\nWi-Fi AP password incorrect");
//             else
//                 Serial.println("\nWi-Fi AP not found....Add API \" nvs_flash_erase() \" before beginProvision()");
//             break;
//         case ARDUINO_EVENT_PROV_CRED_SUCCESS:
//             Serial.println("\nProvisioning Successful");
//             break;
//         case ARDUINO_EVENT_PROV_END:
//             Serial.println("\nProvisioning Ends");
//             break;
//         default:
//             break;
//     }
// }

// void wifi1_init(void)
// {
//     // // Initialize NVS
//     // esp_err_t err = nvs_flash_init();
//     // if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     // {
//     //     ESP_ERROR_CHECK(nvs_flash_erase());
//     //     err = nvs_flash_init();
//     // }
//     // ESP_ERROR_CHECK(err);

//     // // Open NVS handle
//     // err = nvs_open("storage", NVS_READWRITE, &nvs_handler);
//     // if (err != ESP_OK)
//     // {
//     //     Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     // }
//     // else
//     // {
//     //     Serial.println("NVS handle opened successfully");
//     // }
//     // esp_wifi_start();
//         // Initialize the default event loop if not already initialized
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // Initialize the Wi-Fi stack
//     // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     // ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // wifi_prov_mgr_config_t config = {
//     //     .scheme = wifi_prov_scheme_ble,
//     //     .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM,
//     // };
//     // ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
//     Serial.println("Starting BLE Provisioning");
//     WiFi.onEvent(SysProvEvent);
//     Serial.println("Begin Provisioning using BLE");
//     uint8_t uuid[16] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02};
//     // Sample uuid that user can pass during provisioning using BLE
//     WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name, service_key, uuid, reset_provisioned);
//     log_d("ble qr");
//     WiFiProv.printQR(service_name, pop, "ble");

//     vTaskDelay(1000 / portTICK_PERIOD_MS);
// }