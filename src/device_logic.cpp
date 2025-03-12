#include "device_logic.h"

void servo_handle(void * pvParameters){
    if(servo_state == OPEN){
        spin_clockwise();
        vTaskDelay(SERVO_RESPONSE_TIME);
        servo_state = DO_NOTHING;
    }
    else if (servo_state == CLOSE){
        spin_counterclockwise();
        vTaskDelay(SERVO_RESPONSE_TIME);
        servo_state = DO_NOTHING;
    }
    else if(servo_state == DO_NOTHING){
        stop_spin();
        vTaskDelay(SERVO_RESPONSE_TIME);
    }
    else{
        Serial.println("Uninitialized Servo State Error");
        vTaskDelay(SERVO_RESPONSE_TIME);
    }
   
}

void device_logic(void * pvParameters){
     if(!pir){
        if(eco_mode){
            if(max_temp < temp){
                Serial.println("Opening Vent, temperature too hot... ");
                servo_state = OPEN;
            }
        }
        else{

        }

    }
    else{
        Serial.println("Motion Detected, not doing anything.");
    }
    

}