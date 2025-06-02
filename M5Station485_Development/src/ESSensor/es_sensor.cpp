#include "./include/es_sensor.h"
#include "./Global/include/global.h"

static const char *TAG = "SENSOR";

// HardwareSerial RS485Serial(UART_NUM);
QueueHandle_t uart_sensor_queue;
uart_event_t uart_sensor_event;
// SensorValue sensor_value;
// CURRENT_SENSOR current_sensor;

SemaphoreHandle_t rs485_mutex = NULL;

void initRS485(uart_port_t uart_num, int baudrate, int rxPin, int txPin) {
    rs485_mutex = xSemaphoreCreateMutex();

    uart_config_t uart_config =
    {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(uart_num, 1024 * 2, 1024 * 2, 20, &uart_sensor_queue, 0);
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX);
    uint8_t clear_frame[9];
    uart_read_bytes(uart_num, clear_frame, sizeof(clear_frame), 100 / portTICK_RATE_MS);
    uart_flush(uart_num);
}

// uint16_t calculate_crc16(uint8_t *data, size_t length) {
//     uint16_t crc = 0xFFFF;
//     for (size_t i = 0; i < length; i++) {
//         crc ^= data[i];
//         for (int j = 0; j < 8; j++) {
//             if (crc & 1) {
//                 crc = (crc >> 1) ^ 0xA001;
//             } else {
//                 crc >>= 1;
//             }
//         }
//     }
//     return crc;
// }

// Function to convert hex to float
float convertHexToFloat(uint32_t hexValue) {
    // Check for system endianness (little-endian vs big-endian)
    uint32_t test = 1;
    char *testByte = (char *)&test;

    // If the system is little-endian, swap the bytes of the hex value
    if (*testByte != 1) {
        hexValue = ((hexValue >> 24) & 0xFF) |      // Move byte 3 to byte 0
                   ((hexValue << 8) & 0xFF0000) |   // Move byte 1 to byte 2
                   ((hexValue >> 8) & 0xFF00) |     // Move byte 2 to byte 1
                   ((hexValue << 24) & 0xFF000000); // Move byte 0 to byte 3
    }

    // A union to interpret the bits of the hex value as a float
    union {
        uint32_t hex;
        float f;
    } converter;

    // Assign the input hex value to the union
    converter.hex = hexValue;

    // Return the float value
    return converter.f;
}
 
float processData(uint8_t *data)
{
    uint32_t value = (data[5] << 24) | (data[6] << 16) | (data[3] << 8) | data[4];
    float result = convertHexToFloat(value);
    return result;
}

// static void check_current_device(uint8_t current_device)
// {
//     switch (current_device)
//     {
//     case TUR_SENSOR:
//         current_sensor = SENSOR_TUR;
//         break;
//     case PH_SENSOR:
//         current_sensor = SENSOR_PH;
//         break;
//     case EC_SENSOR:
//         current_sensor = SENSOR_EC;
//         break;
//     case ORP_SENSOR:
//         current_sensor = SENSOR_ORP;
//         break;
//     default:
//         current_sensor = NONE_DEVICE;
//         break;
//     }    
// }

// static SENSOR_STATE check_response_frame(uart_port_t uart_num, uint8_t *tx_frame)
// {
//     uint8_t device_id = tx_frame[0];
//     uint8_t function_code = tx_frame[1];
//     uint8_t qty_register = tx_frame[2];
//     uint8_t response_frame[13];
//     int length_rx = 0;
//     uint16_t crc = 0;
//     SENSOR_STATE check_sensor_state = SENSOR_FAIL;

//     TickType_t start_time = xTaskGetTickCount();
//     TickType_t end_time = pdMS_TO_TICKS(RXD_TIMEOUT_MS);
//     while (xTaskGetTickCount() - start_time < end_time)
//     {
//         if (xQueueReceive(uart_sensor_queue, (void *)&uart_sensor_event, (TickType_t)10))
//         {
//             bzero(response_frame, sizeof(response_frame));
//             if (uart_sensor_event.type == UART_DATA)
//             {
//                 length_rx = uart_read_bytes(uart_num, (uint8_t *)response_frame, uart_sensor_event.size, 50 / portTICK_RATE_MS);
//                 break;
//             }
//         }
//     }

//     ESP_LOGI(TAG, "Length of RX frame from sensor: ", "%d", length_rx);

//     if (current_sensor == NONE_DEVICE)
//     {
//         ESP_LOGE(TAG, "No device found!!!");
//         return SENSOR_FAIL;
//     }

//     if (length_rx < 7) {
//         ESP_LOGE(TAG, "Error reading data from sensor!!!");
//         return SENSOR_FAIL;
//     }

//     ESP_LOGI(TAG, "Received data: ");
//     for (size_t i = 0; i < length_rx; i++)
//     {
//         if (response_frame[i] < 0x10)
//         {
//             Serial.print("0");
//         }
//         Serial.print(response_frame[i], HEX);
//         Serial.print(" ");
//     }
//     Serial.println();

//     uint8_t device_id_rx = response_frame[0];
//     uint8_t function_code_rx = response_frame[1];

