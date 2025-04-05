// #include "./include/global.h"
// #include "wifi_provisioning/wifi_config.h"

// const char *TAG = "MQTT";

// bool device_state_update = false;
// esp_mqtt_client_handle_t g_mqtt_client;


// void subscribe_to_topic(const char *topic, int qos)
// {
//   // xEventGroupWaitBits(g_mqtt_event_group, g_constant_ConnectBit, false, true, portMAX_DELAY);
//   if (esp_mqtt_client_subscribe(g_mqtt_client, topic, qos) == -1)
//   {
//     ESP_LOGE(TAG, "Failed to subscribe to topic");
//   }
// }

// void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
// {
    
//     esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
//     esp_mqtt_client_handle_t client = event->client;
//     int msg_id;

//     switch ((esp_mqtt_event_id_t)event->event_id)
//     {
//     case MQTT_EVENT_CONNECTED:
//         ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
//         // xEventGroupSetBits(g_mqtt_event_group, g_constant_ConnectBit);
//         // xEventGroupClearBits(g_manager_event, BROKER_DISCONNECTED_EVENT);
//         // xEventGroupSetBits(g_manager_event, BROKER_CONNECTED_EVENT);

//         msg_id = esp_mqtt_client_publish(client, "v1/devices/me/attributes", "{\"device_state\":\"connected\"}", 0, 1, 0);
//         subscribe_to_topic("v1/devices/me/rpc/request/+",2);
//         device_state_update = true;

//         break;

//         case MQTT_EVENT_DISCONNECTED:
//             // xEventGroupClearBits(g_mqtt_event_group,g_constant_ConnectBit);
//             // xEventGroupClearBits(g_manager_event,BROKER_CONNECTED_EVENT);
//             // xEventGroupSetBits(g_manager_event,BROKER_DISCONNECTED_EVENT);
//             // if (checkwifi_state() == WIFI_CONNECTED)
//             // {
//             //     ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
//             //     // Print the disconnection reason
//             //     const char* disconnect_reason;
//             //     switch (event->error_handle->error_type)
//             //     {
//             //         case MQTT_ERROR_TYPE_NONE:
//             //             disconnect_reason = "No error";
//             //             break;
//             //         case MQTT_ERROR_TYPE_TCP_TRANSPORT:
//             //             disconnect_reason = "TCP Transport layer error";
//             //             esp_err_t err = event->error_handle->esp_tls_last_esp_err;
//             //             if (err != 0)
//             //             {
//             //                 ESP_LOGE(TAG, "Last esp error code: 0x%x, %s", err, esp_err_to_name(err));
//             //             }
//             //             ESP_LOGE(TAG,"Error tls: %d\n", event->error_handle->esp_tls_stack_err);
//             //             ESP_LOGE(TAG,"Error sock: %d\n", event->error_handle->esp_transport_sock_errno);
//             //             break;
//             //         case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
//             //             disconnect_reason = "MQTT Connection refused error";
//             //             switch (event->error_handle->connect_return_code)
//             //             {
//             //                 case MQTT_CONNECTION_ACCEPTED:
//             //                     disconnect_reason = "Connection accepted";
//             //                     break;
//             //                 case MQTT_CONNECTION_REFUSE_PROTOCOL:
//             //                     disconnect_reason = "Wrong protocol version";
//             //                     break;
//             //                 case MQTT_CONNECTION_REFUSE_ID_REJECTED:
//             //                     disconnect_reason = "Client ID rejected";
//             //                     break;
//             //                 case MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE:
//             //                     disconnect_reason = "Server unavailable";
//             //                     break;
//             //                 case MQTT_CONNECTION_REFUSE_BAD_USERNAME:
//             //                     disconnect_reason = "Bad username or password";
//             //                     break;
//             //                 case MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED:
//             //                     disconnect_reason = "Client not authorized";
//             //                     break;
//             //                 default:
//             //                     disconnect_reason = "Unknown reason";
//             //                     break;
//             //             }
//             //             break;
//             //         default:
//             //             disconnect_reason = "Unknown error type";
//             //             break;
//             //     }
//             //     ESP_LOGI(TAG, "Disconnect reason: %s", disconnect_reason);
//             // }

//             ESP_LOGE(TAG, "MQTT_EVENT_DISCONNECTED");

//             break;

//         case MQTT_EVENT_SUBSCRIBED:
//             ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
//             msg_id = esp_mqtt_client_publish(client, "v1/devices/me/attributes", "{\"device_state\":\"subscribed\"}", 0, 0, 0);
//             break;

//         case MQTT_EVENT_UNSUBSCRIBED:
//             ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
//             break;

//         case MQTT_EVENT_PUBLISHED:
//             // if(g_message_id_of_sensordata>0 && event->msg_id ==g_message_id_of_sensordata)
//             // {
//             //     xEventGroupSetBits(g_mqtt_event_group,g_constant_PublishedBit);
//             //     g_message_id_of_sensordata=0;
//             //     ESP_LOGW(TAG,"Publish successfully key:%"PRIu16"\n",nvs_get_tail()-1);
//             // }
//             // else
//             // {
//             //     ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
//             // }
//             ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
//             break;

//         case MQTT_EVENT_DATA:
//             ESP_LOGI(TAG, "MQTT_RECEIVED DATA");
//             printf("DATA=%.*s\r\n", event->data_len, event->data);
//             // ack_sending(event->topic);
//             // mqtt_subscriber(event);
//             break;

