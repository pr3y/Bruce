#include "display.h"
#include "utils.h"
#include "mykeyboard.h" 
#include "wg.h" //for isConnectedWireguard to print wireguard lock
#include "settings.h" //for timeStr
#include "modules/others/webInterface.h" // for server
#include <JPEGDecoder.h>

#define MAX_MENU_SIZE (int)(tftHeight/25)

/***************************************************************************************
** Function name: displayScrollingText
** Description:   Scroll large texts into screen
***************************************************************************************/
void displayScrollingText(const String& text, Opt_Coord& coord) {
  int len = text.length();
  String displayText = text + "        "; // Add spaces for smooth looping
  int scrollLen = len + 8; // Full text plus space buffer
  static int i=0;
  static long _lastmillis=0;
  tft.setTextColor(coord.fgcolor,coord.bgcolor);
  if (len < coord.size) {
    // Text fits within limit, no scrolling needed
    return;
  } else if(millis()>_lastmillis+200) {
    String scrollingPart = displayText.substring(i, i + (coord.size - 1)); // Display charLimit characters at a time
    tft.fillRect(coord.x, coord.y, (coord.size-1) * LW * tft.textsize, LH * tft.textsize, bruceConfig.bgColor); // Clear display area
    tft.setCursor(coord.x, coord.y);
    tft.setCursor(coord.x, coord.y);
    tft.print(scrollingPart);
    if (i >= scrollLen - coord.size) i = -1; // Loop back
    _lastmillis=millis();
    i++;
    if(i==1) _lastmillis=millis()+1000;
  }
}

