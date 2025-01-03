#include "interface.h"
#include "core/powerSave.h"

#include <M5Unified.h>


/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
  M5.begin(); //Need to test if SDCard inits with the new setup
}


/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
  uint8_t percent=0;
  percent = M5.Power.getBatteryLevel();
  return  (percent < 0) ? 0
        : (percent >= 100) ? 100
        :  percent;
}


/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
  uint8_t _tmp = (255*brightval)/100;
  M5.Lcd.setBrightness(_tmp);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, check(SelPress), AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    if(M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed()) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }    
    if(M5.BtnA.isPressed()) {
        PrevPress = true;
        EscPress = true;
    }
    if(M5.BtnC.isPressed()) {
        NextPress = true;
    }
    if(M5.BtnB.isPressed()) {
        SelPress = true;
    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed()));
    }
}

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