//         case MQTT_EVENT_ERROR:
//             // if(checkwifi_state() == WIFI_CONNECTED)
//             // {
//             //     ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
//             //     esp_mqtt_connect_return_code_t error_code = event->error_handle->connect_return_code;
//             //     switch(error_code)
//             //     {
//             //         case MQTT_CONNECTION_ACCEPTED:
//             //             ESP_LOGE(TAG, "Connection accepted");
//             //             break;
//             //         case MQTT_CONNECTION_REFUSE_PROTOCOL:
//             //             ESP_LOGE(TAG, "Connection refused, unacceptable protocol version");
//             //             break;
//             //         case MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE:
//             //             ESP_LOGE(TAG, "Connection refused, server unavailable");
//             //             break;
//             //         case MQTT_CONNECTION_REFUSE_BAD_USERNAME:
//             //             ESP_LOGE(TAG, "Connection refused, bad username or password");
//             //             break;
//             //         case MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED:
//             //             ESP_LOGE(TAG, "Connection refused, not authorized");
//             //             break;
//             //         default:
//             //             ESP_LOGE(TAG, "Connection refused, unknown reason");
//             //             break;
//             //     }
//             //     esp_mqtt_error_type_t error_type = event->error_handle->error_type;
//             //     switch(error_type)
//             //     {
//             //         case MQTT_ERROR_TYPE_NONE:
//             //             ESP_LOGE(TAG, "No error");
//             //             break;
//             //         case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
//             //             ESP_LOGE(TAG, "Connection refused");
//             //             break;
//             //         case MQTT_ERROR_TYPE_TCP_TRANSPORT:
//             //             ESP_LOGE(TAG, "TCP transport error");
//             //             break;
//             //         case MQTT_ERROR_TYPE_SUBSCRIBE_FAILED:
//             //             ESP_LOGE(TAG, "Subscribe failed");
//             //             break;
//             //         default:
//             //             ESP_LOGE(TAG, "Unknown error type");
//             //             break;
//             //     }
//             //     esp_err_t err = event->error_handle->esp_tls_last_esp_err;
//             //     if (err != 0)
//             //     {
//             //         ESP_LOGE(TAG, "Last esp error code: 0x%x, %s", err, esp_err_to_name(err));
//             //     }
//             //     ESP_LOGE(TAG,"Error tls: %d\n", event->error_handle->esp_tls_stack_err);
//             //     ESP_LOGE(TAG,"Error sock: %d\n", event->error_handle->esp_transport_sock_errno);
//             // }

//             ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
//             break;

//         default:
//             ESP_LOGI(TAG, "Other event id:%d", event->event_id);
//             break;
//     }

// }

// void mqtt_init(void)
// {
//     esp_mqtt_client_config_t mqtt_cfg = {
//         .uri = "demo.thingsboard.io",
//         .username = "demo1",
//         .keepalive = 30,
//     };

//     esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
//     // esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
//     esp_mqtt_client_start(client);
//     ESP_LOGI(TAG, "MQTT client started");
// }

// // #define QRCODE_BASE_URL "https://espressif.github.io/esp-jumpstart/qrcode.html"
// // #define MANUAL_WIFI_CONNECT true
// // #define AUTOMATIC_WIFI_CONNECT false

// // static const char *WIFI_TAG = "Wifi";
// // const char *TIMESTAMP_TAG = "Timestamp";

// // static wifi_state_t g_state_wifi = WIFI_DISCONNECTED;
// // EventGroupHandle_t g_wifi_event_group;
// // const int g_constant_ConnectedBit = BIT0;
// // const int g_connected_by_scan = BIT7;
// // const int g_connected_by_provision = BIT6;
// // const int g_connected = BIT6 | BIT7;
// // const int g_initialized = BIT5;
// // bool g_wifi_changed = false;
// // WifiConfig wifiConfigs[MAX_WIFI_CONFIGS];
// // bool wifi_prov_error = false;

// // // void set_timestamp(time_t unix_timestamp)
// // // {
// // //     struct timeval tv = {.tv_sec = unix_timestamp};
// // //     settimeofday(&tv, NULL);
// // //     ESP_LOGI(TIMESTAMP_TAG, "Set timestamp to %lld", unix_timestamp);
// // // }

// // // static void time_sync_notification_cb(struct timeval *tv)
// // // {
// // //     ESP_LOGI(TIMESTAMP_TAG, "Notification of a time synchronization event");
// // // }

// // // void sync_timestamp(EventGroupHandle_t event_manager)
// // // {
// // //     time_t now = 0;
// // //     struct tm timeinfo = {0};
// // //     time(&now);
// // //     localtime_r(&now, &timeinfo);

// // //     // Is time set? If not, tm_year will be (1970 - 1900).
// // //     if (timeinfo.tm_year < (2016 - 1900))
// // //     {
// // //         ESP_LOGW(TIMESTAMP_TAG, "Time is not set yet. Initializing and starting SNTP.");
// // //         esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
// // //         config.sync_cb = time_sync_notification_cb; // Note: This is only needed if we want
// // //         esp_netif_sntp_init(&config);

// // //         // wait for time to be set
// // //         int retry = 0;
// // //         const int retry_count = 10;
// // //         while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count)
// // //         {
// // //             ESP_LOGW(TIMESTAMP_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
// // //         }

// // //         // update 'now' variable with current time
// // //         time(&now);
// // //         localtime_r(&now, &timeinfo);
// // //         esp_netif_sntp_deinit();
// // //     }

// // //     char strftime_buf[64];
// // //     // Set timezone to Vietnam Standard Time
// // //     setenv("TZ", "ICT-7", 1);
// // //     tzset();
// // //     localtime_r(&now, &timeinfo);
// // //     strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
// // //     ESP_LOGW(TIMESTAMP_TAG, "The current date/time in Vietnam is: %s", strftime_buf);

// // //     // notify sync timestamp successfully
// // //     if (timeinfo.tm_year < (2016 - 1900))
// // //     {
// // //         xEventGroupSetBits(event_manager, SYNC_TIME_SERVER_FAILED_EVENT);
// // //         ESP_LOGE(TIMESTAMP_TAG, "SNTP initialization failed. Please check your internet connection or try again later.");
// // //     }
// // //     else
// // //     {
// // //         xEventGroupSetBits(event_manager, SYNC_TIME_SERVER_SUCESS_EVENT);
// // //         ESP_LOGI(TIMESTAMP_TAG, "System time set successfully");
// // //     }
// // //     // end sync timestamp successfully
// // // }

