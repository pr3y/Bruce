#include "PowerManagementService.hpp"
#include "ArduinoJson.h"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <WiFi.h>
#include <globals.h>

PowerManagementService::PowerManagementService() {}

PowerManagementService::~PowerManagementService() {}

void PowerManagementService::setup(BLEServer *pServer) {

    powermngt_service = pServer->createService(BLEUUID("0134b0a9-d14f-40b3-a595-4056062a33bd"));

    reboot_char = powermngt_service->createCharacteristic(
        BLEUUID("aa2095ec-e710-4462-b9af-93a133410a29"), // Battery Level
        BLECharacteristic::PROPERTY_WRITE
    );

    reboot_char->setCallbacks(new PowerManagementCB());

    powermngt_service->start();
    pServer->getAdvertising()->addServiceUUID(powermngt_service->getUUID());
}

void PowerManagementService::end() { powermngt_service->stop(); }
