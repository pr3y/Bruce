#include "interface.h"
#include "core/powerSave.h"
#include <Wire.h>

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/soc_caps.h>
#include <soc/adc_channel.h>

  // Setup for Trackball
void IRAM_ATTR ISR_up();
void IRAM_ATTR ISR_down();
void IRAM_ATTR ISR_left();
void IRAM_ATTR ISR_right();

bool trackball_interrupted = false;
int8_t trackball_up_count = 0;
int8_t trackball_down_count = 0;
int8_t trackball_left_count = 0;
int8_t trackball_right_count = 0;
void IRAM_ATTR ISR_up()   { trackball_interrupted = true; trackball_up_count = 1;   }
void IRAM_ATTR ISR_down() { trackball_interrupted = true; trackball_down_count = 1; }
void IRAM_ATTR ISR_left() { trackball_interrupted = true; trackball_left_count = 1; }
void IRAM_ATTR ISR_right(){ trackball_interrupted = true; trackball_right_count = 1;}

void ISR_rst(){
  trackball_up_count = 0;
  trackball_down_count = 0;
  trackball_left_count = 0;
  trackball_right_count = 0;
  trackball_interrupted = false;
}

#define LILYGO_KB_SLAVE_ADDRESS     0x55
#define KB_I2C_SDA       18
#define KB_I2C_SCL       8
#define SEL_BTN 0
#define UP_BTN 3
#define DW_BTN 15
#define L_BTN 2
#define R_BTN 1
#define PIN_POWER_ON 10
/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    delay(500); // time to ESP32C3 start and enable the keyboard
    if(!Wire.begin(KB_I2C_SDA, KB_I2C_SCL)) Serial.println("Fail starting ESP32-C3 keyboard");

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(SEL_BTN, INPUT);

    pinMode(9, OUTPUT); // LoRa Radio CS Pin to HIGH (Inhibit the SPI Communication for this module)
    digitalWrite(9, HIGH);

    // Setup for Trackball
    pinMode(UP_BTN, INPUT_PULLUP);
    attachInterrupt(UP_BTN, ISR_up, FALLING);
    pinMode(DW_BTN, INPUT_PULLUP);
    attachInterrupt(DW_BTN, ISR_down, FALLING);
    pinMode(L_BTN, INPUT_PULLUP);
    attachInterrupt(L_BTN, ISR_left, FALLING);
    pinMode(R_BTN, INPUT_PULLUP);
    attachInterrupt(R_BTN, ISR_right, FALLING);
}


/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    int percent=0;
    uint8_t _batAdcCh = ADC1_GPIO4_CHANNEL;
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
    char keyValue = 0;
    // 0 - UP
    // 1 - Down
    // 2 - Left
    // 3 - Right
    if (trackball_interrupted)
    {
      uint8_t xx=1;
      uint8_t yy=1;
      xx += trackball_left_count;
      xx -= trackball_right_count;
      yy -= trackball_up_count;
      yy += trackball_down_count;
      if(xx==1 && yy==1) {
        ISR_rst();
      } else {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
      }
      delay(50);
      // Print "bot - xx - yy",  1 is normal value for xx and yy 0 and 2 means movement on the axis
      //Serial.print(bot); Serial.print("-"); Serial.print(xx); Serial.print("-"); Serial.println(yy);
      if (xx < 1 || yy < 1)        { ISR_rst();   PrevPress = true;  } // left , Up
      else if (xx > 1 || yy > 1 )  { ISR_rst();   NextPress = true;  } // right, Down
    }

    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0) {
        keyValue = Wire.read();
    }
    if (keyValue!=(char)0x00) {
        KeyStroke.Clear();
        KeyStroke.hid_keys.push_back(keyValue);
        if(keyValue==' ') KeyStroke.exit_key=true; // key pressed to try to exit
        if (keyValue==(char)0x08)     KeyStroke.del=true;
        if (keyValue==(char)0x0D)     KeyStroke.enter=true;
        if (digitalRead(SEL_BTN)==BTN_ACT)      KeyStroke.fn=true;
        KeyStroke.word.push_back(keyValue);
        KeyStroke.pressed=true;
    } else KeyStroke.pressed=false;

    if(digitalRead(SEL_BTN)==BTN_ACT || KeyStroke.enter) {
        if(!wakeUpScreen()) { SelPress = true; AnyKeyPress = true; }
        else goto END;
    }
    if(keyValue==0x08) { EscPress = true; AnyKeyPress = true; }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (digitalRead(SEL_BTN)==BTN_ACT));
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
