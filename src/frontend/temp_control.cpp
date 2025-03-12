#include "temp_control.h"

void increment_temp(){
    max_temp = max_temp > TEMP_HARD_CAP_MAX ? max_temp : max_temp+1;
    min_temp = min_temp < TEMP_HARD_CAP_MIN ? min_temp : min_temp+1;
}
void decreaset_temp(){
    max_temp = max_temp > TEMP_HARD_CAP_MAX ? max_temp : max_temp-1;
    min_temp = min_temp < TEMP_HARD_CAP_MIN ? min_temp : min_temp-1;
}