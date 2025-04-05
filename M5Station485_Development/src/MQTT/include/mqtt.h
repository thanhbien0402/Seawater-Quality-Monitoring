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
extern TaskHandle_t publisherTaskHandle;
extern EventGroupHandle_t mqtt_event_group;

void initEventGroup();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttTask(void* parameter);
void publisherTask(void* parameter);
void checkPumpStatusTask(void* parameter);
void controlTask(void *arg);
std::string extractParamValue(const std::string& message, const std::string& key);

#endif // MQTT_H