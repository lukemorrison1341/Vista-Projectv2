#ifndef MAIN_H
#define MAIN_H
#define LED_PIN 2  // Built-in LED is usually on GPIO2
#include "device-setup.h"
#include "backend/backend.h"
#include "components/sensor/sensors.h"
#include "components/servo/servo.h"
#include "frontend/frontend.h"
#include "device_logic.h"


/*
    Purpose: Main function. Conditionally create FreeRTOS tasks
    Check if device has been configured. If not configured, start device configuration process. Also includes the handle server for the device configuration webpage.
    If already configured, create all necessary FreeRTOS Tasks. 
    Check that device can connect to backend. This is BLOCKING
    Configure the sensors
*/

TaskHandle_t sensor_read_task = NULL;
TaskHandle_t ip_send_task = NULL;
TaskHandle_t frontend_handle_task = NULL;
TaskHandle_t handle_config_server_task = NULL;
TaskHandle_t send_sensor_data_task = NULL;
TaskHandle_t heartbeat_task = NULL;
TaskHandle_t servo_handle_task = NULL;
TaskHandle_t device_logic_task = NULL;
TaskHandle_t send_backend_task = NULL;

void handle_server(void * pvParameters);


#endif