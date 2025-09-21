#include "InfoService.hpp"
#include "ArduinoJson.h"
#include <BLEDevice.h>
#include <WiFi.h>
#include <globals.h>

InfoService::InfoService() : BruceBLEService() {}

InfoService::~InfoService() {}

void InfoService::setup(NimBLEServer *pServer) {

    pService = pServer->createService(NimBLEUUID("f971c8aa-7c27-42f4-a718-83b97329130c"));

    info_char = pService->createCharacteristic(
        NimBLEUUID("e1884dc6-3d67-43fb-8be2-9ad88cc8ba7e"), // Battery Level
        NIMBLE_PROPERTY::READ
    );

    JsonDocument doc;

    doc["version"] = BRUCE_VERSION;
    doc["commit"] = GIT_COMMIT_HASH;
    doc["sdk"] = ESP.getSdkVersion();
    doc["mac"] = WiFi.macAddress();
    doc["device"] = DEVICE_NAME;

    doc["wifi_ip"] = wifiConnected ? wifiIP : "null";

    std::string data;

    serializeJson(doc, data);
    info_char->setValue(data); // initial value

    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void InfoService::end() {
    //pService->stop();
}
