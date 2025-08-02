#include "ble_api.hpp"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEUtils.h>

BLE_API::BLE_API() {}

BLE_API::~BLE_API() {}

void BLE_API::setup() {
    BLEDevice::init("Bruce");
    BLEDevice::setPower(ESP_PWR_LVL_P9); // 9 dBm, tweak if you want

    pServer = BLEDevice::createServer();
    // pServer->setCallbacks(new BatteryCallback());

    info_service.setup(pServer);
    battery_service.setup(pServer);
    pServer->getAdvertising()->start();
}

void BLE_API::end() {
    info_service.end();
    battery_service.end();
    BLEDevice::deinit(true);
}
