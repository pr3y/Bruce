#ifndef BRUCE_DISPLAYSERVICE_H
#define BRUCE_DISPLAYSERVICE_H

#include "../src/modules/ble_api/services/BruceBLEService.hpp"
#include <NimBLEServer.h>

class DisplayService : public BruceBLEService {
    NimBLECharacteristic *navigation = nullptr;
    TaskHandle_t task_handle = nullptr;
public:
    DisplayService(/* args */);
    ~DisplayService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    NimBLECharacteristic *display_info = nullptr;
    uint16_t *_mtu = &mtu;
};

#endif // BRUCE_DISPLAYSERVICE_H
