#include "device_logic.h"

boolean send_new_config = false;

void device_logic(void * pvParameters){  //TODO: 
    /*
        Implemented Algorithm from Senior Design Software Decision Tree
    */

    
    while(1){
        //Serial.println("DEVICE LOGIC TASK");
        //if(!sensors_configured) return;
        read_variable_state();
        
        if(force_state == FORCE_DEFAULT){ //Not enforcing open/close

            if(eco_mode){//Eco Mode
                if(!pir){ 
                    if(max_temp < temp){
                        Serial.printf("\nOpening Vent Max Temp %d < %f Temp, temperature too hot... \n",max_temp,temp);
                        servo_state = OPEN;

                    }
                    else if(min_temp > temp){
                        Serial.printf("\nClosing Vent Min Temp %d > %f Temp, temperature too cold...\n",min_temp,temp);
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
            } else { //Vacant mode
                if(max_temp < temp){
                        servo_state = OPEN;
                        Serial.printf("\nOpening Vent Max Temp %d < %f Temp, temperature too hot... \n",max_temp,temp);
                        
                }
                else if(min_temp > temp){
                        servo_state = CLOSE;
                        Serial.printf("\nClosing Vent Min Temp %d > %f Temp, temperature too cold...\n",min_temp,temp);
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
        } else {
                if(force_state == FORCE_OPEN && vent_state != VENT_OPEN){
                    servo_state = OPEN;
                    Serial.println("Forcing the Vent open");
                }
                else if (force_state == FORCE_CLOSE && vent_state != VENT_CLOSE){
                    servo_state = CLOSE;
                    Serial.println("Forcing the vent closed");
                }
            }

            //Serial.println("Servo Handle");
            servo_handle();

            vTaskDelay(DEVICE_LOGIC_DELAY);
        }     
    }
    
void read_variable_state(){
        file.begin("device_config");
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