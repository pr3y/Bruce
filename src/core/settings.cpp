#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include <EEPROM.h>




/*********************************************************************
**  Function: setBrightness
**  save brightness value into EEPROM
**********************************************************************/
void setBrightness(int bright, bool save) {
  if(bright>100) bright=100;

  #if !defined(STICK_C_PLUS)
  int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bright/100 ));
  analogWrite(BACKLIGHT, bl);
  #else
  axp192.ScreenBreath(bright);
  #endif

  EEPROM.begin(EEPROMSIZE); // open eeprom
  EEPROM.write(2, bright); //set the byte
  EEPROM.commit(); // Store data to EEPROM
  EEPROM.end(); // Free EEPROM memory
}

/*********************************************************************
**  Function: getBrightness
**  save brightness value into EEPROM
**********************************************************************/
void getBrightness() {
  EEPROM.begin(EEPROMSIZE);
  int bright = EEPROM.read(2);
  EEPROM.end(); // Free EEPROM memory
  if(bright>100) {
    bright = 100;
    #if !defined(STICK_C_PLUS)
    int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bright/100 ));
    analogWrite(BACKLIGHT, bl);
    #else
    axp192.ScreenBreath(bright);
    #endif
    setBrightness(100);
  }

  #if !defined(STICK_C_PLUS)
  int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bright/100 ));
  analogWrite(BACKLIGHT, bl);
  #else
  axp192.ScreenBreath(bright);
  #endif
}

/*********************************************************************
**  Function: gsetRotation
**  get orientation from EEPROM
**********************************************************************/
int gsetRotation(bool set){
  EEPROM.begin(EEPROMSIZE);
  int getRot = EEPROM.read(0);
  int result = ROTATION;

  if(getRot==1 && set) result = 3;
  else if(getRot==3 && set) result = 1;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }

  if(set) {
    rotation = result;
    tft.setRotation(result);
    EEPROM.write(0, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end(); // Free EEPROM memory
  returnToMenu=true;
  return result;
}


/*********************************************************************
**  Function: setBrightnessMenu
**  Handles Menu to set brightness
**********************************************************************/
void setBrightnessMenu() {
  options = {
    {"100%", [=]() { setBrightness(100); }},
    {"75 %", [=]() { setBrightness(75); }},
    {"50 %", [=]() { setBrightness(50); }},
    {"25 %", [=]() { setBrightness(25); }},
    {" 0 %", [=]() { setBrightness(1); }},
  };
  delay(200);
  loopOptions(options, true);
  delay(200);
}

/*********************************************************************
**  Function: setClock
**  Handles Menu to set timezone to NTP
**********************************************************************/
const char* ntpServer = "pool.ntp.org";
long  selectedTimezone;
const int   daylightOffset_sec = 0;
int timeHour;

TimeChangeRule BRST = {"BRST", Last, Sun, Oct, 0, timeHour};
Timezone myTZ(BRST, BRST);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, selectedTimezone, daylightOffset_sec);


void setUIColor(){
    EEPROM.begin(EEPROMSIZE);
    //int color = EEPROM.read(5);

    options = {
      {"Default",  [&]() { FGCOLOR=TFT_PURPLE+0x3000;EEPROM.write(5,0);EEPROM.commit(); }},
      {"White",  [&]() { FGCOLOR=TFT_WHITE; EEPROM.write(5,1);EEPROM.commit(); }},
      {"Red",   [&]() { FGCOLOR=TFT_RED; EEPROM.write(5,2);EEPROM.commit(); }},
      {"Green",   [&]() { FGCOLOR=TFT_DARKGREEN; EEPROM.write(5,3);EEPROM.commit(); }},
      {"Blue",  [&]() { FGCOLOR=TFT_BLUE; EEPROM.write(5,4);EEPROM.commit(); }},
      {"Yellow",   [&]() { FGCOLOR=TFT_YELLOW; EEPROM.write(5,5);EEPROM.commit(); }},
      {"Magenta",   [&]() { FGCOLOR=TFT_MAGENTA; EEPROM.write(5,6);EEPROM.commit(); }},
      {"Orange",   [&]() { FGCOLOR=TFT_ORANGE; EEPROM.write(5,7);EEPROM.commit(); }},
    };
    delay(200);
    loopOptions(options);
    tft.setTextColor(TFT_BLACK, FGCOLOR);
    EEPROM.end();
    }



