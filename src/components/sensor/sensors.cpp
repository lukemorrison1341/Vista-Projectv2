#include "sensors.h"

boolean pir = false;
float temp = 0.0f;
float hum = 0.0f;
TwoWire TempWire = TwoWire(1);
TwoWire HumWire = TwoWire(0);

boolean get_pir(){
    return digitalRead(PIR_INPUT) == HIGH ? true : false;
}
void config_pir(){
    pinMode(PIR_INPUT,INPUT);
    Serial.println("PIR Sensor configured");
}

void config_temp(){
  pinMode(TEMP_SDA, INPUT);
  pinMode(TEMP_SCL, INPUT);
  TempWire.begin(TEMP_SDA, TEMP_SCL, 100000);  
  Serial.println("Temperature sensor initialized.");
}

float read_humid(){
    HumWire.requestFrom(HIH8120_ADDR, (uint8_t)4);
    if (HumWire.available() == 4) {
      uint8_t b1 = HumWire.read();
      uint8_t b2 = HumWire.read();
      uint8_t b3 = HumWire.read();
      uint8_t b4 = HumWire.read();

      // -- Extract humidity (lower 14 bits) --
      // Top 2 bits of b1 can be status, so mask them out:
      uint16_t rawHum = ((b1 & 0x3F) << 8) | b2;
      float humidity = (float(rawHum) * 100.0f) / 16384.0f; // per Honeywell datasheet
      Serial.print("HIH8120 => Humidity: ");
      Serial.print(humidity, 2);
      Serial.println("\n");
      return humidity;
    } else {
      Serial.println("HIH8120 => No data available");
      return 0.0f;
    }

}

float read_temp(){
    // Start a write to specify which register we want
    TempWire.beginTransmission(MLX90614_ADDR);
    TempWire.write(0x07);               // 0x07 = object temperature #1
    TempWire.endTransmission(false);    // use repeated start

    // Now read 3 bytes: LSB, MSB, and PEC (which we can ignore if not using)
    TempWire.requestFrom((uint8_t)MLX90614_ADDR, (uint8_t)3);
    if (TempWire.available() == 3) {
      uint8_t lsb = TempWire.read();
      uint8_t msb = TempWire.read();
      uint8_t pec = TempWire.read(); // Packet Error Code (not used here)

      // Combine LSB + MSB into 16-bit raw reading
      uint16_t rawIr = (msb << 8) | lsb;
      // Each bit counts as 0.02 Kelvin:  Temp(K) = raw * 0.02
      float tempK = (float)rawIr * 0.02;
      float tempC = tempK - 273.15;

      Serial.print("MLX90614 => Object Temp: ");
      Serial.print(tempC, 2);
      Serial.println(" °C");
      return tempC;
    } else {
        return 0.0f;
      Serial.println("MLX90614 => No data available");
    }
}

void config_humid(){
    pinMode(HUMID_SDA, INPUT);
  pinMode(HUMID_SCL, INPUT);
  HumWire.begin(HUMID_SDA, HUMID_SCL, 100000); 
  Serial.println("Humidity Sensor configured");

}

void config_sensors(){
    config_pir();
    config_temp();
    config_humid();
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

