#include "sensors.h"

boolean pir = false;
float temp = 0.0f;
float hum = 0.0f;
boolean sensors_configured = false;
TwoWire TempWire = TwoWire(1);
TwoWire HumWire = TwoWire(0);
boolean sensors_read = false;
signed int pir_sensitivity = 0;
int adcRaw = 0;
float voltage = 0.0;

boolean get_pir(){

    
    adcRaw = analogRead(PIR_PIN);
    voltage = (adcRaw / 4095.0) * VREF;

    Serial.print("ADC Raw: ");
    Serial.print(adcRaw);
    Serial.print(" | Voltage: ");
    Serial.println(voltage, 3);
    if(voltage > MOTION_VOLTAGE_MAX || voltage < MOTION_VOLTAGE_MIN){
      Serial.println("Motion detected");
      pir_sensitivity = pir_sensitivity < 3 ? pir_sensitivity++ : pir_sensitivity;
     
      
  }
  else{
      Serial.println("No motion");
      pir_sensitivity = pir_sensitivity > -3 ? pir_sensitivity-- : pir_sensitivity;
  }
  if(pir_sensitivity > -1){
    return false;
  }
  return true;
}
void config_pir(){
    //pinMode(PIR_INPUT,INPUT);
    Serial.println("PIR Sensor configured");

     analogReadResolution(12);
}

void config_temp(){
  pinMode(TEMP_SDA, INPUT);
  pinMode(TEMP_SCL, INPUT);
  TempWire.begin(TEMP_SDA, TEMP_SCL, 100000);  
  Serial.println("Temperature sensor initialized.");
}

float read_humid(){

    HumWire.beginTransmission(HIH8120_ADDR);
    HumWire.write(0x00);
    HumWire.endTransmission();
    vTaskDelay(50); //Says to do this
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
      return 50.0f;
    }

}

float read_temp(){
    // Start a write to specify which register we want
    
    TempWire.beginTransmission(MLX90614_ADDR);
    TempWire.write(0x07);               // 0x07 = object temperature #1
    TempWire.endTransmission(false);    // use repeated start
    vTaskDelay(5000);
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
      return (1.80f*tempC)+32.0f; //Farenheit conversion
    } else {  
      Serial.println("MLX90614 => No data available");
      return 77.0f;
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
    sensors_configured=true;
}

void read_sensors(void * pvParameters){
    while(1){
        if(!sensors_configured) return;
        Serial.println("Reading Sensors");
        pir = get_pir();
       // pir_times_read_high += pir_value;
        //pir_times_read_low += pir_value == 0 ? 1 : 0;
        /*if(pir_times_read_high > PIR_SENSISITIVITY_COUNT){
          Serial.println("PIR sensor read enough times for motion to be detected.");
          pir_times_read_high = 0;
          pir=1; //Send motion detected signal
        }
        else if (pir_times_read_low > PIR_SENSISITIVITY_COUNT){
          Serial.println("PIR sensor read no motion enough times for no motion to be detected.");
          pir_times_read_low=0;
          pir=0;
    
        }
        */
        Serial.printf("Reading PIR Sensor: %d\n",pir);
        temp = read_temp();
        hum = read_humid();
        sensors_read=true;
        vTaskDelay(SENSOR_DELAY); 
    }
}