/***************************************************************************************
** Function name: TouchFooter
** Description:   Draw touch screen footer
***************************************************************************************/
void TouchFooter(uint16_t color) {
  tft.drawRoundRect(5,tftHeight+2,tftWidth-10,43,5,color);
  tft.setTextColor(color);
  tft.setTextSize(FM);
  tft.drawCentreString("PREV",tftWidth/6,tftHeight+4,1);
  tft.drawCentreString("SEL",tftWidth/2,tftHeight+4,1);
  tft.drawCentreString("NEXT",5*tftWidth/6,tftHeight+4,1);
}
/***************************************************************************************
** Function name: TouchFooter
** Description:   Draw touch screen footer
***************************************************************************************/
void MegaFooter(uint16_t color) {
  tft.drawRoundRect(5,tftHeight+2,tftWidth-10,43,5,color);
  tft.setTextColor(color);
  tft.setTextSize(FM);
  tft.drawCentreString("Exit",tftWidth/6,tftHeight+4,1);
  tft.drawCentreString("UP",tftWidth/2,tftHeight+4,1);
  tft.drawCentreString("DOWN",5*tftWidth/6,tftHeight+4,1);
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

void turnOffDisplay() {
  setBrightness(0,false);
}

bool wakeUpScreen(){
  previousMillis = millis();
  if(isScreenOff){
    isScreenOff = false;
    dimmer = false;
    getBrightness();
    delay(200);
    return true;
  }else if(dimmer){
    dimmer = false;
    getBrightness();
    delay(200);
    return true;
  }
  return false;
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
    if(text.length()*LW*FM<(tftWidth-2*FM*LW)) size = FM;
    else size = FP;
    tft.fillSmoothRoundRect(10,tftHeight/2-13,tftWidth-20,26,7,bgcolor);
    tft.fillSmoothRoundRect(10,tftHeight/2-13,tftWidth-20,26,7,bgcolor);
    tft.setTextColor(fgcolor,bgcolor);
    if(size==FM) {
      tft.setTextSize(FM);
      tft.setCursor(tftWidth/2 - FM*3*text.length(), tftHeight/2-8);
    }
    else {
      tft.setTextSize(FP);
      tft.setCursor(tftWidth/2 - FP*3*text.length(), tftHeight/2-8);
    }
    tft.println(text);
}

void displayError(String txt, bool waitKeyPress)   {
  #ifndef HAS_SCREEN
    Serial.println("ERR: " + txt);
    return;
  #endif
  displayRedStripe(txt);
  delay(200);
  while(waitKeyPress && !check(AnyKeyPress)) delay(100);
}

void displayWarning(String txt, bool waitKeyPress) {
  #ifndef HAS_SCREEN
    Serial.println("WARN: " + txt);
    return;
  #endif
  displayRedStripe(txt, TFT_BLACK,TFT_YELLOW);
  delay(200);
  while(waitKeyPress && !check(AnyKeyPress)) delay(100);
}

void displayInfo(String txt, bool waitKeyPress)    {
  #ifndef HAS_SCREEN
    Serial.println("INFO: " + txt);
    return;
  #endif
  // todo: add newlines to txt if too long
  displayRedStripe(txt, TFT_WHITE, TFT_BLUE);
  delay(200);
  while(waitKeyPress && !check(AnyKeyPress)) delay(100);
}

void displaySuccess(String txt, bool waitKeyPress) {
  #ifndef HAS_SCREEN
    Serial.println("SUCCESS: " + txt);
    return;
  #endif
  // todo: add newlines to txt if too long
  displayRedStripe(txt, TFT_WHITE, TFT_DARKGREEN);
  delay(200);
  while(waitKeyPress && !check(AnyKeyPress)) delay(100);
}

void displayTextLine(String txt, bool waitKeyPress) {
  #ifndef HAS_SCREEN
    Serial.println("MESSAGE: " + txt);
    return;
  #endif
  // todo: add newlines to txt if too long
  displayRedStripe(txt, getComplementaryColor2(bruceConfig.priColor), bruceConfig.priColor);
  delay(200);
  while(waitKeyPress && !check(AnyKeyPress)) delay(100);
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
  if (s.isEmpty()) {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(s);
    return;
  }

  String buff;
  size_t start = 0;
  int _maxCharsInLine = (tftWidth - (padx+1) * BORDER_PAD_X) / (FP*LW);

  // automatically split into multiple lines
  while( !(buff = s.substring(start, start + _maxCharsInLine)).isEmpty() ){
    tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
    tft.println(buff);
    start += buff.length();
  }
}
void padprintln(const char str[], int16_t padx) {
  if (str == "") {
  tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
  tft.println(str);
    return;
  }

  String buff;
  size_t start = 0;
  int _maxCharsInLine = (tftWidth - (padx+1) * BORDER_PAD_X) / (FP*LW);

  // automatically split into multiple lines
  while( !(buff = String(str).substring(start, start + _maxCharsInLine)).isEmpty() ){
    tft.setCursor(padx * BORDER_PAD_X, tft.getCursorY());
    tft.println(buff);
    start += buff.length();
  }
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
  Opt_Coord coord;
  bool redraw = true;
  int menuSize = options.size();
  if(options.size()>MAX_MENU_SIZE) {
    menuSize = MAX_MENU_SIZE;
    }
  if(index>0) tft.fillRoundRect(tftWidth*0.10,tftHeight/2-menuSize*(FM*8+4)/2 -5,tftWidth*0.8,(FM*8+4)*menuSize+10,5,bruceConfig.bgColor);
  if(index>=options.size()) index=0;
  bool first=true;
  while(1){
    if (redraw) {
      if(submenu) drawSubmenu(index, options, subText);
      else coord=drawOptions(index, options, bruceConfig.priColor, bruceConfig.bgColor);
      if(bright){
        uint8_t bv = String(options[index].label.c_str()).toInt();  // Grabs the int value from menu option
        if(bv>0) setBrightness(bv,false);                           // If valid, apply brightnes
        else setBrightness(bruceConfig.bright,false);               // if "Main Menu", bv==0, return brightness to default
      }
      redraw=false;
      if(first) while(SelPress) delay(100); // to avoid miss click due to heavy fingers
    }
    if(!submenu) {
      String txt=options[index].label.c_str();
      displayScrollingText(txt, coord);
    }

    if(check(PrevPress) || check(UpPress)) {
    #ifdef HAS_KEYBOARD
      if(index==0) index = options.size() - 1;
      else if(index>0) index--;
      redraw = true;
    #else
    long _tmp=millis();
    while(check(PrevPress)) { if(millis()-_tmp>200) tft.drawArc(tftWidth/2, tftHeight/2, 25,15,0,360*(millis()-(_tmp+200))/500,getColorVariation(bruceConfig.priColor),bruceConfig.bgColor); }
    if(millis()-_tmp>700) { // longpress detected to exit
      break;
    }
    else {
      if(index==0) index = options.size() - 1;
      else if(index>0) index--;
      redraw = true;
    }
    #endif
    }
    /* DW Btn to next item */
    if(check(NextPress) || check(DownPress)) {
      index++;
      if((index+1)>options.size()) index = 0;
      redraw = true;
    }

    /* Select and run function */
    if(check(SelPress)) {
      Serial.println("Selected: " + String(options[index].label.c_str()));
      options[index].operation();
      break;
    }

    #ifdef HAS_KEYBOARD
      if(check(EscPress)) break;
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
    #elif defined(T_EMBED) || defined(HAS_TOUCH)
      if(check(EscPress)) break;
    #endif
  }
  while(SelPress) delay(100); // to avoid miss click due to heavy fingers
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
    tft.fillRect(6, 27, tftWidth-12, tftHeight-33, bruceConfig.bgColor);
    tft.drawRect(18, tftHeight - 47, 204, 17, bruceConfig.priColor);
    displayRedStripe(message, TFT_WHITE, bruceConfig.priColor);
  }
  tft.fillRect(20, tftHeight - 45, barWidth, 13, bruceConfig.priColor);
}

