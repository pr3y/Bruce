#ifndef BRUCE_BLE_SERVICE_H
#define BRUCE_BLE_SERVICE_H

#include <NimBLEServer.h>

class BruceBLEService {
protected:
    NimBLEService *pService = nullptr;
public:
    virtual ~BruceBLEService() = default;
    virtual void setup(NimBLEServer *pServer) = 0;
    virtual void end() = 0;
    NimBLEService* getService() const { return pService; }
    void setService(NimBLEService* service) { pService = service; }
};

#endif // BRUCE_BLE_SERVICE_H
