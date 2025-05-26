#pragma once
#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <esp_gap_ble_api.h>
void aj_adv(int ble_choice);
void ibeacon(char* DeviceName="Bruce iBeacon", char* BEACON_UUID="8ec76ea3-6668-48da-9866-75be8bc86f4d", int ManufacturerId=0x4C00);
