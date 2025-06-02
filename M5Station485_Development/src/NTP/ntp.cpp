#include "./include/ntp.h"
#include "./Global/include/global.h"
#include "./Cron/include/cronjob.h"

static const char *NTP_TAG = "NTP";

void initialize_sntp(void) {
    ESP_LOGI(NTP_TAG, "Initializing SNTP...");

    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    sntp_setservername(0, SNTP_SERVER_NAME);
    sntp_setservername(1, "1.vn.pool.ntp.org");
    sntp_setservername(2, "0.asia.pool.ntp.org");
    sntp_init();

    setenv("TZ", "ICT-7", 1); // Set timezone to Vietnam Standard Time
    tzset(); // Apply timezone settings
}

volatile bool wait_for_sntp_sync() {
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;

    while (timeinfo.tm_year < (2020 - 1900) && ++retry < SNTP_SYNC_TIMEOUT_SEC) {
        ESP_LOGI(NTP_TAG, "Waiting for SNTP sync... (%d/%d)", retry, SNTP_SYNC_TIMEOUT_SEC);
        vTaskDelay(pdMS_TO_TICKS(1000));
        time(&now);                      // Get current time
        localtime_r(&now, &timeinfo);    // Convert to local time

        // Print current time
        ESP_LOGI(NTP_TAG, "Current time: %04d-%02d-%02d %02d:%02d:%02d", 
            timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }

    if (handle_cron_jobs_task_handle) {
        xTaskNotifyGive(handle_cron_jobs_task_handle); // Notify cron job task to start
    }

    return (timeinfo.tm_year >= (2020 - 1900)); 
}

void get_time_from_sntp(void *pvParameters) {
    ESP_LOGI(NTP_TAG, "Getting time from SNTP...");

    while (1) {
        wait_for_sntp_sync();

        if (wait_for_sntp_sync()) {
            vTaskDelete(NULL); // Delete the task if sync is successful
        }
    }
}