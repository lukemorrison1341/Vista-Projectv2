#include "backend.h"
#include "sensors.h"
String serverURI = "http://vista-ucf.com:5000"; //backend URL
void send_ip(void * pvParameters) {
    while(1) {
        
        file.begin("device_config",false); //read device configuration 
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverURI + "/api/device-config");  
            http.addHeader("Content-Type", "application/json");
            
            String jsonPayload = "{";
            jsonPayload += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
            jsonPayload += "\"username\":\"" + file.getString("username","") + "\",";
            jsonPayload += "\"password\":\"" + file.getString("user_password","") + "\",";
            jsonPayload += "\"device_name\":\"" + file.getString("device_name","") + "\"";
            jsonPayload += "}";
        
            int httpResponseCode = http.POST(jsonPayload);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.println("Server response(/api/device-config): " + response);
            } else {
                Serial.println("Error sending request. Code: " + String(httpResponseCode));
            }
            http.end(); // Close connection
        } else {
            Serial.println("WiFi not connected");
        }

        file.end();
        Serial.printf("IP %s Sent To Backend\n",WiFi.localIP().toString());
        vTaskDelay(pdMS_TO_TICKS(10000)); // Send IP every minute
    }
}

boolean wifi_connect(){ 
    file.begin("device_config",false);
    WiFi.mode(WIFI_STA);
    if(file.getBool("enterprise"),false){ //check if WiFi network was an enterprise network.
        Serial.println("\nConnecting to Enterprise Network\n");
        esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)file.getString("wifi_username","").c_str(), strlen(file.getString("wifi_username","").c_str()));
        esp_wifi_sta_wpa2_ent_set_username((uint8_t *)file.getString("wifi_username","").c_str(), strlen(file.getString("wifi_username","").c_str()));
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *)file.getString("password","").c_str(), strlen(file.getString("password","").c_str()));

        // Enable WPA2 Enterprise with default configuration
        esp_wifi_sta_wpa2_ent_enable();


        WiFi.begin(ssid);

    }

    
    else{
        Serial.println("Connecting to non-enterprise WiFi network...");
        WiFi.begin(file.getString("ssid",""),file.getString("password",""));
    }
    
    uint32_t count =0; 
    uint16_t test_count=0;
    while(WiFi.status() != WL_CONNECTED){
        if (count % 8192 == 0) {
            test_count++;
            if(count % 65536 == 0) Serial.print(".");
            if(test_count>254){
                Serial.println("WiFi connection timeout.");
                file.end();
                return false; //Time-out
            }
        }
        count++;
    }
    file.end();
    Serial.println("WiFi connected.");
    return true;
}


//TODO: Add rest of the sensors.
void send_sensor_data(void* pvParameters){ //Send sensors to backend in relay situation
    while(1){
        send_pir_data();
        vTaskDelay(SENSOR_SEND_DELAY);
    }
    
}

void send_pir_data(){
    Serial.println("Sending PIR data to: " + serverURI + "/api/data/pir");
        if (WiFi.status() == WL_CONNECTED) {
            file.begin("device_config",false); //read device configuration for username
            HTTPClient http;
            http.begin(serverURI + "/api/data/pir");  
            http.addHeader("Content-Type", "application/json");
            
            String jsonPayload = "{";
            jsonPayload += "\"username\":\"" + file.getString("username","") + "\",";
            jsonPayload += "\"pir\":" + String(pir);  // ✅ Remove quotes from `pir`
            jsonPayload += "}";
                    
            int httpResponseCode = http.POST(jsonPayload);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.println("Server response(/api/data/pir): " + response);
            } else {
                Serial.println("Error sending request. Code: " + String(httpResponseCode));
            }
            http.end(); 
        } else {
            Serial.println("WiFi not connected");
        }
    

}

void send_heartbeat(void * pvParameters){
    file.begin("device_config",false); //read device configuration for username
    HTTPClient http;
    http.begin(serverURI + "/api/status");  
    while(1)
    {
        Serial.println("Sending Heartbeat: " + serverURI + "/api/device-status");

        http.addHeader("Content-Type", "application/json");
        
        String jsonPayload = "{";
        jsonPayload += "\"username\":\"" + file.getString("username","") + "\"";
        jsonPayload += "}";
        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Server response(/api/device-status): " + response);
        } else {
            Serial.println("Error sending request. Code: " + String(httpResponseCode));
        }
        http.end(); // Close connection
        vTaskDelay(HEARTBEAT_SEND_DELAY);
    } 

       
    }





boolean connect_backend(){

    if(!wifi_connect()){
        Serial.println("WiFi not connected, connecting to wifi....");
        return false;
    }

    if(WiFi.status() != WL_CONNECTED) {
        return false;
    }
    //Test packet
    HTTPClient http;
    http.begin(serverURI + "/api/test");  

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        String response = http.getString();
        http.end();
        return true;
    } else {
        http.end();
        return false;   
    }
}