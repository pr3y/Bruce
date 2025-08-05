#include "interface.h"
#include <Arduino.h>
#include <BLEServer.h>

class PowerManagementService {
private:
    BLECharacteristic *reboot_char = NULL;
    BLEService *powermngt_service;

    class PowerManagementCB : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pChar) override {
            uint8_t *val = pChar->getData();

            if (val[0] == 0x00) { powerOff(); }
            if (val[0] == 0x01) { ESP.restart(); }
        }
    };

public:
    PowerManagementService(/* args */);
    ~PowerManagementService();
    void setup(BLEServer *pServer);
    void end();
};
