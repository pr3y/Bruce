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

void DisplayService::setup(NimBLEServer *pServer) {
    display_service = pServer->createService(NimBLEUUID("ff669f6a-4304-45af-a7ef-c176441c0e19"));

    display_info = display_service->createCharacteristic(
        NimBLEUUID("b7b7852e-a5b9-4857-8e98-5f505cbf6b63"),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    xTaskCreate(my_task, "DisplayService", 24000, display_info, 6, &task_handle);

    tft.setLogging();
    display_service->start();
    pServer->getAdvertising()->addServiceUUID(display_info->getUUID());
}

void DisplayService::end() {
    //display_service->stop();
}
