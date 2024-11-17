#pragma once
#include <Arduino.h>
#include <vector>

//#define HAS_KEYBOARD    //has keyboard to use 
//#define HAS_KEYBOARD_HID //has keyboard to use 
//#define KB_HID_EXIT_MSG "Mid Btn + Space to exit"

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio();


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
** Function: checkNextPress
** location: mykeyboard.cpp
** Verifies Upper Btn to go to previous item
**********************************************************************/
bool checkNextPress();


/*********************************************************************
** Function: checkPrevPress
** location: mykeyboard.cpp
** Verifies Down Btn to go to next item
**********************************************************************/
bool checkPrevPress();


/*********************************************************************
** Function: checkSelPress
** location: mykeyboard.cpp
** Verifies if Select or OK was pressed
**********************************************************************/
bool checkSelPress();


/*********************************************************************
** Function: checkEscPress
** location: mykeyboard.cpp
** Verifies if Escape btn was pressed
**********************************************************************/
bool checkEscPress();


/*********************************************************************
** Function: checkAnyKeyPress
** location: mykeyboard.cpp
** Verifies id any of the keys was pressed
**********************************************************************/
bool checkAnyKeyPress();


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
};
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
