#ifndef BRUCE_DISPLAYSERVICE_H
#define BRUCE_DISPLAYSERVICE_H

#include <NimBLEServer.h>

class DisplayService {
private:
    NimBLECharacteristic *display_info = nullptr;
    NimBLECharacteristic *navigation = nullptr;
    NimBLEService *display_service = nullptr;
    TaskHandle_t task_handle = nullptr;
public:
    DisplayService(/* args */);
    ~DisplayService();
    void setup(NimBLEServer *pServer);
    void end();
};

#endif // BRUCE_DISPLAYSERVICE_H
