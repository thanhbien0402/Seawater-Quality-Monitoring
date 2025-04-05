#ifndef NVS_STORE_H
#define NVS_STORE_H

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <inttypes.h>
#include "./ESSensor/include/es_sensor.h"

#define STORAGE_NAMESPACE       "storage_data"
#define MAX_STORAGE_SIZE        6000
#define MAX_KEY_LENGTH          16

typedef union {
    uint64_t Frame;
    uint8_t Data[8];
} DataStore;

typedef enum {
    eNVS_EMPTY = 0,
    eNVS_FULL = 1,
    eNVS_ERROR = 2,
    eNVS_READY = 3
} NVS_STATE;

typedef enum {
    eNVS_WRITE = 0,
    eNVS_READ = 1
} NVS_OPERATION;

esp_err_t nvs_init(void);
esp_err_t nvs_store_commit(nvs_handle_t handle, NVS_OPERATION operation);
esp_err_t nvs_store(DataStore data);
esp_err_t nvs_store_again(DataStore data);
NVS_STATE nvs_load(DataStore *data);
uint16_t nvs_get_tail(void);

DataStore nvs_transfer_data_to_storage(SensorData data);
SensorData nvs_transfer_storage_to_data(DataStore stored_data);

#endif // NVS_STORE_H