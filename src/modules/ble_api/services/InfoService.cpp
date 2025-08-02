#include "InfoService.hpp"
#include "ArduinoJson.h"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <WiFi.h>
#include <globals.h>

InfoService::InfoService() {}

InfoService::~InfoService() {}

void InfoService::setup(BLEServer *pServer) {

    info_service = pServer->createService(BLEUUID("f971c8aa-7c27-42f4-a718-83b97329130c"));

    info_char = info_service->createCharacteristic(
        BLEUUID("e1884dc6-3d67-43fb-8be2-9ad88cc8ba7e"), // Battery Level
        BLECharacteristic::PROPERTY_READ
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

    info_service->start();
    pServer->getAdvertising()->addServiceUUID(info_service->getUUID());
}

void InfoService::end() { info_service->stop(); }