// // void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
// // {
// //     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
// //     {
// //         ESP_LOGW(WIFI_TAG, "Wi-Fi started");
// //         esp_wifi_connect(); // Attempt to connect to the AP
// //     }
// //     else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Disconnected from AP");
// //         xEventGroupClearBits(g_wifi_event_group, g_constant_ConnectedBit);
// //         // xEventGroupClearBits(g_manager_event, WIFI_CONNECTED_EVENT);
// //         // xEventGroupSetBits(g_manager_event, WIFI_DISCONNECTD_EVENT);
// //         g_state_wifi = WIFI_DISCONNECTED;
// //     }
// //     else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
// //     {
// //         g_state_wifi = WIFI_CONNECTED;
// //         ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
// //         ESP_LOGI(WIFI_TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
// //         ESP_LOGI(WIFI_TAG, "Connected to AP!");
// //         // if (UDP_ENABLE)
// //         // {
// //         //     printf("%d\n", id_to_port(g_envisor_id));
// //         //     udp_logging_init(CONFIG_LOG_UDP_SERVER_IP, id_to_port(g_envisor_id), udp_logging_vprintf);
// //         // }
// //         xEventGroupSetBits(g_wifi_event_group, g_constant_ConnectedBit);
// //         // xEventGroupClearBits(g_manager_event, WIFI_DISCONNECTD_EVENT);
// //         // xEventGroupSetBits(g_manager_event, WIFI_CONNECTED_EVENT);
// //     }
// // }
// // void prov_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
// // {
// //     if (event_base == WIFI_PROV_EVENT)
// //     {
// //         switch (event_id)
// //         {
// //         case WIFI_PROV_START:
// //             // xEventGroupSetBits(g_manager_event, PROVISIONING_START_EVENT);
// //             ESP_LOGI(WIFI_TAG, "Provisioning started");
// //             break;
// //         case WIFI_PROV_CRED_RECV:
// //         {
// //             wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
// //             ESP_LOGI(WIFI_TAG, "Received Wi-Fi credentials"
// //                                "\n\tSSID     : %s\n\tPassword : %s",
// //                      (const char *)wifi_sta_cfg->ssid,
// //                      (const char *)wifi_sta_cfg->password);
// //             add_newconfig((const char *)wifi_sta_cfg->ssid, (const char *)wifi_sta_cfg->password, false);
// //             break;
// //         }
// //         case WIFI_PROV_CRED_FAIL:
// //         {
// //             wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
// //             ESP_LOGE(WIFI_TAG, "Provisioning failed!\n\tReason : %s"
// //                                "\n\tPlease reset to factory and retry provisioning",
// //                      (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
// //             ESP_LOGE(WIFI_TAG, "Provisioning failed, rebooting...");
// //             esp_wifi_restore();
// //             vTaskDelay(5000 / portTICK_PERIOD_MS);
// //             esp_restart();
// //             break;
// //         }
// //         case WIFI_PROV_CRED_SUCCESS:
// //             ESP_LOGI(WIFI_TAG, "Provisioning successful");
// //             break;
// //         case WIFI_PROV_END:
// //             /* De-initialize manager once provisioning is finished */
// //             ESP_LOGI(WIFI_TAG, "Provisioning ended");
// //             // xEventGroupSetBits(g_manager_event, PROVISIONING_FINISH_EVENT);
// //             xEventGroupSetBits(g_wifi_event_group, g_connected_by_provision);
// //             break;
// //         default:
// //             break;
// //         }
// //     }
// //     // else if (event_base == PROTOCOMM_TRANSPORT_BLE_EVENT)
// //     // {
// //     //     switch (event_id)
// //     //     {
// //     //     case PROTOCOMM_TRANSPORT_BLE_CONNECTED:
// //     //         ESP_LOGI(WIFI_TAG, "BLE transport: Connected!");
// //     //         break;
// //     //     case PROTOCOMM_TRANSPORT_BLE_DISCONNECTED:
// //     //         ESP_LOGI(WIFI_TAG, "BLE transport: Disconnected!");
// //     //         break;
// //     //     default:
// //     //         break;
// //     //     }
// //     // }
// //     // else if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT)
// //     // {
// //     //     switch (event_id)
// //     //     {
// //     //     case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
// //     //         ESP_LOGI(WIFI_TAG, "Secured session established!");
// //     //         break;
// //     //     case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
// //     //         ESP_LOGE(WIFI_TAG, "Received invalid security parameters for establishing secure session!");
// //     //         break;
// //     //     case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
// //     //         ESP_LOGE(WIFI_TAG, "Received incorrect username and/or PoP for establishing secure session!");
// //     //         break;
// //     //     default:
// //     //         break;
// //     //     }
// //     // }
// // }
// // void wifi_stack_init()
// // {
// //     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
// //     ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // Initialize the Wi-Fi stack

// //     esp_event_handler_instance_t instance_any_id;
// //     esp_event_handler_instance_t instance_got_ip;

// //     g_wifi_event_group = xEventGroupCreate();

// //     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &prov_event_handler, NULL));
// //     // ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_TRANSPORT_BLE_EVENT, ESP_EVENT_ANY_ID, &prov_event_handler, NULL));
// //     // ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT,
// //     //                                            ESP_EVENT_ANY_ID,
// //     //                                            &prov_event_handler,
// //     //                                            NULL));
// //     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
// //                                                         ESP_EVENT_ANY_ID,
// //                                                         &wifi_event_handler,
// //                                                         NULL,
// //                                                         &instance_any_id)); // Register event handler for Wi-Fi events
// //     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
// //                                                         IP_EVENT_STA_GOT_IP,
// //                                                         &wifi_event_handler,
// //                                                         NULL,
// //                                                         &instance_got_ip)); // Register event handler for IP events
// //     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
// // }
// // void wifi_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport)
// // {
// //     if (!name || !transport)
// //     {
// //         ESP_LOGW(WIFI_TAG, "Cannot generate QR code payload. Data missing.");
// //         return;
// //     }
// //     char payload[150] = {0};
// //     if (pop)
// //     {
// //         snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\""
// //                                            ",\"pop\":\"%s\",\"transport\":\"%s\"}",
// //                  "v1", name, pop, transport);
// //     }
// //     ESP_LOGI(WIFI_TAG, "Scan this QR code from the provisioning application for Provisioning.");
// //     esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
// //     esp_qrcode_generate(&cfg, payload);
// //     ESP_LOGI(WIFI_TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
// //     vTaskDelay(100 / portTICK_PERIOD_MS);
// // }
// // void get_device_service_name(char *service_name, size_t max)
// // {
// //     uint8_t eth_mac[6];
// //     const char *ssid_prefix = "envisor_id_";
// //     esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
// //     snprintf(service_name, max, "%s%02X%02X%02X", ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
// // }
// // WIFI_CONFIG_STATE read_wifi_config(void)
// // {
// //     nvs_handle_t nvs_handle;
// //     esp_err_t err;