/***************************************************************************************
** Function name: drawOptions
** Description:   Função para desenhar e mostrar as opçoes de contexto
***************************************************************************************/
Opt_Coord drawOptions(int index,std::vector<Option>& options, uint16_t fgcolor, uint16_t bgcolor) {
    Opt_Coord coord;
    int menuSize = options.size();
    if(options.size()>MAX_MENU_SIZE) {
      menuSize = MAX_MENU_SIZE;
      }

    if(index==0) tft.fillRoundRect(tftWidth*0.10,tftHeight/2-menuSize*(FM*8+4)/2 -5,tftWidth*0.8,(FM*8+4)*menuSize+10,5,bgcolor);

    tft.setTextColor(fgcolor,bgcolor);
    tft.setTextSize(FM);
    tft.setCursor(tftWidth*0.10+5,tftHeight/2-menuSize*(FM*8+4)/2);

    int i=0;
    int init = 0;
    int cont = 1;
    if(index==0) tft.fillRoundRect(tftWidth*0.10,tftHeight/2-menuSize*(FM*8+4)/2 -5,tftWidth*0.8,(FM*8+4)*menuSize+10,5,bgcolor);
    menuSize = options.size();
    if(index>=MAX_MENU_SIZE) init=index-MAX_MENU_SIZE+1;
    for(i=0;i<menuSize;i++) {
      if(i>=init) {
        if(options[i].selected) tft.setTextColor(getColorVariation(fgcolor),bgcolor); // if selected, change Text color
        else tft.setTextColor(fgcolor,bgcolor);

        String text="";
        if(i==index) { 
          text+=">";
          coord.x=tftWidth*0.10+5+FM*LW;
          coord.y=tft.getCursorY()+4;
          coord.size=(tftWidth*0.8 - 10)/(LW*FM) - 1;
          coord.fgcolor=fgcolor;
          coord.bgcolor=bgcolor;
        }
        else text +=" ";
        text += String(options[i].label.c_str()) + "              ";
        tft.setCursor(tftWidth*0.10+5,tft.getCursorY()+4);
        tft.println(text.substring(0,(tftWidth*0.8 - 10)/(LW*FM) - 1));
        cont++;
      }
      if(cont>MAX_MENU_SIZE) goto Exit;
    }
    Exit:
    if(options.size()>MAX_MENU_SIZE) menuSize = MAX_MENU_SIZE;
    tft.drawRoundRect(tftWidth*0.10,tftHeight/2-menuSize*(FM*8+4)/2 -5,tftWidth*0.8,(FM*8+4)*menuSize+10,5,fgcolor);
    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
    return coord;
}

