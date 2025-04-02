#include "backend.h"


const String serverURI = "http://vista-ucf.com:5000"; //backend URL

void send_ip() {
    
        static String username = "UNINITIALIZED";
        static String user_password = "UNINITIALIZED";
        static String device_name = "UNINITIALIZED";
        static boolean read_file = false;
        file.begin("device_config",false); //read device configuration 
        if (WiFi.status() == WL_CONNECTED) {
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
                Serial.println("Error sending request. Code: " + String(httpResponseCode));
            }
            http.end(); // Close connection
            //WiFi.mode(WIFI_OFF);  
            file.end();      
        } 
           
        else {
            Serial.println("WiFi not connected (send_ip)");
        }

       
        Serial.printf("IP %s Sent To Backend\n",WiFi.localIP().toString());
        vTaskDelay(IP_SEND_DELAY); // Send IP every minute
    
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
    //WiFi.begin();
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
                    Serial.println("Error sending request. Code: " + String(httpResponseCode));
                }
                http.end(); // Close connection
                
               
        }
        else{
            Serial.println("WiFi not connected send_heartbeat");
        }
        vTaskDelay(HEARTBEAT_SEND_DELAY);
       
}


void backend_send_task(void * pvParameters){ //TODO: RETRIEVE CONFIGURATION, SEND OTHER DATA OTHER THAN PIR
    while(1)
    {
        esp_deregister_freertos_idle_hook_for_cpu(my_idle_hook_cb, 0);
        Serial.println("Turning off idle mode");
        //HERE: need to ocnnect to WiFi again I think
        vTaskDelay(10000); //WIFI Delay
        send_heartbeat();
        Serial.println("Sent Heartbeat");
        send_ip();
        vTaskDelay(IP_SEND_DELAY);
        //send_sensor_data();
        send_data();
        Serial.println("Sent Sensor Data");
        esp_register_freertos_idle_hook_for_cpu(my_idle_hook_cb, 0);
        Serial.println("Turned on idle mode");
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

                Serial.printf("Sending Device Data: TEMP %f HUMIDITY %f PIR %d /api/data");

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


//MAKE THESE two separate tasks

void get_config(){


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
        http.begin(serverURI + "/api/get_config");
        Serial.println("Getting device configuration data");

        int httpCode = http.GET();

        if(httpCode > 0){
            String response = http.getString();
            Serial.println("Got device configuration data:");
            Serial.printf("%s\n",response);

                    StaticJsonDocument<256> doc;
                    DeserializationError error = deserializeJson(doc, response);

                    min_temp = doc["min_temp"];
                    max_temp = doc["max_temp"];
                    min_humid = doc["min_humid"];
                    max_humid = doc["max_humid"];
                    bool motion_enabled = doc["motion_detection_enabled"]; //TODO: Do something with this
                    eco_mode = doc["eco_mode_enabled"];
                    updateDeviceConfig(min_humid,max_humid,min_temp,max_temp, (eco_mode ? "eco mode" : "not eco mode"), "UNINITIALIZED");
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