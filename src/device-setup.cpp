#include "device-setup.h"

const char* ap_ssid = "VISTA_Access_Point";
const char* ap_password = "12345678";
WebServer server(80);
Preferences file;
boolean connected = false;
String device_name = "UNINITIALIZED";
String ssid = "UNINITIALIZED";
String password = "UNINITIALIZED";
String wifi_username = "UNINITIALIZED";

String username = "UNINITIALIZED";
String user_password = "UNINITIALIZED";


boolean enterprise = false; //Initial value (only gets changed when frontend changes it)
// Function to handle POST requests to /wifi-setup
void handleWifiSetup() {
    if (server.method() != HTTP_POST) {
      server.send(405, "text/plain", "Method Not Allowed");
      return;
    }
  
    // Parse the request body (assume JSON data)
    String body = server.arg("plain");  // Raw request body
    Serial.printf("Received WiFi setup request: %s\n", body.c_str());
  
    // Parse JSON data
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      Serial.printf("Failed to parse JSON: %s\n", error.c_str());
      server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"Invalid JSON\"}");
      return;
    }

  
    // Extract WiFi credentials from JSON
    const char* ssid_char = doc["ssid"];
    const char* password_char = doc["password"];

    ssid = String(ssid_char);
    password = String(password_char);
    enterprise = doc["enterprise"];
  
    Serial.printf("WiFi Credentials - SSID: %s, Password: %s Enterprise %d \n", ssid.c_str(), password.c_str(),enterprise);

    // Respond with success

    if(!enterprise){
      if(test_wifi(ssid_char,password_char)){
        server.send(200, "application/json", "{\"status\": \"success\"}");
      }
      else{
        server.send(200,"application/json","{\"status\": \"fail\"}");
      }
    }
    else{
      Serial.println("Testing Enterprise WiFi network.");
      const char * wifi_username_char = doc["username"];
      wifi_username = String(wifi_username_char);
      Serial.printf("\nWIFI USERNAME %s",wifi_username.c_str());
      if(test_enterprise_wifi(ssid_char,password_char,wifi_username_char)){
        server.send(200, "application/json", "{\"status\": \"success\"}");
      }
      else{
        server.send(200,"application/json","{\"status\": \"fail\"}");
      }
      

    }
      
  
  }
  
  void handleDeviceName(){
    if(server.method() != HTTP_POST){
      server.send(405, "text/plain", "Method Not Allowed");
      return;
    }
    if(!connected){
      server.send(200, "application/json", "{\"status\":\"fail\"}");
      return;
    }
  
  
    // Parse the request body (assume JSON data)
  
    int headerCount = server.headers();
    for (int i = 0; i < headerCount; i++) {
      String headerName = server.headerName(i);
      String headerValue = server.header(i);
      Serial.printf("%s: %s\n", headerName.c_str(), headerValue.c_str());
    }
  
    String body = server.arg("plain");  // Raw request body
    Serial.printf("Received Device Name : %s\n", body.c_str());
  
    // Parse JSON data
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, body);
  
    if (error) {
      Serial.printf("Failed to parse JSON: %s\n", error.c_str());
      server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"Invalid JSON\"}");
      return;
    }
  
    const char * device_name_char = doc["device_name"];
    const char * username_char = doc["username"];
    const char * user_password_char = doc["user_password"];

    device_name = String(device_name_char);
    username = String(username_char);
    user_password = String(user_password_char);
    
    Serial.printf("Device name: %s Username: %s Password: %s",device_name_char, username_char, user_password_char);
  
    server.send(200,"application/json","{\"status\":\"success\"}");

    save_configuration();
  
  }
  
  boolean test_wifi(const char * ssid, const char * password){
  
    // Set Wi-Fi mode to station
    WiFi.mode(WIFI_MODE_APSTA);
   
    // Initiate connection to the specified SSID and password
    WiFi.begin(ssid, password);
    
    // Wait for the connection attempt to complete
    uint8_t status = WiFi.waitForConnectResult();
    
    // Check if the connection was successful
    if (status == WL_CONNECTED) {
      Serial.println("Connection successful");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      connected = true;
      return true;
    } else {
      Serial.println("Connection failed");
      return false;
    }
  
    WiFi.mode(WIFI_AP); //set back to AP mode 
  
  }
  
  
  void handleFileRequest() {
    String path = server.uri();  // Get the requested URI
    Serial.printf("Request URI: %s\n", path.c_str());
  
    if (path == "/") path = "/index.html";  // Default to index.html for root
    Serial.printf("Resolved path: %s\n", path.c_str());
  
    File file = LittleFS.open(path, "r");
    if (!file) {
      server.send(404, "text/plain", "File Not Found");
      Serial.printf("File not found: %s\n", path.c_str());
      return;
    }
  
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";
    else if (path.endsWith(".png")) contentType = "image/png";
    else if (path.endsWith(".jpg")) contentType = "image/jpeg";
  
    server.streamFile(file, contentType);
    file.close();
    Serial.printf("Served file: %s\n", path.c_str());
  }
