#pragma once
#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include <esp_gap_ble_api.h>
void aj_adv(int ble_choice, bool customSet = 0);
