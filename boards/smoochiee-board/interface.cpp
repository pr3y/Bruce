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
    pinMode(45, OUTPUT);
   
   
    digitalWrite(45,HIGH);
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
    int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bruceConfig.bright/100 ));
    analogWrite(TFT_BL, bl);
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

    if(digitalRead(SEL_BTN)==BTN_ACT || digitalRead(UP_BTN)==BTN_ACT || digitalRead(DW_BTN)==BTN_ACT || digitalRead(R_BTN)==BTN_ACT || digitalRead(L_BTN)==BTN_ACT) {
        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }    
    if(digitalRead(L_BTN)==BTN_ACT) {
        PrevPress = true;
    }
    if(digitalRead(DW_BTN)==BTN_ACT) {
        EscPress = true;
    }
    if(digitalRead(R_BTN)==BTN_ACT) {
        NextPress = true;
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
  char keys[4][12][2] = { //4 lines, with 12 characteres, BTN_ACT and high caps
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

      delay(200);
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
    int z=0;

    if(check(SelPress))  {
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
    }

    /* Down Btn to move in X axis (to the right) */
    if(digitalRead(DW_BTN)==BTN_ACT)
    {
      x++;
      if(y<0 && x>3) x=0;
      if(x>11) x=0;
      else if (x<0) x=11;
      redraw = true;
    }
    if(digitalRead(UP_BTN)==BTN_ACT)
    {
      x--;
      if(y<0 && x>3) x=0;
      if(x>11) x=0;
      else if (x<0) x=11;
      redraw = true;
    }
    /* UP Btn to move in Y axis (Downwards) */
    if(digitalRead(R_BTN)==BTN_ACT) {    
      y++;
      if(y>3) { y=-1; }
      else if(y<-1) y=3;
      redraw = true;
    }
    if(digitalRead(L_BTN)==BTN_ACT) {    
      y--;
      if(y>3) { y=-1; }
      else if(y<-1) y=3;
      redraw = true;
    }
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