/***************************************************************************************
** Function name: drawOptions
** Description:   Função para desenhar e mostrar as opçoes de contexto
***************************************************************************************/
void drawSubmenu(int index,std::vector<Option>& options, String system) {
    int menuSize = options.size();
    if(index==0) drawMainBorder();
    tft.setTextColor(bruceConfig.priColor,bruceConfig.bgColor);
    tft.fillRect(6,26,tftWidth-12,20,bruceConfig.bgColor);
    tft.fillRoundRect(6,26,tftWidth-12,tftHeight-32,5,bruceConfig.bgColor);
    tft.setTextSize(FP);
    tft.setCursor(12,30);
    tft.setTextColor(bruceConfig.priColor);
    tft.println(system);

    if (index-1>=0) {
      tft.setTextSize(FM);
      tft.setTextColor(bruceConfig.secColor);
      tft.drawCentreString(options[index-1].label.c_str(),tftWidth/2, 42+(tftHeight-134)/2,SMOOTH_FONT);
    } else {
      tft.setTextSize(FM);
      tft.setTextColor(bruceConfig.secColor);
      tft.drawCentreString(options[menuSize-1].label.c_str(),tftWidth/2, 42+(tftHeight-134)/2,SMOOTH_FONT);
    }

    int selectedTextSize = options[index].label.length() <= tftWidth/(LW*FG)-1 ? FG : FM;
    tft.setTextSize(selectedTextSize);
    tft.setTextColor(bruceConfig.priColor);
    tft.drawCentreString(
      options[index].label.c_str(),
      tftWidth/2,
      67+(tftHeight-134)/2+((selectedTextSize-1)%2)*LH/2,
      SMOOTH_FONT
    );

    if (index+1<menuSize) {
      tft.setTextSize(FM);
      tft.setTextColor(bruceConfig.secColor);
      tft.drawCentreString(options[index+1].label.c_str(),tftWidth/2, 102+(tftHeight-134)/2,SMOOTH_FONT);
    } else {
      tft.setTextSize(FM);
      tft.setTextColor(bruceConfig.secColor);
      tft.drawCentreString(options[0].label.c_str(),tftWidth/2, 102+(tftHeight-134)/2,SMOOTH_FONT);
    }

    tft.drawFastHLine(
      tftWidth/2 - options[index].label.size()*selectedTextSize*LW/2,
      67+(tftHeight-134)/2+((selectedTextSize-1)%2)*LH/2+selectedTextSize*LH,
      options[index].label.size()*selectedTextSize*LW,
      bruceConfig.priColor
    );
    tft.fillRect(tftWidth-5,0,5,tftHeight,bruceConfig.bgColor);
    tft.fillRect(tftWidth-5,index*tftHeight/menuSize,5,tftHeight/menuSize,bruceConfig.priColor);

    #if defined(HAS_TOUCH)
    tft.drawCentreString("/\\",tftWidth/2,42+(tftHeight-134)/2-30,1);
    tft.drawCentreString("\\/",tftWidth/2,102+(tftHeight-134)/2+30,1);
    tft.setTextColor(getColorVariation(bruceConfig.priColor),bruceConfig.bgColor);
    tft.drawString("[ x ]",7,7,1);
    TouchFooter();
    #endif

}

void drawMainBorder(bool clear) {
    #if defined(HAS_RTC)
      cplus_RTC _rtc;
      RTC_TimeTypeDef _time;
    #endif
    if(clear){
      tft.fillScreen(bruceConfig.bgColor);
      tft.fillScreen(bruceConfig.bgColor);
    }
    setTftDisplay(12, 12, bruceConfig.priColor, 1, bruceConfig.bgColor);
    tft.setTextDatum(0);

    // if(wifiConnected) {tft.print(timeStr);} else {tft.print("BRUCE 1.0b");}

    int i=0;
    drawBatteryStatus();
    if(sdcardMounted) { tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor); tft.setTextSize(FP); tft.drawString("SD", tftWidth - (85 + 20*i),12); i++; } // Indication for SD card on screen
    if(gpsConnected) { drawGpsSmall(tftWidth - (85 + 20*i), 7); i++; }
    if(wifiConnected) { drawWifiSmall(tftWidth - (85 + 20*i), 7); i++;}               //Draw Wifi Symbol beside battery
    if(BLEConnected) { drawBLESmall(tftWidth - (85 + 20*i), 7); i++; }       //Draw BLE beside Wifi
    if(isConnectedWireguard) { drawWireguardStatus(tftWidth - (85 + 21*i), 7); i++; }//Draw Wg bedide BLE, if the others exist, if not, beside battery


    tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, bruceConfig.priColor);
    tft.drawLine(5, 25, tftWidth - 6, 25, bruceConfig.priColor);
    if (clock_set) {
        setTftDisplay(12, 12, bruceConfig.priColor, 1, bruceConfig.bgColor);
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
      setTftDisplay(12, 12, bruceConfig.priColor, 1, bruceConfig.bgColor);
      tft.print("BRUCE " + String(BRUCE_VERSION));
    }
    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
}

