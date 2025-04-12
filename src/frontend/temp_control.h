#ifndef TEMP_CONTROL_H
#define TEMP_CONTROL_H
#include "device_config.h"
#define TEMP_HARD_CAP_MAX 80 //Hard cap (can't increment higher than this)
#define TEMP_HARD_CAP_MIN 60 //Hard cap (can't decrease lower than this)

/*
Purpose: Glorified Max temp+Min temp update (taken from the web app page). User increments the temperature, max_temp = desired_temperature+1, min_temp = desired_temperature-1
*/

#endif