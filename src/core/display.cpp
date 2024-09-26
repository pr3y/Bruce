#include "display.h"
#include "mykeyboard.h"
#include "wg.h" //for isConnectedWireguard to print wireguard lock
#include "settings.h" //for timeStr
#include "modules/others/webInterface.h" // for server
#include <JPEGDecoder.h>

#define MAX_MENU_SIZE (int)(HEIGHT/25)

#if defined(CARDPUTER) || defined(STICK_C_PLUS2)  //Battery Calculation
  #include <driver/adc.h>
  #include <esp_adc_cal.h>
  #include <soc/soc_caps.h>
  #include <soc/adc_channel.h>
#endif

/***************************************************************************************
** Function name: TouchFooter
** Description:   Draw touch screen footer
***************************************************************************************/
void TouchFooter(uint16_t color) {
  tft.drawRoundRect(5,HEIGHT+2,WIDTH-10,43,5,color);
  tft.setTextColor(color);
  tft.setTextSize(FM);
  tft.drawCentreString("PREV",WIDTH/6,HEIGHT+4,1);
  tft.drawCentreString("SEL",WIDTH/2,HEIGHT+4,1);
  tft.drawCentreString("NEXT",5*WIDTH/6,HEIGHT+4,1);
}
/***************************************************************************************
** Function name: TouchFooter
** Description:   Draw touch screen footer
***************************************************************************************/
void MegaFooter(uint16_t color) {
  tft.drawRoundRect(5,HEIGHT+2,WIDTH-10,43,5,color);
  tft.setTextColor(color);
  tft.setTextSize(FM);
  tft.drawCentreString("Exit",WIDTH/6,HEIGHT+4,1);
  tft.drawCentreString("UP",WIDTH/2,HEIGHT+4,1);
  tft.drawCentreString("DOWN",5*WIDTH/6,HEIGHT+4,1);
}

/***************************************************************************************
** Function name: resetTftDisplay
** Description:   set cursor to 0,0, screen and text to default color
***************************************************************************************/
void resetTftDisplay(int x, int y, uint16_t fc, int size, uint16_t bg, uint16_t screen) {
    tft.setCursor(x,y);
    tft.fillScreen(screen);
    tft.setTextSize(size);
    tft.setTextColor(fc,bg);
}

/***************************************************************************************
** Function name: setTftDisplay
** Description:   set cursor, font color, size and bg font color
***************************************************************************************/
void setTftDisplay(int x, int y, uint16_t fc, int size, uint16_t bg) {
    if (x>=0 && y<0)        tft.setCursor(x,tft.getCursorY());          // if -1 on x, sets only y
    else if (x<0 && y>=0)   tft.setCursor(tft.getCursorX(),y);          // if -1 on y, sets only x
    else if (x>=0 && y>=0)  tft.setCursor(x,y);                         // if x and y > 0, sets both
    tft.setTextSize(size);
    tft.setTextColor(fc,bg);
}

/***************************************************************************************
** Function name: displayRedStripe
** Description:   Display Red Stripe with information
***************************************************************************************/
void displayRedStripe(String text, uint16_t fgcolor, uint16_t bgcolor) {
    // detect if not running in interactive mode -> show nothing onscreen and return immediately
    if(server || isSleeping || isScreenOff) return;  // webui is running

    int size;
    if(fgcolor==bgcolor && fgcolor==TFT_WHITE) fgcolor=TFT_BLACK;
    if(text.length()*LW*FM<(WIDTH-2*FM*LW)) size = FM;
    else size = FP;
    tft.fillSmoothRoundRect(10,HEIGHT/2-13,WIDTH-20,26,7,bgcolor);
    tft.fillSmoothRoundRect(10,HEIGHT/2-13,WIDTH-20,26,7,bgcolor);
    tft.setTextColor(fgcolor,bgcolor);
    if(size==FM) {
      tft.setTextSize(FM);
      tft.setCursor(WIDTH/2 - FM*3*text.length(), HEIGHT/2-8);
    }
    else {
      tft.setTextSize(FP);
      tft.setCursor(WIDTH/2 - FP*3*text.length(), HEIGHT/2-8);
    }
    tft.println(text);
}

