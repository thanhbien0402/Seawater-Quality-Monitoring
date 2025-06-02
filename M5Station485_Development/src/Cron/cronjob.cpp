#include "./include/cronjob.h"
#include "./Global/include/global.h"
#include "./ESSensor/include/es_sensor.h"
#include <cJSon.h>
// #include <sstream>

static const char *CRON_TAG = "CRON";

SemaphoreHandle_t cronjob_mutex = NULL;
TaskHandle_t cronjob_reload_task_handle = NULL;
TaskHandle_t handle_cron_jobs_task_handle = NULL;

static CronJob cronjob_list[MAX_CRON_JOBS] = {0};
static int cronjob_count = 0;

volatile bool executed_cronjob = false;
volatile bool pumpStatus_executed_cronjob = false;
volatile uint32_t id_executed_cronjob = 0;
char executed_cron_expr[24] = {0};  // Stores the cron expression of the executed job

static uint32_t calculate_checksum(const CronJob *job) {
    const uint8_t *data = (const uint8_t *)job;
    uint32_t checksum = 0x12345678;  // Initial checksum value

    size_t checksum_offset = offsetof(CronJob, checksum);
    for (size_t i = 0; i < sizeof(CronJob); i++) {
        if (i >= checksum_offset && i < checksum_offset + sizeof(job->checksum)) {
            continue;  // Skip checksum field
        }
        checksum = (checksum << 5) + checksum + data[i];  // Better mixing
    }

    return checksum;
}

