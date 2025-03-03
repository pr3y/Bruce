#include "core/powerSave.h"
#include "interface.h"
#include <globals.h>

#include <RotaryEncoder.h>
//extern RotaryEncoder encoder;
extern RotaryEncoder *encoder;
IRAM_ATTR void checkPosition();

// Battery libs
#if defined(T_EMBED_1101)
    // Power handler for battery detection
    #include <Wire.h>
    #include <HAL.hpp>
    #include <esp32-hal-dac.h>
    PMIC pmic;
#elif defined(T_EMBED)
    #include <driver/adc.h>
    #include <esp_adc_cal.h>
    #include <soc/soc_caps.h>
    #include <soc/adc_channel.h>
#endif

#ifdef USE_BQ27220_VIA_I2C
    #include <bq27220.h>
    BQ27220 bq;
#endif
TwoWire* myWire = &Wire;
/***************************************************************************************
** Function name: _setup_gpio()
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(SEL_BTN,INPUT);
    #ifdef T_EMBED_1101
      // T-Embed CC1101 has a antenna circuit optimized to each frequency band, controlled by SW0 and SW1
      //Set antenna frequency settings
      pinMode(CC1101_SW1_PIN, OUTPUT);
      pinMode(CC1101_SW0_PIN, OUTPUT);

      // Chip Select CC1101, SD and TFT to HIGH State to fix SD initialization
      pinMode(CC1101_SS_PIN, OUTPUT);
      digitalWrite(CC1101_SS_PIN,HIGH);
      pinMode(TFT_CS, OUTPUT);
      digitalWrite(TFT_CS, HIGH);
      pinMode(SDCARD_CS, OUTPUT);
      digitalWrite(SDCARD_CS, HIGH);

      // Power chip pin
      pinMode(PIN_POWER_ON, OUTPUT);
      digitalWrite(PIN_POWER_ON, HIGH);  // Power on CC1101 and LED
      bool pmu_ret = false;
      myWire->begin(GROVE_SDA, GROVE_SCL);
      pmu_ret = pmic.init(Wire, GROVE_SDA, GROVE_SCL, BQ25896_SLAVE_ADDRESS);
      if(pmu_ret) {
          pmic.setSysPowerDownVoltage(3300);
          pmic.setInputCurrentLimit(3250);
          Serial.printf("getInputCurrentLimit: %d mA\n",pmic.getInputCurrentLimit());
          pmic.disableCurrentLimitPin();
          pmic.setChargeTargetVoltage(4208);
          pmic.setPrechargeCurr(64);
          pmic.setChargerConstantCurr(832);
          pmic.getChargerConstantCurr();
          Serial.printf("getChargerConstantCurr: %d mA\n",pmic.getChargerConstantCurr());
          pmic.enableMeasure(HAL::PMIC::MeasureMode::CONTINUOUS);
          pmic.enableCharge();
          pmic.enableOTG();
          pmic.disableOTG();
      }
    #else
      pinMode(BAT_PIN,INPUT); // Battery value
    #endif
    
    // Start with default IR, RF and RFID Configs, replace old
    bruceConfig.rfModule=CC1101_SPI_MODULE;
    bruceConfig.rfidModule=PN532_I2C_MODULE;
    bruceConfig.irRx=1;
    
    #ifdef T_EMBED_1101
    pinMode(BK_BTN, INPUT);
    #endif
    pinMode(ENCODER_KEY, INPUT);
    // use TWO03 mode when PIN_IN1, PIN_IN2 signals are both LOW or HIGH in latch position.
    encoder = new RotaryEncoder(ENCODER_INA, ENCODER_INB, RotaryEncoder::LatchMode::TWO03);

    // register interrupt routine
    attachInterrupt(digitalPinToInterrupt(ENCODER_INA), checkPosition, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_INB), checkPosition, CHANGE);
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
  int percent=0;
  #if defined(USE_BQ27220_VIA_I2C)
    //percent=bq.getChargePcnt(); // this function runs bq.getRemainCap()/bq.getFullChargeCap().... bq.getFullChargeCap() is hardcoded int 3000.
    percent=bq.getRemainCap()/10.7; // My battery is 1300mAh and bq.getRemainCap() doesn't go upper than 1083, that is why i'm dividing by 10.7 (var/1070)*100
  #elif defined(T_EMBED)
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
  #endif
  
  return  (percent < 0) ? 0
        : (percent >= 100) ? 100
        :  percent;
}
/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if(brightval == 0){
      analogWrite(TFT_BL, brightval);
    } else {
      int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval /100 ));
      analogWrite(TFT_BL, bl);
    }
}

//RotaryEncoder encoder(ENCODER_INA, ENCODER_INB, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder *encoder = nullptr;
IRAM_ATTR void checkPosition() {
    encoder->tick(); // just call tick() to check the state.
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static int _last_dir = 0;
    _last_dir = (int)encoder->getDirection();
    if(_last_dir!=0 || digitalRead(SEL_BTN)==BTN_ACT) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }    
    if(_last_dir>0) {
        _last_dir=0;
        PrevPress = true;
    }
    if(_last_dir<0) {
        _last_dir=0;
        NextPress = true;
    }
    if(digitalRead(SEL_BTN)==BTN_ACT) {
        _last_dir=0;
        SelPress = true;
    }

    #ifdef T_EMBED_1101
    if(digitalRead(BK_BTN)==BTN_ACT) {
        AnyKeyPress = true;
        EscPress = true;
    }
    #endif
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (digitalRead(SEL_BTN)==BTN_ACT));
    }
}

void powerOff() {
  #ifdef T_EMBED_1101
    PPM.shutdown();
  #endif
}

void checkReboot() {
  #ifdef T_EMBED_1101
    int countDown;
    /* Long press power off */
    if (digitalRead(BK_BTN)==BTN_ACT)
    {
        uint32_t time_count = millis();
        while (digitalRead(BK_BTN)==BTN_ACT)
        {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if(countDown<4) tft.drawCentreString("DeepSleep in "+String(countDown)+"/3",tftWidth/2,12,1);
                else { 
                  tft.fillScreen(bruceConfig.bgColor);
                  while(digitalRead(BK_BTN)==BTN_ACT);
                  delay(200);
                  digitalWrite(PIN_POWER_ON,LOW); 
                  esp_sleep_enable_ext0_wakeup(GPIO_NUM_6,LOW); 
                  esp_deep_sleep_start();
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
