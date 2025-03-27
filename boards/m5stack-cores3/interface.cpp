#include "interface.h"
#include "core/powerSave.h"
#include "core/utils.h"

#include <M5Unified.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
  M5.begin();
 }


/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
  int percent;
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
  M5.Display.setBrightness(brightval);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    M5.update();
    auto t = M5.Touch.getDetail();
    if (t.isPressed() || t.isHolding()) {

      if(bruceConfig.rotation==3) {
          t.y = (tftHeight+20)-t.y;
          t.x = tftWidth-t.x;
      }
      if(bruceConfig.rotation==0) {
          int tmp=t.x;
          t.x = tftWidth-t.y;
          t.y = tmp;
      }
      if(bruceConfig.rotation==2) {
          int tmp=t.x;
          t.x = t.y;
          t.y = (tftHeight+20)-tmp;
      }

      if(!wakeUpScreen()) AnyKeyPress = true;
      else goto END;

      // Touch point global variable
      touchPoint.x = t.x;
      touchPoint.y = t.y;
      touchPoint.pressed=true;
      touchHeatMap(touchPoint);
    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      M5.update();
      t = M5.Touch.getDetail();
      while((millis()-tmp)<200 && (t.isPressed() || t.isHolding())) {
        M5.update();
        t = M5.Touch.getDetail();
        delay(10);
      }
    }
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() { M5.Power.powerOff(); }
void goToDeepSleep() { M5.Power.deepSleep(); }


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() { }
