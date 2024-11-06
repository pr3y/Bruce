#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "powerSave.h"
#include "modules/rf/rf.h"  // for initRfModule

#ifdef USE_CC1101_VIA_SPI
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#endif


/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void setBrightness(int brightval, bool save) {
  if(bruceConfig.bright>100) bruceConfig.setBright(100);

  #if defined(STICK_C_PLUS2) || defined(CARDPUTER) || defined(T_EMBED)  || defined(T_DECK)
   if(brightval == 0){
      analogWrite(BACKLIGHT, brightval);
    } else {
      int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval/100 ));
      analogWrite(BACKLIGHT, bl);
    }
  #elif defined(STICK_C_PLUS)
    axp192.ScreenBreath(brightval);
  #elif defined(CORE2)
    M5.Axp.ScreenBreath(brightval);
  #elif defined(CORE)
  uint8_t _tmp = (255*brightval)/100;
  M5.Lcd.setBrightness(_tmp);
  #elif defined(M5STACK)
    M5.Display.setBrightness(brightval);
  #elif defined(CYD)
    int dutyCycle;
    if (brightval==100) dutyCycle=255;
    else if (brightval==75) dutyCycle=130;
    else if (brightval==50) dutyCycle=70;
    else if (brightval==25) dutyCycle=20;
    else if (brightval==0) dutyCycle=5;
    else dutyCycle = ((brightval*255)/100);

    log_i("dutyCycle for bright 0-255: %d",dutyCycle);
    ledcWrite(TFT_BRIGHT_CHANNEL,dutyCycle); // Channel 0
  #else
    delay(10);
  #endif

  if(save){
    bruceConfig.setBright(brightval);
  }
}

/*********************************************************************
**  Function: getBrightness
**  get brightness value
**********************************************************************/
void getBrightness() {
  if(bruceConfig.bright>100) {
    bruceConfig.setBright(100);
    #if defined(STICK_C_PLUS2) || defined(CARDPUTER) || defined(T_EMBED)  || defined(T_DECK)
    int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bruceConfig.bright/100 ));
    analogWrite(BACKLIGHT, bl);
    #elif defined(STICK_C_PLUS)
    axp192.ScreenBreath(bruceConfig.bright);
    #elif defined(CORE2)
    M5.Axp.ScreenBreath(bruceConfig.bright);
    #elif defined(CORE)
    uint8_t _tmp = (255*bruceConfig.bright)/100;
    M5.Lcd.setBrightness(_tmp);
    #elif defined(M5STACK)
    M5.Display.setBrightness(bruceConfig.bright);
   #elif defined(CYD)
    int dutyCycle;
    if (bruceConfig.bright==100) dutyCycle=255;
    else if (bruceConfig.bright==75) dutyCycle=130;
    else if (bruceConfig.bright==50) dutyCycle=70;
    else if (bruceConfig.bright==25) dutyCycle=20;
    else if (bruceConfig.bright==0) dutyCycle=5;
    else dutyCycle = ((bruceConfig.bright*255)/100);
    log_i("dutyCycle for bright 0-255: %d",dutyCycle);
    ledcWrite(TFT_BRIGHT_CHANNEL,dutyCycle); // Channel 0
    #else
    delay(10);
  #endif
    setBrightness(100);
  }

  #if defined(STICK_C_PLUS2) || defined(CARDPUTER) || defined(T_EMBED)  || defined(T_DECK)
  int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bruceConfig.bright/100 ));
  analogWrite(BACKLIGHT, bl);
  #elif defined(STICK_C_PLUS)
  axp192.ScreenBreath(bruceConfig.bright);
  #elif defined(CORE2)
  M5.Axp.ScreenBreath(bruceConfig.bright);
  #elif defined(CORE)
  uint8_t _tmp = (255*bruceConfig.bright)/100;
  M5.Lcd.setBrightness(_tmp);
  #elif defined(M5STACK)
  M5.Display.setBrightness(bruceConfig.bright);
  #elif defined(CYD)
  int dutyCycle;
  if (bruceConfig.bright==100) dutyCycle=255;
  else if (bruceConfig.bright==75) dutyCycle=130;
  else if (bruceConfig.bright==50) dutyCycle=70;
  else if (bruceConfig.bright==25) dutyCycle=20;
  else if (bruceConfig.bright==0) dutyCycle=5;
  else dutyCycle = ((bruceConfig.bright*255)/100);
  log_i("dutyCycle for bright 0-255: %d",dutyCycle);
  ledcWrite(TFT_BRIGHT_CHANNEL,dutyCycle); // Channel 0
  #else
  delay(10);
  #endif
}