void drawMainBorderWithTitle(String title, bool clear) {
  drawMainBorder(clear);
  printTitle(title);
}

void printTitle(String title) {
  tft.setCursor((tftWidth - (title.length() * FM*LW)) / 2, BORDER_PAD_Y);
  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
  tft.setTextSize(FM);

  title.toUpperCase();
  tft.println(title);

  tft.setTextSize(FP);
}

void printSubtitle(String subtitle, bool withLine) {
  int16_t cursorX = (tftWidth - (subtitle.length() * FP*LW)) / 2;
  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
  tft.setTextSize(FP);

  tft.setCursor(cursorX, BORDER_PAD_Y + FM*LH);
  tft.println(subtitle);

  if (withLine) {
    String line = "";
    for (byte i = 0; i < subtitle.length(); i++) line += "-";

    tft.setCursor(cursorX, tft.getCursorY());
    tft.println(line);
  }
}

void printFootnote(String text) {
  tft.setTextSize(FP);
  tft.drawRightString(text, tftWidth-BORDER_PAD_X, tftHeight-BORDER_PAD_X-FP*LH, SMOOTH_FONT);
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
  int percent=0;

  return  (percent < 0) ? 0
        : (percent >= 100) ? 100
        :  percent;
}

/***************************************************************************************
** Function name: drawBatteryStatus()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
void drawBatteryStatus() {
    tft.drawRoundRect(tftWidth - 42, 7, 34, 17, 2, bruceConfig.priColor);
    int bat = getBattery();
    tft.setTextSize(FP);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.drawRightString((bat==100 ? "" : " ")  + String(bat) + "%", tftWidth - 45, 12, 1);
    tft.fillRoundRect(tftWidth - 40, 9, 30 * bat / 100, 13, 2, bruceConfig.priColor);
    tft.drawLine(tftWidth - 30, 9, tftWidth - 30, 9 + 13, bruceConfig.bgColor);
    tft.drawLine(tftWidth - 20, 9, tftWidth - 20, 9 + 13, bruceConfig.bgColor);
}

/***************************************************************************************
** Function name: drawWireguardStatus()
** Description:   Draws a padlock when connected
***************************************************************************************/
void drawWireguardStatus(int x, int y) {
  tft.fillRect(x,y,20,17,bruceConfig.bgColor);
    if(isConnectedWireguard){
        tft.drawRoundRect(10+x, 0+y, 10, 16, 5, TFT_GREEN);
        tft.fillRoundRect(10+x, 12+y, 10, 5, 0, TFT_GREEN);
    } else {
    tft.drawRoundRect(1+x, 0+y, 10, 16, 5, bruceConfig.priColor);
    tft.fillRoundRect(0+x, 12+y, 10, 5, 0, bruceConfig.bgColor);
    tft.fillRoundRect(10+x, 12+y, 10, 5, 0, bruceConfig.priColor);
    }

}

