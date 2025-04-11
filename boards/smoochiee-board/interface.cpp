#include "core/powerSave.h"

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/soc_caps.h>
#include <soc/adc_channel.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/


// Power handler for battery detection
#include <Wire.h>
#include <XPowersLib.h>
XPowersPPM PPM;


void _setup_gpio() {

    pinMode(UP_BTN, INPUT);   // Sets the power btn as an INPUT
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
    pinMode(R_BTN, INPUT);
    pinMode(L_BTN, INPUT);

    pinMode(CC1101_SS_PIN, OUTPUT);
    pinMode(NRF24_SS_PIN, OUTPUT);
 
    digitalWrite(CC1101_SS_PIN,HIGH);
    digitalWrite(NRF24_SS_PIN,HIGH);
    // Starts SPI instance for CC1101 and NRF24 with CS pins blocking communication at start
    CC_NRF_SPI.begin(CC1101_SCK_PIN, CC1101_MISO_PIN, CC1101_MOSI_PIN);

    bruceConfig.rfModule=CC1101_SPI_MODULE;
    bruceConfig.irRx=RXLED;

    bool pmu_ret = false;
      Wire.begin(GROVE_SDA, GROVE_SCL);
      pmu_ret = PPM.init(Wire, GROVE_SDA, GROVE_SCL, BQ25896_SLAVE_ADDRESS);
      if(pmu_ret) {
          PPM.setSysPowerDownVoltage(3300);
          PPM.setInputCurrentLimit(3250);
          Serial.printf("getInputCurrentLimit: %d mA\n",PPM.getInputCurrentLimit());
          PPM.disableCurrentLimitPin();
          PPM.setChargeTargetVoltage(4208);
          PPM.setPrechargeCurr(64);
          PPM.setChargerConstantCurr(832);
          PPM.getChargerConstantCurr();
          Serial.printf("getChargerConstantCurr: %d mA\n",PPM.getChargerConstantCurr());
          PPM.enableADCMeasure();
          PPM.enableCharge();
          PPM.enableOTG();
          PPM.disableOTG();
      }

}


/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100+
***************************************************************************************/
int getBattery() {
    uint8_t percent=0;
    percent=(PPM.getSystemVoltage()-3300)*100/(float)(4150-3350);

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
       if(brightval == 0){
        analogWrite(TFT_BL, brightval);
      } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval /100 ));
        analogWrite(TFT_BL, bl);
      }
}


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
    UpPress      = false;
    DownPress    = false;

    if(digitalRead(SEL_BTN)==BTN_ACT || digitalRead(UP_BTN)==BTN_ACT || digitalRead(DW_BTN)==BTN_ACT || digitalRead(R_BTN)==BTN_ACT || digitalRead(L_BTN)==BTN_ACT) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }
    if(digitalRead(L_BTN)==BTN_ACT) {
        PrevPress = true;
    }
    if(digitalRead(R_BTN)==BTN_ACT) {
        NextPress = true;
    }
    if(digitalRead(UP_BTN)==BTN_ACT) {
        UpPress = true;
    }
    if(digitalRead(DW_BTN)==BTN_ACT) {
        DownPress = true;
        EscPress = true;
    }
    if(digitalRead(SEL_BTN)==BTN_ACT) {
        SelPress = true;
    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (digitalRead(SEL_BTN)==BTN_ACT || digitalRead(UP_BTN)==BTN_ACT || digitalRead(DW_BTN)==BTN_ACT || digitalRead(R_BTN)==BTN_ACT || digitalRead(L_BTN)==BTN_ACT));
    }
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN,BTN_ACT);
    esp_deep_sleep_start();
}


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {
    int countDown;
    /* Long press power off */
    if (digitalRead(L_BTN)==BTN_ACT && digitalRead(R_BTN)==BTN_ACT)
    {
        uint32_t time_count = millis();
        while (digitalRead(L_BTN)==BTN_ACT && digitalRead(R_BTN)==BTN_ACT)
        {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if(countDown<4) tft.drawCentreString("PWR OFF IN "+String(countDown)+"/3",tftWidth/2,12,1);
                else {
                  tft.fillScreen(bruceConfig.bgColor);
                  while(digitalRead(L_BTN)==BTN_ACT || digitalRead(R_BTN)==BTN_ACT);
                  delay(200);
                  powerOff();
                }
                delay(10);
            }
        }

        // Clear text after releasing the button
        delay(30);
        tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
    }
}
/***************************************************************************************
** Function name: isCharging()
** Description:   Determines if the device is charging
***************************************************************************************/
bool isCharging() {
    return PPM.isCharging();  // Return the charging status from BQ27220
  }
