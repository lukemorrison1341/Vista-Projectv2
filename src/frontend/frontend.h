    #ifndef FRONTEND_H
    #define FRONTEND_H
    #include "../components/sensor/sensors.h"
    #include "../backend/backend.h"
    #include "../device-setup.h"
    #include "../device_logic.h"
    #include "temp_control.h"
    #include "user_settings.h"
    #include "device_config.h"
    #include "../device_logic.h"
    #include "../main.h"

    /*
        PURPOSE: Central hub for the frontend server, receive requests from web app in situation where frontend connection successful.
    */

    void create_endpoints();
    extern TaskHandle_t frontend_handle_task;
    void handlePIRRequest(); //Send PIR status to frontend
    void handle_frontend_server(void * pvParameters);
    void handleDataRequest(); //Send all data (PIR,Current Temperature,Current Humidity, Vent Status)
    void handleDeviceConfigRequest();
    void handleFrontendTest(); //Test for frontend connection to determine backend-relay
    #endif
