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
#define ADC_PIN 34 // GPIO 34
#define LOW_THRESH 1000 // This value will need to be changed to get it right, but it should be fairly low
#define HIGH_THRESH 4050 // Same with this value, it should be close, but just under the 3.3/5V ADC value. 

// I2C addresses temperature, humidity
#define HIH8120_ADDR 0x27
#define MLX90614_ADDR 0x5A

#define SENSOR_DELAY 4070 //Time between each sensor is read

#include <Arduino.h>
#include <Wire.h>

extern boolean pir;
extern boolean pir_adc_calc; // returns high when motion/occupancy detected.... i think
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