//     if ((current_sensor == SENSOR_TUR && device_id_rx == TUR_SENSOR && device_id == TUR_SENSOR) ||
//         (current_sensor == SENSOR_PH && device_id_rx == PH_SENSOR && device_id == PH_SENSOR) ||
//         (current_sensor == SENSOR_EC && device_id_rx == EC_SENSOR && device_id == EC_SENSOR) ||
//         (current_sensor == SENSOR_ORP && device_id_rx == ORP_SENSOR && device_id == ORP_SENSOR))
//     {
//         crc = calculate_crc16(response_frame, uart_sensor_event.size - 2);
//         if ((response_frame[uart_sensor_event.size - 2] == (crc & 0xFF)) && (response_frame[uart_sensor_event.size - 1] == ((crc >> 8) & 0xFF)))
//         {
//             check_sensor_state = SENSOR_OK;
//             ESP_LOGI(TAG, "CRC check success!!!");
//             switch (current_sensor)
//             {
//             case SENSOR_TUR:          
//                 sensor_value.TUR_Value.tur_value = processData(response_frame);
//                 ESP_LOGI(TAG, "TUR value: %.2f NTU", sensor_value.TUR_Value.tur_value);
//                 // Serial.print("TUR value: "); // NTU
//                 // Serial.println(sensor_value.TUR_Value.tur_value, 2);
//                 break;

//             case SENSOR_PH:
//                 sensor_value.PH_Value.ph_value = processData(response_frame);
//                 ESP_LOGI(TAG, "PH value: %.2f", sensor_value.PH_Value.ph_value);
//                 // Serial.print("PH value: "); // pH
//                 // Serial.println(sensor_value.PH_Value.ph_value, 2);
//                 break;

//             case SENSOR_EC:
//                 sensor_value.EC_Value.ec_value = processData(response_frame);
//                 ESP_LOGI(TAG, "EC value: %.2f μS/cm", sensor_value.EC_Value.ec_value);
//                 // Serial.print("EC value: "); // μS/cm
//                 // Serial.println(sensor_value.EC_Value.ec_value, 2);
//                 break;

//             case SENSOR_ORP:
//                 sensor_value.ORP_Value.orp_value = processData(response_frame);
//                 ESP_LOGI(TAG, "ORP value: %.2f mV", sensor_value.ORP_Value.orp_value);
//                 // Serial.print("ORP value: "); // mV
//                 // Serial.println(sensor_value.ORP_Value.orp_value, 2);
//                 break;
            
//             default:
//                 break;
//             }
//         }
//         else
//         {
//             ESP_LOGI(TAG, "CRC check failed!!!");
//             return SENSOR_FAIL;
//         }
//     }

//     return check_sensor_state;
// }



// SENSOR_STATE readData(uart_port_t uart_num, uint8_t device_id, uint16_t register_address, uint16_t data)
// {
//     SENSOR_STATE check_sensor_state = SENSOR_FAIL;
//     uint8_t tx_frame[8];
//     tx_frame[0] = device_id;
//     tx_frame[1] = FUNCTION_CODE_READ;
//     tx_frame[2] = (register_address >> 8) & 0xFF; // register address high byte
//     tx_frame[3] = register_address & 0xFF;       // register address low byte
//     tx_frame[4] = (data >> 8) & 0xFF;         // qty of register high byte
//     tx_frame[5] = data & 0xFF;    // qty of register low byte
    
//     // Calculate CRC16
//     uint16_t crc = calculate_crc16(tx_frame, 6);
//     tx_frame[6] = crc & 0xFF; // crc low byte
//     tx_frame[7] = (crc >> 8) & 0xFF; // crc high byte

//     RS485Serial.write(tx_frame, sizeof(tx_frame));
//     for (uint8_t i = 0; i < sizeof(tx_frame); i++)
//     {
//         Serial.print(tx_frame[i], HEX);
//         Serial.print(" ");
//     }
//     Serial.println();

//     uart_flush(uart_num);
//     xQueueReset(uart_sensor_queue);
//     uart_write_bytes(uart_num, tx_frame, sizeof(tx_frame));
//     check_sensor_state = check_response_frame(uart_num, tx_frame);

//     return check_sensor_state;
// }

uint8_t data_tur[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB};
uint8_t data_ph[8] = {0x02, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xF8};
uint8_t data_ec[8] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x5E};
uint8_t data_orp[8] = {0x05, 0x03, 0x00, 0x01, 0x00, 0x02, 0x94, 0x4F};