void setClock() {
  bool auto_mode=true;
  options = {
    {"NTP adjust", [&]() { auto_mode=true; }},
    {"Manually set", [&]() { auto_mode=false; }},
  };
  delay(200);
  loopOptions(options);
  delay(200);

  if (auto_mode) {
    if(!wifiConnected) wifiConnectMenu();
    int tmz;

    options = {
      {"Brasilia",  [&]() { timeClient.setTimeOffset(-3 * 3600); tmz=0; }},
      {"Pernambuco",[&]() { timeClient.setTimeOffset(-2 * 3600); tmz=1; }},
      {"New York",  [&]() { timeClient.setTimeOffset(-4 * 3600); tmz=2; }},
      {"Lisbon",    [&]() { timeClient.setTimeOffset(1 * 3600);  tmz=3; }},
      {"Hong Kong", [&]() { timeClient.setTimeOffset(8 * 3600);  tmz=4; }},
      {"Sydney",    [&]() { timeClient.setTimeOffset(10 * 3600); tmz=5; }},
      {"Tokyo",     [&]() { timeClient.setTimeOffset(9 * 3600);  tmz=6; }},
      {"Moscow",    [&]() { timeClient.setTimeOffset(3 * 3600);  tmz=7; }},
      {"Amsterdan", [&]() { timeClient.setTimeOffset(2 * 3600);  tmz=8; }},
    };
    delay(200);
    loopOptions(options);
    EEPROM.begin(EEPROMSIZE); // open eeprom
    EEPROM.write(8, tmz);     // set the byte
    EEPROM.commit();          // Store data to EEPROM
    EEPROM.end();             // Free EEPROM memory

    delay(200);
    timeClient.begin();
    timeClient.update();
    localTime = myTZ.toLocal(timeClient.getEpochTime());
    rtc.setTime(timeClient.getEpochTime());
  }
  else {
    int hr, mn, am;
    options = {
      {"00", [&]() { hr=0; }},
      {"01", [&]() { hr=1; }},
      {"02", [&]() { hr=2; }},
      {"03", [&]() { hr=3; }},
      {"04", [&]() { hr=4; }},
      {"05", [&]() { hr=5; }},
      {"06", [&]() { hr=6; }},
      {"07", [&]() { hr=7; }},
      {"08", [&]() { hr=8; }},
      {"09", [&]() { hr=9; }},
      {"10", [&]() { hr=10; }},
      {"11", [&]() { hr=11; }},
    };
    delay(200);
    loopOptions(options,false,true,"Set Hour");
    delay(200);
    options = {
      {"00", [&]() { mn=0; }},
      {"01", [&]() { mn=1; }},
      {"02", [&]() { mn=2; }},
      {"03", [&]() { mn=3; }},
      {"04", [&]() { mn=4; }},
      {"05", [&]() { mn=5; }},
      {"06", [&]() { mn=6; }},
      {"07", [&]() { mn=7; }},
      {"08", [&]() { mn=8; }},
      {"09", [&]() { mn=9; }},
      {"10", [&]() { mn=10; }},
      {"11", [&]() { mn=11; }},
      {"12", [&]() { mn=12; }},
      {"13", [&]() { mn=13; }},
      {"14", [&]() { mn=14; }},
      {"15", [&]() { mn=15; }},
      {"16", [&]() { mn=16; }},
      {"17", [&]() { mn=17; }},
      {"18", [&]() { mn=18; }},
      {"19", [&]() { mn=19; }},
      {"20", [&]() { mn=20; }},
      {"21", [&]() { mn=21; }},
      {"22", [&]() { mn=22; }},
      {"23", [&]() { mn=23; }},
      {"24", [&]() { mn=24; }},
      {"25", [&]() { mn=25; }},
      {"26", [&]() { mn=26; }},
      {"27", [&]() { mn=27; }},
      {"28", [&]() { mn=28; }},
      {"29", [&]() { mn=29; }},
      {"30", [&]() { mn=30; }},
      {"31", [&]() { mn=31; }},
      {"32", [&]() { mn=32; }},
      {"33", [&]() { mn=33; }},
      {"34", [&]() { mn=34; }},
      {"35", [&]() { mn=35; }},
      {"36", [&]() { mn=36; }},
      {"37", [&]() { mn=37; }},
      {"38", [&]() { mn=38; }},
      {"39", [&]() { mn=39; }},
      {"40", [&]() { mn=40; }},
      {"41", [&]() { mn=41; }},
      {"42", [&]() { mn=42; }},
      {"43", [&]() { mn=43; }},
      {"44", [&]() { mn=44; }},
      {"45", [&]() { mn=45; }},
      {"46", [&]() { mn=46; }},
      {"47", [&]() { mn=47; }},
      {"48", [&]() { mn=48; }},
      {"49", [&]() { mn=49; }},
      {"50", [&]() { mn=50; }},
      {"51", [&]() { mn=51; }},
      {"52", [&]() { mn=52; }},
      {"53", [&]() { mn=53; }},
      {"54", [&]() { mn=54; }},
      {"55", [&]() { mn=55; }},
      {"56", [&]() { mn=56; }},
      {"57", [&]() { mn=57; }},
      {"58", [&]() { mn=58; }},
      {"59", [&]() { mn=59; }},
    };
    delay(200);
    loopOptions(options,false,true,"Set Minute");
    delay(200);
    options = {
      {"AM", [&]() { am=0; }},
      {"PM", [&]() { am=12; }},
    };
    delay(200);
    loopOptions(options);
    delay(200);
    rtc.setTime(0,mn,hr+am,20,06,2024); // send me a gift, @Pirata!
  }
  clock_set=true;
  runClockLoop();
}

