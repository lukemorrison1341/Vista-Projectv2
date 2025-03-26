#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H
#include <Arduino.h>
#include "frontend.h"
#include <stdlib.h>
extern boolean eco_mode;
extern unsigned int max_temp;
extern unsigned int min_temp; 
extern unsigned int max_humid;
extern unsigned int min_humid;

extern boolean reset_device;

extern boolean device_changed;

/*
Purpose: Update variables stored from device configuration page in web application. Change the max/min temp, min/max humidity, change device mode, reset the dvice
*/

void handleDeviceModeRequest();
void initialize_device();
void updateDeviceConfig(int min_humid, int max_humid, int min_temp, int max_temp, String device_mode, String vent_status);

#endif