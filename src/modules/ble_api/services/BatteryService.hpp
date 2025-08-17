#pragma once
#include <NimBLEServer.h>

class BatteryService {
private:
    NimBLECharacteristic *battery_char = nullptr;
    void battery_handler_task();
    TaskHandle_t battery_task_handle = nullptr;
    NimBLEService *pBatSvc;

public:
    BatteryService(/* args */);
    ~BatteryService();
    void setup(NimBLEServer *pServer);
    void end();
};
