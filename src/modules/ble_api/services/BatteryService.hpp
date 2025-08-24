#pragma once
#include <NimBLEServer.h>
#include "BruceBLEService.hpp"

class BatteryService : public BruceBLEService {
private:
    NimBLECharacteristic *battery_char = nullptr;
    void battery_handler_task();
    TaskHandle_t battery_task_handle = nullptr;

public:
    BatteryService(/* args */);
    ~BatteryService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};
