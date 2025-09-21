#include "ble_api.hpp"
#include <NimBLEDevice.h>
#include <globals.h>

BLE_API::BLE_API() = default;

class BLEAPICallback : public NimBLEServerCallbacks {
    BLE_API *api;

    void onConnect(NimBLEServer *pServer, NimBLEConnInfo& connInfo) override {
        pServer->updateConnParams(connInfo.getConnHandle(), 6, 6, 0, 60);
        // 7.5 ms di conn-interval
        // zero latency
        // 600 ms timeout
    };

    void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) override {
        api->update_mtu(MTU);
    };
public:
    explicit BLEAPICallback(BLE_API *api) : api(api) {}
};

void BLE_API::setup() {
    NimBLEDevice::init("Bruce");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // 9 dBm, tweak if you want
    NimBLEDevice::setMTU(BLE_ATT_MTU_MAX);

    pServer = NimBLEDevice::createServer();

    pServer->setCallbacks(new BLEAPICallback(this));

    battery_service.setup(pServer);
    serial_service.setup(pServer);
    serialDevice = &serial_service;

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->enableScanResponse(false);    // Save some battery
    pAdvertising->start();
}

void BLE_API::update_mtu(uint16_t mtu) {
    battery_service.setMTU(mtu);
}

void BLE_API::end() {
    extern USBSerial USBserial;
    battery_service.end();
    NimBLEDevice::deinit(true);
    serialDevice = &USBserial;
}