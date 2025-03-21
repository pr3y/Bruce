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
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    checkPowerSaveTime();
    PrevPress    = false;
    NextPress    = false;
    SelPress     = false;
    AnyKeyPress  = false;
    EscPress     = false;

    if(false /*Conditions fot all inputs*/) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }
    if(false /*Conditions for previous btn*/) {
        PrevPress = true;
    }
    if(false /*Conditions for Next btn*/) {
        NextPress = true;
    }
    if(false /*Conditions for Esc btn*/) {
        EscPress = true;
    }
    if(false /*Conditions for Select btn*/) {
        SelPress = true;
    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && false /*Conditions fot all inputs*/);
    }
}


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

/***************************************************************************************
** Function name: isCharging()
** Description:   Determines if the device is charging
***************************************************************************************/
bool isCharging() {
  #ifdef USE_BQ27220_VIA_I2C
      extern BQ27220 bq; //may not be needed
      return bq.getIsCharging();  // Return the charging status from BQ27220
  #elif defined(USE_AXP)
      extern AXP axp; //may not be needed also
      return axp.isCharging();    // Return the charging status from AXP (not yet tested)
  #else
      return false;  // Default case if no power chip is defined
  #endif
  }
