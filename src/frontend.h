#ifndef FRONTEND_H
#define FRONTEND_H
#include "sensors.h"
#include "backend.h"
#include "device-setup.h"
#include "device_logic.h"
void create_endpoints();
extern TaskHandle_t frontend_handle_task;
void handlePIRRequest(); //Send PIR status to frontend
void handle_frontend_server(void * pvParameters);
void handleDeviceModeRequest();
#endif