/***************************************************************************************
** Function name: listFiles
** Description:   Função para desenhar e mostrar o menu principal
***************************************************************************************/
#define MAX_ITEMS (int)(tftHeight-20)/(LH*2)
Opt_Coord listFiles(int index, std::vector<FileList> fileList) {
    Opt_Coord coord;
    if(index==0){
      tft.fillScreen(bruceConfig.bgColor);
      tft.fillScreen(bruceConfig.bgColor);
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
    int nchars = (tftWidth-20)/(6*tft.textsize);
    String txt=">";
    while(i<arraySize) {
        if(i>=start) {
            tft.setCursor(10,tft.getCursorY());
            if(fileList[i].folder==true) tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
            else if(fileList[i].operation==true) tft.setTextColor(ALCOLOR, bruceConfig.bgColor);
            else { tft.setTextColor(bruceConfig.priColor,bruceConfig.bgColor); }

            if (index==i) { 
              txt=">";
              coord.x=10+FM*LW;
              coord.y=tft.getCursorY();
              coord.size=nchars;
              coord.fgcolor=fileList[i].folder? getColorVariation(bruceConfig.priColor):bruceConfig.priColor;
              coord.bgcolor=bruceConfig.bgColor;
            }
            else txt=" ";
            txt+=fileList[i].filename + "                 ";
            tft.println(txt.substring(0,nchars));
        }
        i++;
        if (i==(start+MAX_ITEMS) || i==arraySize) break;
    }
    tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, bruceConfig.priColor);
    tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, bruceConfig.priColor);
    return coord;
}


// desenhos do menu principal, sprite "draw" com 80x80 pixels

void drawWifiSmall(int x, int y) {
  tft.fillRect(x,y,16,16,bruceConfig.bgColor);
  tft.fillCircle(9+x,14+y,1,bruceConfig.priColor);
  tft.drawArc(9+x,14+y,4,6,130,230,bruceConfig.priColor, bruceConfig.bgColor);
  tft.drawArc(9+x,14+y,10,12,130,230,bruceConfig.priColor, bruceConfig.bgColor);
}

void drawBLESmall(int x, int y) {
  tft.fillRect(x,y,17,17,bruceConfig.bgColor);
  tft.drawWideLine(8+x, 8+y, 4+x, 5+y, 2, bruceConfig.priColor,bruceConfig.bgColor);
  tft.drawWideLine(8+x, 8+y, 4+x,13+y, 2, bruceConfig.priColor,bruceConfig.bgColor);
  tft.drawTriangle(8+x, 8+y, 8+x, 0+y,13+x,4+y,bruceConfig.priColor);
  tft.drawTriangle(8+x, 8+y, 8+x,16+y,13+x,12+y,bruceConfig.priColor);
}

void drawBLE_beacon(int x, int y, uint16_t color) {
  tft.fillRect(x,y,40,80,bruceConfig.bgColor);
  tft.drawWideLine(40+x,53+y,2+x,26+y,5,color,bruceConfig.bgColor);
  tft.drawWideLine(40+x,26+y,2+x,53+y,5,color,bruceConfig.bgColor);
  tft.drawWideLine(40+x,53+y,20+x,68+y,5,color,bruceConfig.bgColor);
  tft.drawWideLine(40+x,26+y,20+x,12+y,5,color,bruceConfig.bgColor);
  tft.drawWideLine(20+x,12+y,20+x,68+y,5,color,bruceConfig.bgColor);
  tft.fillTriangle(40+x,26+y,20+x,40+y,20+x,12+y,color);
  tft.fillTriangle(40+x,53+y,20+x,40+y,20+x,68+y,color);
}

void drawGPS(int x, int y) {
  tft.fillRect(x,y,80,80,bruceConfig.bgColor);
  tft.drawEllipse(40+x,70+y,15,8,bruceConfig.priColor);
  tft.drawArc(40+x,25+y,23,7,0,340,bruceConfig.priColor,bruceConfig.bgColor);
  tft.fillTriangle(40+x,70+y,20+x,64+y,60+x,64+y,bruceConfig.priColor);
}

void drawGpsSmall(int x, int y) {
  tft.fillRect(x,y,17,17,bruceConfig.bgColor);
  tft.drawEllipse(9+x,14+y,4,3,bruceConfig.priColor);
  tft.drawArc(9+x,6+y,5,2,0,340,bruceConfig.priColor,bruceConfig.bgColor);
  tft.fillTriangle(9+x,15+y,5+x,9+y,13+x,9+y,bruceConfig.priColor);
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

bool showJpeg(FS fs, String filename, int x, int y, bool center) {
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
    if(center) { 
      x=(tftWidth-JpegDec.width)/2;
      y=(tftHeight-JpegDec.height)/2;
    }
    jpegRender(x, y);
  }

  delete[] data_array; // free heap before leaving
  return true;
}


