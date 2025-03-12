#ifndef FRONTEND_H
#define FRONTEND_H
#include "../components/sensor/sensors.h"
#include "../backend/backend.h"
#include "../device-setup.h"
#include "../device_logic.h"
#include "temp_control.h"
#include "user_settings.h"
#include "device_config.h"
void create_endpoints();
extern TaskHandle_t frontend_handle_task;
void handlePIRRequest(); //Send PIR status to frontend
void handle_frontend_server(void * pvParameters);
void handleDeviceModeRequest();
void handleFrontendTest(); //Test for frontend connection to determine backend-relay
#endif