/*********************************************************************
**  Function: gsetRotation
**  get/set rotation value
**********************************************************************/
int gsetRotation(bool set){
  int getRot = bruceConfig.rotation;
  int result = ROTATION;

  if(getRot==1 && set) result = 3;
  else if(getRot==3 && set) result = 1;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }

  if(set) {
    bruceConfig.setRotation(result);
    tft.setRotation(result);
  }
  returnToMenu=true;
  return result;
}

/*********************************************************************
**  Function: setBrightnessMenu
**  Handles Menu to set brightness
**********************************************************************/
void setBrightnessMenu() {
  int idx=0;
  if(bruceConfig.bright==100) idx=0;
  else if(bruceConfig.bright==75) idx=1;
  else if(bruceConfig.bright==50) idx=2;
  else if(bruceConfig.bright==25) idx=3;
  else if(bruceConfig.bright== 1) idx=4;

  options = {
    {"100%", [=]() { setBrightness(100); }, bruceConfig.bright == 100 },
    {"75 %", [=]() { setBrightness(75);  }, bruceConfig.bright == 75 },
    {"50 %", [=]() { setBrightness(50);  }, bruceConfig.bright == 50 },
    {"25 %", [=]() { setBrightness(25);  }, bruceConfig.bright == 25 },
    {" 1 %", [=]() { setBrightness(1);   }, bruceConfig.bright == 1 },
    {"Main Menu", [=]() { backToMenu(); }},
  };
  delay(200);
  loopOptions(options, true,false,"",idx);
  delay(200);
}


/*********************************************************************
**  Function: setSleepMode
**  Turn screen off and reduces cpu clock
**********************************************************************/
void setSleepMode() {
  sleepModeOn();
  while (1) {
    #if defined(CARDPUTER)
      if (checkAnyKeyPress())
    #else
      if (checkSelPress())
    #endif
    {
      sleepModeOff();
      returnToMenu = true;
      break;
    }
  }
}

/*********************************************************************
**  Function: setDimmerTimeMenu
**  Handles Menu to set dimmer time
**********************************************************************/
void setDimmerTimeMenu() {
  int idx=0;
  if(bruceConfig.dimmerSet==10) idx=0;
  else if(bruceConfig.dimmerSet==20) idx=1;
  else if(bruceConfig.dimmerSet==30) idx=2;
  else if(bruceConfig.dimmerSet==60) idx=3;
  else if(bruceConfig.dimmerSet== 0) idx=4;
  options = {
    {"10s", [=]() { bruceConfig.setDimmer(10); }, bruceConfig.dimmerSet == 10 },
    {"20s", [=]() { bruceConfig.setDimmer(20); }, bruceConfig.dimmerSet == 20 },
    {"30s", [=]() { bruceConfig.setDimmer(30); }, bruceConfig.dimmerSet == 30 },
    {"60s", [=]() { bruceConfig.setDimmer(60); }, bruceConfig.dimmerSet == 60 },
    {"Disabled", [=]() { bruceConfig.setDimmer(0); }, bruceConfig.dimmerSet == 0 },
  };
  delay(200);
  loopOptions(options,idx);
  delay(200);
}

