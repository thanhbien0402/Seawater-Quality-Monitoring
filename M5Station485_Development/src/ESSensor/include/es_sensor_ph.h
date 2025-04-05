#ifndef ES_SENSOR_PH_H
#define ES_SENSOR_PH_H

#include "es_sensor.h"

typedef enum
{
    PH_VALUE = 0,
    TEMP_VALUE = 1
} ph_value;

void ph_get_data(ph_value select_value, ph_sensor *sensor_data);
SENSOR_STATE ph_read_measurement(uart_port_t uart_num);

#endif // ES_SENSOR_PH_H