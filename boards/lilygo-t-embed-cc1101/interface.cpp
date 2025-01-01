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
    #include <XPowersLib.h>
    #include <esp32-hal-dac.h>
    XPowersPPM PPM;
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

      // Chip Select CC1101 to HIGH State
      pinMode(CC1101_SS_PIN, OUTPUT);
      digitalWrite(CC1101_SS_PIN,HIGH);

      // Power chip pin
      pinMode(PIN_POWER_ON, OUTPUT);
      digitalWrite(PIN_POWER_ON, HIGH);  // Power on CC1101 and LED
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
    percent=bq.getChargePcnt();
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
int _new_pos = 0;
int _last_pos = 0;
volatile int _last_dir = 0;
IRAM_ATTR void checkPosition() {
    encoder->tick(); // just call tick() to check the state.
}

/*********************************************************************
** Function: InputHandler
** Handles the variables checkPrevPress, checkNextPress, checkSelPress, checkAnyKeyPress and checkEscPress
**********************************************************************/
void InputHandler(void) {
    checkPowerSaveTime();
    checkPrevPress    = false;
    checkNextPress    = false;
    checkSelPress     = false;
    checkAnyKeyPress  = false;
    checkEscPress     = false;
    _last_dir = (int)encoder->getDirection();
    _last_pos = _new_pos;
    _new_pos = encoder->getPosition();
    if(_last_dir!=0 || digitalRead(SEL_BTN)==BTN_ACT) {
        if(!wakeUpScreen()) checkAnyKeyPress = true;
        else goto END;
    }    
    if(_last_dir>0) {
        _last_dir=0;
        checkPrevPress = true;
    }
    if(_last_dir<0) {
        _last_dir=0;
        checkNextPress = true;
    }
    if(digitalRead(SEL_BTN)==BTN_ACT) {
        _last_dir=0;
        checkSelPress = true;
    }

    #ifdef T_EMBED_1101
    if(digitalRead(BK_BTN)==BTN_ACT) {
        checkAnyKeyPress = true;
        checkEscPress = true;
    }
    #endif
    END:
    if(checkAnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (digitalRead(SEL_BTN)==BTN_ACT));
    }
}