// //     // Initialize NVS
// //     err = nvs_open_from_partition("nvs", "wifi_store", NVS_READWRITE, &nvs_handle);
// //     if (err != ESP_OK)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Error opening NVS handle!");
// //         return eWIFI_CONFIG_ERROR;
// //     }
// //     // Load existing configurations
// //     size_t required_size = MAX_WIFI_CONFIGS * sizeof(WifiConfig);
// //     err = nvs_get_blob(nvs_handle, "wifi_configs", wifiConfigs, &required_size);
// //     if (err == ESP_ERR_NVS_NOT_FOUND)
// //     {
// //         // First time initialization
// //         ESP_LOGI(WIFI_TAG, "No configurations found.....Using default wifi configratuion");
// //         memset(wifiConfigs, 0, required_size);
// //         return eWIFI_CONFIG_NOT_INITIALIZED;
// //     }
// //     else if (err != ESP_OK)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Error reading NVS!");
// //         nvs_close(nvs_handle);
// //         return eWIFI_CONFIG_ERROR;
// //     }
// //     else
// //     {
// //         ESP_LOGI(WIFI_TAG, "Configurations loaded successfully");
// //         return eWIFI_CONFIG_INITIALIZED;
// //     }
// // }
// // int list_wifi_stored(void)
// // {
// //     int config_count = 0;
// //     read_wifi_config();
// //     for (int i = 0; i < MAX_WIFI_CONFIGS; i++)
// //     {
// //         ESP_LOGI(WIFI_TAG, "SSID: %s, password: %s, length: %d", wifiConfigs[i].ssid, wifiConfigs[i].password, strlen(wifiConfigs[i].ssid));
// //         if (strlen(wifiConfigs[i].ssid) > 0)
// //         {
// //             config_count++;
// //         }
// //     }
// //     return config_count;
// // }
// // wifi_state_t connect_wifi(char *ssid, char *password, bool change)
// // {
    
// //     if(change)
// //     {
// //         ESP_LOGI(WIFI_TAG, "Disconnecting from current WiFi...");
// //         esp_wifi_disconnect();
// //     }
// //     // wifi_config_t wifi_config = {
// //     //     .sta = {
// //     //         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
// //     //         .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
// //     //         // .sae_h2e_identifier = "",
// //     //     },
// //     // };
// //     wifi_config_t wifi_config;

// //     wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
// //     wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

// //     strcpy((char *)wifi_config.sta.ssid, ssid);
// //     strcpy((char *)wifi_config.sta.password, password);
// //     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));  // Set Wi-Fi configuration
// //     ESP_LOGI(WIFI_TAG,"Ready to connect to ssid: %s    password: %s",(char *)wifi_config.sta.ssid,(char *)wifi_config.sta.password);
// //     ESP_LOGW(WIFI_TAG,"Connecting to new wifi..............");
    
// //     esp_wifi_connect();
// //     g_wifi_changed = false;
// //     vTaskDelay(10000/portTICK_PERIOD_MS);
// //     return g_state_wifi;
// // }
// // void add_newconfig(const char *ssid, const char *password, bool change)
// // {
// //     nvs_handle_t nvs_handle;
// //     esp_err_t err;
// //     // Initialize NVS
// //     err = nvs_open_from_partition("nvs", "wifi_store", NVS_READWRITE, &nvs_handle);
// //     if (err != ESP_OK)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Error opening NVS handle!");
// //         return;
// //     }

// //     // Load existing configurations
// //     size_t required_size = MAX_WIFI_CONFIGS * sizeof(WifiConfig);
// //     err = nvs_get_blob(nvs_handle, "wifi_configs", wifiConfigs, &required_size);
// //     if (err == ESP_ERR_NVS_NOT_FOUND)
// //     {
// //         // First time initialization
// //         ESP_LOGI(WIFI_TAG, "No configurations found, adding new one.");
// //         memset(wifiConfigs, 0, required_size);
// //     }
// //     else if (err != ESP_OK)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Error reading NVS!");
// //         nvs_close(nvs_handle);
// //         return;
// //     }
// //     // check if the ssid already exists
// //     for (int i = 0; i < MAX_WIFI_CONFIGS; i++)
// //     {
// //         if (strcmp(wifiConfigs[i].ssid, ssid) == 0)
// //         {
// //             {
// //                 ESP_LOGI(WIFI_TAG, "SSID already exists, index: %d", i);
// //                 // Delete all duplicated SSID and password entries if they exist, ensure all configs are unique
// //                 memset(wifiConfigs[i].ssid, 0, strlen(wifiConfigs[i].ssid));
// //                 memset(wifiConfigs[i].password, 0, strlen(wifiConfigs[i].password));
// //             }
// //         }
// //     }
// //     // Move all configurations, not leaving any gaps between configs
// //     for (int i = 1; i < MAX_WIFI_CONFIGS - 1; i++)
// //     {
// //         if (strlen(wifiConfigs[i].ssid) > 0 && strlen(wifiConfigs[i - 1].ssid) == 0)
// //         {
// //             int j = i;
// //             while (strlen(wifiConfigs[j - 1].ssid) == 0)
// //             {
// //                 wifiConfigs[j - 1] = wifiConfigs[j];
// //                 memset(wifiConfigs[j].ssid, 0, strlen(wifiConfigs[j].ssid));
// //                 memset(wifiConfigs[j].password, 0, strlen(wifiConfigs[j].password));
// //                 j--;
// //             }
// //         }
// //     }
// //     // Add the new config to the beginning and shift others down
// //     for (int i = MAX_WIFI_CONFIGS - 1; i > 0; i--)
// //     {
// //         wifiConfigs[i] = wifiConfigs[i - 1];
// //     }
// //     strncpy(wifiConfigs[0].ssid, ssid, sizeof(wifiConfigs[0].ssid) - 1);
// //     wifiConfigs[0].ssid[sizeof(wifiConfigs[0].ssid) - 1] = '\0';
// //     strncpy(wifiConfigs[0].password, password, sizeof(wifiConfigs[0].password) - 1);
// //     wifiConfigs[0].password[sizeof(wifiConfigs[0].password) - 1] = '\0';
// //     // Save the updated configurations
// //     err = nvs_set_blob(nvs_handle, "wifi_configs", wifiConfigs, required_size);
// //     if (err != ESP_OK)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Error saving to NVS!");
// //         nvs_close(nvs_handle);
// //         return;
// //     }
// //     err = nvs_commit(nvs_handle);
// //     if (err != ESP_OK)
// //     {
// //         ESP_LOGE(WIFI_TAG, "Error committing to NVS!");
// //     }
// //     nvs_close(nvs_handle);
// //     if (change)
// //     {
// //         g_wifi_changed = true;
// //     }
// // }
// // wifi_state_t checkwifi_state(void)
// // {
// //     return g_state_wifi;
// // }

