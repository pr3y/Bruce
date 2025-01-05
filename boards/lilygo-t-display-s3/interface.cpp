#include "core/powerSave.h"
#include "interface.h"
#include <globals.h>

#if defined(T_DISPLAY_S3)
#include <esp_adc_cal.h>
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio()
{
  // setup buttons
  pinMode(DW_BTN, INPUT_PULLUP);
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(SEL_BTN, INPUT_PULLUP);

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  pinMode(BAT_PIN, INPUT);

  // Start with default IR, RF and RFID Configs, replace old
  bruceConfig.rfModule = CC1101_SPI_MODULE;
  bruceConfig.rfidModule = PN532_I2C_MODULE;
  bruceConfig.irRx = 1;
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery()
{
  int percent = 0;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  uint32_t raw = analogRead(BAT_PIN);
  uint32_t v1 = esp_adc_cal_raw_to_voltage(raw, &adc_chars) * 2;

  if (v1 > 4150)
  {
    percent = 0;
  }
  else
  {
    percent = map(v1, 3200, 4150, 0, 100);
  }

  return (percent < 0)      ? 0
         : (percent >= 100) ? 100
                            : percent;
}

/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval)
{
  if (brightval == 0)
  {
    analogWrite(TFT_BL, brightval);
  }
  else
  {
    int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
    analogWrite(TFT_BL, bl);
  }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void)
{
  checkPowerSaveTime();
  PrevPress = false;
  NextPress = false;
  SelPress = false;
  AnyKeyPress = false;
  EscPress = false;
  UpPress = false;
  DownPress = false;

  if (digitalRead(SEL_BTN) == BTN_ACT || digitalRead(UP_BTN) == BTN_ACT || digitalRead(DW_BTN) == BTN_ACT)
  {
    if (!wakeUpScreen())
      AnyKeyPress = true;
    else
      goto END;
  }

  if (digitalRead(UP_BTN) == BTN_ACT)
  {
    PrevPress = true;
  }

  if (digitalRead(DW_BTN) == BTN_ACT)
  {
    NextPress = true;
  }

  if (digitalRead(SEL_BTN) == BTN_ACT)
  {
    SelPress = true;
  }

END:
  if (AnyKeyPress)
  {
    long tmp = millis();
    while ((millis() - tmp) < 200 && (digitalRead(SEL_BTN) == BTN_ACT || digitalRead(UP_BTN) == BTN_ACT || digitalRead(DW_BTN) == BTN_ACT))
      ;
  }
}

void powerOff()
{
  #ifdef T_DISPLAY_S3
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN,BTN_ACT); 
    esp_deep_sleep_start();
  #endif
}

void checkReboot()
{
#ifdef T_DISPLAY_S3
  int countDown;
  /* Long press power off */
  if (digitalRead(UP_BTN) == BTN_ACT && digitalRead(DW_BTN) == BTN_ACT)
  {
    uint32_t time_count = millis();
    while (digitalRead(UP_BTN) == BTN_ACT && digitalRead(DW_BTN) == BTN_ACT)
    {
      // Display poweroff bar only if holding button
      if (millis() - time_count > 500)
      {
        tft.setTextSize(1);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        countDown = (millis() - time_count) / 1000 + 1;
        if (countDown < 4)
          tft.drawCentreString("PWR OFF IN " + String(countDown) + "/3", tftWidth / 2, 12, 1);
        else
        {
          tft.fillScreen(bruceConfig.bgColor);
          while (digitalRead(UP_BTN) == BTN_ACT || digitalRead(DW_BTN) == BTN_ACT);
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
#endif
}