#include "ble_api.hpp"
#include <NimBLEDevice.h>

BLE_API::BLE_API() {}

BLE_API::~BLE_API() {}

class MyServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) override {
        // Improve performance
        pServer->updateConnParams(desc->conn_handle,
                                  6, 6,     // 7.5 ms di conn-interval
                                  0,        // zero latency
                                  60);      // 600 ms timeout
    }
};

void BLE_API::setup() {
    NimBLEDevice::init("Bruce");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // 9 dBm, tweak if you want
    NimBLEDevice::setMTU(BLE_ATT_MTU_MAX);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    display_service.setup(pServer);
    info_service.setup(pServer);
    battery_service.setup(pServer);
    pwrmngt_service.setup(pServer);
    file_manager_service.setup(pServer);

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
}

void BLE_API::end() {
    info_service.end();
    battery_service.end();
    pwrmngt_service.end();
    NimBLEDevice::deinit(true);
}