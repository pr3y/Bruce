#pragma once
#include <Arduino.h>
#include <vector>

struct keyStroke { // DO NOT CHANGE IT!!!!!
    bool pressed=false;
    bool exit_key=false;
    bool fn = false;
    bool del = false;
    bool enter = false;
    uint8_t modifiers = 0;
    std::vector<char> word;
    std::vector<uint8_t> hid_keys;
    std::vector<uint8_t> modifier_keys;
    

    // Clear function
    void Clear() {
        pressed = false;
        exit_key = false;
        fn = false;
        del = false;
        enter = false;
        modifiers = 0;
        word.clear();
        hid_keys.clear();
        modifier_keys.clear();
    }
};

extern keyStroke KeyStroke;

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio();

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio();

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery();


/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval);


/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void);


/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** Starts keyboard to type data
**********************************************************************/
String keyboard(String mytext, int maxSize, String msg);


/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff();


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot();

#if defined(HAS_KEYBOARD) // related functions

/*********************************************************************
** Function: _checkKeyPress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
keyStroke _getKeyPress(); // must return something that the keyboards won´t recognize by default

/*********************************************************************
** Function: _checkNextPagePress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
bool _checkNextPagePress();

/*********************************************************************
** Function: _checkPrevPagePress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
bool _checkPrevPagePress();

#endif
