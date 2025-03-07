#ifndef BACKEND_H
#define BACKEND_H
#include <WiFi.h>
#include <HTTPClient.h>
#include "device-setup.h" 
#define SENSOR_SEND_DELAY 1000
#define HEARTBEAT_SEND_DELAY 30000 
boolean connect_backend();

void send_ip(void * pvParameters); //send IP to backend server, FreeRTOS task
void send_sensor_data(void * pvParameters);
void send_pir_data();
void send_heartbeat(void * pvParameters);
boolean wifi_connect();
extern String serverURI;
extern TaskHandle_t ip_send_task;
extern TaskHandle_t send_sensor_data_task;
#endif