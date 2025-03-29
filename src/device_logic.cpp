#include "device_logic.h"



void device_logic(void * pvParameters){  //TODO: 
    /*
        Implemented Algorithm from Senior Design Software Decision Tree
    */

    read_variable_state();
    while(1){
        if(!sensors_configured) return;
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
                    Serial.printf("Opening Vent, humidity out of range %f...",hum);
                    servo_state = OPEN;
                }
            }
        }

        vTaskDelay(DEVICE_LOGIC_DELAY);

    }
}
    
void read_variable_state(){
        file.begin("device_state");
        max_temp = file.getInt("max_temp",max_temp);
        min_temp = file.getInt("min_temp",min_temp);
        max_humid = file.getInt("max_humid",max_humid);
        min_humid = file.getInt("min_humid",min_humid);
        eco_mode = file.getBool("deviceMode",eco_mode);
        Serial.println("Reading device state");
        Serial.printf("\nMAX TEMP: %d MIN TEMP: %d MAX_HUMID %d MIN_HUMID %d DEVICE MODE %d\n", max_temp,min_temp,max_humid,min_humid,eco_mode);
        file.end();
        return;
}