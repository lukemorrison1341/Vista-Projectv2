#ifndef MAIN_H
#define MAIN_H
#define LED_PIN 2  // Built-in LED is usually on GPIO2
#include "device-setup.h"
#include "backend/backend.h"
#include "components/sensor/sensors.h"
#include "components/servo/servo.h"
#include "frontend/frontend.h"
#include "device_logic.h"
#include "frontend/device_config.h"
#include "esp_freertos_hooks.h"
#include "esp_heap_caps.h"


/*
    Purpose: Main function. Conditionally create FreeRTOS tasks
    Check if device has been configured. If not configured, start device configuration process. Also includes the handle server for the device configuration webpage.
    If already configured, create all necessary FreeRTOS Tasks. 
    Check that device can connect to backend. This is BLOCKING
    Configure the sensors
*/

extern TaskHandle_t sensor_read_task;
extern TaskHandle_t ip_send_task;
extern TaskHandle_t frontend_handle_task;
extern TaskHandle_t handle_config_server_task;
extern TaskHandle_t send_sensor_data_task;
extern TaskHandle_t heartbeat_task;
extern TaskHandle_t servo_handle_task;
extern TaskHandle_t device_logic_task;
extern TaskHandle_t send_backend_task;

void handle_server(void * pvParameters);
bool my_idle_hook_cb();


#endif