void displayError(String txt)   { 
  #ifndef HAS_SCREEN
    Serial.println("ERR: " + txt);
    return;
  #endif
  displayRedStripe(txt);
  delay(200);
  while(!checkAnyKeyPress()) delay(100);
}

void displayWarning(String txt) {
  #ifndef HAS_SCREEN
    Serial.println("WARN: " + txt);
    return;
  #endif
  displayRedStripe(txt, TFT_BLACK,TFT_YELLOW);
  delay(200);
  while(!checkAnyKeyPress()) delay(100);
}

void displayInfo(String txt)    {
  #ifndef HAS_SCREEN
    Serial.println("INFO: " + txt);
    return;
  #endif
  // todo: add newlines to txt if too long
  displayRedStripe(txt, TFT_WHITE, TFT_BLUE);
  delay(200);
  while(!checkAnyKeyPress()) delay(100);
}

void displaySuccess(String txt) {
  #ifndef HAS_SCREEN
    Serial.println("SUCCESS: " + txt);
    return;
  #endif
  // todo: add newlines to txt if too long
  displayRedStripe(txt, TFT_WHITE, TFT_DARKGREEN);
  delay(200);
  while(!checkAnyKeyPress()) delay(100);
}

void setPadCursor(int16_t padx, int16_t pady) {
  for (int y=0; y<pady; y++) tft.println();
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
}

void padprintf(int16_t padx, const char *format, ...) {
  char buffer[64];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.printf("%s", buffer);
}
void padprintf(const char *format, ...) {
  char buffer[64];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  tft.setCursor(BORDER_PAD_X, tft.getCursorY());
  tft.printf("%s", buffer);
}

void padprint(const String &s, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(s);
}
void padprint(const char str[], int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(str);
}
void padprint(char c, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(c);
}
void padprint(unsigned char b, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(b, base);
}
void padprint(int n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, base);
}
void padprint(unsigned int n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, base);
}
void padprint(long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, base);
}
void padprint(unsigned long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, base);
}
void padprint(long long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, base);
}
void padprint(unsigned long long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, base);
}
void padprint(double n, int digits, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.print(n, digits);
}

void padprintln(const String &s, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(s);
}
void padprintln(const char str[], int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(str);
}
void padprintln(char c, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(c);
}
void padprintln(unsigned char b, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(b, base);
}
void padprintln(int n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, base);
}
void padprintln(unsigned int n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, base);
}
void padprintln(long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, base);
}
void padprintln(unsigned long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, base);
}
void padprintln(long long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, base);
}
void padprintln(unsigned long long n, int base, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, base);
}
void padprintln(double n, int digits, int16_t padx) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(n, digits);
}

/*********************************************************************
**  Function: loopOptions
**  Where you choose among the options in menu
**********************************************************************/
int loopOptions(std::vector<Option>& options, bool bright, bool submenu, String subText,int index){
  bool redraw = true;
  int menuSize = options.size();
  if(options.size()>MAX_MENU_SIZE) {
    menuSize = MAX_MENU_SIZE;
    }
  if(index>0) tft.fillRoundRect(WIDTH*0.10,HEIGHT/2-menuSize*(FM*8+4)/2 -5,WIDTH*0.8,(FM*8+4)*menuSize+10,5,BGCOLOR);
  if(index>=options.size()) index=0;
  while(1){
    if (redraw) {
      if(submenu) drawSubmenu(index, options, subText);
      else drawOptions(index, options, FGCOLOR, BGCOLOR);
      if(bright){
        setBrightness(String(options[index].label.c_str()).toInt(),false);
      }
      redraw=false;
      delay(200);
    }

    if(checkPrevPress()) {
    #ifdef CARDPUTER
      if(index==0) index = options.size() - 1;
      else if(index>0) index--;
      redraw = true;
    #else
      break;
    #endif
    }
    /* DW Btn to next item */
    if(checkNextPress()) {
      index++;
      if((index+1)>options.size()) index = 0;
      redraw = true;
    }

    /* Select and run function */
    if(checkSelPress()) {
      Serial.println("Selecionado " + String(options[index].label.c_str()));
      options[index].operation();
      break;
    }

    #ifdef CARDPUTER
      if(checkEscPress()) break;
      int pressed_number = checkNumberShortcutPress();
      if(pressed_number>=0) {
        if(index == pressed_number) {
          // press 2 times the same number to confirm
          options[index].operation();
          break;
        }
        // else only highlight the option
        index = pressed_number;
        if((index+1)>options.size()) index = options.size() - 1;
        redraw = true;
      }
    #endif
  }
  delay(200);
  return index;
}

