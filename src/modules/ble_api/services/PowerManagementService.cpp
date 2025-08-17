#include "PowerManagementService.hpp"
#include <BLEDevice.h>
#include <BLEUtils.h>

PowerManagementService::PowerManagementService() {}

PowerManagementService::~PowerManagementService() {}

void PowerManagementService::setup(NimBLEServer *pServer) {

    powermngt_service = pServer->createService(NimBLEUUID("0134b0a9-d14f-40b3-a595-4056062a33bd"));

    reboot_char = powermngt_service->createCharacteristic(
        NimBLEUUID("aa2095ec-e710-4462-b9af-93a133410a29"), // Battery Level
        NIMBLE_PROPERTY::WRITE
    );

    reboot_char->setCallbacks(new PowerManagementCB());

    powermngt_service->start();
    pServer->getAdvertising()->addServiceUUID(powermngt_service->getUUID());
}

void PowerManagementService::end() {
    //powermngt_service->stop();
}