//####################################################################################################
// Draw a GIF on the TFT
// derived from https://github.com/bitbank2/AnimatedGIF/blob/master/examples/TFT_eSPI_memory/TFT_eSPI_memory.ino
// and https://github.com/bitbank2/AnimatedGIF/blob/master/examples/best_practices_example/best_practices_example.ino
//####################################################################################################

#include <AnimatedGIF.h>

#define NORMAL_SPEED
#define GIF_BUFFER_SIZE 100
//#define USE_DMA

#ifdef USE_DMA
  uint16_t usTemp[2][GIF_BUFFER_SIZE]; // Global to support DMA use
#else
  uint16_t usTemp[1][GIF_BUFFER_SIZE];    // Global to support DMA use
#endif
bool     dmaBuf = 0;

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette;
  int x, y, iWidth, iCount;

  // Display bounds check and cropping
  iWidth = pDraw->iWidth;
  if (iWidth + pDraw->iX > tftWidth)
    iWidth = tftWidth - pDraw->iX;
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line
  if (y >= tftWidth || pDraw->iX >= tftWidth || iWidth < 1)
    return;

  // Old image disposal
  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    pEnd = s + iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < iWidth)
    {
      c = ucTransparent - 1;
      d = &usTemp[0][0];
      while (c != ucTransparent && s < pEnd && iCount < GIF_BUFFER_SIZE )
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      } // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        // DMA would degrtade performance here due to short line segments
        tft.setAddrWindow(pDraw->iX + x, y, iCount, 1);
        tft.pushPixels(usTemp, iCount);
        x += iCount;

        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          x++;
        else
          s--;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;

    // Unroll the first pass to boost DMA performance
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    if (iWidth <= GIF_BUFFER_SIZE)
      for (iCount = 0; iCount < iWidth; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];
    else
      for (iCount = 0; iCount < GIF_BUFFER_SIZE; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA // 71.6 fps (ST7796 84.5 fps)
    tft.dmaWait();
    tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
    tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
    dmaBuf = !dmaBuf;
#else // 57.0 fps
    tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
    tft.pushPixels(&usTemp[0][0], iCount);
#endif

    iWidth -= iCount;
    // Loop if pixel buffer smaller than width
    while (iWidth > 0)
    {
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      if (iWidth <= GIF_BUFFER_SIZE)
        for (iCount = 0; iCount < iWidth; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];
      else
        for (iCount = 0; iCount < GIF_BUFFER_SIZE; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA
      tft.dmaWait();
      tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
      dmaBuf = !dmaBuf;
#else
      tft.pushPixels(&usTemp[0][0], iCount);
#endif
      iWidth -= iCount;
    }
  }
} /* GIFDraw() */

void * GIFOpenFile(const char *fname, int32_t *pSize)
{
  static File FSGifFile;  // MEMO: declared static to survive return
  if(SD.exists(fname)) FSGifFile = SD.open(fname);
  else if(LittleFS.exists(fname)) FSGifFile = LittleFS.open(fname);
  if (FSGifFile) {
    *pSize = FSGifFile.size();
    return (void *)&FSGifFile;
  }
  return NULL;
}

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL){
     f->close();
     //log_n("Close file 1!");
  }
   //log_n("Close file 2!");
}

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  //Serial.print("GIFReadFile 1!\n");
  int32_t iBytesRead;
  iBytesRead = iLen;
  File *f = static_cast<File *>(pFile->fHandle);
  // Note: If you read a file all the way to the last byte, seek() stops working
  if ((pFile->iSize - pFile->iPos) < iLen)
      iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
  if (iBytesRead <= 0)
      return 0;
  iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
  pFile->iPos = f->position();
  //Serial.print("GIFReadFile 2!");
  return iBytesRead;
}

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
  //log_n("GIFSeekFile 1!");
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
  //log_d("Seek time = %d us\n", i);
  //log_n("GIFSeekFile 2!");
  return pFile->iPos;
}