/***************************************************************************************
** Function name: progressHandler
** Description:   Função para manipular o progresso da atualização
** Dependencia: prog_handler =>>    0 - Flash, 1 - LittleFS
***************************************************************************************/
void progressHandler(int progress, size_t total, String message) {
  int barWidth = map(progress, 0, total, 0, 200);
  if(barWidth <3) {
    tft.fillRect(6, 27, WIDTH-12, HEIGHT-33, BGCOLOR);
    tft.drawRect(18, HEIGHT - 47, 204, 17, FGCOLOR);
    displayRedStripe(message, TFT_WHITE, FGCOLOR);
  }
  tft.fillRect(20, HEIGHT - 45, barWidth, 13, FGCOLOR);
}

/***************************************************************************************
** Function name: drawOptions
** Description:   Função para desenhar e mostrar as opçoes de contexto
***************************************************************************************/
void drawOptions(int index,std::vector<Option>& options, uint16_t fgcolor, uint16_t bgcolor) {
    int menuSize = options.size();
    if(options.size()>MAX_MENU_SIZE) {
      menuSize = MAX_MENU_SIZE;
      }

    if(index==0) tft.fillRoundRect(WIDTH*0.10,HEIGHT/2-menuSize*(FM*8+4)/2 -5,WIDTH*0.8,(FM*8+4)*menuSize+10,5,bgcolor);

    tft.setTextColor(fgcolor,bgcolor);
    tft.setTextSize(FM);
    tft.setCursor(WIDTH*0.10+5,HEIGHT/2-menuSize*(FM*8+4)/2);

    int i=0;
    int init = 0;
    int cont = 1;
    if(index==0) tft.fillRoundRect(WIDTH*0.10,HEIGHT/2-menuSize*(FM*8+4)/2 -5,WIDTH*0.8,(FM*8+4)*menuSize+10,5,bgcolor);
    menuSize = options.size();
    if(index>=MAX_MENU_SIZE) init=index-MAX_MENU_SIZE+1;
    for(i=0;i<menuSize;i++) {
      if(i>=init) {
        if(options[i].selected) tft.setTextColor(fgcolor-0x1111,bgcolor); // if selected, change Text color
        else tft.setTextColor(fgcolor,bgcolor);

        String text="";
        if(i==index) text+=">";
        else text +=" ";
        text += String(options[i].label.c_str()) + "              ";
        tft.setCursor(WIDTH*0.10+5,tft.getCursorY()+4);
        tft.println(text.substring(0,(WIDTH*0.8 - 10)/(LW*FM) - 1));
        cont++;
      }
      if(cont>MAX_MENU_SIZE) goto Exit;
    }
    Exit:
    if(options.size()>MAX_MENU_SIZE) menuSize = MAX_MENU_SIZE;
    tft.drawRoundRect(WIDTH*0.10,HEIGHT/2-menuSize*(FM*8+4)/2 -5,WIDTH*0.8,(FM*8+4)*menuSize+10,5,fgcolor);
    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
}

