#ifndef DEVICE_SETUP_H
#define DEVICE_SETUP_H

#include <WiFi.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "esp_wpa2.h"

extern const char* ap_ssid;
extern const char* ap_password;
//store the name and wifi information 
extern String ssid;
extern String password;
extern Preferences file;
extern TaskHandle_t handle_config_server_task;
extern WebServer server;
extern boolean connected;
extern String device_name;
extern String username;
extern String user_password;

/*
    Purpose: Run immediately upon first turning device on (can also be reset to perform device configuration again), store WiFi credentials, user information.
    Host small web page and create WiFi Access Point network for the user to connect to, host the web page at 192.168.4.1, user connects and enters all credentials. 
    After completion, set a Preferences variable to be true so that the device can remember that it has already been configured.
*/

// Function prototypes
void setup_server();
void print_device_config();
void save_configuration();
void clear_configuration(); 
boolean check_initialization(); //Checks if initialization process has been confirmed
boolean test_wifi(const char* ssid, const char* password);
boolean test_enterprise_wifi(const char* ssid, const char * password, const char * wifi_username);
void handleWifiSetup();
void handleDeviceName();
void handleFileRequest();


#endif // DEVICE_SETUP_H