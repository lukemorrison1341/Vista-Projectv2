#include <Arduino.h>
#include "main.h"
#include "esp_heap_caps.h"

void setup() {
  clear_configuration();
  pinMode(LED_PIN,OUTPUT);
  Serial.begin(115200);
  if (!LittleFS.begin()) {
    Serial.println("Failed to initialize LittleFS!");
    return;
  }
  Serial.println("File System initialized");
  
  if(!check_initialization()){ //uninitialized device configuration, start device configuration
    setup_server(); 
    xTaskCreate(handle_server, "Device Configuration Task", 8192, NULL, 1, NULL);
  }
  else{
    
    print_device_config();
    Serial.println("Attempting backend connection");
    
    while(!connect_backend()){ //BLOCKING

    }

      digitalWrite(LED_PIN,HIGH);
      //Startup Sequences
      config_sensors();
      xTaskCreate(send_ip, "Send IP to Backend", 16384, NULL, 1, &ip_send_task); //Sends IP to backend periodically
      xTaskCreate(read_sensors, "Sensor Read Task", 8192, NULL, 1, &sensor_read_task); //Read sensors periodically
      create_endpoints(); //Create the endpoints for the frontend-server
      xTaskCreate(handle_frontend_server, "Frontend Server",16384,NULL,1,&frontend_handle_task);
      xTaskCreate(send_sensor_data, "Send Sensor Data To Backend",16384,NULL,1,&send_sensor_data_task);
      xTaskCreate(send_heartbeat, "Send Heartbeat to Backend",16384,NULL,1,&heartbeat_task);
    
        
  }
     
  }



void handle_server(void * pvParameters){ //FreeRTOS task
  while(1){
      server.handleClient();
  }
}

void loop(){
  while (1) {
      Serial.print("Free Heap: ");
      Serial.println(esp_get_free_heap_size());
      vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
