#include "DisplayService.h"
#include "globals.h"

#include <BLEDevice.h>
#include <BLEUtils.h>

DisplayService::DisplayService() {}

DisplayService::~DisplayService() {}

[[noreturn]] void my_task(void *pvParameters) {
    uint8_t binData[MAX_LOG_ENTRIES * MAX_LOG_SIZE];
    size_t binSize = -1;
    NimBLECharacteristic *pChar = static_cast<NimBLECharacteristic *>(pvParameters);
    while (true) {
        uint8_t newData[MAX_LOG_ENTRIES * MAX_LOG_SIZE];
        size_t newSize = 0;
        tft.getBinLog(newData, newSize);

        if ((binSize != newSize || memcmp(newData, binData, newSize) != 0)) {
            memcpy(binData, newData, newSize);
            binSize = newSize;
            pChar->setValue(newData, newSize);
            pChar->notify();
        }

        delay(500);
    }
};

class NavigationCB: public NimBLECharacteristicCallbacks {
    static void resetInput() {
        AnyKeyPress = false;
        PrevPress = false;
        EscPress = false;
        NextPress = false;
        SelPress = false;
        LongPress = false;
    }
    void onWrite(NimBLECharacteristic *pCharacteristic) override {  // payload will be 1 byte for direction and 1 byte to implement longPress
                                                                    // For example 0x0000 0x0100, 0x0101, etc...
        const uint8_t direction = pCharacteristic->getValue()[0];
        const uint8_t reset = pCharacteristic->getValue()[1];

        resetInput();

        switch (direction) {
            case 0:
                PrevPress = true;
                break;
            case 1:
                NextPress = true;
                break;
            case 2:
                EscPress = true;
                break;
            case 3:
                SelPress = true;
                break;
            default:
                Serial.println("Invalid BLE payload: " + String(direction));
        }


        AnyKeyPress = true;

        if (reset) {
            delay(200);
            resetInput();
        } else {
            LongPress = true;
        }
    }
};

void DisplayService::setup(NimBLEServer *pServer) {
    display_service = pServer->createService(NimBLEUUID("ff669f6a-4304-45af-a7ef-c176441c0e19"));

    display_info = display_service->createCharacteristic(
        NimBLEUUID("b7b7852e-a5b9-4857-8e98-5f505cbf6b63"), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    navigation = display_service->createCharacteristic(NimBLEUUID("5c93a6d1-4e45-48c0-9928-0426adc1bbd2"), NIMBLE_PROPERTY::WRITE);
    navigation->setCallbacks(new NavigationCB());

    tft.setLogging();

    xTaskCreate(my_task, "DisplayService", 24000, display_info, 6, &task_handle);

    display_service->start();

    pServer->getAdvertising()->addServiceUUID(display_info->getUUID());
}

void DisplayService::end() {
    // display_service->stop();
}
