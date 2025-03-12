#include "device_config.h"
boolean eco_mode = false;
unsigned int max_temp = 0;
unsigned int min_temp = 0; 
unsigned int max_humid = 0;
unsigned int min_humid = 0;
boolean reset_device = false;
boolean device_changed = false;

void initialize_device_config(){ //Store user information in Preferences file.
    
}

void handleDeviceModeRequest(){
    if(server.method() == HTTP_OPTIONS) //Handle CORS Policy
    {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);  // ✅ 204 No Content oreflight
        return;
    }

    if (server.method() != HTTP_POST) {
        server.send(405, "text/plain", "Method Not Allowed");
        return;
      }
    String body = server.arg("plain");  // Raw request body
    Serial.println("Received device mode POST request");
    
    StaticJsonDocument<256> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc,body);

    if(error){
        Serial.print("JSON Parsing Failed(/api/device/mode): ");
        Serial.println(error.c_str());
        server.send(400,"application/json", "{\"status\": \"error\", \"message\": \"Invalid JSON\"}");
    }
    const char * device_mode_char = jsonDoc["mode"];

    if(String(device_mode_char) == "eco"){
        Serial.println("Setting device mode to Eco.");
        eco_mode = true;
    }
    else{
        Serial.println("Setting device mode to not Eco.");
        eco_mode = false;
    } 

    // ✅ Set Response Headers
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

    server.send(200, "application/json", "{\"status\": \"success\"}");
}