float read_sensor_data(const uint8_t* command, size_t command_length, const char* sensor_name) {
    if (xSemaphoreTake(rs485_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take mutex for reading sensor data");
        return -999.0; // Return an invalid value
    }
    uint8_t response_frame[9];
    const int max_retries = 3;
    float sensor_value = -999.0; // Default invalid value

    for (int attempt = 1; attempt <= max_retries; ++attempt) {
        uart_write_bytes(UART_NUM_2, command, command_length);
        int length_rx = uart_read_bytes(UART_NUM_2, response_frame, sizeof(response_frame), 70 / portTICK_RATE_MS);

        if (length_rx > 0) {
            ESP_LOGI(TAG, "Received value for %s on attempt %d:", sensor_name, attempt);
            ESP_LOG_BUFFER_HEX(TAG, response_frame, length_rx);

            sensor_value = processData(response_frame);
            ESP_LOGI(TAG, "%s value: %.2f", sensor_name, sensor_value);
            xSemaphoreGive(rs485_mutex); // Release the mutex
            return sensor_value; // Return the value if successfully read
        }
        uart_flush(UART_NUM_2); // Clear the UART buffer for the next attempt
        ESP_LOGW(TAG, "No response for %s, retrying %d/%d...", sensor_name, attempt, max_retries);
        vTaskDelay(250 / portTICK_RATE_MS);
    }
    uart_flush(UART_NUM_2); // Clear the UART buffer after all attempts
    xSemaphoreGive(rs485_mutex); // Release the mutex if all retries fail
    ESP_LOGE(TAG, "Failed to read %s after %d attempts", sensor_name, max_retries);
    return sensor_value; // Return the default invalid value if all retries fail
}

void turnPumpRelay(bool state) {
    if (xSemaphoreTake(rs485_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take mutex for relay control");
        return;
    }

    uint8_t command[8];
    uint8_t response_frame[8];
    const int MAX_RETRIES = 3;

    // Prepare the command based on the desired state (ON/OFF)
    if (state) {
        // Command to turn on relay No. 1 (manual mode)
        uint8_t on_command[] = {0xFF, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x99, 0xE4};
        memcpy(command, on_command, sizeof(command));
    } else {
        uint8_t off_command[] = {0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x14};
        memcpy(command, off_command, sizeof(command));
        // memcpy(command, (uint8_t[]){0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x14}, sizeof(command));
    }

    for (int retry = 0; retry < MAX_RETRIES; retry++) {
        uart_write_bytes(UART_NUM_2, command, sizeof(command));
        int length_rx = uart_read_bytes(UART_NUM_2, response_frame, sizeof(response_frame), 50 / portTICK_RATE_MS);

        ESP_LOGI(TAG, "Sending command: ");
        for (int i = 0; i < sizeof(command); i++) {
            printf("%02X ", command[i]);
        }
        printf("\n");

        if (length_rx > 0) {
            ESP_LOGI(TAG, "Received response: ");
            for (int i = 0; i < length_rx; i++) {
                printf("%02X ", response_frame[i]);
            }
            printf("\n");

            // Compare the response with the sent command
            if (memcmp(command, response_frame, sizeof(command)) == 0) {
                ESP_LOGI(TAG, "Relay turned %s successfully", state ? "ON" : "OFF");
                xSemaphoreGive(rs485_mutex); // Release the mutex
                return;
            } else {
                ESP_LOGW(TAG, "Response does not match the command, retrying...");
            }
        } else {
            ESP_LOGW(TAG, "No response received, retrying...");
        }

        vTaskDelay(250 / portTICK_RATE_MS); // Delay between retries
    }
    xSemaphoreGive(rs485_mutex); // Release the mutex if all retries fail
    ESP_LOGE(TAG, "Failed to turn relay %s after %d attempts.", state ? "ON" : "OFF", MAX_RETRIES);
}

bool readPumpStatus() {
    if (xSemaphoreTake(rs485_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take mutex for pump status check");
        return false;
    }
    const uint8_t command[] = {0xFF, 0x01, 0x00, 0x00, 0x00, 0x08, 0x28, 0x12};
    uint8_t response_frame[8];
    const int MAX_RETRIES = 3;

    for (int retry = 1; retry <= MAX_RETRIES; retry++) {
        uart_write_bytes(UART_NUM_2, command, sizeof(command));
        int length_rx = uart_read_bytes(UART_NUM_2, response_frame, sizeof(response_frame), 70 / portTICK_RATE_MS);

        if (length_rx > 0) {
            ESP_LOGI(TAG, "Received response:");
            for (int i = 0; i < length_rx; i++) {
                printf("%02X ", response_frame[i]);
            }
            printf("\n");

            if (response_frame[3] == 0x00) {
                ESP_LOGI(TAG, "Pump relay is OFF");
                xSemaphoreGive(rs485_mutex); // Release the mutex
                return false;
            } else if (response_frame[3] == 0x01) {
                ESP_LOGI(TAG, "Pump relay is ON");
                xSemaphoreGive(rs485_mutex); // Release the mutex
                return true;
            } else {
                ESP_LOGW(TAG, "Unknown pump relay status");
            }
        } else {
            ESP_LOGW(TAG, "No response for pump relay status, retrying %d/%d", retry, MAX_RETRIES);
        }
        
        uart_flush(UART_NUM_2);
        vTaskDelay(250 / portTICK_RATE_MS);
    }

    xSemaphoreGive(rs485_mutex); // Release the mutex if all retries fail
    ESP_LOGE(TAG, "Failed to read pump relay status after %d attempts", MAX_RETRIES);
    return false;
}