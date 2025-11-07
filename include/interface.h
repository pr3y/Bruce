/**
 * @file interface.h
 * @brief This file contains the function declarations for the interface.
 */
#pragma once
#include <Arduino.h>
#include <vector>

/**
 * @brief Initial setup for the device.
 * @details This function is located in `main.cpp`.
 */
void _setup_gpio();

/**
 * @brief Second stage gpio setup to make a few functions work.
 * @details This function is located in `main.cpp`.
 */
void _post_setup_gpio();

/**
 * @brief Delivers the battery value from 1-100.
 * @details This function is located in `display.cpp`.
 * @return The battery value.
 */
int getBattery();


/**
 * @brief Set brightness value.
 * @details This function is located in `settings.cpp`.
 * @param brightval The brightness value.
 */
void _setBrightness(uint8_t brightval);


/**
 * @brief Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress.
 */
void InputHandler(void);


/**
 * @brief Turns off the device (or try to).
 * @details This function is located in `mykeyboard.cpp`.
 */
void powerOff();

/**
 * @brief Puts the device into DeepSleep.
 * @details This function is located in `mykeyboard.cpp`.
 */
void goToDeepSleep();

/**
 * @brief Btn logic to turnoff the device (name is odd btw).
 * @details This function is located in `mykeyboard.cpp`.
 */
void checkReboot();

/**
 * @brief Determines if the device is charging.
 * @details This function is located in `interface.cpp`.
 * @return True if the device is charging, false otherwise.
 */
bool isCharging();
