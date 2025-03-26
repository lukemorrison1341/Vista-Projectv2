#include "device_config.h"
boolean eco_mode = false;
unsigned int max_temp = 75;
unsigned int min_temp = 65; 
unsigned int max_humid = 70;
unsigned int min_humid = 50;
boolean reset_device = false;

void initialize_device_config(){ //Store user information in Preferences file.
    
}

void handleDeviceConfigRequest(){ //both POST and GET allowed
    if(server.method() == HTTP_OPTIONS) //Handle CORS Policy
    {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);  // ✅ 204 No Content oreflight
        return;
    }

    if (server.method() == HTTP_POST) {
        String body = server.arg("plain");  // Raw request body
        Serial.println("Received device config POST request");
    
        StaticJsonDocument<256> jsonDoc;
        DeserializationError error = deserializeJson(jsonDoc,body);

        if(error){
            Serial.print("JSON Parsing Failed(/api/device/config): ");
            Serial.println(error.c_str());
            server.send(400,"application/json", "{\"status\": \"error\", \"message\": \"Invalid JSON\"}");
        }
          const char * device_mode_char = jsonDoc["mode"];
          int min_temp_char = jsonDoc["min_temperature"];
          int max_temp_char = jsonDoc["max_temperature"];
          int min_humid_char = jsonDoc["min_humidity"];
          int max_humid_char = jsonDoc["max_humidity"];
          const char * vent_status_char = jsonDoc["vent_status"];

          if(!device_mode_char || !min_temp_char || !max_temp_char || !min_humid_char || !max_humid_char || !vent_status_char){
            
            Serial.println("Null values in device config POST endpoint!!");
            return;
          }

          Serial.printf("\nENDPOINT: MIN TEMP%d MAX TEMP %d MIN HUMID %d MAX HUMID %d DEVICE MODE %s VENT STATUS %s\n",min_temp_char,max_temp_char,min_humid_char,max_humid_char,device_mode_char,vent_status_char);      
          updateDeviceConfig(min_temp_char,max_temp_char,min_humid_char,max_humid_char, String(device_mode_char), String(vent_status_char));
          server.send(200, "application/json","{\"status\": \"updated\"}" );
          return;
    
        }
    else if(server.method() == HTTP_GET){
      Serial.println("Received Device Config GET Request");
      StaticJsonDocument<256> jsonResponse;
      file.begin("device_state");
      jsonResponse["min_temp"] = file.getInt("min_temp",min_temp);
      jsonResponse["max_temp"] = file.getInt("max_temp",max_temp);
      jsonResponse["min_humid"] = file.getInt("min_humid",min_humid);
      jsonResponse["max_humid"] = file.getInt("max_humid",max_humid);
      jsonResponse["mode"] = file.getBool("deviceMode",false) ? "eco mode" : "vacant mode";
      //jsonResponse["vent_status"] = vent_status;
      String jsonPayload;
         serializeJson(jsonResponse, jsonPayload);

          server.sendHeader("Access-Control-Allow-Origin", "*");
          server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
          server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
          server.send(200, "application/json", jsonPayload);
      file.end();  
          return;
    }
    //invalid server method
    

    // ✅ Set Response Headers
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

    server.send(400, "application/json", "{\"status\": \"fail\"}");
    return;
}

void initialize_device(){
  Serial.println("Initializing Device...");
  file.begin("device_state");
  if(file.getBool("initialized",false) == true){
    Serial.println("Device already been initialized");
    return;
  }
  /*
    Initial values before user has ever changed them.
  */
 Serial.println("Device never has been initialized");
  file.putInt("max_temp", max_temp);
  file.putInt("min_temp",min_temp); 
  file.putInt("max_humid",max_humid);
  file.putInt("min_humid",min_humid);
  file.putBool("deviceMode", eco_mode);
  file.putString("vent_status","open");
  file.putBool("initialized",true);
  return;
  
}


void updateDeviceConfig(int min_temp_new, int max_temp_new, int min_humid_new, int max_humid_new, String device_mode_new, String vent_status_new){
    file.begin("device_state");
    if(device_mode_new == "eco mode"){
        Serial.println("Setting device mode to Eco.");
        eco_mode = true;
        
        
    }
    else{
        Serial.println("Setting device mode to not Eco.");
        eco_mode = false;
    } 
    file.putBool("deviceMode",eco_mode);
    file.putInt("max_temp",max_temp_new);
    max_temp = max_temp_new;
    file.putInt("min_temp",min_temp_new);
    min_temp = min_temp_new;
    file.putInt("max_humid",max_humid_new);
    max_humid = max_humid_new;
    file.putInt("min_humid",min_humid_new);
    min_humid = min_humid_new;
    file.end();
    file.begin("device_state");
    Serial.printf("\nMAX TEMP: %d MIN TEMP: %d MAX_HUMID %d MIN_HUMID %d DEVICE MODE %d", file.getInt("max_temp",max_temp_new),file.getInt("min_temp",min_temp_new),file.getInt("max_humid",max_humid_new),file.getInt("min_humid",min_humid_new),file.getBool("deviceMode",eco_mode));
    file.end();
    return;

}
