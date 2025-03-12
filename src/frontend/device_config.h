#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H
#include <Arduino.h>
#include "frontend.h"

extern boolean eco_mode;
extern unsigned int max_temp;
extern unsigned int min_temp; 
extern unsigned int max_humid;
extern unsigned int min_humid;

void handleDeviceModeRequest();

#endif