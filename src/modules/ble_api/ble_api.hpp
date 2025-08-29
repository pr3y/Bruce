#ifndef BLE_API_H
#define BLE_API_H

#include "services/BatteryService.hpp"
#include "services/DisplayService.h"
#include "services/InfoService.hpp"
#include "services/PowerManagementService.hpp"
#include "services/FileManagerService.h"
#include <NimBLEServer.h>

class BLE_API {
private:
    NimBLEServer *pServer;
    InfoService info_service;
    BatteryService battery_service;
    PowerManagementService pwrmngt_service;
    DisplayService display_service;
    FileManagerService file_manager_service;
public:
    BLE_API(/* args */);
    ~BLE_API();
    void setup();
    void end();
    void update_mtu(uint16_t mtu);
};

#endif
