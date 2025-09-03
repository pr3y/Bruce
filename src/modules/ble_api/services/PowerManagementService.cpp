#include "PowerManagementService.hpp"
#include <BLEDevice.h>
#include <BLEUtils.h>

PowerManagementService::PowerManagementService() : BruceBLEService() {}

PowerManagementService::~PowerManagementService() {}

void PowerManagementService::setup(NimBLEServer *pServer) {

    pService = pServer->createService(NimBLEUUID("0134b0a9-d14f-40b3-a595-4056062a33bd"));

    reboot_char = pService->createCharacteristic(
        NimBLEUUID("aa2095ec-e710-4462-b9af-93a133410a29"), // Battery Level
        NIMBLE_PROPERTY::WRITE
    );

    reboot_char->setCallbacks(new PowerManagementCB());
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void PowerManagementService::end() {
    //if (pService) pService->stop();
}
