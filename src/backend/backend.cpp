#include "backend.h"


const String serverURI = "http://vista-ucf.com:5000"; //backend URL
boolean low_power_mode = false;
void send_ip() {
    
        static String username = "UNINITIALIZED";
        static String user_password = "UNINITIALIZED";
        static String device_name = "UNINITIALIZED";
        static boolean read_file = false;
        file.begin("device_config",false); //read device configuration 
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Sending IP");
            HTTPClient http;
            http.begin(serverURI + "/api/device-config");  
            http.addHeader("Content-Type", "application/json");
            
            String jsonPayload = "{";
            if(!read_file){
                read_file= true;
                username = file.getString("username","");
                user_password = file.getString("user_password","");
                device_name = file.getString("device_name","");
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
                Serial.println("Error sending request /api/device-config. Code: " + String(httpResponseCode));
            }
            http.end(); // Close connection
            //WiFi.mode(WIFI_OFF);  
            file.end();      
        } 
           
        else {
            Serial.println("WiFi not connected (send_ip)");
        }

       
        Serial.printf("IP %s Sent To Backend\n",WiFi.localIP().toString());
        //vTaskDelay(IP_SEND_DELAY); // Send IP every minute
    
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
    //WiFi.setSleep(true);
    Serial.println("WiFi connected.");
    return true;
}


//TODO: Add rest of the sensors.
void send_sensor_data(){ //Send sensors to backend in relay situation
    
        send_pir_data();
        vTaskDelay(SENSOR_SEND_DELAY);
    
    
}

void send_pir_data(){ //TODO: Get rid of this endpoint - just have one endpoint that sends everything in a big JSON
    //WiFi.begin();
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
            //WiFi.mode(WIFI_OFF);
        } else {
            Serial.println("WiFi not connected (send_pir_data)");
        }
        
    

}

void send_heartbeat(){
    static String username = "UNINITIALIZED";
    static boolean file_read = false;
    if(WiFi.status() == WL_CONNECTED){
            
                HTTPClient http;
            http.begin(serverURI + "/api/status");  
            
            if(!file_read){
                file.begin("device_config",false); //read device configuration for username
                username = file.getString("username","");
                file_read = true;
                file.end();
            }

                Serial.println("Sending Heartbeat: " + serverURI + "/api/device-status");

                http.addHeader("Content-Type", "application/json");
                
                String jsonPayload = "{";
                
                jsonPayload += "\"username\":\"" + username + "\"";
                jsonPayload += "}";
                int httpResponseCode = http.POST(jsonPayload);

                if (httpResponseCode > 0) {
                    String response = http.getString();
                    Serial.println("Server response(/api/device-status): " + response);
                } else {
                    Serial.println("Error sending request /api/device-status. Code: " + String(httpResponseCode));
                }
                http.end(); // Close connection
                
               
        }
        else{
            Serial.println("WiFi not connected send_heartbeat");
        }
        vTaskDelay(HEARTBEAT_SEND_DELAY);
       
}


void backend_send_task(void * pvParameters){ 
    while(1)
    {
        send_heartbeat();
        Serial.println("Sent Heartbeat");
        send_ip();
        vTaskDelay(IP_SEND_DELAY);
        send_data();
        Serial.println("Sent Sensor Data");
        Serial.println("Retrieving device configurations");
        get_config();
       
        vTaskDelay(BACKEND_SEND_DELAY);     
    }

}


