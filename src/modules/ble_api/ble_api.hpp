#ifndef BLE_API_H
#define BLE_API_H

#include "Arduino.h"
#include "services/BatteryService.hpp"
#include "services/InfoService.hpp"
#include "services/PowerManagementService.hpp"
#include <BLEServer.h>

class BLE_API {
private:
    BLEServer *pServer;
    InfoService info_service;
    BatteryService battery_service;
    PowerManagementService pwrmngt_service;

public:
    BLE_API(/* args */);
    ~BLE_API();
    void setup();
    void end();
};

#endif