/***************************************************************************************
** Function name: drawOptions
** Description:   Função para desenhar e mostrar as opçoes de contexto
***************************************************************************************/
void drawSubmenu(int index,std::vector<Option>& options, String system) {
    int menuSize = options.size();
    if(index==0) drawMainBorder();
    tft.setTextColor(FGCOLOR,BGCOLOR);
    tft.fillRect(6,26,WIDTH-12,20,BGCOLOR);
    tft.fillRoundRect(6,26,WIDTH-12,HEIGHT-32,5,BGCOLOR);
    tft.setTextSize(FP);
    tft.setCursor(12,30);
    tft.setTextColor(FGCOLOR);
    tft.println(system);

    if (index-1>=0) {
      tft.setTextSize(FM);
      tft.setTextColor(FGCOLOR-0x2000);
      tft.drawCentreString(options[index-1].label.c_str(),WIDTH/2, 42+(HEIGHT-134)/2,SMOOTH_FONT);
    } else {
      tft.setTextSize(FM);
      tft.setTextColor(FGCOLOR-0x2000);
      tft.drawCentreString(options[menuSize-1].label.c_str(),WIDTH/2, 42+(HEIGHT-134)/2,SMOOTH_FONT);
    }
      tft.setTextSize(FG);
      tft.setTextColor(FGCOLOR);
      tft.drawCentreString(options[index].label.c_str(),WIDTH/2, 67+(HEIGHT-134)/2,SMOOTH_FONT);

    if (index+1<menuSize) {
      tft.setTextSize(FM);
      tft.setTextColor(FGCOLOR-0x2000);
      tft.drawCentreString(options[index+1].label.c_str(),WIDTH/2, 102+(HEIGHT-134)/2,SMOOTH_FONT);
    } else {
      tft.setTextSize(FM);
      tft.setTextColor(FGCOLOR-0x2000);
      tft.drawCentreString(options[0].label.c_str(),WIDTH/2, 102+(HEIGHT-134)/2,SMOOTH_FONT);
    }
    tft.drawFastHLine(WIDTH/2 - options[index].label.size()*FG*LW/2, 67+FG*LH+(HEIGHT-134)/2,options[index].label.size()*FG*LW,FGCOLOR);
    tft.fillRect(WIDTH-5,0,5,HEIGHT,BGCOLOR);
    tft.fillRect(WIDTH-5,index*HEIGHT/menuSize,5,HEIGHT/menuSize,FGCOLOR);

    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif

}

void drawMainBorder(bool clear) {
    #if defined(HAS_RTC)
      cplus_RTC _rtc;
      RTC_TimeTypeDef _time;
    #endif
    if(clear){
      tft.fillScreen(BGCOLOR);
      tft.fillScreen(BGCOLOR);
    }
    setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);

    // if(wifiConnected) {tft.print(timeStr);} else {tft.print("BRUCE 1.0b");}

    int i=0;
    drawBatteryStatus();
    if(sdcardMounted) { tft.setTextColor(FGCOLOR, BGCOLOR); tft.setTextSize(FP); tft.drawString("SD", WIDTH - (85 + 20*i),12); i++; } // Indication for SD card on screen
    if(gpsConnected) { drawGpsSmall(WIDTH - (85 + 20*i), 7); i++; }
    if(wifiConnected) { drawWifiSmall(WIDTH - (85 + 20*i), 7); i++;}               //Draw Wifi Symbol beside battery
    if(BLEConnected) { drawBLESmall(WIDTH - (85 + 20*i), 7); i++; }       //Draw BLE beside Wifi
    if(isConnectedWireguard) { drawWireguardStatus(WIDTH - (85 + 21*i), 7); i++; }//Draw Wg bedide BLE, if the others exist, if not, beside battery


    tft.drawRoundRect(5, 5, WIDTH - 10, HEIGHT - 10, 5, FGCOLOR);
    tft.drawLine(5, 25, WIDTH - 6, 25, FGCOLOR);
    if (clock_set) {
        setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);
      #if defined(HAS_RTC)
        _rtc.GetTime(&_time);
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", _time.Hours, _time.Minutes);
        tft.print(timeStr);
      #else
        updateTimeStr(rtc.getTimeStruct());
        tft.print(timeStr);
      #endif
    }
    else {
      setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);
      tft.print("BRUCE " + String(BRUCE_VERSION));
    }
    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
}

void drawMainBorderWithTitle(String title, bool clear) {
  drawMainBorder(clear);

  tft.setCursor(BORDER_PAD_X, BORDER_PAD_Y);
  tft.setTextColor(FGCOLOR, BGCOLOR);
  tft.setTextSize(FM);
  padprintln(title);

  tft.setTextSize(FP);
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
  int percent=0;
  #if defined(STICK_C_PLUS)
  float b = axp192.GetBatVoltage();
  percent = ((b - 3.0) / 1.2) * 100;

  #elif defined(CARDPUTER) || defined(STICK_C_PLUS2)

    #if defined(CARDPUTER)
      uint8_t _batAdcCh = ADC1_GPIO10_CHANNEL;
      uint8_t _batAdcUnit = 1;
    #elif defined(STICK_C_PLUS2)
      uint8_t _batAdcCh = ADC1_GPIO38_CHANNEL;
      uint8_t _batAdcUnit = 1;
    #endif

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

  //#elif defined(NEW_DEVICE)
  #elif defined(CORE2)
    percent = M5.Axp.GetBatteryLevel();
  #elif defined(M5STACK)
    percent = M5.Power.getBatteryLevel();
  #else
  percent = 0;

  #endif
  return  (percent < 0) ? 0
        : (percent >= 100) ? 100
        :  percent;
}

