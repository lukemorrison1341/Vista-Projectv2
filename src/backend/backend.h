#ifndef BACKEND_H
#define BACKEND_H
#include <WiFi.h>
#include <HTTPClient.h>
#include "device-setup.h" 
#define SENSOR_SEND_DELAY 1000
#define HEARTBEAT_SEND_DELAY 30000 

/*
    PURPOSE: Send Data to Backend no matter what. In case of backend relay, all the data needed is there. 
*/

boolean connect_backend();

void send_ip(void * pvParameters); //send IP to backend server, FreeRTOS task
void send_sensor_data(void * pvParameters);
void send_pir_data();
void send_heartbeat(void * pvParameters);
void backend_send_task(void * pvParameters);
boolean wifi_connect();
extern const String serverURI;
extern TaskHandle_t ip_send_task;
extern TaskHandle_t send_backend_task;




#endif