/*********************************************************************
**  Function: setUIColor
**  Set and store main UI color
**********************************************************************/
#define LIGHT_BLUE 0x96FE
void setUIColor(){
  int idx=0;
  if(bruceConfig.priColor==DEFAULT_PRICOLOR) idx=0;
  else if(bruceConfig.priColor==TFT_WHITE) idx=1;
  else if(bruceConfig.priColor==TFT_RED) idx=2;
  else if(bruceConfig.priColor==TFT_DARKGREEN) idx=3;
  else if(bruceConfig.priColor==TFT_BLUE) idx=4;
  else if(bruceConfig.priColor==LIGHT_BLUE) idx=5;
  else if(bruceConfig.priColor==TFT_YELLOW) idx=6;
  else if(bruceConfig.priColor==TFT_MAGENTA) idx=7;
  else if(bruceConfig.priColor==TFT_ORANGE) idx=8;
  else idx=9;  // custom theme

  options = {
    {"Default",   [=]() { bruceConfig.setTheme(DEFAULT_PRICOLOR);}, bruceConfig.priColor==DEFAULT_PRICOLOR},
    {"White",     [=]() { bruceConfig.setTheme(TFT_WHITE);     }, bruceConfig.priColor==TFT_WHITE     },
    {"Red",       [=]() { bruceConfig.setTheme(TFT_RED);       }, bruceConfig.priColor==TFT_RED       },
    {"Green",     [=]() { bruceConfig.setTheme(TFT_DARKGREEN); }, bruceConfig.priColor==TFT_DARKGREEN },
    {"Blue",      [=]() { bruceConfig.setTheme(TFT_BLUE);      }, bruceConfig.priColor==TFT_BLUE      },
    {"Light Blue",[=]() { bruceConfig.setTheme(LIGHT_BLUE);    }, bruceConfig.priColor==LIGHT_BLUE    },
    {"Yellow",    [=]() { bruceConfig.setTheme(TFT_YELLOW);    }, bruceConfig.priColor==TFT_YELLOW    },
    {"Magenta",   [=]() { bruceConfig.setTheme(TFT_MAGENTA);   }, bruceConfig.priColor==TFT_MAGENTA   },
    {"Orange",    [=]() { bruceConfig.setTheme(TFT_ORANGE);    }, bruceConfig.priColor==TFT_ORANGE    },
  };

  if (idx == 9) options.push_back({"Custom Theme", [=]() { backToMenu(); }, true});
  options.push_back({"Main Menu", [=]() { backToMenu(); }});

  delay(200);
  loopOptions(options, idx);
  tft.setTextColor(bruceConfig.bgColor, bruceConfig.priColor);
}

/*********************************************************************
**  Function: setSoundConfig
**  Enable or disable sound
**********************************************************************/
void setSoundConfig() {
  options = {
    {"Sound off", [=]() { bruceConfig.setSoundEnabled(0); }, bruceConfig.soundEnabled == 0},
    {"Sound on",  [=]() { bruceConfig.setSoundEnabled(1); }, bruceConfig.soundEnabled == 1},
  };
  delay(200);
  loopOptions(options, bruceConfig.soundEnabled);
  delay(200);
}

/*********************************************************************
**  Function: setWifiStartupConfig
**  Enable or disable wifi connection at startup
**********************************************************************/
void setWifiStartupConfig() {
  options = {
    {"Disable", [=]() { bruceConfig.setWifiAtStartup(0); }, bruceConfig.wifiAtStartup == 0},
    {"Enable",  [=]() { bruceConfig.setWifiAtStartup(1); }, bruceConfig.wifiAtStartup == 1},
  };
  delay(200);
  loopOptions(options, bruceConfig.wifiAtStartup);
  delay(200);
}

/*********************************************************************
**  Function: setRFModuleMenu
**  Handles Menu to set the RF module in use
**********************************************************************/
void setRFModuleMenu() {
  int result = 0;
  int idx=0;
  if(bruceConfig.rfModule==M5_RF_MODULE) idx=0;
  else if(bruceConfig.rfModule==CC1101_SPI_MODULE) idx=1;

  options = {
    {"M5 RF433T/R",    [&]() { result = M5_RF_MODULE; }},
#ifdef USE_CC1101_VIA_SPI
    {"CC1101 on SPI",  [&]() { result = CC1101_SPI_MODULE; }},
#endif
/* WIP:
 * #ifdef USE_CC1101_VIA_PCA9554
 * {"CC1101+PCA9554",  [&]() { result = 2; }},
 * #endif
*/
  };
  delay(200);
  loopOptions(options, idx);  // 2fix: idx highlight not working?
  delay(200);
  if(result == CC1101_SPI_MODULE) {
    #ifdef USE_CC1101_VIA_SPI
    ELECHOUSE_cc1101.Init();
    if (ELECHOUSE_cc1101.getCC1101()){
      bruceConfig.setRfModule(CC1101_SPI_MODULE);
      return;
    }
    #endif
    // else display an error
    displayError("CC1101 not found");
    while(!checkAnyKeyPress());
  }
  // fallback to "M5 RF433T/R" on errors
  bruceConfig.setRfModule(M5_RF_MODULE);
}

