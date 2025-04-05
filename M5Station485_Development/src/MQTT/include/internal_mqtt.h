#ifndef INTERNAL_MQTT_H
#define INTERNAL_MQTT_H

#include "freertos/event_groups.h"

#define MQTT_TASK_DONE_BIT              (1 << 1)
#define PUMP_STATUS_CHANGED_BIT         (1 << 0)

#endif // INTERNAL_MQTT_H