// // void set_wifi_max_tx_power(void)
// // {
// //     esp_err_t err = esp_wifi_set_max_tx_power(20);
// //     // Kiểm tra và log kết quả
// //     ESP_LOGE(WIFI_TAG, "Esp set power: %s", esp_err_to_name(err));

// //     int8_t tx_power;
// //     esp_wifi_get_max_tx_power(&tx_power);
// //     tx_power = tx_power / 4;
// //     ESP_LOGI(WIFI_TAG, "Current max TX power: %d dBm", tx_power);
// // }
// // // void wifi_init()
// // // {
// // //     bool provision = false;
// // //     bool no_wifi_stored = false;
// // //     wifi_state_t wifi_state;
// // //     int retries = 0;
// // //     esp_wifi_start();
// // //     do {
// // //         wifi_state = scan_and_connect_wifi(MANUAL_WIFI_CONNECT);

// // //         switch(wifi_state)
// // //         {
// // //             case WIFI_CONFIG_INITIALIZED:
// // //                 ESP_LOGW(WIFI_TAG, "Wifi initialized");
// // //                 xEventGroupSetBits(g_wifi_event_group, g_initialized);
// // //                 return;

// // //             case WIFI_CONFIG_NO_WIFI_STORED:
// // //                 ESP_LOGI(WIFI_TAG, "No wifi stored");
// // //                 no_wifi_stored = true;
// // //                 provision = true;
// // //                 break;

// // //             case WIFI_CONFIG_NOT_INITIALIZED:
// // //                 if(!provision)
// // //                 {
// // //                     provision = true;
// // //                 }
// // //                 break;
// // //             case WIFI_CONNECTED:
// // //                 break;
// // //             case WIFI_DISCONNECTED:
// // //                 break;
// // //             case WIFI_CONFIG_ERROR:
// // //                 break;
// // //             default:
// // //                 // Handle these states if needed
// // //                 break;
// // //         }
// // //         if (provision) {
// // //             start_provisioning(no_wifi_stored);
// // //             if (no_wifi_stored) {
// // //                 ESP_LOGI(WIFI_TAG, "Waiting for Wi-Fi connection...");
// // //                 xEventGroupWaitBits(g_wifi_event_group, g_connected, pdTRUE, pdFALSE, portMAX_DELAY);
// // //                 wifi_prov_mgr_deinit();
// // //                 ESP_LOGW(WIFI_TAG, "Wifi initialized");
// // //                 xEventGroupSetBits(g_wifi_event_group, g_initialized);
// // //                 return;
// // //             }
// // //             // Wait for provisioning to complete
// // //             EventBits_t checkbits = xEventGroupWaitBits(g_wifi_event_group, g_connected, pdTRUE, pdFALSE, pdMS_TO_TICKS(60000));
// // //             if ((checkbits & g_connected_by_provision) == g_connected_by_provision || (checkbits & g_connected_by_scan) == g_connected_by_scan )
// // //             {
// // //                 wifi_prov_mgr_deinit();
// // //                 ESP_LOGW(WIFI_TAG, "Wifi initialized");
// // //                 xEventGroupSetBits(g_wifi_event_group, g_initialized);
// // //                 return;
// // //             }
// // //             else
// // //             {
// // //                 ESP_LOGI(WIFI_TAG, "Waiting for provisioning to complete...");
// // //                 // Continue the loop to scan and connect again
// // //             }
// // //         }
// // //         retries += 1;
// // //     } while (wifi_state != WIFI_CONNECTED && retries <= MAX_RETRIES);
// // //     ESP_LOGW(WIFI_TAG, "Wifi initialized");
// // //     xEventGroupSetBits(g_wifi_event_group, g_initialized);

// // // }
// // void wifi_init()
// // {    
// //     // Check and initialize event group if it’s NULL
// //     // if (g_wifi_event_group == NULL) {
// //     //     g_wifi_event_group = xEventGroupCreate();
// //     //     if (g_wifi_event_group == NULL) {
// //     //         ESP_LOGE(WIFI_TAG, "Failed to create event group");
// //     //         return; // Exit if event group creation failed
// //     //     }
// //     // }
// //     bool provision = false;
// //     bool no_wifi_stored = false;
// //     wifi_state_t wifi_state;
// //     int retries = 0;

// //     esp_wifi_start();
    
// //     do {
// //         wifi_state = scan_and_connect_wifi(MANUAL_WIFI_CONNECT);

// //         switch(wifi_state) {
// //             case WIFI_CONFIG_INITIALIZED:
// //                 ESP_LOGW(WIFI_TAG, "Wifi initialized");
// //                 xEventGroupSetBits(g_wifi_event_group, g_initialized);
// //                 return;