/*********************************************************************
**  Function: setRFFreqMenu
**  Handles Menu to set the default frequency for the RF module
**********************************************************************/
void setRFFreqMenu() {
  float result = 433.92;
  String freq_str = keyboard(String(bruceConfig.rfFreq), 10, "Default frequency:");
  if(freq_str.length() > 1) {
    result = freq_str.toFloat();  // returns 0 if not valid
    if(result>=300 && result<=928) { // TODO: check valid freq according to current module?
      bruceConfig.setRfFreq(result);
      return;
    }
  }
  // else
  displayError("Invalid frequency");
  bruceConfig.setRfFreq(433.92);  // reset to default
  delay(1000);
}

/*********************************************************************
**  Function: setRFIDModuleMenu
**  Handles Menu to set the RFID module in use
**********************************************************************/
void setRFIDModuleMenu() {
  options = {
    {"M5 RFID2",      [=]() { bruceConfig.setRfidModule(M5_RFID2_MODULE); },  bruceConfig.rfidModule == M5_RFID2_MODULE},
    {"PN532 on I2C",  [=]() { bruceConfig.setRfidModule(PN532_I2C_MODULE); }, bruceConfig.rfidModule == PN532_I2C_MODULE},
    {"PN532 on SPI",  [=]() { bruceConfig.setRfidModule(PN532_SPI_MODULE); }, bruceConfig.rfidModule == PN532_SPI_MODULE},
  };
  delay(200);
  loopOptions(options, bruceConfig.rfidModule);
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

void setClock() {
  bool auto_mode=true;

  #if defined(HAS_RTC)
    RTC_TimeTypeDef TimeStruct;
    cplus_RTC _rtc;
    _rtc.GetBm8563Time();
  #endif

  options = {
    {"NTP Timezone", [&]() { auto_mode=true; }},
    {"Manually set", [&]() { auto_mode=false; }},
    {"Main Menu", [=]() { backToMenu(); }},
  };
  delay(200);
  loopOptions(options);
  delay(200);

  if (auto_mode) {
    if(!wifiConnected) wifiConnectMenu();

    options = {
      {"Brasilia",    [&]() { bruceConfig.setTmz(-3); }, bruceConfig.tmz==-3 },
      {"Pernambuco",  [&]() { bruceConfig.setTmz(-2); }, bruceConfig.tmz==-2 },
      {"Los Angeles", [&]() { bruceConfig.setTmz(-8); }, bruceConfig.tmz==-8 },
      {"New York",    [&]() { bruceConfig.setTmz(-5); }, bruceConfig.tmz==-5 },
      {"Lisbon",      [&]() { bruceConfig.setTmz(0);  }, bruceConfig.tmz==0  },
      {"Paris",       [&]() { bruceConfig.setTmz(1);  }, bruceConfig.tmz==1  },
      {"Athens",      [&]() { bruceConfig.setTmz(2);  }, bruceConfig.tmz==2  },
      {"Moscow",      [&]() { bruceConfig.setTmz(3);  }, bruceConfig.tmz==3  },
      {"Dubai",       [&]() { bruceConfig.setTmz(4);  }, bruceConfig.tmz==4  },
      {"Hong Kong",   [&]() { bruceConfig.setTmz(8);  }, bruceConfig.tmz==8  },
      {"Tokyo",       [&]() { bruceConfig.setTmz(9);  }, bruceConfig.tmz==9  },
      {"Sydney",      [&]() { bruceConfig.setTmz(10); }, bruceConfig.tmz==10 },
      {"Main Menu",   [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options);
    delay(200);
    timeClient.setTimeOffset(bruceConfig.tmz * 3600);
    timeClient.begin();
    timeClient.update();
    localTime = myTZ.toLocal(timeClient.getEpochTime());

    #if defined(HAS_RTC)
      struct tm *timeinfo = localtime(&localTime);
      TimeStruct.Hours   = timeinfo->tm_hour;
      TimeStruct.Minutes = timeinfo->tm_min;
      TimeStruct.Seconds = timeinfo->tm_sec;
      _rtc.SetTime(&TimeStruct);
    #else
      rtc.setTime(timeClient.getEpochTime());
    #endif

    clock_set = true;
    runClockLoop();
  }
  else {
    int hr, mn, am;
    options = { };
    for(int i=0; i<12;i++) options.push_back({String(String(i<10?"0":"") + String(i)).c_str(), [&]() { delay(1); }});

    delay(200);
    hr=loopOptions(options,false,true,"Set Hour");
    delay(200);
    options = { };
    for(int i=0; i<60;i++) options.push_back({String(String(i<10?"0":"") + String(i)).c_str(), [&]() { delay(1); }});

    delay(200);
    mn=loopOptions(options,false,true,"Set Minute");
    delay(200);
    options = {
      {"AM", [&]() { am=0; }},
      {"PM", [&]() { am=12; }},
    };
    delay(200);
    loopOptions(options);
    delay(200);

    #if defined(HAS_RTC)
      TimeStruct.Hours   = hr+am;
      TimeStruct.Minutes = mn;
      TimeStruct.Seconds = 0;
      _rtc.SetTime(&TimeStruct);
    #else
      rtc.setTime(0,mn,hr+am,20,06,2024); // send me a gift, @Pirata!
    #endif
    clock_set = true;
    runClockLoop();
  }
}

void runClockLoop() {
  int tmp=0;

  #if defined(HAS_RTC)
    RTC_TimeTypeDef _time;
    cplus_RTC _rtc;
    _rtc.GetBm8563Time();
    _rtc.GetTime(&_time);
  #endif

  // Delay due to SelPress() detected on run
  tft.fillScreen(bruceConfig.bgColor);
  tft.fillScreen(bruceConfig.bgColor);
  delay(300);

  for (;;){
  if(millis()-tmp>1000) {
    #if !defined(HAS_RTC)
      updateTimeStr(rtc.getTimeStruct());
    #endif
    Serial.print("Current time: ");
    Serial.println(timeStr);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.drawRect(10, 10, WIDTH-15,HEIGHT-15, bruceConfig.priColor);
    tft.setCursor(64, HEIGHT/3+5);
    tft.setTextSize(4);
    #if defined(HAS_RTC)
      _rtc.GetBm8563Time();
      _rtc.GetTime(&_time);
      char timeString[9];  // Buffer para armazenar a string formatada "HH:MM:SS"
      snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", _time.Hours, _time.Minutes, _time.Seconds);
      tft.drawCentreString(timeString,WIDTH/2,HEIGHT/2-13,1);
    #else
      tft.drawCentreString(timeStr,WIDTH/2,HEIGHT/2-13,1);
    #endif
    tmp=millis();
  }

    // Checks para sair do loop
    if(checkSelPress() or checkEscPress()) { // Apertar o botão power dos sticks
      tft.fillScreen(bruceConfig.bgColor);
      returnToMenu=true;
      break;
      //goto Exit;
    }
    delay(10);
  }
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Tx Pin
**********************************************************************/
int gsetIrTxPin(bool set){
  int result = bruceConfig.irTx;

  if(result>50) bruceConfig.setIrTxPin(LED);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = IR_TX_PINS;
    int idx=100;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==bruceConfig.irTx && idx==100) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() { bruceConfig.setIrTxPin(pin.second); }, pin.second==bruceConfig.irTx });
    }
    delay(200);
    loopOptions(options, idx);
    delay(200);
    Serial.println("Saved pin: " + String(bruceConfig.irTx));
  }

  returnToMenu=true;
  return bruceConfig.irTx;
}

