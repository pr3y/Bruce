#include "interface.h"
#include "core/powerSave.h"

#include <Keyboard.h>
Keyboard_Class Keyboard;

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    Keyboard.begin();
    pinMode(0, INPUT);
    pinMode(10, INPUT);     // Pin that reads the
}

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/soc_caps.h>
#include <soc/adc_channel.h>
/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    uint8_t percent;
    uint8_t _batAdcCh = ADC1_GPIO10_CHANNEL;
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
    Keyboard.update();
    if(Keyboard.isPressed() || digitalRead(0)==LOW) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;

        keyStroke key;
        Keyboard_Class::KeysState status = Keyboard.keysState();
        for (auto i : status.hid_keys) key.hid_keys.push_back(i);
        for (auto i : status.word)  {
            key.word.push_back(i);
            if(i=='`') key.exit_key=true; // key pressed to try to exit
        }
        for (auto i : status.modifier_keys) key.modifier_keys.push_back(i);
        if (status.del)     key.del=true;
        if (status.enter)   key.enter=true;
        if (status.fn)      key.fn=true;
        key.pressed=true;
        KeyStroke = key;
    } else KeyStroke.pressed=false;

    if(Keyboard.isKeyPressed(',') || Keyboard.isKeyPressed(';'))            PrevPress = true;
    if(Keyboard.isKeyPressed('`') || Keyboard.isKeyPressed(KEY_BACKSPACE))  EscPress = true;
    if(Keyboard.isKeyPressed('/') || Keyboard.isKeyPressed('.'))            NextPress = true;
    if(Keyboard.isKeyPressed(KEY_ENTER) || digitalRead(0)==LOW)             SelPress = true;
    if(Keyboard.isKeyPressed('/'))                                          NextPagePress = true;  // right arrow
    if(Keyboard.isKeyPressed(','))                                          PrevPagePress = true;  // left arrow

    END:
    if(AnyKeyPress) {
      long tmp=millis();
      Keyboard.update();
      while((millis()-tmp)<200 && (Keyboard.isPressed() || digitalRead(0)==LOW)) { Keyboard.update(); delay(10); }
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

