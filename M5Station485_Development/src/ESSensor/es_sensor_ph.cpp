#include "./include/es_sensor_ph.h"

extern SensorValue sensor_value;

void ph_get_data(ph_value select_value, ph_sensor *sensor_data)
{
    switch (select_value)
    {
    case PH_VALUE:
        sensor_data->ph_value = sensor_value.PH_Value.ph_value;
        break;

    case TEMP_VALUE:
        sensor_data->temp_value = sensor_value.PH_Value.temp_value;
        break;

    default:
        break;
    }
}

SENSOR_STATE ph_read_measurement(uart_port_t uart_num)
{
    if (es_uart_read_data(uart_num, PH_SENSOR, PH_READ_MEASUREMENT))
    {
        Serial.println("Read PH measurement successfully!!!");
        return SENSOR_OK;
    }
    else 
    {
        Serial.println("Read PH measurement failed!!!");
        return SENSOR_FAIL;
    }
}