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
bool menuPress(int bot){
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
      return false;
    } else if(bot==4) return true; // any btn
    delay(50);
    // Print "bot - xx - yy",  1 is normal value for xx and yy 0 and 2 means movement on the axis
    //Serial.print(bot); Serial.print("-"); Serial.print(xx); Serial.print("-"); Serial.println(yy);
    if (xx < 1 && bot==2)       { ISR_rst();   return true;  } // left
    else if (xx > 1 && bot==3)  { ISR_rst();   return true;  } // right
    else if (yy < 1 && bot==0)  { ISR_rst();   return true;  } // up
    else if (yy > 1 && bot==1)  { ISR_rst();   return true;  } // down
    else return false;
  }
  else return false;

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
  if(brightval>100) brightval=100;
   if(brightval == 0){
      analogWrite(BACKLIGHT, brightval);
    } else {
      int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval/100 ));
      analogWrite(BACKLIGHT, bl);
    }
}


/*********************************************************************
** Function: checkNextPress
** location: mykeyboard.cpp
** Verifies Upper Btn to go to previous item
**********************************************************************/
bool checkNextPress(){
    if(menuPress(1) || menuPress(3)) {
        if(wakeUpScreen()){
            delay(200);
            return false;
        }
        return true;
    }
    else return false;
}

/*********************************************************************
** Function: checkPrevPress
** location: mykeyboard.cpp
** Verifies Down Btn to go to next item
**********************************************************************/
bool checkPrevPress() {
    if(menuPress(0) || menuPress(2)) {
        if(wakeUpScreen()){
            delay(200);
            return false;
        }
        return true;
    }
    else return false;
}

/*********************************************************************
** Function: checkSelPress
** location: mykeyboard.cpp
** Verifies if Select or OK was pressed
**********************************************************************/
bool checkSelPress(){
    checkPowerSaveTime();
    char keyValue = 0;
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0) {
        keyValue = Wire.read();
    }
    if(digitalRead(SEL_BTN)==BTN_ACT || keyValue==0x0D) {
        if(wakeUpScreen()){
            delay(200);
            return false;
        }
        return true;
    }
    else return false;
}

/*********************************************************************
** Function: checkEscPress
** location: mykeyboard.cpp
** Verifies if Escape btn was pressed
**********************************************************************/
bool checkEscPress(){
  char keyValue = 0;
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0) {
        keyValue = Wire.read();
    }
    if(keyValue==0x08) // delete keyboard btn
    {
        if(wakeUpScreen()){
            delay(200);
            return false;
        }
        returnToMenu=true;
        return true;
    }
    else { return false; }
}

/*********************************************************************
** Function: checkAnyKeyPress
** location: mykeyboard.cpp
** Verifies id any of the keys was pressed
**********************************************************************/
bool checkAnyKeyPress() {
    if(menuPress(4)) return true;

    return false;
}


/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** Starts keyboard to type data
**********************************************************************/
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

#if defined(HAS_TOUCH)
  int k=0;
  for(x2=0; x2<12;x2++) {
    for(y2=0; y2<4; y2++) {
      box_list[k].key=keys[y2][x2][0];
      box_list[k].key_sh=keys[y2][x2][1];
      box_list[k].color = ~bruceConfig.bgColor;
      box_list[k].x=x2*_x;
      box_list[k].y=y2*_y+54;
      box_list[k].w=_x;
      box_list[k].h=_y;
      k++;
    }
  }
  // OK
  box_list[k].key=' ';
  box_list[k].key_sh=' ';
  box_list[k].color = ~bruceConfig.bgColor;
  box_list[k].x=0;
  box_list[k].y=0;
  box_list[k].w=53;
  box_list[k].h=22;
  k++;
  // CAP
  box_list[k].key=' ';
  box_list[k].key_sh=' ';
  box_list[k].color = ~bruceConfig.bgColor;
  box_list[k].x=55;
  box_list[k].y=0;
  box_list[k].w=50;
  box_list[k].h=22;
  k++;
  // DEL
  box_list[k].key=' ';
  box_list[k].key_sh=' ';
  box_list[k].color = ~bruceConfig.bgColor;
  box_list[k].x=107;
  box_list[k].y=0;
  box_list[k].w=50;
  box_list[k].h=22;
  k++;
  // SPACE
  box_list[k].key=' ';
  box_list[k].key_sh=' ';
  box_list[k].color = ~bruceConfig.bgColor;
  box_list[k].x=159;
  box_list[k].y=0;
  box_list[k].w=tftWidth-164;
  box_list[k].h=22;

  k=0;
  x2=0;
  y2=0;
#endif

  int i=0;
  int j=-1;
  bool redraw=true;
  delay(200);
  int cX =0;
  int cY =0;
  tft.fillScreen(bruceConfig.bgColor);
  while(1) {
    if(redraw) {
      tft.setCursor(0,0);
      tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
      tft.setTextSize(FM);

      //Draw the rectangles
      if(y<0) {
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
      #if defined(HAS_TOUCH)
      TouchFooter();
      #endif
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

    /* When Select a key in keyboard */
    char keyValue = 0;
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0) {
        keyValue = Wire.read();
    }
    if (keyValue != (char)0x00) {
        Serial.print("keyValue : ");
        Serial.print(keyValue);
        Serial.print(" -> Hex  0x");
        Serial.println(keyValue,HEX);
        wakeUpScreen();
        tft.setCursor(cX,cY);

        if(mytext.length()<maxSize && keyValue!=0x08 && keyValue!=0x0D) {
          mytext += keyValue;
          if(mytext.length()!=20 && mytext.length()!=20) tft.print(keyValue);
          cX=tft.getCursorX();
          cY=tft.getCursorY();
          if(mytext.length()==20) redraw = true;
          if(mytext.length()==39) redraw = true;
        }
        if (keyValue==0x08 && mytext.length() > 0) { // delete 0x08
          // Handle backspace key
          mytext.remove(mytext.length() - 1);
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
          if(mytext.length()==19) redraw = true;
          if(mytext.length()==38) redraw = true;        
        }
        if (keyValue==0x0D) {
          break;
        }
        //delay(200);
    }
    if(checkSelPress()) break;
    
    delay(5);

  }

  //Resets screen when finished writing
  tft.fillRect(0,0,tftWidth,tftHeight,bruceConfig.bgColor);
  resetTftDisplay();

  return mytext;
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
** Function: _checkKeyPress
** location: mykeyboard.cpp
** returns the key from the keyboard
** ISSUES: Usb-HID and BLE-HID need HID Mapping
**********************************************************************/
keyStroke _getKeyPress() { 
    keyStroke key;
    char keyValue = 0;
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0) {
        keyValue = Wire.read();
    }
    if (keyValue!=(char)0x00) {
        wakeUpScreen();
        //for (auto i : status.hid_keys) key.hid_keys.push_back(i);
        key.hid_keys.push_back(keyValue);
        if(keyValue==' ') key.exit_key=true; // key pressed to try to exit
        //for (auto i : status.modifier_keys) key.modifier_keys.push_back(i);
        if (keyValue==(char)0x08)     key.del=true;
        if (keyValue==(char)0x0D)     key.enter=true;
        if (digitalRead(SEL_BTN)==BTN_ACT)      key.fn=true;
        key.word.push_back(keyValue);
        key.pressed=true;
    } else key.pressed=false;

    return key;
 } // must return something that the keyboards won´t recognize by default

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