/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin
**********************************************************************/
int gsetIrRxPin(bool set){
  int result = bruceConfig.irRx;

  if(result>45) bruceConfig.setIrRxPin(GROVE_SCL);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = IR_RX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==bruceConfig.irRx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() {bruceConfig.setIrRxPin(pin.second);}, pin.second==bruceConfig.irRx });
    }
    delay(200);
    loopOptions(options);
    delay(200);
  }

  returnToMenu=true;
  return bruceConfig.irRx;
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin
**********************************************************************/
int gsetRfTxPin(bool set){
  int result = bruceConfig.rfTx;

  if(result>45) bruceConfig.setRfTxPin(GROVE_SDA);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = RF_TX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==bruceConfig.rfTx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() {bruceConfig.setRfTxPin(pin.second);}, pin.second==bruceConfig.rfTx });
    }
    delay(200);
    loopOptions(options);
    delay(200);
  }

  returnToMenu=true;
  return bruceConfig.rfTx;
}

/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin
**********************************************************************/
int gsetRfRxPin(bool set){
  int result = bruceConfig.rfRx;

  if(result>36) bruceConfig.setRfRxPin(GROVE_SCL);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = RF_RX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==bruceConfig.rfRx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() {bruceConfig.setRfRxPin(pin.second);}, pin.second==bruceConfig.rfRx });
    }
    delay(200);
    loopOptions(options);
    delay(200);
  }

  returnToMenu=true;
  return bruceConfig.rfRx;
}
