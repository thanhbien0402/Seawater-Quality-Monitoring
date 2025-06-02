#ifndef ES_SENSOR_H
#define ES_SENSOR_H

#include "common_register.h"
#include <HardwareSerial.h>
#include "driver/uart.h"
#include <vector>

// extern HardwareSerial RS485Serial;
// Usage examples

typedef struct
{
    uint8_t device_id;
    float tur_value;
    float temp_value;
} tur_sensor;

typedef struct 
{
    uint8_t device_id;
    float ph_value;
    float temp_value;
} ph_sensor;

typedef struct
{
    uint8_t device_id;
    float ec_value;
    float temp_value;
} ec_sensor;

typedef struct
{
    uint8_t device_id;
    float orp_value;
    float temp_value;
} orp_sensor;

typedef struct
{
    tur_sensor TUR_Value;
    ph_sensor PH_Value;
    ec_sensor EC_Value;
    orp_sensor ORP_Value;
} SensorValue;

typedef struct
{
    tur_sensor TUR_Value;
    ph_sensor PH_Value;
    ec_sensor EC_Value;
    orp_sensor ORP_Value;
    uint32_t Timestamp;
} SensorData;

typedef enum
{
    NONE_DEVICE = 0,
    SENSOR_TUR  = 1,
    SENSOR_PH  = 2,
    SENSOR_EC  = 3,
    SENSOR_ORP  = 4
} CURRENT_SENSOR;

typedef enum
{
  SENSOR_FAIL = 0,
  SENSOR_OK   = 1
} SENSOR_STATE;

float convertHexToFloat(uint32_t hexValue);
float processData(uint8_t *data);
void initRS485(uart_port_t uart_num, int baudrate, int rxPin, int txPin);
SENSOR_STATE writeData(uint8_t device_id, uint16_t register_address, uint16_t data);
SENSOR_STATE readData(uint8_t device_id, uint16_t register_address, uint16_t data);
SENSOR_STATE es_uart_read_data(uart_port_t uart_num, uint8_t device_id, uint16_t register_address);


extern uint8_t data_tur[8];
extern uint8_t data_ph[8];
extern uint8_t data_ec[8];
extern uint8_t data_orp[8];

float read_sensor_data(const uint8_t* command, size_t command_length, const char* sensor_name);
// void read_data_ph();
// void read_data_orp();
// void read_data_ec();
// void read_data_tur();
void turnPumpRelay(bool state);
bool readPumpStatus();

#endif // ES_SENSOR_H