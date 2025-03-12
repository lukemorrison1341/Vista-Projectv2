#include "frontend.h"
enum CONNECTION_TYPE connection_type = BOTH; 

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
    connection_type = FRONTEND;

}

void handle_frontend_server(void * pvParameters){
    while(1){
        server.handleClient();
    }
}