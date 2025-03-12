/*
Purpose: Read the state of each sensor, act as a spot to hold the current values of each sensor for easy read and retrieval.
*/

#ifndef SENSORS_H
#define SENSORS_H
#define PIR_INPUT 14
#define TEMP_SDA 25
#define TEMP_SCL 26
#define HUMID_SDA 22
#define HUMID_SCL 21

// I2C addresses temperature, humidity
#define HIH8120_ADDR 0x27
#define MLX90614_ADDR 0x5A

#define SENSOR_DELAY 1000 //Time between each sensor is read

#include <Arduino.h>
#include <Wire.h>

extern boolean pir;
extern float temp;
extern float hum;
extern TaskHandle_t sensor_read_task;
extern TwoWire TempWire; 
extern TwoWire HumWire;
void config_pir();
void config_temp();
void config_humid();
void config_servo();
boolean get_pir();
float read_temp(); //returns in celsius
float read_humid();
unsigned int get_temp();
void config_sensors();
void read_sensors(void * pvParameters);

#endif