bool showGIF(FS fs, String filename, int x, int y) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
//#if defined(ARDUINO_M5STACK_CARDPUTER)
  if(!fs.exists(filename))
    return false;
  static AnimatedGIF gif;  // MEMO: triggers stack canary if not static
  gif.begin(BIG_ENDIAN_PIXELS);
  if( gif.open( filename.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw ) )
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip select is locked low
    // TODO: keep looping? pass x and y offsets
    // while(!check(AnyKeyPress) && ...)
    while (gif.playFrame(true, NULL))  // MEMO: single-frame images will exit the loop after a while without pressing any key
    {
      yield();
      if(check(AnyKeyPress)) break;
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
    return true;
  }
  displayError("error opening GIF");
#else
  displayError("GIF unsupported on this device");
#endif
  return false;
}

/***************************************************************************************
** Function name: getComplementaryColor2
** Description:   Get simple complementary color in RGB565 format
***************************************************************************************/
uint16_t getComplementaryColor2(uint16_t color) {
  int r = 31-((color >> 11) & 0x1F);
  int g = 63-((color >> 5) & 0x3F);
  int b = 31-(color & 0x1F);
  return (r<<11) | (g<<5) | b;
}
/***************************************************************************************
** Function name: getComplementaryColor
** Description:   Get complementary color in RGB565 format
***************************************************************************************/
uint16_t getComplementaryColor(uint16_t color) {
  double r = ((color >> 11) & 0x1F) / 31.0;
  double g = ((color >> 5) & 0x3F) / 63.0;
  double b = (color & 0x1F) / 31.0;

  double cmax = fmax(r, fmax(g, b));
  double cmin = fmin(r, fmin(g, b));
  double delta = cmax - cmin;

  double hue = 0.0;
  if (delta == 0) hue = 0.0;
  else if (cmax == r) hue = 60 * fmod((g-b) / delta, 6);
  else if (cmax == g) hue = 60 * ((b-r) / delta + 2);
  else hue = 60 * ((r-g) / delta + 4);

  if (hue < 0) hue += 360;

  double lightness = (cmax + cmin) / 2;
  double saturation = (delta == 0) ? 0 : delta / (1 - std::abs(2 * lightness - 1));

  double compHue = fmod(hue + 180, 360);

  double c = (1 - std::abs(2 * lightness - 1)) * saturation;
  double x = c * (1 - std::abs(fmod(compHue / 60, 2) - 1));
  double m = lightness - c / 2;

  double compR = 0, compG = 0, compB = 0;
  if (compHue >= 0 && compHue < 60) {
    compR = c;
    compG = x;
  } else if (compHue >= 60 && compHue < 120) {
    compR = x;
    compG = c;
  } else if (compHue >= 120 && compHue < 180) {
    compG = c;
    compB = x;
  } else if (compHue >= 180 && compHue < 240) {
    compG = x;
    compB = c;
  } else if (compHue >= 240 && compHue < 300) {
    compB = c;
    compR = x;
  } else {
    compB = x;
    compR = c;
  }

  uint16_t compl_color = uint8_t(compR * 31) << 11 | uint8_t(compG * 63) << 5 | uint8_t(compB * 31);

  // change black color
  if (compl_color == 0) compl_color = color - 0x1111;

  return compl_color;
}


/***************************************************************************************
** Function name: getColorVariation
** Description:   Get a variation of color in RGB565 format
***************************************************************************************/
uint16_t getColorVariation(uint16_t color, int delta, int direction) {
  uint8_t r = ((color >> 11) & 0x1F);
  uint8_t g = ((color >> 5) & 0x3F);
  uint8_t b = (color & 0x1F);

  float brightness = 0.299*r/31 + 0.587*g/63 + 0.114*b/31;

  if (direction < 0 || (direction == 0 && brightness >= 0.5)) {
    r = max(0, r-delta);
    g = max(0, g-2*delta);
    b = max(0, b-delta);
  } else {
    r = min(31, r+delta);
    g = min(63, g+2*delta);
    b = min(31, b+delta);
  }

  uint16_t compl_color = r << 11 | g << 5 | b;

  return compl_color;
}
