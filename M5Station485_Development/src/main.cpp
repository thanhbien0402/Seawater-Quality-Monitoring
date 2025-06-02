#include "./LED/include/blinky_led.h"
#include "./Button/include/button.h"
#include "./WiFi/include/wifiprovision.h"
#include "./MQTT/include/mqtt.h"
#include "./ESSensor/include/es_sensor.h"
#include "./NVS/include/nvs_store.h"
#include "./Global/include/global.h"
#include "./OTA/include/ota.h"
#include "./GPS/include/gps.h"
#include "./Model/include/model.h"
#include "./Cron/include/cronjob.h"

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
    // control_queue = xQueueCreate(2, 200 * sizeof(char));
    // control_def.ValueQueue = control_queue;
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

void setup()
{
    Serial.begin(115200);

    system_init();
    initEventGroup();
    initRS485(UART_NUM_2, BAUDRATE, RXD, TXD);
    initModel();

    xTaskCreate(connectWiFi, "connectWiFi", 4096, NULL, 10, NULL);
    // xTaskCreate(task_M5_setup, "task_M5_setup", 4096, NULL, 5, NULL);
    // xTaskCreate(handleBlinkyLed, "blinky_led", 4096, NULL, 5, NULL);   
    // xTaskCreate(handleButtonA, "buttonA", 4096, NULL, 5, NULL);
    // xTaskCreate(handleButtonB, "buttonB", 4096, NULL, 5, NULL);
    // xTaskCreate(handleButtonC, "buttonC", 4096, NULL, 5, NULL);
    xTaskCreate(updateGPSPosition, "updateGPSPosition", 4096, NULL, 5, NULL);
    xTaskCreate(mqttTask, "mqttTask", 4096, NULL, 5, NULL);
    xTaskCreate(checkPumpStatusTask, "checkPumpStatusTask", 4096, NULL, 2, NULL);
    xTaskCreate(publisherTask, "publisherTask", 4096, NULL, 5, NULL);
    xTaskCreate(publishExecutedCronTask, "publishExecutedCronTask", 4096, NULL, 5, &publish_executed_cronjob_handle);
    xTaskCreate(otaUpdateTask, "otaUpdateTask", 8192, NULL, 10, &otaUpdateTask_handle);
    xTaskCreate(controlTask, "controlTask", 4096, NULL, 5, NULL);
    cronjob_init();
}

void loop() {
}