void send_data(){

        static String username = "UNINITIALIZED";
        static boolean file_read = false;
        if(WiFi.status() == WL_CONNECTED){
            
            HTTPClient http;
            http.begin(serverURI + "/api/data");  
            
            if(!file_read){
                file.begin("device_config",false); //read device configuration for username
                username = file.getString("username","");
                file_read = true;
                file.end();
            }

                Serial.printf("Sending Device Data: TEMP %f HUMIDITY %f PIR %d /api/data",temp,hum,pir);

                http.addHeader("Content-Type", "application/json");
                
    

                String jsonPayload = "{";
                jsonPayload += "\"username\":\"" + username + "\",";
                jsonPayload += "\"temperature\":" + String(temp) + ",";
                jsonPayload += "\"humidity\":" + String(hum, 2) + ",";
                jsonPayload += "\"pir\":" + String(pir) + ",";
                jsonPayload += "\"vent\":" + String(vent_state);
                jsonPayload += "}";
                int httpResponseCode = http.POST(jsonPayload);

                if (httpResponseCode > 0) {
                    String response = http.getString();
                    Serial.println("Server response(/api/device-status): " + response);
                } else {
                    Serial.println("Error sending request. Code: " + String(httpResponseCode));
                }
                http.end(); // Close connection
                
               
        }
        else{
            Serial.println("WiFi not connected send_data");
        }
        vTaskDelay(HEARTBEAT_SEND_DELAY); //make a different delay
    
}
void get_config(){ //make this a task


    static String username = "UNINITIALIZED";
    static boolean file_read = false;

    if(!file_read){
        file_read=true;
        file.begin("device_config",false); //read device configuration for username
        username = file.getString("username","");
        file_read = true;
        file.end();
    }

    if(WiFi.status() == WL_CONNECTED){
        HTTPClient http;
        String url = serverURI + "/api/get_config?username=" + username; //add username as query
        http.begin(url);
        Serial.println("Getting device configuration data");

        int httpCode = http.GET();

        if(httpCode > 0){
            String response = http.getString();
            Serial.println("Got device configuration data:");
            Serial.printf("%s\n",response.c_str());

                    StaticJsonDocument<256> doc;
                    DeserializationError error = deserializeJson(doc, response);

                    min_temp = doc["min_temp"];
                    max_temp = doc["max_temp"];
                    min_humid = doc["min_humidity"];
                    max_humid = doc["max_humidity"];
                    bool motion_enabled = doc["motion_detection_enabled"]; //TODO: Do something with this
                    eco_mode = doc["eco_mode_enabled"];
                    force_state = doc["vent_state"] == "default" ? FORCE_DEFAULT : doc["vent_state"] == "force_open" ? FORCE_OPEN : FORCE_CLOSE;

                    Serial.printf("\nMIN TEMP %d MAX TEMP %d MIN HUMID %d MAX HUMID %d (before updating file)\n  FORCE STATE %d",min_temp,max_temp,min_humid,max_humid,force_state);
                    updateDeviceConfig(min_temp,max_temp,min_humid,max_humid, (eco_mode ? "eco mode" : "vacant"), "UNINITIALIZED");
        }
        else{
            Serial.println("Server error: Bad response code (get_config)");
        }
        http.end();
    }
    else{
        Serial.println("WiFi failed send_config");
    }

}


void send_config() { //should happen once
    static String username = "UNINITIALIZED";
    static boolean read_file = false;
    int min_temp, max_temp, min_humidity, max_humidity;
    bool motion_detection_enabled, eco_mode_enabled;

    file.begin("device_config", false); // Read device configuration
    Serial.println("Sending device configuration on startup....");
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverURI + "/api/send-config");
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{";

        if (!read_file) {
            read_file = true;
            username = file.getString("username", "");
            min_temp = file.getInt("min_temp", min_temp);
            max_temp = file.getInt("max_temp", max_temp);
            min_humidity = file.getInt("min_humidity", min_humid);
            max_humidity = file.getInt("max_humidity", max_humid);
            motion_detection_enabled = file.getBool("motion_detection_enabled", true);
            eco_mode_enabled = file.getBool("deviceMode",false) ? "eco mode" : "vacant mode";
        }

        jsonPayload += "\"username\":\"" + username + "\",";
        jsonPayload += "\"min_temp\":" + String(min_temp) + ",";
        jsonPayload += "\"max_temp\":" + String(max_temp) + ",";
        jsonPayload += "\"min_humidity\":" + String(min_humidity) + ",";
        jsonPayload += "\"max_humidity\":" + String(max_humidity) + ",";
        jsonPayload += "\"motion_detection_enabled\":" + String(motion_detection_enabled ? 1 : 0) + ",";
        jsonPayload += "\"eco_mode_enabled\":" + String(eco_mode_enabled ? 1 : 0);
        jsonPayload += "}";

        Serial.println("[Config Send] Sending config to backend:");
        Serial.println(jsonPayload);

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Server response(/api/send-config): " + response);
        } else {
            Serial.println("Error sending config. Code: " + String(httpResponseCode));
        }

        http.end(); // Close connection
        file.end();
    } else {
        Serial.println("WiFi not connected (send_config)");
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
        Serial.printf("Server response: %s\n",response.c_str());
        http.end();
        return true;
    } else {
        http.end();
        return false;   
    }
}