/***************************************************************************************
** Function name: drawBatteryStatus()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
void drawBatteryStatus() {
    tft.drawRoundRect(WIDTH - 42, 7, 34, 17, 2, FGCOLOR);
    int bat = getBattery();
    tft.setTextSize(FP);
    tft.setTextColor(FGCOLOR, BGCOLOR);
    tft.drawRightString((bat==100 ? "" : " ")  + String(bat) + "%", WIDTH - 45, 12, 1);
    tft.fillRoundRect(WIDTH - 40, 9, 30 * bat / 100, 13, 2, FGCOLOR);
    tft.drawLine(WIDTH - 30, 9, WIDTH - 30, 9 + 13, BGCOLOR);
    tft.drawLine(WIDTH - 20, 9, WIDTH - 20, 9 + 13, BGCOLOR);
}

/***************************************************************************************
** Function name: drawWireguardStatus()
** Description:   Draws a padlock when connected
***************************************************************************************/
void drawWireguardStatus(int x, int y) {
  tft.fillRect(x,y,20,17,BGCOLOR);
    if(isConnectedWireguard){
        tft.drawRoundRect(10+x, 0+y, 10, 16, 5, TFT_GREEN);
        tft.fillRoundRect(10+x, 12+y, 10, 5, 0, TFT_GREEN);
    } else {
    tft.drawRoundRect(1+x, 0+y, 10, 16, 5, FGCOLOR);
    tft.fillRoundRect(0+x, 12+y, 10, 5, 0, BGCOLOR);
    tft.fillRoundRect(10+x, 12+y, 10, 5, 0, FGCOLOR);
    }

}

/***************************************************************************************
** Function name: listFiles
** Description:   Função para desenhar e mostrar o menu principal
***************************************************************************************/
#define MAX_ITEMS (int)(HEIGHT-20)/(LH*2)
void listFiles(int index, std::vector<FileList> fileList) {
    if(index==0){
      tft.fillScreen(BGCOLOR);
      tft.fillScreen(BGCOLOR);
    }
    tft.setCursor(10,10);
    tft.setTextSize(FM);
    int i=0;
    int arraySize = fileList.size();
    int start=0;
    if(index>=MAX_ITEMS) {
        start=index-MAX_ITEMS+1;
        if(start<0) start=0;
    }
    int nchars = (WIDTH-20)/(6*tft.textsize);
    String txt=">";
    while(i<arraySize) {
        if(i>=start) {
            tft.setCursor(10,tft.getCursorY());
            if(fileList[i].folder==true) tft.setTextColor(FGCOLOR-0x1111, BGCOLOR);
            else if(fileList[i].operation==true) tft.setTextColor(ALCOLOR, BGCOLOR);
            else { tft.setTextColor(FGCOLOR,BGCOLOR); }

            if (index==i) txt=">";
            else txt=" ";
            txt+=fileList[i].filename + "                 ";
            tft.println(txt.substring(0,nchars));
        }
        i++;
        if (i==(start+MAX_ITEMS) || i==arraySize) break;
    }
    tft.drawRoundRect(5, 5, WIDTH - 10, HEIGHT - 10, 5, FGCOLOR);
    tft.drawRoundRect(5, 5, WIDTH - 10, HEIGHT - 10, 5, FGCOLOR);

}


// desenhos do menu principal, sprite "draw" com 80x80 pixels

void drawWifiSmall(int x, int y) {
  tft.fillRect(x,y,17,17,BGCOLOR);
  tft.fillCircle(9+x,14+y,2,FGCOLOR);
  tft.drawArc(9+x,14+y,5,7,130,230,FGCOLOR, BGCOLOR);
  tft.drawArc(9+x,14+y,11,13,130,230,FGCOLOR, BGCOLOR);
}

