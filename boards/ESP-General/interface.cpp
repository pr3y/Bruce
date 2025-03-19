#include "interface.h"
#include "core/powerSave.h"



/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() { }


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
void InputHandler(void) { }

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


/***************************************************************************************
** Function name: isCharging()
** location: interface.cpp
** Description:   Determines if the device is charging
***************************************************************************************/
bool isCharging() {
    #ifdef USE_BQ27220_VIA_I2C
        extern BQ27220 bq; //may not be needed
        return bq.getIsCharging();  // Return the charging status from BQ27220
    #endif
        return false;  // Default case if no power chip is defined
    }