// //             case WIFI_CONFIG_NO_WIFI_STORED:
// //                 ESP_LOGI(WIFI_TAG, "No wifi stored");
// //                 no_wifi_stored = true;
// //                 provision = true;
// //                 break;

// //             case WIFI_CONFIG_NOT_INITIALIZED:
// //                 if (!provision) {
// //                     provision = true;
// //                 }
// //                 break;

// //             case WIFI_CONNECTED:
// //                 ESP_LOGI(WIFI_TAG, "Connected to Wi-Fi");
// //                 xEventGroupSetBits(g_wifi_event_group, g_connected_by_scan);
// //                 break;

// //             case WIFI_DISCONNECTED:
// //                 ESP_LOGW(WIFI_TAG, "Wi-Fi disconnected, retrying...");
// //                 break;

// //             case WIFI_CONFIG_ERROR:
// //                 ESP_LOGE(WIFI_TAG, "Wi-Fi configuration error");
// //                 break;

// //             default:
// //                 break;
// //         }

// //         if (provision) {
// //             start_provisioning(no_wifi_stored);

// //             if (no_wifi_stored) {
// //                 ESP_LOGI(WIFI_TAG, "Waiting for Wi-Fi connection after provisioning...");
// //                 xEventGroupWaitBits(g_wifi_event_group, g_connected, pdTRUE, pdFALSE, portMAX_DELAY);
// //                 wifi_prov_mgr_deinit();
// //                 ESP_LOGW(WIFI_TAG, "Wi-Fi initialized after provisioning");
// //                 xEventGroupSetBits(g_wifi_event_group, g_initialized);
// //                 return;
// //             }

// //             // Wait for provisioning to complete
// //             EventBits_t checkbits = xEventGroupWaitBits(g_wifi_event_group, g_connected, pdTRUE, pdFALSE, pdMS_TO_TICKS(60000));
// //             if ((checkbits & g_connected_by_provision) == g_connected_by_provision || 
// //                 (checkbits & g_connected_by_scan) == g_connected_by_scan) 
// //             {
// //                 wifi_prov_mgr_deinit();
// //                 ESP_LOGW(WIFI_TAG, "Wi-Fi initialized after provisioning");
// //                 xEventGroupSetBits(g_wifi_event_group, g_initialized);
// //                 return;
// //             } else {
// //                 ESP_LOGI(WIFI_TAG, "Provisioning timed out or failed, retrying...");
// //             }
// //         }
        
// //         retries += 1;
// //         vTaskDelay(pdMS_TO_TICKS(1000));  // Optional: delay between retries
// //     } while (wifi_state != WIFI_CONNECTED && retries <= MAX_RETRIES);

// //     ESP_LOGW(WIFI_TAG, "Wi-Fi initialization completed or max retries reached");
// //     xEventGroupSetBits(g_wifi_event_group, g_initialized);
// // }

// // // void start_provisioning(bool no_wifi_stored)
// // // {
// // //     // Initialize the default event loop if not already initialized
// // //     ESP_ERROR_CHECK(esp_event_loop_create_default());

// // //     // // Initialize the Wi-Fi stack
// // //     // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
// // //     // ESP_ERROR_CHECK(esp_wifi_init(&cfg));

// // //     // wifi_prov_mgr_config_t config = {
// // //     //     .scheme = wifi_prov_scheme_ble,
// // //     //     .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM,
// // //     // };
// // //     // ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

// // //     char service_name[30];
// // //     get_device_service_name(service_name, sizeof(service_name));

// // //     wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
// // //     const char *pop = "unicorn2025";
// // //     // wifi_prov_security1_params_t *sec_params = pop;
// // //     const char *service_key = NULL;

// // //     uint8_t custom_service_uuid[16] = {
// // //         0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
// // //         0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
// // //     };
// // //     wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
// // //     ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));
// // //     wifi_prov_print_qr(service_name, NULL, pop, "ble");
// // // }

// // void start_provisioning(bool no_wifi_stored)
// // {
// //     // // Initialize the default event loop if not already initialized
// //     // ESP_ERROR_CHECK(esp_event_loop_create_default());

// //     // // Initialize the Wi-Fi stack
// //     // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
// //     // ESP_ERROR_CHECK(esp_wifi_init(&cfg));

// //     // Configure provisioning manager with BLE scheme
// //     wifi_prov_mgr_config_t config = {
// //         .scheme = wifi_prov_scheme_ble,
// //         .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM,
// //     };
// //     ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

// //     // Generate service name based on device MAC address
// //     char service_name[30];
// //     get_device_service_name(service_name, sizeof(service_name));

// //     // Set up security and other provisioning parameters
// //     wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
// //     const char *pop = "unicorn2025";
// //     const char *service_key = NULL;

// //     // Set a custom UUID for the BLE service
// //     uint8_t custom_service_uuid[16] = {
// //         0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
// //         0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
// //     };
// //     wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);

// //     // Start Wi-Fi provisioning
// //     ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));

// //     // Print QR code for mobile app provisioning
// //     wifi_prov_print_qr(service_name, NULL, pop, "ble");
// // }


// // void wifi_task(void *arg)
// // {
// //     ESP_ERROR_CHECK(esp_netif_init());                // Thiết lập giao tiếp mạng (Wifi, LAN, PPoPE,...)
// //     ESP_ERROR_CHECK(esp_event_loop_create_default()); // Thiết lập vòng lặp bắt sự kiện
// //     esp_netif_create_default_wifi_sta();              // Create the default Wi-Fi station interface
// //     wifi_stack_init();                                // Khởi tạo stack wifi
// //     // EventGroupHandle_t event_manager = (EventGroupHandle_t)arg;
// //     wifi_init();
// //     // start_provisioning(false);
// //     // if (power_save() == ESP_OK)
// //     // {
// //     //     ESP_LOGI(WIFI_TAG, "Power save mode enabled");
// //     // }
// //     // else
// //     // {
// //     //     ESP_LOGE(WIFI_TAG, "Power save mode failed");
// //     // }
// //     // sync_timestamp(event_manager);
// //     while (1)
// //     {
// //         // if (g_wifi_changed)
// //         // {
// //         //     ESP_LOGW(WIFI_TAG, "Changing wifi...");
// //         //     if (connect_wifi(wifiConfigs[0].ssid, wifiConfigs[0].password, true) == WIFI_CONNECTED)
// //         //     {
// //         //         ESP_LOGI(WIFI_TAG, "Changed to wifi: %s", wifiConfigs[0].ssid);
// //         //     }
// //         //     else
// //         //     {
// //         //         ESP_LOGI(WIFI_TAG, "Failed to change wifi");
// //         //     }
// //         //     continue;
// //         // }
        
