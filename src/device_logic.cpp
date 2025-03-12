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
    /*
        Implemented Algorithm from Senior Design Software Decision Tree
    */
        if(eco_mode){//Eco Mode
            if(!pir){ //No motion detected
                if(max_temp < temp){
                    Serial.println("Opening Vent, temperature too hot... ");
                    servo_state = OPEN;
                }
                else if(min_temp > temp){
                    Serial.println("Closing Vent, temperature too cold...");
                    servo_state = CLOSE;
                }
                else{
                    servo_state = DO_NOTHING;
                }
            }
            else{//Motion Detected
                Serial.println("Motion detected, not doing anything...");
                servo_state = DO_NOTHING;
            }
        }
        else{ //Vacant mode
            if(max_temp < temp){
                    servo_state = OPEN;
                    Serial.println("Opening Vent, temperature too hot... ");
            }
            else if(min_temp > temp){
                    servo_state = CLOSE;
                    Serial.println("Closing Vent, temperature too cold...");
            }
            else{ //Temperature in range
                if((min_humid < hum) && (hum < max_humid) ){
                    Serial.println("Closing Vent, humidity in range... ");
                    servo_state = CLOSE;
                }
                else{
                    Serial.println("Opening Vent, humidity out of range...");
                    servo_state = OPEN;
                }
            }
        }

    }
    