void drawBLESmall(int x, int y) {
  tft.fillRect(x,y,17,17,BGCOLOR);
  tft.drawWideLine(8+x, 8+y, 4+x, 5+y, 2, FGCOLOR,BGCOLOR);
  tft.drawWideLine(8+x, 8+y, 4+x,13+y, 2, FGCOLOR,BGCOLOR);
  tft.drawTriangle(8+x, 8+y, 8+x, 0+y,13+x,4+y,FGCOLOR);
  tft.drawTriangle(8+x, 8+y, 8+x,16+y,13+x,12+y,FGCOLOR);
}

void drawBLE_beacon(int x, int y, uint16_t color) {
  tft.fillRect(x,y,40,80,BGCOLOR);
  tft.drawWideLine(40+x,53+y,2+x,26+y,5,color,BGCOLOR);
  tft.drawWideLine(40+x,26+y,2+x,53+y,5,color,BGCOLOR);
  tft.drawWideLine(40+x,53+y,20+x,68+y,5,color,BGCOLOR);
  tft.drawWideLine(40+x,26+y,20+x,12+y,5,color,BGCOLOR);
  tft.drawWideLine(20+x,12+y,20+x,68+y,5,color,BGCOLOR);
  tft.fillTriangle(40+x,26+y,20+x,40+y,20+x,12+y,color);
  tft.fillTriangle(40+x,53+y,20+x,40+y,20+x,68+y,color);
}

void drawGPS(int x, int y) {
  tft.fillRect(x,y,80,80,BGCOLOR);
  tft.drawEllipse(40+x,70+y,15,8,FGCOLOR);
  tft.drawArc(40+x,25+y,23,7,0,340,FGCOLOR,BGCOLOR);
  tft.fillTriangle(40+x,70+y,20+x,64+y,60+x,64+y,FGCOLOR);
}

void drawGpsSmall(int x, int y) {
  tft.fillRect(x,y,17,17,BGCOLOR);
  tft.drawEllipse(9+x,14+y,4,3,FGCOLOR);
  tft.drawArc(9+x,6+y,5,2,0,340,FGCOLOR,BGCOLOR);
  tft.fillTriangle(9+x,15+y,5+x,9+y,13+x,9+y,FGCOLOR);
}





//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// from:  https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Generic/ESP32_SDcard_jpeg/ESP32_SDcard_jpeg.ino
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void jpegRender(int xpos, int ypos) {

  //jpegInfo(); // Print information from the JPEG file (could comment this line out)

  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);
  
  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
  uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // Fetch data from the file, decode and display
  tft.fillRect(xpos,ypos,JpegDec.width,JpegDec.height,TFT_BLACK);
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
      tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    else if ( (mcu_y + win_h) > tft.height())
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }

  tft.setSwapBytes(swapBytes);

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime; // Calculate the time it took

  // print the results to the serial port
  Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  Serial.println("=====================================");

}


bool showJpeg(FS fs, String filename, int x, int y) {
  File picture;
  if(fs.exists(filename)) 
    picture = fs.open(filename, FILE_READ);
  else 
    return false;

  const size_t data_size = picture.size();
  
  // Alloc memory into heap
  uint8_t* data_array = new uint8_t[data_size];
  if (data_array == nullptr) {
    // Fail allocating memory
    picture.close();
    return false;
  }

  uint8_t data;
  int i = 0;
  byte line_len = 0;
  
  while (picture.available()) {
    data = picture.read();
    data_array[i] = data; 
    i++;

    //print array on Serial
    /*
    Serial.print("0x"); 
    if (abs(data) < 16) {
      Serial.print("0"); 
    }
    
    Serial.print(data, HEX); 
    Serial.print(","); // Add value and comma
    line_len++;
    if (line_len >= 32) {
      line_len = 0;
      Serial.println();
    }
    */
  }  
  
  picture.close();

  bool decoded = false;
  if (data_array) {
    decoded = JpegDec.decodeArray(data_array, data_size);
  } else { 
    displayError(filename + " Fail");
    delay(2500);
    delete[] data_array; // free heap before leaving
    return false;
  }
  
  if (decoded) {
    jpegRender(x, y);
  }
  
  delete[] data_array; // free heap before leaving
  return true; 
}

