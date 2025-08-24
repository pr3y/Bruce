#ifndef BRUCE_DISPLAYSERVICE_H
#define BRUCE_DISPLAYSERVICE_H

#include <NimBLEServer.h>
#include "BruceBLEService.hpp"

class DisplayService : public BruceBLEService {
private:
    NimBLECharacteristic *display_info = nullptr;
    NimBLECharacteristic *navigation = nullptr;
    TaskHandle_t task_handle = nullptr;
public:
    DisplayService(/* args */);
    ~DisplayService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};

#endif // BRUCE_DISPLAYSERVICE_H
