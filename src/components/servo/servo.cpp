#include "servo.h"
enum SERVO_STATE servo_state = CLOSE;

void stop_spin(){
  Serial.println("Stopping Servo...");
  ledcWrite(SERVO_CHANNEL,0);

}
void spin_clockwise(){
  Serial.println("Spinning Servo Clockwise...");
  ledcWrite(SERVO_CHANNEL,SLOW_CLOCKWISE_SPIN);
}
void spin_counterclockwise(){
  Serial.println("Spinning Servo Counterclockwise...");
  ledcWrite(SERVO_CHANNEL,SLOW_COUNTERCLOCKWISE_SPIN);
  
}
