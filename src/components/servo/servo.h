
#ifndef SERVO_H
#define SERVO_H
#include <Arduino.h>
#define SERVO_PIN       14        // PWM output pin
#define SERVO_CHANNEL   0         // LEDC PWM channel (0-15 available)
#define SERVO_FREQ      50        // Frequency in Hz (50 Hz -> 20 ms period)
#define SERVO_RES       16        // PWM resolution in bits

#define NO_SPIN (uint32_t)((1.5 / 20.0) * ((1 << SERVO_RES) - 1))
#define SLOW_COUNTERCLOCKWISE_SPIN (uint32_t)((1.53 / 20.0) * ((1 << SERVO_RES) - 1)) //counter is open
#define SLOW_CLOCKWISE_SPIN (uint32_t)((1.3 / 20.0) * ((1 << SERVO_RES) - 1)) //closed

#define SERVO_RESPONSE_TIME 10000
/*
    Purpose: Store the desired next state of the vent (open/close). Include Do nothing state to tell the servo to stop spinning.
    Includes all of the functions for configuring the servo, sending appropriate PWM signal based on servo_state
*/

enum SERVO_STATE{
    OPEN,
    CLOSE,
    DO_NOTHING
};

enum VENT_STATE{
    VENT_OPEN,
    VENT_CLOSE
};

void stop_spin(); //Stop the servo from spinning.
void spin_clockwise();
void spin_counterclockwise();
void config_servo();
void servo_handle(void * pvParameters);
extern boolean servo_configured;

extern enum SERVO_STATE servo_state;
extern enum VENT_STATE vent_state;

extern TaskHandle_t servo_handle_task;



#endif