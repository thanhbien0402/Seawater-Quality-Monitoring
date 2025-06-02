#include "./include/nvs_store.h"
#include <inttypes.h>

static const char *NVS_TAG = "NVS";

static uint16_t head_iterator = 0;
static uint16_t tail_iterator = 0;

esp_err_t nvs_init(void)
{
    nvs_handle my_handle;
    esp_err_t err;
    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage...");
    err = nvs_open_from_partition("nvs", STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to open NVS handle");
        return err;
    }
    ESP_LOGI(NVS_TAG, "Reading head and tail pointers....");
    head_iterator = 0;
    err = nvs_get_u16(my_handle, "head_iterator", &head_iterator);
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        ESP_LOGI(NVS_TAG, "Head iterator: [%" PRIu16 "]", head_iterator);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGW(NVS_TAG, "The value head iterator is not initialized yet!");
        break;
    default:
        ESP_LOGE(NVS_TAG, "Error (%s) reading head!", esp_err_to_name(err));
        return err;
    }
    tail_iterator = 0;
    err = nvs_get_u16(my_handle, "tail_iterator", &tail_iterator);
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        ESP_LOGI(NVS_TAG, "Tail iterator: [%" PRIu16 "]", tail_iterator);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGW(NVS_TAG, "The value tail iterator is not initialized yet!");
        break;
    default:
        ESP_LOGE(NVS_TAG, "Error (%s) reading tail!", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

esp_err_t nvs_store_commit(nvs_handle_t handle, NVS_OPERATION operation)
{
    esp_err_t err;
    if (operation == eNVS_WRITE)
    {
        err = nvs_set_u16(handle, "head_iterator", head_iterator);
        if (err != ESP_OK)
        {
            ESP_LOGE(NVS_TAG, "Failed to set head iterator");
            if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE)
            {
                ESP_LOGE(NVS_TAG, "Not enough space in NVS");
            }
            return err;
        }
        else 
        {
            uint16_t size = (head_iterator - tail_iterator + MAX_STORAGE_SIZE) % MAX_STORAGE_SIZE;
            ESP_LOGI(NVS_TAG, "Data size: %" PRIu16, size);
            ESP_LOGI(NVS_TAG, "Head iterator: [%" PRIu16 "]", head_iterator);
            ESP_LOGI(NVS_TAG, "Tail iterator: [%" PRIu16 "]", tail_iterator);
        }
    }
    else 
    {
        err = nvs_set_u16(handle, "tail_iterator", tail_iterator);
        if (err != ESP_OK)
        {
            ESP_LOGE(NVS_TAG, "Failed to set tail iterator");
            if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE)
            {
                ESP_LOGE(NVS_TAG, "Not enough space in NVS");
            }
            return err;
        }
        else 
        {
            uint16_t size = (head_iterator - tail_iterator + MAX_STORAGE_SIZE) % MAX_STORAGE_SIZE;
            ESP_LOGI(NVS_TAG, "Data size: %" PRIu16, size);
            ESP_LOGI(NVS_TAG, "Head iterator: [%" PRIu16 "]", head_iterator);
            ESP_LOGI(NVS_TAG, "Tail iterator: [%" PRIu16 "]", tail_iterator);
        }
    }

    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to commit NVS changes");
        return err;
    }
    return ESP_OK;
}

esp_err_t nvs_store(DataStore data)
{
    nvs_handle my_handle;
    esp_err_t err;
    err = nvs_open_from_partition("nvs_data", STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to open NVS handle");
        return err;
    }
    uint64_t stored_data = data.Frame;
    int next_head = (head_iterator + 1) % MAX_STORAGE_SIZE;
    if (next_head == tail_iterator)
    {
        ESP_LOGW(NVS_TAG, "NVS storage is full, ready to overwrite...");
        DataStore erase_data;
        nvs_load(&erase_data);
    }
    char key[MAX_KEY_LENGTH];
    snprintf(key, sizeof(key), "key: %" PRIu16 "", head_iterator);
    err = nvs_set_u64(my_handle, key, stored_data);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to store data");
        if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE)
        {
            ESP_LOGE(NVS_TAG, "Not enough space in NVS");
        }
        return err;
    }
    else
    {
        ESP_LOGI(NVS_TAG, "Data stored successfully index[%" PRIu16 "]: %" PRIu64 "", head_iterator, data.Frame);
    }
    head_iterator = next_head;
    nvs_store_commit(my_handle, eNVS_WRITE);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to commit NVS changes");
        return err;
    }
    nvs_close(my_handle);
    return ESP_OK;
}

