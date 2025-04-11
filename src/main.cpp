#include <Arduino.h>
#include "main.h"


TaskHandle_t sensor_read_task = NULL;
TaskHandle_t ip_send_task = NULL;
TaskHandle_t frontend_handle_task = NULL;
TaskHandle_t handle_config_server_task = NULL;
TaskHandle_t send_sensor_data_task = NULL;
TaskHandle_t heartbeat_task = NULL;
TaskHandle_t servo_handle_task = NULL;
TaskHandle_t device_logic_task = NULL;
TaskHandle_t send_backend_task = NULL;


bool my_idle_hook_cb() {
    static int counter = 0;
    static boolean idle = false;
    if ((counter++ % 500 == 0)) {
        Serial.println("[Idle Hook] System is idle");

    }

    esp_sleep_enable_timer_wakeup(10000); // 100 ms
    esp_light_sleep_start();
    return true;
}

void setup() {
  pinMode(LED_PIN,OUTPUT);
  Serial.begin(115200);
  if (!LittleFS.begin()) {
    Serial.println("Failed to initialize LittleFS!");
    LittleFS.begin(true);
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
   
      connection_type = BOTH; //TODO: Dynamically change this, delete tasks based on whether connections fail/succeed
      digitalWrite(LED_PIN,HIGH);
      //Startup Sequences
      
      config_sensors();
      config_servo();
      create_endpoints(); //Create the endpoints for the frontend-server
      /*
      TODO: Set Priority Levels to liking
      */
      //xTaskCreate(send_ip, "Send IP to Backend", 16384, NULL, 1, &ip_send_task); //Sends IP to backend periodically (happens regardless of frontend/backend connection.)

      
     
      
      initialize_device();
      send_ip(); //responsible for also registering the username and password so, is needed
      get_config();
      
      
      xTaskCreate(read_sensors, "Sensor Read Task", 8192, NULL, 15, &sensor_read_task); //Read sensors periodically
      xTaskCreate(handle_frontend_server, "Frontend Server",16384,NULL,15,&frontend_handle_task);
      xTaskCreate(backend_send_task, "Backend Send Data Task",16384,NULL,1,&send_backend_task);
      vTaskDelay(1000); //Wait because it keeps reading 0 temperature value
      while(!sensors_read) //BLOCKING, must at least finish attempting to read the values of sensors for the first time before deciding to open/close
      xTaskCreate(device_logic, "Device Logic Task",16384,NULL,13,&device_logic_task);
      //xTaskCreate(servo_handle, "Servo Decision Making Task",16384,NULL,configMAX_PRIORITIES-1,&servo_handle_task);
      Serial.println("TEST");
      
  }
     
  }



void handle_server(void * pvParameters){ //FreeRTOS task for frontend
  while(1){
      server.handleClient();
  }
}
void loop(){

  if(connection_type == BACKEND)
  {
    Serial.println("Setting low power mode");  
  }
  while (1) {
      Serial.print("Free Heap: ");
      Serial.println(esp_get_free_heap_size());
      vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

