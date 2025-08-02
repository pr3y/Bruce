#include "BatteryService.hpp"
#include "ArduinoJson.h"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <WiFi.h>
#include <globals.h>

BatteryService::BatteryService() {}

BatteryService::~BatteryService() {}

static uint8_t batPercent = 85; // start-up value

void BatteryService::setup(BLEServer *pServer) {

    pBatSvc = pServer->createService(BLEUUID((uint16_t)0x180F));

    battery_char = pBatSvc->createCharacteristic(
        (uint16_t)0x2A19, // Battery Level
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );

    // battery_char->setCallbacks(new BatteryLevelCB());
    battery_char->setValue(&batPercent, 1); // initial value

    pBatSvc->start();
    pServer->getAdvertising()->addServiceUUID(pBatSvc->getUUID());

    xTaskCreate(
        [](void *self) { static_cast<BatteryService *>(self)->battery_handler_task(); },
        "battery_ble_handler",
        2048,
        this,
        tskIDLE_PRIORITY,
        &battery_task_handle
    );
}

void BatteryService::battery_handler_task() {
    while (true) {
        uint8_t val = 10;
        battery_char->setValue(&val, 1);

        delay(60000); // Update battery every minute
    }
}

void BatteryService::end() {
    vTaskDelete(battery_task_handle);
    pBatSvc->stop();
}
