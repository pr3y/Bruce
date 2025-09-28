#ifndef BLE_API_HPP
#define BLE_API_HPP

#include "services/BLESerialService.h"
#include "services/BatteryService.hpp"

class BLE_API {
  public:
    BLE_API();
    void setup();
    void end();
    void update_mtu(uint16_t mtu);

  private:
    NimBLEServer* pServer;
    BatteryService battery_service;
    BLESerialService serial_service;
};

#endif // BLE_API_HPP
