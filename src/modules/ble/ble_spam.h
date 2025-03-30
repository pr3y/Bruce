#pragma once

#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <esp_gap_ble_api.h>

/**
 * @brief Namespace for BLE Spam functionalities.
 */
namespace BLESpam {

/**
 * @brief Enumeration for BLE advertisement choices.
 */
enum class BLEChoice {
    CHOICE_1 = 1,
    CHOICE_2,
    CHOICE_3,
    // Add more choices as needed
};

/**
 * @brief Starts BLE advertising with the specified choice.
 * 
 * @param ble_choice An integer representing the BLE advertisement choice.
 * @return int Returns 0 on success, or an error code on failure.
 */
int aj_adv(BLEChoice ble_choice);

} // namespace BLESpam

#endif // BLE_SPAM_H
