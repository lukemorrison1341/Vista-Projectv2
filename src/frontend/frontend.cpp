#include "frontend.h"
enum CONNECTION_TYPE connection_type = BOTH; 
volatile unsigned long last_frontend_access = 0;
void create_endpoints(){
    Serial.println("Starting Frontend Server...");
    server.on("/api/sensor/pir", HTTP_GET, handlePIRRequest);
    server.on("/api/sensor/pir", HTTP_OPTIONS, handlePIRRequest);
    server.on("/api/device/config", HTTP_POST, handleDeviceConfigRequest);
    server.on("/api/device/config", HTTP_GET, handleDeviceConfigRequest);
    server.on("/api/device/config", HTTP_OPTIONS, handleDeviceConfigRequest);
    server.on("/api/device/reset", HTTP_GET,handleResetDevice);
    server.on("/api/device/reset", HTTP_OPTIONS,handleResetDevice);
    server.on("/api/component/data",HTTP_GET,handleDataRequest);
    server.on("/api/component/data",HTTP_OPTIONS,handleDataRequest);
    server.on("/api/test", HTTP_GET, handleFrontendTest);
    server.on("/api/test", HTTP_OPTIONS, handleFrontendTest);
    server.begin();
    Serial.println("Frontend Server Started");
}



void handleFrontendTest(){

    connection_type = FRONTEND;

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

void handleDataRequest(){ //web app asks for current temperature, humidity, vent, pir status
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
    Serial.printf("Received DATA GET Request\n");

    StaticJsonDocument<256> jsonResponse;
    jsonResponse["pir"] = pir;
    jsonResponse["temp"] = temp;
    jsonResponse["humid"] = hum;
    jsonResponse["vent"] =  servo_state == OPEN ? "open" : "close";

    String jsonPayload;
    serializeJson(jsonResponse, jsonPayload);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "application/json", jsonPayload);  

}

void handle_frontend_server(void *pvParameters) {
    const TickType_t checkInterval = pdMS_TO_TICKS(5000);  // Check every 5s
    unsigned long lastCheck = millis();

    server.begin();
    Serial.println("Frontend server started.");

    last_frontend_access = millis();  // Assume active at start

    while (true) {
        server.handleClient();  // Poll for connections (non-blocking)

        // Every 5s, check if it's been idle for >60s
        if (millis() - lastCheck > 5000) {
            lastCheck = millis();
                
            if (millis() - last_frontend_access > 6000) {
                Serial.println("No frontend access for 6s. Shutting down server.");

                server.stop();             // Stop the web server
                WiFi.setSleep(true);       // Enable modem sleep
                vTaskDelete(NULL);         // Kill this task
                return;
            }
        }

        vTaskDelay(1);  // Let other tasks breathe
    }
}




void handlePIRRequest() { //I might not be using this anymore

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
