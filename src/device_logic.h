#ifndef DEVICE_LOGIC_H
#define DEVICE_LOGIC_H
/*
    Purpose: With information about device mode, current temperature, min/max temperature, min/max humidity, determine when to open/close the vents. 
*/
#include <Arduino.h>
#include "frontend/device_config.h"
#include "components/servo/servo.h"
#define DEVICE_LOGIC_DELAY 10000
void device_logic(void * pvParameters);
void read_variable_state();
extern TaskHandle_t device_logic_task;

enum CONNECTION_TYPE {
    FRONTEND,
    BACKEND,
    BOTH
};
extern enum CONNECTION_TYPE connection_type;
extern boolean send_new_config;
#endif