#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
// #include <string>

// Client objects
extern WiFiClient espClient;
extern PubSubClient client;

// Task handles
extern TaskHandle_t mqttTaskHandle;
extern EventGroupHandle_t mqtt_event_group;

typedef struct {
    char cmd[200];
    QueueHandle_t QueueTaskHandler;
} TaskHandler;

typedef struct {
    QueueHandle_t ValueQueue;
    // EventGroupHandle_t ControlManager;    
} ControlDef;

typedef struct {
    const char *prefix;
    QueueHandle_t *queue;
} queue_map_t;

void initEventGroup();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttTask(void* parameter);
void publisherTask(void* parameter);
void checkPumpStatusTask(void* parameter);
void publishExecutedCronTask(void* parameter);
void controlTask(void *parameter);
void add_subscribe_task(const char *command_name, QueueHandle_t recv_queue);
void processCommand(const char * command, ControlDef* control_def);
void handle_mqtt_command(const char *json_str, const char *requestID);

#endif // MQTT_H