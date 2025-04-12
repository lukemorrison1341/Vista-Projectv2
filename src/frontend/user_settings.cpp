#include "user_settings.h"


void handleResetDevice(){

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
    Serial.printf("Received Reset Device GET Request\n");

    clear_configuration();

    StaticJsonDocument<256> jsonResponse;
    jsonResponse["status"] = "success"; 

    String jsonPayload;
    serializeJson(jsonResponse, jsonPayload);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "application/json", jsonPayload);
    Serial.println("Restarting....");
    ESP.restart();//Restart the device

}