#include "core/powerSave.h"
#include <Wire.h>
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // Setup buttons
    pinMode(PWR_BTN, INPUT);  // PWR button high-active
    pinMode(BOOT_BTN, INPUT); // BOOT button low-active
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    // Implement via PMIC if integrated
    // Placeholder: assume 100% for now
    return 100;
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    // Implement via PMIC or display driver
    // Placeholder: assume set via display driver
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    if (millis() - tm < 200 && !LongPress) return;

    // PWR button: treat as Esc for now
    bool pwrPressed = digitalRead(PWR_BTN) == HIGH;
    // BOOT button: treat as Sel for now, but actually it's for boot
    bool bootPressed = digitalRead(BOOT_BTN) == LOW;

    bool anyPressed = pwrPressed || bootPressed;
    if (anyPressed) tm = millis();
    if (anyPressed && wakeUpScreen()) return;

    AnyKeyPress = anyPressed;
    EscPress = pwrPressed;
    SelPress = bootPressed;
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    // Implement via PMIC
}

void goToDeepSleep() {
    // Implement deep sleep if needed
}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
void checkReboot() {}

/***************************************************************************************
** Function name: isCharging()
** Description:   Determines if the device is charging
***************************************************************************************/
bool isCharging() {
    // Implement via PMIC
    return false;
}
