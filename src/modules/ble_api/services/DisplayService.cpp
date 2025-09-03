#include "DisplayService.h"
#include "globals.h"

#include <BLEDevice.h>
#include <BLEUtils.h>

DisplayService::DisplayService() : BruceBLEService() {}

DisplayService::~DisplayService() = default;

[[noreturn]] void my_task(void *pvParameters) {
    uint8_t binData[(MAX_LOG_ENTRIES * MAX_LOG_SIZE) + 1];
    size_t binSize = -1;
    DisplayService *service = static_cast<DisplayService *>(pvParameters);
    while (true) {
        uint8_t newData[(MAX_LOG_ENTRIES * MAX_LOG_SIZE) + 1];
        size_t newSize = 0;
        tft.getBinLog(newData+1, newSize);

        if (newSize > *service->_mtu) {
            newData[0] = 1; // Must read characteristic since notify will truncate data
        } else {
            newData[0] = 0;
        }

        if (binSize != newSize || memcmp(newData, binData, newSize) != 0) {
            memcpy(binData, newData, newSize);
            binSize = newSize;
            service->display_info->setValue(newData, newSize + 1);
            service->display_info->notify();
        }

        delay(200); // Set 200ms for a more responsive display
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

    static void setDirection(int direction) {
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
    }

    void onWrite(NimBLECharacteristic *pCharacteristic) override {  // payload will be 1 byte for direction and 1 byte to implement longPress
                                                                    // For example 0x0000 0x0100, 0x0101, etc...
        const uint8_t direction = pCharacteristic->getValue()[0];
        const uint8_t longPress = pCharacteristic->getValue()[1];

        resetInput();

        setDirection(direction);

        AnyKeyPress = true;

        if (longPress) {
            LongPress = true;
            const unsigned long tmp = millis();
            while (millis() - tmp < 1000) {
                setDirection(direction);
            }
            LongPress = false;
        } else {
            delay(200);
            resetInput();
        }
    }
};

void DisplayService::setup(NimBLEServer *pServer) {
    pService = pServer->createService(NimBLEUUID("ff669f6a-4304-45af-a7ef-c176441c0e19"));

    display_info = pService->createCharacteristic(
        NimBLEUUID("b7b7852e-a5b9-4857-8e98-5f505cbf6b63"), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    navigation = pService->createCharacteristic(NimBLEUUID("5c93a6d1-4e45-48c0-9928-0426adc1bbd2"), NIMBLE_PROPERTY::WRITE);
    navigation->setCallbacks(new NavigationCB());

    tft.setLogging();

    xTaskCreate(my_task, "DisplayService", 24000, this, 6, &task_handle);

    pService->start();

    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void DisplayService::end() {
    if (task_handle) vTaskDelete(task_handle);
    //if (pService) pService->stop();
}

