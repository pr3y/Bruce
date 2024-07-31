#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>

#include "core/globals.h"
#include "core/display.h"

extern const unsigned char icon_ble[4608];
extern const unsigned char icon_ble_disconnect[4608];

void ble_init();
void ble_test();
void ble_scan();
void DisPlayBLESend();

