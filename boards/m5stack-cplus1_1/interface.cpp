#include "interface.h"
#include "core/powerSave.h"
#include <AXP192.h>
AXP192 axp192;

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
    axp192.begin();           // Start the energy management of AXP192
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    int percent=0;
    float b = axp192.GetBatVoltage();
    percent = ((b - 3.0) / 1.2) * 100;

    return  (percent < 0) ? 0
            : (percent >= 100) ? 100
            :  percent;
}

/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if(brightval>100) brightval=100;
    axp192.ScreenBreath(brightval);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    if(axp192.GetBtnPress()) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
        PrevPress = true;
        EscPress = true;
    }
    if(digitalRead(DW_BTN)==LOW) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
        NextPress = true;
    }
    if(digitalRead(SEL_BTN)==LOW) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
        SelPress = true;
    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (axp192.GetBtnPress() || digitalRead(SEL_BTN)==LOW || digitalRead(DW_BTN)==LOW));
    }
}

void powerOff() { 
    axp192.PowerOff();
}

void checkReboot() {
    int countDown;
    /* Long press power off */
    if (axp192.GetBtnPress())
    {
        uint32_t time_count = millis();
        while (axp192.GetBtnPress())
        {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                tft.setCursor(60, 12);
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                tft.printf(" PWR OFF IN %d/3\n", countDown);
                delay(10);
            }
        }
        // Clear text after releasing the button
        delay(30);
        tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
    }
}