NVS_STATE nvs_load(DataStore *data)
{
    nvs_handle my_handle;
    esp_err_t err;
    char key[MAX_KEY_LENGTH];
    err = nvs_open_from_partition("nvs_data", STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to open NVS handle");
        return eNVS_ERROR;
    }

    if (head_iterator == tail_iterator)
    {
        nvs_close(my_handle);
        ESP_LOGW(NVS_TAG, "NVS storage is empty");
        return eNVS_EMPTY;
    }
    
    uint64_t next_head = (tail_iterator + 1) % MAX_STORAGE_SIZE;
    snprintf(key, sizeof(key), "key: %" PRIu16 "", tail_iterator);
    err = nvs_get_u64(my_handle, key, &data->Frame);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE(NVS_TAG, "Failed to read data");
        return eNVS_ERROR;
    }
    else
    {
        ESP_LOGI(NVS_TAG, "Data loaded successfully, data value %" PRIu16 " = %" PRIu64 "", tail_iterator, data->Frame);
        nvs_erase_key(my_handle, key);
    }

    tail_iterator = next_head;
    nvs_store_commit(my_handle, eNVS_READ);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to commit NVS changes");
        return eNVS_ERROR;
    }
    nvs_close(my_handle);
    return eNVS_READY;
}

DataStore nvs_transfer_data_to_storage(SensorData data)
{
    DataStore stored_data;
    data.PH_Value.ph_value = (uint8_t)(data.PH_Value.ph_value * 10);
    data.ORP_Value.orp_value = (uint8_t)(data.ORP_Value.orp_value * 10); 
    data.EC_Value.ec_value = (uint8_t)(data.EC_Value.ec_value * 10);
    data.TUR_Value.tur_value = (uint8_t)(data.TUR_Value.tur_value * 10);

    stored_data.Data[0] = data.PH_Value.ph_value;
    stored_data.Data[1] = data.ORP_Value.orp_value;
    stored_data.Data[2] = data.EC_Value.ec_value;
    stored_data.Data[3] = data.TUR_Value.tur_value;

    return stored_data;
}

SensorData nvs_transfer_storage_to_data(DataStore stored_data)
{
    SensorData data;
    data.PH_Value.ph_value = (float)(stored_data.Data[0] / 10.0);
    data.ORP_Value.orp_value = (float)(stored_data.Data[1] / 10.0);
    data.EC_Value.ec_value = (float)(stored_data.Data[2] / 10.0);
    data.TUR_Value.tur_value = (float)(stored_data.Data[3] / 10.0);

    ESP_LOGI(NVS_TAG, "Stored_data PH: %.2f, ORP: %.2f, EC: %.2f, TUR: %.2f", data.PH_Value.ph_value, data.ORP_Value.orp_value, data.EC_Value.ec_value, data.TUR_Value.tur_value);

    return data;
}

esp_err_t nvs_store_again(DataStore data)
{
    nvs_handle my_handle;
    esp_err_t err;
    err = nvs_open_from_partition("nvs_data", STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to open NVS handle");
        return err;
    }
    uint64_t stored_data = data.Frame;
    char key[MAX_KEY_LENGTH];
    tail_iterator--;
    snprintf(key, sizeof(key), "key: %" PRIu16 "", tail_iterator);
    err = nvs_set_u64(my_handle, key, stored_data);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to store data");
        if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE)
        {
            ESP_LOGE(NVS_TAG, "Not enough space in NVS");
        }
        return err;
    }
    else
    {
        ESP_LOGI(NVS_TAG, "Data stored successfully index[%" PRIu16 "]: %" PRIu64 "", tail_iterator, data.Frame);
    }

    nvs_store_commit(my_handle, eNVS_WRITE);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Failed to commit NVS changes");
        return err;
    }
    nvs_close(my_handle);
    return ESP_OK;
}

uint16_t nvs_get_tail(void)
{
    return tail_iterator;
}