// //         ESP_LOGI(WIFI_TAG, "Checking for wifi status...");
// //         // if (checkwifi_state() == WIFI_CONNECTED)
// //         // {
// //         //     ESP_LOGI(WIFI_TAG, "Wifi is still connected");

// //         //     //xEventGroupSetBits(event_manager, WIFI_CONNECTED_EVENT);
// //         // }
// //         // else if (checkwifi_state() == WIFI_DISCONNECTED)
// //         // {
// //         //     ESP_LOGI(WIFI_TAG, "Wifi is disconnected");
// //         //     // xEventGroupClearBits(event_manager, WIFI_CONNECTED_EVENT);
// //         //     ESP_LOGI(WIFI_TAG, "Retry connect to Wifi");
// //         //     scan_and_connect_wifi(AUTOMATIC_WIFI_CONNECT);
// //         // }
// //         vTaskDelay(30000 / portTICK_PERIOD_MS);
// //     }
// // }
// // wifi_state_t scan_and_connect_wifi(bool connect_type)
// // {
// //     int config_count = list_wifi_stored();
// //     if(config_count == 0)
// //     {
// //         ESP_LOGI(WIFI_TAG, "No wifi stored");
// //         return WIFI_CONFIG_NO_WIFI_STORED;
// //     }
// //     if(connect_type == AUTOMATIC_WIFI_CONNECT)
// //     {
// //         wifi_ap_record_t *ap_list;
// //         uint16_t ap_num;
// //         esp_wifi_scan_start(NULL, true);
// //         esp_wifi_scan_get_ap_num(&ap_num);
// //         ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_num);
// //         esp_wifi_scan_get_ap_records(&ap_num, ap_list);
// //         for(int i = 0 ;i < ap_num; i++)
// //         {
// //             ESP_LOGI(WIFI_TAG,"SSID: %s, RSSI: %d",ap_list[i].ssid, ap_list[i].rssi);
// //         }
// //         for(int i = 0 ; i< MAX_WIFI_CONFIGS; ++i)
// //         {
// //             for(int j = 0 ;j < ap_num; j++)
// //             {
// //                 if(strcmp(wifiConfigs[i].ssid, (char *)ap_list[j].ssid) == 0)
// //                 {
// //                     ESP_LOGI(WIFI_TAG,"Found the ssid: %s",wifiConfigs[i].ssid);
// //                     if(connect_wifi(wifiConfigs[i].ssid, wifiConfigs[i].password, false) == WIFI_CONNECTED)
// //                     {
// //                         ESP_LOGI(WIFI_TAG,"Connected to wifi: %s",wifiConfigs[i].ssid);
// //                         return WIFI_CONNECTED;
// //                     }
// //                     else continue;
// //                 }
// //             }
// //         }
// //         ESP_LOGI(WIFI_TAG,"No wifi available");
// //         return WIFI_CONFIG_NOT_INITIALIZED;
// //     }
// //     else if(connect_type == MANUAL_WIFI_CONNECT)
// //     {
// //         for(int i = 0 ;i < config_count; i++)
// //         {
// //             if(connect_wifi(wifiConfigs[i].ssid, wifiConfigs[i].password, false) == WIFI_CONNECTED)
// //             {
// //                 ESP_LOGI(WIFI_TAG,"Connected to wifi: %s",wifiConfigs[i].ssid);
// //                 xEventGroupSetBits(g_wifi_event_group, g_connected_by_scan);
// //                 return WIFI_CONFIG_INITIALIZED;
// //             }
// //             else 
// //             {   
// //                 ESP_LOGI(WIFI_TAG,"Failed to connect to wifi: %s",wifiConfigs[i].ssid);
// //                 continue;
// //             }
// //         }
// //         ESP_LOGE(WIFI_TAG,"Failed to connect to all saved wifi");
// //         return WIFI_CONFIG_NOT_INITIALIZED;
// //     }
// //     return WIFI_CONFIG_ERROR;
// // }


// // // static const char *TAG = "app";

// // // /* Signal Wi-Fi events on this event-group */
// // // const int WIFI_CONNECTED_EVENT = BIT0;
// // // EventGroupHandle_t wifi_event_group;

// // // #define PROV_QR_VERSION         "v1"
// // // // #define PROV_TRANSPORT_SOFTAP   "softap"
// // // #define PROV_TRANSPORT_BLE      "ble"
// // // #define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

// // // /* Event handler for catching system events */
// // // static void event_handler(void* arg, esp_event_base_t event_base,
// // //                           int32_t event_id, void* event_data)
// // // {
// // // #ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
// // //     static int retries;
// // // #endif
// // //     if (event_base == WIFI_PROV_EVENT) {
// // //         switch (event_id) {
// // //             case WIFI_PROV_START:
// // //                 ESP_LOGI(TAG, "Provisioning started");
// // //                 break;
// // //             case WIFI_PROV_CRED_RECV: {
// // //                 wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
// // //                 ESP_LOGI(TAG, "Received Wi-Fi credentials"
// // //                          "\n\tSSID     : %s\n\tPassword : %s",
// // //                          (const char *) wifi_sta_cfg->ssid,
// // //                          (const char *) wifi_sta_cfg->password);
// // //                 break;
// // //             }
// // //             case WIFI_PROV_CRED_FAIL: {
// // //                 wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
// // //                 ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
// // //                          "\n\tPlease reset to factory and retry provisioning",
// // //                          (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
// // //                          "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
// // // #ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
// // //                 retries++;
// // //                 if (retries >= CONFIG_EXAMPLE_PROV_MGR_MAX_RETRY_CNT) {
// // //                     ESP_LOGI(TAG, "Failed to connect with provisioned AP, reseting provisioned credentials");
// // //                     wifi_prov_mgr_reset_sm_state_on_failure();
// // //                     retries = 0;
// // //                 }
// // // #endif
// // //                 break;
// // //             }
// // //             case WIFI_PROV_CRED_SUCCESS:
// // //                 ESP_LOGI(TAG, "Provisioning successful");
// // // #ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
// // //                 retries = 0;
// // // #endif
// // //                 break;
// // //             case WIFI_PROV_END:
// // //                 /* De-initialize manager once provisioning is finished */
// // //                 wifi_prov_mgr_deinit();
// // //                 break;
// // //             default:
// // //                 break;
// // //         }
// // //     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
// // //         esp_wifi_connect();
// // //     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
// // //         ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
// // //         ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
// // //         /* Signal main application to continue execution */
// // //         xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
// // //     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
// // //         ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
// // //         esp_wifi_connect();
// // //     }
// // // }

