#ifndef CRONJOB_H
#define CRONJOB_H

#include "./NVS/include/nvs_store.h"

#define MAX_CRON_JOBS 100
#define CRON_NVS_PARTITION "nvs_data"
#define CRON_NVS_NAMESPACE "cron_store"

typedef struct __attribute__((packed)) {
    uint32_t id;
    uint32_t checksum;        
    char action[24];  // add, edit, delete    
    char cron[24]; 
    char relay[24];    
    bool state;          // 0: OFF, 1: ON      
    // bool repeat;        
} CronJob;

extern TaskHandle_t handle_cron_jobs_task_handle;
extern TaskHandle_t publish_executed_cronjob_handle;
extern volatile bool executed_cronjob;
extern volatile bool pumpStatus_executed_cronjob;
extern volatile uint32_t id_executed_cronjob;
extern char executed_cron_expr[24];  // Stores the cron expression of the executed job

void cronjob_init();
void save_cronjob_to_nvs(const CronJob *job);
void delete_cronjob_from_nvs(uint32_t id);
void reload_cronjobs_from_nvs();
void handle_cron_jobs_task(void *pvParameters);
void cronjob_reload_task(void *pvParameters);
void execute_cronjob(const CronJob *job); 
bool cron_match(const char *cron_expr, const struct tm *timeinfo);

#endif // CRONJOB_H