
#ifndef SERVO_H
#define SERVO_H
#include <Arduino.h>
#define SERVO_PIN       27        // PWM output pin
#define SERVO_CHANNEL   0         // LEDC PWM channel (0-15 available)
#define SERVO_FREQ      50        // Frequency in Hz (50 Hz -> 20 ms period)
#define SERVO_RES       16        // PWM resolution in bits

#define NO_SPIN (uint32_t)((1.5 / 20.0) * ((1 << SERVO_RES) - 1))
#define SLOW_CLOCKWISE_SPIN (uint32_t)((1.6 / 20.0) * ((1 << SERVO_RES) - 1))
#define SLOW_COUNTERCLOCKWISE_SPIN (uint32_t)((1.3 / 20.0) * ((1 << SERVO_RES) - 1))

#define SERVO_RESPONSE_TIME 1000

enum SERVO_STATE{
    OPEN,
    CLOSE,
    DO_NOTHING
};

void stop_spin(); //Stop the servo from spinning.
void spin_clockwise();
void spin_counterclockwise();

extern enum SERVO_STATE servo_state;



#endif