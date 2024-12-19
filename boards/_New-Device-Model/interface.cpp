#include "interface.h"
#include "core/powerSave.h"


/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() { }

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() { }

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { return 0; }


/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { }


/*********************************************************************
** Function: checkNextPress
** location: mykeyboard.cpp
** Verifies Upper Btn to go to previous item
**********************************************************************/
bool checkNextPress(){ return false; }


/*********************************************************************
** Function: checkPrevPress
** location: mykeyboard.cpp
** Verifies Down Btn to go to next item
**********************************************************************/
bool checkPrevPress() { return false; }


/*********************************************************************
** Function: checkSelPress
** location: mykeyboard.cpp
** Verifies if Select or OK was pressed
**********************************************************************/
bool checkSelPress(){ return false; }


/*********************************************************************
** Function: checkEscPress
** location: mykeyboard.cpp
** Verifies if Escape btn was pressed
**********************************************************************/
bool checkEscPress(){ return false; }


/*********************************************************************
** Function: checkAnyKeyPress
** location: mykeyboard.cpp
** Verifies id any of the keys was pressed
**********************************************************************/
bool checkAnyKeyPress() { return false; }


/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** Starts keyboard to type data
**********************************************************************/
String keyboard(String mytext, int maxSize, String msg) { }


/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() { }


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() { }


/*********************************************************************
** Function: _checkKeyPress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
keyStroke _getKeyPress() { 
    keyStroke key;
    return key;
 } // must return something that the keyboards won´t recognize by default

/*********************************************************************
** Function: _checkNextPagePress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
bool _checkNextPagePress() { return false; }

/*********************************************************************
** Function: _checkPrevPagePress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
bool _checkPrevPagePress() { return false; }