void save_cronjob_to_nvs(const CronJob *job) {
    if (xSemaphoreTake(cronjob_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(CRON_TAG, "Failed to take mutex for cron jobs");
        return;
    }
    nvs_handle_t cron_nvs;
    esp_err_t err = nvs_open_from_partition(CRON_NVS_PARTITION, CRON_NVS_NAMESPACE, NVS_READWRITE, &cron_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(CRON_TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        xSemaphoreGive(cronjob_mutex);
        return;
    }

    CronJob stored_job = *job;
    stored_job.checksum = 0; // Clear checksum before calculating
    stored_job.checksum = calculate_checksum(&stored_job); // Calculate checksum

    char key[16];
    snprintf(key, sizeof(key), "job_%u", job->id);

    err = nvs_set_blob(cron_nvs, key, &stored_job, sizeof(CronJob));
    if (err == ESP_OK) {
        nvs_commit(cron_nvs);
        ESP_LOGI(CRON_TAG, "Cron job saved to NVS: ID %u", job->id);

        if (cronjob_reload_task_handle) {
            xTaskNotifyGive(cronjob_reload_task_handle);
        }
    } else {
        ESP_LOGE(CRON_TAG, "Failed to save cron job to NVS: %s", esp_err_to_name(err));
    }
    nvs_close(cron_nvs);
    xSemaphoreGive(cronjob_mutex);
}

void delete_cronjob_from_nvs(uint32_t id) {
    if (xSemaphoreTake(cronjob_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(CRON_TAG, "Failed to take mutex for cron jobs");
        return;
    }
    nvs_handle_t cron_nvs;
    esp_err_t err = nvs_open_from_partition(CRON_NVS_PARTITION, CRON_NVS_NAMESPACE, NVS_READWRITE, &cron_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(CRON_TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        xSemaphoreGive(cronjob_mutex);
        return;
    }

    char key[16];
    snprintf(key, sizeof(key), "job_%u", id);

    err = nvs_erase_key(cron_nvs, key);
    if (err == ESP_OK) {
        nvs_commit(cron_nvs);
        ESP_LOGI(CRON_TAG, "Cron job deleted from NVS: ID %u", id);

        if (cronjob_reload_task_handle) {
            xTaskNotifyGive(cronjob_reload_task_handle);
        }
    } else {
        ESP_LOGE(CRON_TAG, "Failed to delete cron job from NVS: %s", esp_err_to_name(err));
    }
    nvs_close(cron_nvs);
    xSemaphoreGive(cronjob_mutex);
}

void reload_cronjobs_from_nvs() {
    if (xSemaphoreTake(cronjob_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(CRON_TAG, "Failed to take mutex for cron jobs");
        return;
    }

    cronjob_count = 0;

    nvs_handle_t cron_nvs;
    esp_err_t err = nvs_open_from_partition(CRON_NVS_PARTITION, CRON_NVS_NAMESPACE, NVS_READONLY, &cron_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(CRON_TAG, "Failed to open NVS partition: %s", esp_err_to_name(err));
        xSemaphoreGive(cronjob_mutex);
        return;
    }
    
    // Get the list of keys from NVS
    nvs_iterator_t it = nvs_entry_find(CRON_NVS_PARTITION, CRON_NVS_NAMESPACE, NVS_TYPE_BLOB);
    while (it != NULL && cronjob_count < MAX_CRON_JOBS) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        
        // Check if the key starts with "job_"
        if (strncmp(info.key, "job_", 4) == 0) {
            CronJob job;
            size_t size = sizeof(CronJob);
            err = nvs_get_blob(cron_nvs, info.key, &job, &size);
            
            if (err == ESP_OK) {
                if (job.checksum == calculate_checksum(&job)) {
                    cronjob_list[cronjob_count++] = job;
                    ESP_LOGI(CRON_TAG, "Loaded cron job from NVS: ID %u", job.id);
                } else {
                    ESP_LOGW(CRON_TAG, "Checksum mismatch for job with key %s", info.key);
                }
            } else {
                ESP_LOGE(CRON_TAG, "Error reading key '%s': %s", info.key, esp_err_to_name(err));
            }
        }
        
        // Move to next key
        it = nvs_entry_next(it);
    }
    
    // Release iterator
    nvs_release_iterator(it);
    
    nvs_close(cron_nvs);

    if (cronjob_count == 0) {
        ESP_LOGW(CRON_TAG, "No valid cron jobs found in NVS.");
    } else {
        ESP_LOGI(CRON_TAG, "Total cron jobs loaded: %d", cronjob_count);
    }
    xSemaphoreGive(cronjob_mutex);
}

void execute_cronjob(const CronJob *job) {
    ESP_LOGI(CRON_TAG, "Executing cron job: ID %u, Relay %s, State %s", job->id, job->relay, job->state ? "On" : "Off");
    
    // Control relay based on job state
    turnPumpRelay(job->state);
    
    // Set execution flags for MQTT notification
    executed_cronjob = true;
    pumpStatus_executed_cronjob = job->state;
    id_executed_cronjob = job->id;
    
    // Store the cron expression
    strncpy(executed_cron_expr, job->cron, sizeof(executed_cron_expr) - 1);
    executed_cron_expr[sizeof(executed_cron_expr) - 1] = '\0';
    
    // Notify the publisher task about the executed cron job
    if (publish_executed_cronjob_handle != NULL) {
        xTaskNotifyGive(publish_executed_cronjob_handle);
        ESP_LOGI(CRON_TAG, "Notified publisher task about executed cron job");
    } else {
        ESP_LOGW(CRON_TAG, "Publisher task handle is NULL, cannot notify");
    }
}

void handle_cron_jobs_task(void *pvParameters) {
    ESP_LOGI(CRON_TAG, "Cron job handler task waiting for notification...");
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGI(CRON_TAG, "Cron job handler task started.");

    while (true) {
        struct timeval tv;
        struct tm timeinfo;
        time_t now;

        gettimeofday(&tv, NULL);
        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_sec == 0) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TickType_t xLastWakeTime = xTaskGetTickCount(); 

    while (1) {
        struct timeval tv;
        struct tm timeinfo;
        time_t now;

        gettimeofday(&tv, NULL);
        time(&now);
        localtime_r(&now, &timeinfo);

        ESP_LOGI(CRON_TAG, "Current time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

        if (xSemaphoreTake(cronjob_mutex, portMAX_DELAY) == pdTRUE) {
            for (size_t i = 0; i < cronjob_count; i++) {
                ESP_LOGI(CRON_TAG, "Checking cron job: ID %u, Cron %s", cronjob_list[i].id, cronjob_list[i].cron);
                if (cron_match(cronjob_list[i].cron, &timeinfo)) {
                    execute_cronjob(&cronjob_list[i]);
                }
            }
            xSemaphoreGive(cronjob_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(60000));
    }
}

void cronjob_reload_task(void *pvParameters) {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_LOGI(CRON_TAG, "Reloading cron jobs from NVS...");
        reload_cronjobs_from_nvs();
    }
}

void cronjob_init() {
    cronjob_mutex = xSemaphoreCreateMutex();
    if (cronjob_mutex == NULL) {
        ESP_LOGE(CRON_TAG, "Failed to create mutex for cron jobs");
        return;
    }

    reload_cronjobs_from_nvs();
    xTaskCreate(handle_cron_jobs_task, "CronJobTask", 1024 * 8, NULL, 10, &handle_cron_jobs_task_handle);
    xTaskCreate(cronjob_reload_task, "CronJobReloadTask", 1024 * 4, NULL, 9, &cronjob_reload_task_handle);  
}

bool match_field(const char *field, int value) {
    if (strcmp(field, "*") == 0 || strcmp(field, "*/1") == 0) return true;

    char field_copy[32];
    strncpy(field_copy, field, sizeof(field_copy) - 1);
    field_copy[sizeof(field_copy) - 1] = '\0';

    char *token = strtok(field_copy, ",");
    while (token) {
        // Check for */n
        if (strncmp(token, "*/", 2) == 0) {
            int step = atoi(token + 2);
            if (step > 0) 
                if (value % step == 0) return true;
        }
        // Check for range (e.g., 10-15)
        else if (strchr(token, '-') != NULL) {
            int start, end;
            if (sscanf(token, "%d-%d", &start, &end) == 2) {
                if (value >= start && value <= end) return true;
            }
        }
        // Check for exact number
        else {
            if (atoi(token) == value) return true;
        }

        token = strtok(NULL, ",");
    }

    return false;
}

bool cron_match(const char *cron_expr, const struct tm *timeinfo) {
    char cron_copy[64];
    strncpy(cron_copy, cron_expr, sizeof(cron_copy) - 1);
    cron_copy[sizeof(cron_copy) - 1] = '\0';

    char *fields[5];
    char *token = strtok(cron_copy, " ");
    int i = 0;
    while (token && i < 5) {
        fields[i++] = token;
        token = strtok(NULL, " ");
    }

    if (i < 5) {
        ESP_LOGE(CRON_TAG, "Invalid cron expression: \"%s\" (expected 5 fields, got %d)", cron_expr, i);
        return false;
    }

    int time_values[5] = {
        timeinfo->tm_min,                                       // minute
        timeinfo->tm_hour,                                      // hour
        timeinfo->tm_mday,                                      // day of month
        timeinfo->tm_mon + 1,                                   // month (tm_mon starts at 0)
        timeinfo->tm_wday == 0 ? 7 : timeinfo->tm_wday          // day of week (0=Sunday)
    };

    for (int j = 0; j < 5; j++) {
        if (!match_field(fields[j], time_values[j])) {
            return false;
        }
    }

    return true;
}