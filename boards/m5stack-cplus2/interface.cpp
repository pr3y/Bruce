#include "interface.h"
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
void _setup_gpio() { 
    pinMode(UP_BTN, INPUT);   // Sets the power btn as an INPUT
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
    pinMode(4, OUTPUT);     // Keeps the Stick alive after take off the USB cable
    digitalWrite(4,HIGH);   // Keeps the Stick alive after take off the USB cable
    gpio_pulldown_dis(GPIO_NUM_36);
    gpio_pullup_dis(GPIO_NUM_36);
}


/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { 
    uint8_t percent;
    uint8_t _batAdcCh = ADC1_GPIO38_CHANNEL;
    uint8_t _batAdcUnit = 1;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten((adc1_channel_t)_batAdcCh, ADC_ATTEN_DB_12);
    static esp_adc_cal_characteristics_t* adc_chars = nullptr;
    static constexpr int BASE_VOLATAGE = 3600;
    adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize((adc_unit_t)_batAdcUnit, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, BASE_VOLATAGE, adc_chars);
    int raw;
    raw = adc1_get_raw((adc1_channel_t)_batAdcCh);
    uint32_t volt = esp_adc_cal_raw_to_voltage(raw, adc_chars);

    float mv = volt * 2;
    percent = (mv - 3300) * 100 / (float)(4150 - 3350);

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
    if(digitalRead(UP_BTN)==LOW || digitalRead(SEL_BTN)==LOW || digitalRead(DW_BTN)==LOW) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }    
    if(digitalRead(UP_BTN)==LOW) {
        PrevPress = true;
        EscPress = true;
    }
    if(digitalRead(DW_BTN)==LOW) {
        NextPress = true;
    }
    if(digitalRead(SEL_BTN)==LOW) {
        SelPress = true;
    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (digitalRead(UP_BTN)==LOW || digitalRead(SEL_BTN)==LOW || digitalRead(DW_BTN)==LOW));
    }
}


/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    digitalWrite(4,LOW);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)UP_BTN,LOW); 
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
    if (digitalRead(UP_BTN)==LOW)
    {
        uint32_t time_count = millis();
        while (digitalRead(UP_BTN)==LOW)
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