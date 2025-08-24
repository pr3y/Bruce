#include "core/powerSave.h"
#include <interface.h>

#include <Keyboard.h>
Keyboard_Class Keyboard;

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    Keyboard.begin();
    pinMode(0, INPUT);
    pinMode(10, INPUT); // Pin that reads the
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    pinMode(GPIO_NUM_10, INPUT);
    uint8_t percent;
    uint32_t volt = analogReadMilliVolts(GPIO_NUM_10);

    float mv = volt;
    percent = (mv - 3300) * 100 / (float)(4150 - 3350);

    return (percent < 0) ? 0 : (percent >= 100) ? 100 : percent;
}
bool isCharging() { return false; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval == 0) {
        analogWrite(TFT_BL, brightval);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    if (millis() - tm < 200 && !LongPress) return;

    bool shoulder = digitalRead(0);
    Keyboard.update();
    if (Keyboard.isPressed() || shoulder == LOW) {
        tm = millis();
        if (!wakeUpScreen()) AnyKeyPress = true;
        else return;
        keyStroke key;
        Keyboard_Class::KeysState status = Keyboard.keysState();
        bool arrow_up = false;
        bool arrow_dw = false;
        bool arrow_ry = false;
        bool arrow_le = false;
        if (shoulder == LOW) SelPress = true;
        for (auto i : status.hid_keys) key.hid_keys.emplace_back(i);
        for (auto i : status.word) {
            if (i == '`' || i == KEY_BACKSPACE) EscPress = true;

            if (i == ';') {
                arrow_up = true;
                PrevPress = true;
            }
            if (i == '.') {
                arrow_dw = true;
                NextPress = true;
            }
            if (i == '/') {
                arrow_ry = true;
                NextPress = true;
                NextPagePress = true;
            }
            if (i == ',') {
                arrow_le = true;
                PrevPress = true;
                PrevPagePress = true;
            }
            if (status.fn && arrow_up) key.word.emplace_back(0xDA);
            else if (status.fn && arrow_dw) key.word.emplace_back(0xD9);
            else if (status.fn && arrow_ry) key.word.emplace_back(0xD7);
            else if (status.fn && arrow_le) key.word.emplace_back(0xD8);
            else if (status.fn && i == '`') key.word.emplace_back(0xB1);
            else key.word.emplace_back(i);
        }
        // Add CTRL, ALT and Tab to keytroke without modifier
        key.alt = status.alt;
        key.ctrl = status.ctrl;
        key.gui = status.opt;
        // Add Tab key
        if (status.tab) key.word.emplace_back(0xB3);

        for (auto i : status.modifier_keys) key.modifier_keys.emplace_back(i);
    skip_mod:
        if (status.del) key.del = true;
        if (status.enter) {
            key.enter = true;
            key.exit_key = true;
            SelPress = true;
        }
        if (status.fn) key.fn = true;
        if (key.fn && key.del) {
            key.word.emplace_back(0xD4);
            key.del = false;
            key.fn = false;
        }
        key.pressed = true;
        KeyStroke = key;
    } else KeyStroke.Clear();
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