void save_configuration(){ //Once done, stores key "initialized" -> true, initialization finished
  Serial.println("Saving Device Configurations...");
  if(check_initialization())
  {
    Serial.println("Initialization has already occured (cannot save device configuration).");
    return;
  }

  file.begin("device_config",false); //read+write 
  file.putString("device_name",device_name.c_str());
  file.putString("ssid",ssid.c_str());
  file.putString("password",password.c_str());
  file.putString("username",username.c_str());
  if(enterprise){
    Serial.printf("\nStoring WiFi username: %s",wifi_username.c_str());
    file.putString("wifi_username",wifi_username.c_str());
  }
  
  file.putString("user_password",user_password.c_str());
  file.putBool("enterprise",enterprise);
  file.putBool("initialized",true);
  file.end();
}

void clear_configuration(){ //Clears all device configuration data
  Serial.println("Clearing device configurations");
  if(!check_initialization()){
    Serial.println("Cannot clear device configuration, device not configured");
    return;
  }
  file.begin("device_config",false);
  file.clear();
  file.end();
  file.begin("device_state",false);
  file.clear();
  file.end();

}
boolean check_initialization(){
    file.begin("device_config",true); //read only
      if(file.isKey("initialized")){
        file.end();
          return true;
      }
      
      file.end();
      return false;
}

boolean test_enterprise_wifi(const char * ssid, const char * password, const char * wifi_username){
  WiFi.mode(WIFI_MODE_APSTA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)wifi_username, strlen(wifi_username));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)wifi_username, strlen(wifi_username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));

  // Enable WPA2 Enterprise with default configuration
  esp_wifi_sta_wpa2_ent_enable();


   WiFi.begin(ssid);

   // Wait for the connection attempt to complete
    uint8_t status = WiFi.waitForConnectResult();
    
    // Check if the connection was successful
    if (status == WL_CONNECTED) {
      Serial.println("Connection successful");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      connected = true;
      return true;
    } else {
      Serial.println("Connection failed");
      return false;
    }
  
    WiFi.mode(WIFI_AP); //set back to AP mode 
  
    return true;

}


void print_device_config(){
  file.begin("device_config",true); 
  
  Serial.printf("SSID: %s  WIFI_PASSSWORD: %s ENTERPRISE %d WIFI_USERNAME %s NAME: %s  USER_USERNAME: %s USER_PASSWORD %s  \n",file.getString("ssid","").c_str(),file.getString("password","").c_str(), file.getBool("enterprise",false),file.getString("wifi_username",""),file.getString("device_name","").c_str(),file.getString("username","").c_str(),file.getString("user_password","").c_str());
  file.end();
}
void setup_server(){ //create HTTP server
  Serial.println("Starting ESP32 Access Point...");
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Set up routes
  server.on("/wifi-setup", handleWifiSetup);  // Add the new endpoint
  server.on("/device-name",handleDeviceName);
  server.onNotFound(handleFileRequest);

  server.begin();
  Serial.println("Web server started");

}