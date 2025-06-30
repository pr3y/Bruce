#include "core/powerSave.h"
#include <interface.h>

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
    pinMode(10, INPUT); // Pin that reads the
}

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/adc_channel.h>
#include <soc/soc_caps.h>
/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    uint8_t percent;
    uint8_t _batAdcCh = ADC1_GPIO10_CHANNEL;
    uint8_t _batAdcUnit = 1;
    static uint32_t lastVolt = 5000;
    static unsigned long lastTime = 0;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten((adc1_channel_t)_batAdcCh, ADC_ATTEN_DB_12);
    static esp_adc_cal_characteristics_t *adc_chars = nullptr;
    static constexpr int BASE_VOLATAGE = 3600;
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(
        (adc_unit_t)_batAdcUnit, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, BASE_VOLATAGE, adc_chars
    );
    int raw;
    raw = adc1_get_raw((adc1_channel_t)_batAdcCh);
    uint32_t volt = esp_adc_cal_raw_to_voltage(raw, adc_chars);

    float mv = volt * 2;
    percent = (mv - 3300) * 100 / (float)(4150 - 3350);

    return (percent < 0) ? 0 : (percent >= 100) ? 100 : percent;
}
bool isCharging() { return false; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval == 0) {
        analogWrite(TFT_BL, brightval);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    if (millis() - tm < 200 && !LongPress) return;

    bool shoulder = digitalRead(0);
    Keyboard.update();
    if (Keyboard.isPressed() || shoulder == LOW) {
        tm = millis();
        if (!wakeUpScreen()) AnyKeyPress = true;
        else return;
        keyStroke key;
        Keyboard_Class::KeysState status = Keyboard.keysState();
        bool arrow_up = false;
        bool arrow_dw = false;
        bool arrow_ry = false;
        bool arrow_le = false;
        if (shoulder == LOW) SelPress = true;
        for (auto i : status.hid_keys) key.hid_keys.emplace_back(i);
        for (auto i : status.word) {
            if (i == '`' || i == KEY_BACKSPACE) EscPress = true;

            if (i == ';') {
                arrow_up = true;
                PrevPress = true;
            }
            if (i == '.') {
                arrow_dw = true;
                NextPress = true;
            }
            if (i == '/') {
                arrow_ry = true;
                NextPress = true;
                NextPagePress = true;
            }
            if (i == ',') {
                arrow_le = true;
                PrevPress = true;
                PrevPagePress = true;
            }
            if (status.fn && arrow_up) key.word.emplace_back(0xDA);
            else if (status.fn && arrow_dw) key.word.emplace_back(0xD9);
            else if (status.fn && arrow_ry) key.word.emplace_back(0xD7);
            else if (status.fn && arrow_le) key.word.emplace_back(0xD8);
            else if (status.fn && i == '`') key.word.emplace_back(0xB1);
            else key.word.emplace_back(i);
        }
        // Add CTRL, ALT and Tab to keytroke without modifier
        key.alt = status.alt;
        key.ctrl = status.ctrl;
        key.gui = status.opt;
        // Add Tab key
        if (status.tab) key.word.emplace_back(0xB3);

        for (auto i : status.modifier_keys) key.modifier_keys.emplace_back(i);
    skip_mod:
        if (status.del) key.del = true;
        if (status.enter) {
            key.enter = true;
            key.exit_key = true;
            SelPress = true;
        }
        if (status.fn) key.fn = true;
        if (key.fn && key.del) {
            key.word.emplace_back(0xD4);
            key.del = false;
            key.fn = false;
        }
        key.pressed = true;
        KeyStroke = key;
    } else KeyStroke.Clear();
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
