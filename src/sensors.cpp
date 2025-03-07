#include "sensors.h"

boolean pir = false;

boolean get_pir(){
    return digitalRead(PIR_INPUT) == HIGH ? true : false;

}
void config_pir(){
    pinMode(PIR_INPUT,INPUT);
    Serial.println("PIR Sensor configured");
}
void config_sensors(){
    config_pir();
}

void read_sensors(void * pvParameters){
    while(1){
        Serial.println("Reading Sensors");
        pir = get_pir();
        Serial.printf("Reading PIR Sensor: %d\n",pir);
        Serial.println("Sensors Read");
        vTaskDelay(SENSOR_DELAY); 
    }
}