void runClockLoop() {
  int tmp=0;
  tft.fillScreen(BGCOLOR);
  for (;;){
  if(millis()-tmp>1000) {
    updateTimeStr(rtc.getTimeStruct());

    Serial.print("Current time: ");
    Serial.println(timeStr);
    tft.setTextColor(FGCOLOR,BGCOLOR);
    tft.drawRect(45,40,150,45, FGCOLOR);
    tft.setCursor(60, 50);
    tft.setTextSize(4);
    tft.println(timeStr);

  }

   // Checks para sair do loop
  #ifndef CARDPUTER
    if(checkPrevPress()) { // Apertar o botão power dos sticks
      tft.fillScreen(BGCOLOR);
      returnToMenu=true;
      break;
      //goto Exit;
  }
    #else
    Keyboard.update();
    if(Keyboard.isKeyPressed('`')) {
      tft.fillScreen(BGCOLOR);
      returnToMenu=true;
      break;
      //goto Exit;
    }   // apertar ESC no Car.writeer
    #endif

  }
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Pin from EEPROM
**********************************************************************/
int gsetIrTxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(6);
  if(result>50) result = LED;
  if(set) {
    options = {
      {"Default", [&]() { result = LED; }},
      {"M5 IR Mod", [&]() { result = GROVE_SDA; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
      {"Groove W", [&]() { result = GROVE_SCL; }},
      {"Groove Y", [&]() { result = GROVE_SDA; }},

    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(6, result);
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  IrTx = result;
  return result;
}

/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin from EEPROM
**********************************************************************/
int gsetIrRxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(63);
  if(result>36) result = GROVE_SCL;
  if(set) {
    options = {
      {"M5 IR Mod", [&]() { result = GROVE_SCL; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
      {"Groove W", [&]() { result = GROVE_SCL; }},
      {"Groove Y", [&]() { result = GROVE_SDA; }},

    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(63, result);
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  IrRx = result;
  return result;
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin from EEPROM
**********************************************************************/
int gsetRfTxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(7);
  if(result>36) result = GROVE_SDA;
  if(set) {
    options = {
      {"Default TX", [&]() { result = GROVE_SDA; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(7, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  RfTx = result;
  return result;
}
/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin from EEPROM
**********************************************************************/
int gsetRfRxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(7);
  if(result>36) result = GROVE_SCL;
  if(set) {
    options = {
      {"Default RX", [&]() { result = GROVE_SCL; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(7, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  RfRx = result;
  return result;
}
