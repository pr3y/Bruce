#include "interface.h"
#include <Arduino.h>
#include <NimBLEServer.h>
#include "BruceBLEService.hpp"

class PowerManagementService : public BruceBLEService {
private:
    NimBLECharacteristic *reboot_char = nullptr;

    class PowerManagementCB : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic *pChar) override {
            uint8_t val = pChar->getValue()[0];

            if (val == 0x00) { powerOff(); }
            if (val == 0x01) { ESP.restart(); }
        }
    };

public:
    PowerManagementService(/* args */);
    ~PowerManagementService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};
