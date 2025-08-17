#include "interface.h"
#include <Arduino.h>
#include <NimBLEServer.h>

class PowerManagementService {
private:
    NimBLECharacteristic *reboot_char = NULL;
    NimBLEService *powermngt_service;

    class PowerManagementCB : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic *pChar) override {
            uint8_t val = pChar->getValue()[0];

            if (val == 0x00) { powerOff(); }
            if (val == 0x01) { ESP.restart(); }
        }
    };

public:
    PowerManagementService(/* args */);
    ~PowerManagementService();
    void setup(NimBLEServer *pServer);
    void end();
};
