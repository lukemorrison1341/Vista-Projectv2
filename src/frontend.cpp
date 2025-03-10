#include "frontend.h"
void create_endpoints(){
    Serial.println("Starting Frontend Server...");
    server.on("/api/sensor/pir", HTTP_GET, handlePIRRequest);
    server.on("/api/sensor/pir", HTTP_OPTIONS, handlePIRRequest);
    server.on("/api/device/mode", HTTP_POST, handleDeviceModeRequest);
    server.on("/api/device/mode", HTTP_OPTIONS, handleDeviceModeRequest);
    server.on("/api/test", HTTP_GET, handleFrontendTest);
    server.on("/api/test", HTTP_OPTIONS, handleFrontendTest);
    server.begin();
    Serial.println("Frontend Server Started");

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

void handlePIRRequest() {

    if(server.method() == HTTP_OPTIONS) //Handle CORS Policy
    {
        Serial.println("Handling CORS preflight request...");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);  // ✅ 204 No Content oreflight
        return;
    }

    if (server.method() != HTTP_GET) {
        server.send(405, "text/plain", "Method Not Allowed");
        return;
      }
    Serial.printf("Received PIR GET Request\n");

    StaticJsonDocument<256> jsonResponse;
    jsonResponse["pir"] = get_pir(); 

    String jsonPayload;
    serializeJson(jsonResponse, jsonPayload);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "application/json", jsonPayload);  
}


void handleFrontendTest(){

    if(server.method() == HTTP_OPTIONS) //Handle CORS Policy
    {
        Serial.println("Handling CORS preflight request...");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);  // ✅ 204 No Content oreflight
        return;
    }

    if (server.method() != HTTP_GET) {
        server.send(405, "text/plain", "Method Not Allowed");
        return;
      }
    Serial.printf("Received Frontend Test GET Request\n");

    StaticJsonDocument<256> jsonResponse;
    jsonResponse["status"] = "success";

    String jsonPayload;
    serializeJson(jsonResponse, jsonPayload);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "application/json", jsonPayload);

}

void handle_frontend_server(void * pvParameters){
    while(1){
        server.handleClient();
    }
}