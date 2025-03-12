#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H
#include <Arduino.h>
#include "frontend.h"

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

#endif