// // // static void wifi_init_sta(void)
// // // {
// // //     /* Start Wi-Fi in station mode */
// // //     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
// // //     ESP_ERROR_CHECK(esp_wifi_start());
// // // }

// // // static void get_device_service_name(char *service_name, size_t max)
// // // {
// // //     uint8_t eth_mac[6];
// // //     const char *ssid_prefix = "PROV_";
// // //     esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
// // //     snprintf(service_name, max, "%s%02X%02X%02X",
// // //              ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
// // // }

// // // /* Handler for the optional provisioning endpoint registered by the application.
// // //  * The data format can be chosen by applications. Here, we are using plain ascii text.
// // //  * Applications can choose to use other formats like protobuf, JSON, XML, etc.
// // //  */
// // // esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
// // //                                           uint8_t **outbuf, ssize_t *outlen, void *priv_data)
// // // {
// // //     if (inbuf) {
// // //         ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
// // //     }
// // //     char response[] = "SUCCESS";
// // //     *outbuf = (uint8_t *)strdup(response);
// // //     if (*outbuf == NULL) {
// // //         ESP_LOGE(TAG, "System out of memory");
// // //         return ESP_ERR_NO_MEM;
// // //     }
// // //     *outlen = strlen(response) + 1; /* +1 for NULL terminating byte */

// // //     return ESP_OK;
// // // }

// // // static void wifi_prov_print_qr(const char *name, const char *pop, const char *transport)
// // // {
// // //     if (!name || !transport) {
// // //         ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
// // //         return;
// // //     }
// // //     char payload[150] = {0};
// // //     if (pop) {
// // //         snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
// // //                     ",\"pop\":\"%s\",\"transport\":\"%s\"}",
// // //                     PROV_QR_VERSION, name, pop, transport);
// // //     } else {
// // //         snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
// // //                     ",\"transport\":\"%s\"}",
// // //                     PROV_QR_VERSION, name, transport);
// // //     }
// // // #ifdef CONFIG_EXAMPLE_PROV_SHOW_QR
// // //     ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
// // //     esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
// // //     esp_qrcode_generate(&cfg, payload);
// // // #endif /* CONFIG_APP_WIFI_PROV_SHOW_QR */
// // //     ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
// // // }

// // // void wifi_task(void *arg)
// // // {
// // //     /* Initialize NVS partition */
// // //     esp_err_t ret = nvs_flash_init();
// // //     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
// // //         /* NVS partition was truncated
// // //          * and needs to be erased */
// // //         ESP_ERROR_CHECK(nvs_flash_erase());

// // //         /* Retry nvs_flash_init */
// // //         ESP_ERROR_CHECK(nvs_flash_init());
// // //     }

// // //     /* Initialize TCP/IP */
// // //     ESP_ERROR_CHECK(esp_netif_init());

// // //     /* Initialize the event loop */
// // //     ESP_ERROR_CHECK(esp_event_loop_create_default());
// // //     wifi_event_group = xEventGroupCreate();

// // //     /* Register our event handler for Wi-Fi, IP and Provisioning related events */
// // //     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
// // //     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
// // //     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

// // //     /* Initialize Wi-Fi including netif with default config */
// // //     esp_netif_create_default_wifi_sta();

// // //     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
// // //     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

// // //     /* Configuration for the provisioning manager */
// // //     wifi_prov_mgr_config_t config = {
// // //         .scheme = wifi_prov_scheme_ble,
// // //         .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
// // //     };

// // //     /* Initialize provisioning manager with the
// // //      * configuration parameters set above */
// // //     ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

// // //     bool provisioned = false;
// // // // #ifdef CONFIG_EXAMPLE_RESET_PROVISIONED
// // //     wifi_prov_mgr_reset_provisioning();
// // // // #else
// // //     /* Let's find out if the device is provisioned */
// // //     // ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

// // // // #endif
// // //     /* If device is not yet provisioned start provisioning service */
// // //     if (!provisioned) {
// // //         ESP_LOGI(TAG, "Starting provisioning");

// // //         char service_name[12];
// // //         get_device_service_name(service_name, sizeof(service_name));

// // //         wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
// // //         const char *pop = "abcd1234";
// // //         const char *service_key = NULL;

// // //         uint8_t custom_service_uuid[] = {
// // //             /* LSB <---------------------------------------
// // //              * ---------------------------------------> MSB */
// // //             0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
// // //             0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
// // //         };
// // //         wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
// // //         ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));
// // //         wifi_prov_print_qr(service_name, pop, PROV_TRANSPORT_BLE);

// // //     } else {
// // //         ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");

// // //         /* We don't need the manager as device is already provisioned,
// // //          * so let's release it's resources */
// // //         wifi_prov_mgr_deinit();

// // //         /* Start Wi-Fi station */
// // //         wifi_init_sta();
// // //     }

// // //     /* Wait for Wi-Fi connection */
// // //     xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, false, true, portMAX_DELAY);

// // //     /* Start main application now */
// // //     while (1) {
// // //         ESP_LOGI(TAG, "Hello World!");
// // //         vTaskDelay(1000 / portTICK_PERIOD_MS);
// // //     }
// // // }