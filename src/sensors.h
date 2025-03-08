#ifndef SENSORS_H
#define SENSORS_H
#define PIR_INPUT 14
#define TEMP_INPUT 1
#define HUMID_INPUT 2
#define SERVO_INPUT 4

#define SENSOR_DELAY 1000 //Time between each sensor is read

#include <Arduino.h>

extern boolean pir;
extern TaskHandle_t sensor_read_task;
void config_pir();
void config_temp();
void config_humid();
void config_servo();
boolean get_pir();
unsigned int get_temp();
void config_sensors();
void read_sensors(void * pvParameters);

#endif