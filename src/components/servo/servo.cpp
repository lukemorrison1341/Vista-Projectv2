#include "servo.h"
enum SERVO_STATE servo_state = OPEN;
enum VENT_STATE vent_state = VENT_OPEN;
boolean servo_configured = false;
void stop_spin(){
  //Serial.println("Stopping Servo...");
  ledcWrite(SERVO_CHANNEL,0);

}
void spin_clockwise(){
  Serial.println("Spinning Servo Clockwise...");
  ledcWrite(SERVO_CHANNEL,SLOW_CLOCKWISE_SPIN);
  vTaskDelay(SERVO_RESPONSE_TIME);
  Serial.println("Stopping spin...");
  stop_spin();

}
void spin_counterclockwise(){
  Serial.println("Spinning Servo Counterclockwise...");
  ledcWrite(SERVO_CHANNEL,SLOW_COUNTERCLOCKWISE_SPIN);
  vTaskDelay(SERVO_RESPONSE_TIME);
  Serial.println("Stopping spin...");
  stop_spin();
  
}

void config_servo(){
  Serial.println("Configuring Servo...");
  ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);
}


void servo_handle(){
    if(!servo_configured){
        config_servo();
        servo_configured=true;
    }
   
      vTaskDelay(SERVO_RESPONSE_TIME);
        if(servo_state == OPEN){

          if(vent_state == VENT_OPEN){
            //Serial.println("Vent is already open.");
            return;
          }
            spin_counterclockwise();
            vTaskDelay(SERVO_RESPONSE_TIME);
            servo_state = DO_NOTHING;
            Serial.println("Setting vent state to open");
            vent_state = VENT_OPEN;
        }
    else if (servo_state == CLOSE){
        if(vent_state == VENT_CLOSE){
          //Serial.println("Vent is already closed.");
          return;
        }
            spin_clockwise();
            vTaskDelay(SERVO_RESPONSE_TIME);
            servo_state = DO_NOTHING;
            Serial.println("Setting vent state to close");
            vent_state = VENT_CLOSE;
        }
    else if(servo_state == DO_NOTHING){
            stop_spin();
            //vTaskDelay(SERVO_RESPONSE_TIME);
        }
    else{
            Serial.println("Uninitialized Servo State Error");
            //vTaskDelay(SERVO_RESPONSE_TIME);
        }
    
   
}