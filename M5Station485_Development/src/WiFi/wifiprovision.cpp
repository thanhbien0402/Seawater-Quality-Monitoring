#include "./include/wifiprovision.h"
#include "./Global/include/global.h"
#include "./NVS/include/nvs_store.h"
#include "./NTP/include/ntp.h"

static const char *WIFI_TAG = "WiFi";
volatile bool sync_ntp_first = true;

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
    
    if (sync_ntp_first) {
        initialize_sntp();
        sync_ntp_first = false;

        if (wait_for_sntp_sync()) {
            ESP_LOGI(WIFI_TAG, "NTP sync successful");
        } else {
            xTaskCreate(get_time_from_sntp, "Get Time from SNTP", 4096, NULL, 5, NULL);
            ESP_LOGE(WIFI_TAG, "NTP sync failed");
        }
    }
    // Save connected Wi-Fi credentials to NVS
    // save_connected_wifi_to_nvs();
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

void save_connected_wifi_to_nvs() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_stored", NVS_READWRITE, &nvs_handle);

    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return;
    }

    // Get SSID and password from Wi-Fi connection
    std::string ssid = WIFI_SSID;
    std::string password = WIFI_PASSWORD;

    // Save SSID and password to NVS
    err = nvs_set_str(nvs_handle, "ssid", ssid.c_str());
    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error saving SSID to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }
    err = nvs_set_str(nvs_handle, "password", password.c_str());
    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error saving password to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error committing changes to NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(WIFI_TAG, "Wi-Fi credentials saved to NVS successfully");
    }

    nvs_close(nvs_handle);
    
}

esp_err_t load_wifi_from_nvs(char *ssid, size_t ssid_len, char* password, size_t password_len) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_stored", NVS_READONLY, &nvs_handle);

    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }

    // Read SSID and Password from NVS
    err = nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len);
    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error reading SSID from NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_get_str(nvs_handle, "password", password, &password_len);
    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "Error reading password from NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    nvs_close(nvs_handle);
    ESP_LOGI(WIFI_TAG, "Loaded Wi-Fi credentials from NVS successfully");
    ESP_LOGI(WIFI_TAG, "SSID: %s", ssid);
    return ESP_OK;
}

void scanWiFi(void *pvParameters) {
    while (true) {
        ESP_LOGI(WIFI_TAG, "Starting Wi-Fi scan...");

        WiFi.mode(WIFI_STA);       // Đặt ở chế độ station
        WiFi.disconnect();         // Ngắt kết nối để scan
        vTaskDelay(100 / portTICK_PERIOD_MS);

        int n = WiFi.scanNetworks();
        if (n == 0) {
            ESP_LOGW(WIFI_TAG, "No Wi-Fi networks found.");
        } else {
            ESP_LOGI(WIFI_TAG, "%d Wi-Fi networks found:", n);
            for (int i = 0; i < n; ++i) {
                String ssid = WiFi.SSID(i);
                int32_t rssi = WiFi.RSSI(i);
                wifi_auth_mode_t encryption = WiFi.encryptionType(i);

                const char* enc_type;
                switch (encryption) {
                    case WIFI_AUTH_OPEN: enc_type = "Open"; break;
                    case WIFI_AUTH_WEP: enc_type = "WEP"; break;
                    case WIFI_AUTH_WPA_PSK: enc_type = "WPA"; break;
                    case WIFI_AUTH_WPA2_PSK: enc_type = "WPA2"; break;
                    case WIFI_AUTH_WPA_WPA2_PSK: enc_type = "WPA/WPA2"; break;
                    case WIFI_AUTH_WPA2_ENTERPRISE: enc_type = "WPA2-Enterprise"; break;
                    case WIFI_AUTH_WPA3_PSK: enc_type = "WPA3"; break;
                    default: enc_type = "Unknown"; break;
                }

                ESP_LOGI(WIFI_TAG, "[%d] SSID: %s | RSSI: %d dBm | Encryption: %s",
                         i + 1, ssid.c_str(), rssi, enc_type);
            }
            WiFi.scanDelete();  // Giải phóng bộ nhớ
        }

        vTaskDelay(30000 / portTICK_PERIOD_MS);  // Quét lại sau 30 giây
    }
}