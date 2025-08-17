#ifndef __BLE_COMMON_H__
#define __BLE_COMMON_H__

//#include "BLE2902.h"
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEBeacon.h>

#include <globals.h>
#include "core/display.h"

void ble_test();

void ble_scan();

void disPlayBLESend();

#endif
