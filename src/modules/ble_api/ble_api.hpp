#ifndef BLE_API_H
#define BLE_API_H

#include "services/BatteryService.hpp"
#include "services/BLESerialService.h"
#include <Arduino.h>
#include <NimBLEServer.h>

class BLE_API {
    NimBLEServer *pServer;
    BatteryService battery_service;
    BLESerialService serial_service;
public:
    BLE_API(/* args */);
    void setup();
    void end();
    void update_mtu(uint16_t mtu);
};

#endif
