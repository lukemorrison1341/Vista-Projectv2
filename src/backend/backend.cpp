#include "backend.h"
#include "../components/sensor/sensors.h"

const String serverURI = "http://vista-ucf.com:5000"; //backend URL

void send_ip(void * pvParameters) {
    while(1) {
        static String username = "UNINITIALIZED";
        static String user_password = "UNINITIALIZED";
        static String device_name = "UNINITIALIZED";
        static boolean opened_file = false;
        file.begin("device_config",false); //read device configuration 
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverURI + "/api/device-config");  
            http.addHeader("Content-Type", "application/json");
            
            String jsonPayload = "{";
            if(!opened_file){
                username = file.getString("username","");
                user_password = file.getString("user_password","");
                device_name = file.getString("device_name","");
                opened_file = true;
                
            }

            jsonPayload += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
            jsonPayload += "\"username\":\"" + username + "\",";
            jsonPayload += "\"password\":\"" + user_password + "\",";
            jsonPayload += "\"device_name\":\"" + device_name + "\"";
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
        vTaskDelay(IP_SEND_DELAY); // Send IP every minute
    }
}

boolean wifi_connect(){ 
    file.begin("device_config",false);
    WiFi.mode(WIFI_STA);
    if(file.getBool("enterprise",false)){ //check if WiFi network was an enterprise network.
        Serial.printf("\nConnecting to Enterprise Network\n %s\n%s\n",file.getString("wifi_username","").c_str(),file.getString("password","").c_str());
        esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)file.getString("wifi_username","").c_str(), strlen(file.getString("wifi_username","").c_str()));
        esp_wifi_sta_wpa2_ent_set_username((uint8_t *)file.getString("wifi_username","").c_str(), strlen(file.getString("wifi_username","").c_str()));
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *)file.getString("password","").c_str(), strlen(file.getString("password","").c_str()));

        // Enable WPA2 Enterprise with default configuration
        esp_wifi_sta_wpa2_ent_enable();

        WiFi.begin(file.getString("ssid","").c_str());

    }

    
    else{
        Serial.println("Connecting to non-enterprise WiFi network...");
        WiFi.begin(file.getString("ssid",""),file.getString("password",""));
    }
    //Just fancy printing stuff
    uint32_t count =0; 
    uint16_t test_count=0;
    while(WiFi.status() != WL_CONNECTED){
        if (count % 8192 == 0) {
            test_count++;
            if(count % 65536 == 0) Serial.print(".");
            if(test_count>8192){
                Serial.println("WiFi connection timeout.");
                file.end();
                return false; //Time-out
            }
        }
        count++;
    }
    file.end();
    WiFi.setSleep(true);
    Serial.println("WiFi connected.");
    return true;
}


//TODO: Add rest of the sensors.
void send_sensor_data(){ //Send sensors to backend in relay situation
    
        send_pir_data();
        vTaskDelay(SENSOR_SEND_DELAY);
    
    
}

void send_pir_data(){ //TODO: Get rid of this endpoint - just have one endpoint that sends everything in a big JSON
    WiFi.begin();
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
            file.end();
            WiFi.mode(WIFI_OFF);
        } else {
            Serial.println("WiFi not connected");
        }
        
    

}

void send_heartbeat(){
    file.begin("device_config",false); //read device configuration for username
    HTTPClient http;
    http.begin(serverURI + "/api/status");  
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


void backend_send_task(void * pvParameters){
    while(1)
    {
        send_heartbeat();
        vTaskDelay(HEARTBEAT_SEND_DELAY);
        send_sensor_data();
        vTaskDelay(SENSOR_SEND_DELAY);
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