/* Starts keyboard to type data */
String keyboard(String mytext, int maxSize, String msg) {
  String _mytext = mytext;

  resetTftDisplay();
  bool caps=false;
  int x=0;
  int y=-1;
  int x2=0;
  int y2=0;
  char keys[4][12][2] = { //4 lines, with 12 characteres, low and high caps
    {
      { '1', '!' },//1
      { '2', '@' },//2
      { '3', '#' },//3
      { '4', '$' },//4
      { '5', '%' },//5
      { '6', '^' },//6
      { '7', '&' },//7
      { '8', '*' },//8
      { '9', '(' },//9
      { '0', ')' },//10
      { '-', '_' },//11
      { '=', '+' } //12
     },
    {
      { 'q', 'Q' },//1
      { 'w', 'W' },//2
      { 'e', 'E' },//3
      { 'r', 'R' },//4
      { 't', 'T' },//5
      { 'y', 'Y' },//6
      { 'u', 'U' },//7
      { 'i', 'I' },//8
      { 'o', 'O' },//9
      { 'p', 'P' },//10
      { '[', '{' },//11
      { ']', '}' } //12
    },
    {
      { 'a', 'A' },//1
      { 's', 'S' },//2
      { 'd', 'D' },//3
      { 'f', 'F' },//4
      { 'g', 'G' },//5
      { 'h', 'H' },//6
      { 'j', 'J' },//7
      { 'k', 'K' },//8
      { 'l', 'L' },//9
      { ';', ':' },//10
      { '"', '\'' },//11
      { '|', '\\' } //12
    },
    {
      { '\\', '|' },//1
      { 'z', 'Z' },//2
      { 'x', 'X' },//3
      { 'c', 'C' },//4
      { 'v', 'V' },//5
      { 'b', 'B' },//6
      { 'n', 'N' },//7
      { 'm', 'M' },//8
      { ',', '<' },//9
      { '.', '>' },//10
      { '?', '/' },//11
      { '/', '/' } //12
    }
  };
  int _x = tftWidth/12;
  int _y = (tftHeight - 54)/4;
  int _xo = _x/2-3;

  int i=0;
  int j=-1;
  bool redraw=true;
  int cX =0;
  int cY =0;
  tft.fillScreen(bruceConfig.bgColor);
  while(1) {
    if(redraw) {
      tft.setCursor(0,0);
      tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
      tft.setTextSize(FM);

      //Draw the rectangles
      if(y<0 || y2<0) {
        tft.fillRect(0,1,tftWidth,22,bruceConfig.bgColor);
        tft.drawRect(7,2,46,20,TFT_WHITE);       // Ok Rectangle
        tft.drawRect(55,2,50,20,TFT_WHITE);      // CAP Rectangle
        tft.drawRect(107,2,50,20,TFT_WHITE);     // DEL Rectangle
        tft.drawRect(159,2,74,20,TFT_WHITE);     // SPACE Rectangle
        tft.drawRect(3,32,tftWidth-3,20,bruceConfig.priColor); // mystring Rectangle


        if(x==0 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(7,2,50,20,TFT_WHITE); }
        else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
        tft.drawString("OK", 18, 4);


        if(x==1 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(55,2,50,20,TFT_WHITE); }
        else if(caps) { tft.fillRect(55,2,50,20,TFT_DARKGREY); tft.setTextColor(TFT_WHITE, TFT_DARKGREY); }
        else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
        tft.drawString("CAP", 64, 4);


        if(x==2 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(107,2,50,20,TFT_WHITE); }
        else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
        tft.drawString("DEL", 115, 4);

        if(x>2 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(159,2,74,20,TFT_WHITE); }
        else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
        tft.drawString("SPACE", 168, 4);
      }

      tft.setTextSize(FP);
      tft.setTextColor(TFT_WHITE, 0x5AAB);
      tft.drawString(msg.substring(0,38), 3, 24);

      tft.setTextSize(FM);

      // reseta o quadrado do texto
      if (mytext.length() == 19 || mytext.length() == 20 || mytext.length() == 38 || mytext.length() == 39) tft.fillRect(3,32,tftWidth-3,20,bruceConfig.bgColor); // mystring Rectangle
      // escreve o texto
      tft.setTextColor(TFT_WHITE);
      if(mytext.length()>19) {
        tft.setTextSize(FP);
        if(mytext.length()>38) {
          tft.drawString(mytext.substring(0,38), 5, 34);
          tft.drawString(mytext.substring(38,mytext.length()), 5, 42);
        }
        else {
          tft.drawString(mytext, 5, 34);
        }
      } else {
        tft.drawString(mytext, 5, 34);
      }
      //desenha o retangulo colorido
      tft.drawRect(3,32,tftWidth-3,20,bruceConfig.priColor); // mystring Rectangle


      tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
      tft.setTextSize(FM);


      for(i=0;i<4;i++) {
        for(j=0;j<12;j++) {
          //use last coordenate to paint only this letter
          if(x2==j && y2==i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); tft.fillRect(j*_x,i*_y+54,_x,_y,bruceConfig.bgColor);}
          /* If selected, change font color and draw Rectangle*/
          if(x==j && y==i) { tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor); tft.fillRect(j*_x,i*_y+54,_x,_y,~bruceConfig.bgColor);}


          /* Print the letters */
          if(!caps) tft.drawChar(keys[i][j][0], (j*_x+_xo), (i*_y+56));
          else tft.drawChar(keys[i][j][1], (j*_x+_xo), (i*_y+56));

          /* Return colors to normal to print the other letters */
          if(x==j && y==i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
        }
      }
      // save actual key coordenate
      x2=x;
      y2=y;
      redraw = false;

    }

    //cursor handler
    if(mytext.length()>19) {
      tft.setTextSize(FP);
      if(mytext.length()>38) {
        cY=42;
        cX=5+(mytext.length()-38)*LW;
      }
      else {
        cY=34;
        cX=5+mytext.length()*LW;
      }
    } else {
      cY=34;
      cX=5+mytext.length()*LW*2;
    }

    int z=0;

    if(checkSelPress)  {
      tft.setCursor(cX,cY);
      if(caps) z=1;
      else z=0;
      if(x==0 && y==-1) break;
      else if(x==1 && y==-1) caps=!caps;
      else if(x==2 && y==-1 && mytext.length() > 0) {
        DEL:
        mytext.remove(mytext.length()-1);
        int fS=FM;
        if(mytext.length()>19) { tft.setTextSize(FP); fS=FP; }
        else tft.setTextSize(FM);
        tft.setCursor((cX-fS*LW),cY);
        tft.setTextColor(bruceConfig.priColor,bruceConfig.bgColor);
        tft.print(" ");
        tft.setTextColor(TFT_WHITE, 0x5AAB);
        tft.setCursor(cX-fS*LW,cY);
        cX=tft.getCursorX();
        cY=tft.getCursorY();
      }
      else if(x>2 && y==-1 && mytext.length()<maxSize) mytext += " ";
      else if(y>-1 && mytext.length()<maxSize) {
        ADD:
        mytext += keys[y][x][z];
        if(mytext.length()!=20 && mytext.length()!=20) tft.print(keys[y][x][z]);
        cX=tft.getCursorX();
        cY=tft.getCursorY();
      }
      redraw = true;
      delay(200);
    }

    /* Down Btn to move in X axis (to the right) */
    if(checkNextPress)
    {
      #ifdef T_EMBED_1101
      if(digitalRead(BK_BTN) == BTN_ACT) { y++; }
      else
      #endif
      if ((x >= 3 && y < 0) || x == 11) { y++; x = 0; } 
      else x++;

      if (y > 3) y = -1;
      redraw = true;
    }
    /* UP Btn to move in Y axis (Downwards) */
    if(checkPrevPress) {
      #ifdef T_EMBED_1101
      if(digitalRead(BK_BTN) == BTN_ACT) { y--; }
      else 
      #endif
      if(x==0) { y--; x--; }
      else x--;

      if(y<-1) { y=3; x=11; }
      else if(y<0 && x<0) x=3;
      else if (x<0) x=11;
      
      redraw = true;
    }

  }

  //Resets screen when finished writing
  tft.fillRect(0,0,tftWidth,tftHeight,bruceConfig.bgColor);
  resetTftDisplay();

  return mytext;
}

void powerOff() {
  #ifdef T_EMBED_1101
    digitalWrite(PIN_POWER_ON,LOW); 
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_6,LOW); 
    esp_deep_sleep_start();
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
                if(countDown<4) tft.drawCentreString("PWR OFF IN "+String(countDown)+"/3",tftWidth/2,12,1);
                else { 
                  tft.fillScreen(bruceConfig.bgColor);
                  while(digitalRead(BK_BTN)==BTN_ACT);
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
