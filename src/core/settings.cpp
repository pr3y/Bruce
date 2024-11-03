#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "powerSave.h"
#include "eeprom.h"
#include "modules/rf/rf.h"  // for initRfModule

#ifdef USE_CC1101_VIA_SPI
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#endif


/*********************************************************************
**  Function: setBrightness
**  save brightness value into EEPROM
**********************************************************************/
void setBrightness(int brightval, bool save) {
  if(bruceConfig.bright>100) bruceConfig.bright=100;

  #if defined(STICK_C_PLUS2) || defined(CARDPUTER)
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
    bruceConfig.bright=brightval;
    write_eeprom(EEPROM_BRIGHT, brightval);
  }
}

/*********************************************************************
**  Function: getBrightness
**  save brightness value into EEPROM
**********************************************************************/
void getBrightness() {
  bruceConfig.bright = read_eeprom(EEPROM_BRIGHT);
  if(bruceConfig.bright>100) {
    bruceConfig.bright = 100;
    #if defined(STICK_C_PLUS2) || defined(CARDPUTER)
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

  #if defined(STICK_C_PLUS2) || defined(CARDPUTER)
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
**  get orientation from EEPROM
**********************************************************************/
int gsetRotation(bool set){
  int getRot = read_eeprom(EEPROM_ROT);
  int result = ROTATION;

  if(getRot==1 && set) result = 3;
  else if(getRot==3 && set) result = 1;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }

  if(set) {
    bruceConfig.rotation = result;
    tft.setRotation(result);
    write_eeprom(EEPROM_ROT, result);
  }
  returnToMenu=true;
  return result;
}

/*********************************************************************
**  Function: setDimmerTime
**  Set a timer for screen dimmer
**********************************************************************/
void setDimmerTime(int dimmerTime) {
  if(dimmerTime>60 || dimmerTime<0) dimmerTime = 0;

  bruceConfig.dimmerSet=dimmerTime;
  write_eeprom(EEPROM_DIMMER, bruceConfig.dimmerSet);
}

/*********************************************************************
**  Function: getDimmerSet
**  Get dimmerSet value from EEPROM
**********************************************************************/
void getDimmerSet() {
  bruceConfig.dimmerSet = read_eeprom(EEPROM_DIMMER);
  if(bruceConfig.dimmerSet>60 || bruceConfig.dimmerSet<0) setDimmerTime(0);
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
    {"10s", [=]() { setDimmerTime(10); }, bruceConfig.dimmerSet == 10 },
    {"20s", [=]() { setDimmerTime(20); }, bruceConfig.dimmerSet == 20 },
    {"30s", [=]() { setDimmerTime(30); }, bruceConfig.dimmerSet == 30 },
    {"60s", [=]() { setDimmerTime(60); }, bruceConfig.dimmerSet == 60 },
    {"Disabled", [=]() { setDimmerTime(0); }, bruceConfig.dimmerSet == 0 },
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
  if(FGCOLOR==DEFAULTFGCOLOR) idx=0;
  else if(FGCOLOR==TFT_WHITE) idx=1;
  else if(FGCOLOR==TFT_RED) idx=2;
  else if(FGCOLOR==TFT_DARKGREEN) idx=3;
  else if(FGCOLOR==TFT_BLUE) idx=4;
  else if(FGCOLOR==LIGHT_BLUE) idx=5;
  else if(FGCOLOR==TFT_YELLOW) idx=6;
  else if(FGCOLOR==TFT_MAGENTA) idx=7;
  else if(FGCOLOR==TFT_ORANGE) idx=8;

  options = {
    {"Default",   [&]() { FGCOLOR=DEFAULTFGCOLOR;}, FGCOLOR==DEFAULTFGCOLOR},
    {"White",     [&]() { FGCOLOR=TFT_WHITE;     }, FGCOLOR==TFT_WHITE     },
    {"Red",       [&]() { FGCOLOR=TFT_RED;       }, FGCOLOR==TFT_RED       },
    {"Green",     [&]() { FGCOLOR=TFT_DARKGREEN; }, FGCOLOR==TFT_DARKGREEN },
    {"Blue",      [&]() { FGCOLOR=TFT_BLUE;      }, FGCOLOR==TFT_BLUE      },
    {"Light Blue",[&]() { FGCOLOR=LIGHT_BLUE;    }, FGCOLOR==LIGHT_BLUE    },
    {"Yellow",    [&]() { FGCOLOR=TFT_YELLOW;    }, FGCOLOR==TFT_YELLOW    },
    {"Magenta",   [&]() { FGCOLOR=TFT_MAGENTA;   }, FGCOLOR==TFT_MAGENTA   },
    {"Orange",    [&]() { FGCOLOR=TFT_ORANGE;    }, FGCOLOR==TFT_ORANGE    },
    {"Main Menu", [=]() { backToMenu(); }},
  };
  delay(200);
  loopOptions(options, idx);
  tft.setTextColor(TFT_BLACK, FGCOLOR);

  write_eeprom(EEPROM_FGCOLOR0, int((FGCOLOR >> 8) & 0x00FF));
  write_eeprom(EEPROM_FGCOLOR1, int(FGCOLOR & 0x00FF));
}

/*********************************************************************
**  Function: setSoundConfig
**  Enable or disable sound
**********************************************************************/
void setSoundConfig() {
  int result = 0;

  options = {
    {"Sound off", [&]() { result = 0; }, bruceConfig.soundEnabled == 0},
    {"Sound on",  [&]() { result = 1; }, bruceConfig.soundEnabled == 1},
  };
  delay(200);
  loopOptions(options, bruceConfig.soundEnabled);
  delay(200);

  bruceConfig.soundEnabled = result;
}

/*********************************************************************
**  Function: setRFModuleMenu
**  Handles Menu to set the RF module in use
**********************************************************************/
void setRFModuleMenu() {
  // TODO: save the setting in the EEPROM too?
  int result = 0;
  int idx=0;
  if(bruceConfig.rfModule==M5_RF_MODULE) idx=0;
  else if(bruceConfig.rfModule==CC1101_SPI_MODULE) idx=1;

  options = {
    {"M5 RF433T/R",    [&]() { result = 0; }},
#ifdef USE_CC1101_VIA_SPI
    {"CC1101 on SPI",  [&]() { result = 1; }},
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
  if(result == 1) {
    #ifdef USE_CC1101_VIA_SPI
    ELECHOUSE_cc1101.Init();
    if (ELECHOUSE_cc1101.getCC1101()){
      bruceConfig.rfModule=CC1101_SPI_MODULE;
      write_eeprom(EEPROM_RF_MODULE, bruceConfig.rfModule);
      return;
    }
    #endif
    // else display an error
    displayError("CC1101 not found");
    while(!checkAnyKeyPress());
  }
  // fallback to "M5 RF433T/R" on errors
  bruceConfig.rfModule=0;
  write_eeprom(EEPROM_RF_MODULE, bruceConfig.rfModule);
}

/*********************************************************************
**  Function: setRFFreqMenu
**  Handles Menu to set the default frequency for the RF module
**********************************************************************/
void setRFFreqMenu() {
  // TODO: save the setting in the EEPROM too?
  float result = 433.92;
  String freq_str = keyboard(String(bruceConfig.rfFreq), 10, "Default frequency:");
  if(freq_str.length()>1)
  {
    result = freq_str.toFloat();  // returns 0 if not valid
    if(result>=300 && result<=928) { // TODO: check valid freq according to current module?
        bruceConfig.rfFreq=result;
        return;
    }
  }
  // else
  displayError("Invalid frequency");
  bruceConfig.rfFreq=433.92;  // reset to default
  delay(1000);
}

/*********************************************************************
**  Function: setRFIDModuleMenu
**  Handles Menu to set the RFID module in use
**********************************************************************/
void setRFIDModuleMenu() {
  int result = 0;

  options = {
    {"M5 RFID2",      [&]() { result = M5_RFID2_MODULE; },  bruceConfig.rfidModule == M5_RFID2_MODULE},
    {"PN532 on I2C",  [&]() { result = PN532_I2C_MODULE; }, bruceConfig.rfidModule == PN532_I2C_MODULE},
    {"PN532 on SPI",  [&]() { result = PN532_SPI_MODULE; }, bruceConfig.rfidModule == PN532_SPI_MODULE},
  };
  delay(200);
  loopOptions(options, bruceConfig.rfidModule);
  delay(200);

  bruceConfig.rfidModule = result;
  write_eeprom(EEPROM_RFID_MODULE, bruceConfig.rfidModule);
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

  if (!returnToMenu) {
      if (auto_mode) {
        if(!wifiConnected) wifiConnectMenu();
        if(!returnToMenu) {
            options = {
              {"Brasilia",  [&]() { timeClient.setTimeOffset(-3 * 3600); bruceConfig.tmz=0; }, bruceConfig.tmz==0 },
              {"Pernambuco",[&]() { timeClient.setTimeOffset(-2 * 3600); bruceConfig.tmz=1; }, bruceConfig.tmz==1 },
              {"New York",  [&]() { timeClient.setTimeOffset(-4 * 3600); bruceConfig.tmz=2; }, bruceConfig.tmz==2 },
              {"Lisbon",    [&]() { timeClient.setTimeOffset(1 * 3600);  bruceConfig.tmz=3; }, bruceConfig.tmz==3 },
              {"Hong Kong", [&]() { timeClient.setTimeOffset(8 * 3600);  bruceConfig.tmz=4; }, bruceConfig.tmz==4 },
              {"Sydney",    [&]() { timeClient.setTimeOffset(10 * 3600); bruceConfig.tmz=5; }, bruceConfig.tmz==5 },
              {"Tokyo",     [&]() { timeClient.setTimeOffset(9 * 3600);  bruceConfig.tmz=6; }, bruceConfig.tmz==6 },
              {"Moscow",    [&]() { timeClient.setTimeOffset(3 * 3600);  bruceConfig.tmz=7; }, bruceConfig.tmz==7 },
              {"Amsterdam", [&]() { timeClient.setTimeOffset(2 * 3600);  bruceConfig.tmz=8; }, bruceConfig.tmz==8 },
              {"Main Menu", [=]() { backToMenu(); }},
            };
            if (!returnToMenu) {
                delay(200);
                loopOptions(options);
                write_eeprom(EEPROM_TMZ, bruceConfig.tmz);

                delay(200);
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

                clock_set=true;
                runClockLoop();
            }
         }
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
        clock_set=true;
        runClockLoop();
      }
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
  tft.fillScreen(BGCOLOR);
  delay(300);

  for (;;){
  if(millis()-tmp>1000) {
    #if !defined(HAS_RTC)
      updateTimeStr(rtc.getTimeStruct());
    #endif
    Serial.print("Current time: ");
    Serial.println(timeStr);
    tft.setTextColor(FGCOLOR,BGCOLOR);
    tft.drawRect(10, 10, WIDTH-15,HEIGHT-15, FGCOLOR);
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
      tft.fillScreen(BGCOLOR);
      returnToMenu=true;
      break;
      //goto Exit;
    }
    delay(10);
  }
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Pin from EEPROM
**********************************************************************/
int gsetIrTxPin(bool set){
  int result = read_eeprom(EEPROM_IR_TX);

  if(result>50) bruceConfig.irTx = LED;
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
        options.push_back({pin.first, [=]() { bruceConfig.irTx=pin.second; }, pin.second==bruceConfig.irTx });
    }
    delay(200);
    loopOptions(options, idx);
    delay(200);
    Serial.println("Saved pin: " + String(bruceConfig.irTx));
    write_eeprom(EEPROM_IR_TX, bruceConfig.irTx);
  }

  returnToMenu=true;
  return bruceConfig.irTx;
}

/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin from EEPROM
**********************************************************************/
int gsetIrRxPin(bool set){
  int result = read_eeprom(EEPROM_IR_RX);

  if(result>45) bruceConfig.irRx = GROVE_SCL;
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
        options.push_back({pin.first, [=]() {bruceConfig.irRx=pin.second;}, pin.second==bruceConfig.irRx });
    }
    delay(200);
    loopOptions(options);
    delay(200);
    write_eeprom(EEPROM_IR_RX, bruceConfig.irRx);
  }

  returnToMenu=true;
  return bruceConfig.irRx;
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin from EEPROM
**********************************************************************/
int gsetRfTxPin(bool set){
  int result = read_eeprom(EEPROM_RF_TX);

  if(result>45) bruceConfig.rfTx = GROVE_SDA;
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
        options.push_back({pin.first, [=]() {bruceConfig.rfTx=pin.second;}, pin.second==bruceConfig.rfTx });
    }
    delay(200);
    loopOptions(options);
    delay(200);
    write_eeprom(EEPROM_RF_TX, bruceConfig.rfTx);
  }

  returnToMenu=true;
  return bruceConfig.rfTx;
}
/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin from EEPROM
**********************************************************************/
int gsetRfRxPin(bool set){
  int result = read_eeprom(EEPROM_RF_RX);

  if(result>36) bruceConfig.rfRx = GROVE_SCL;
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
        options.push_back({pin.first, [=]() {bruceConfig.rfRx=pin.second;}, pin.second==bruceConfig.rfRx });
    }
    delay(200);
    loopOptions(options);
    delay(200);
    write_eeprom(EEPROM_RF_RX, bruceConfig.rfRx);
  }

  returnToMenu=true;
  return bruceConfig.rfRx;
}

void getConfigs() {
  int count=0;
  FS* fs = &LittleFS;
  if(setupSdCard()) fs = &SD;

  if(!fs->exists(CONFIG_FILE)) {
    File file;
    file = fs->open(CONFIG_FILE, FILE_WRITE);
    if(file) {
      // init with default settings
    #if ROTATION > 1
      file.print("[{\"rot\":3,\"dimmerSet\":10,\"bright\":100,\"wuiUsr\":\"admin\",\"wuiPwd\":\"bruce\",\"Bruce_FGCOLOR\":43023,\"irTx\":" + String(LED) + ",\"irRx\":" + String(GROVE_SCL) + ",\"rfTx\":" + String(GROVE_SDA) + ",\"rfRx\":" + String(GROVE_SCL) + ",\"tmz\":3,\"rfModule\":0,\"rfFreq\":433.92,\"rfFxdFreq\":1,\"rfScanRange\":3,\"rfidModule\":" + String(bruceConfig.rfidModule) + ",\"wifi\":[{\"ssid\":\"myNetSSID\",\"pwd\":\"myNetPassword\"}],\"wifi_ap\":{\"ssid\":\"BruceNet\",\"pwd\":\"brucenet\"},\"wigleBasicToken\":\"\",\"devMode\":0,\"soundEnabled\":1}]");
      #else
      file.print("[{\"rot\":1,\"dimmerSet\":10,\"bright\":100,\"wuiUsr\":\"admin\",\"wuiPwd\":\"bruce\",\"Bruce_FGCOLOR\":43023,\"irTx\":" + String(LED) + ",\"irRx\":" + String(GROVE_SCL) + ",\"rfTx\":" + String(GROVE_SDA) + ",\"rfRx\":" + String(GROVE_SCL) + ",\"tmz\":3,\"rfModule\":0,\"rfFreq\":433.92,\"rfFxdFreq\":1,\"rfScanRange\":3,\"rfidModule\":" + String(bruceConfig.rfidModule) + ",\"wifi\":[{\"ssid\":\"myNetSSID\",\"pwd\":\"myNetPassword\"}],\"wigleBasicToken\":\"\",\"devMode\":0,\"soundEnabled\":1}]");
    #endif
    }
    file.close();
    delay(50);
  } else log_i("getConfigs: config.conf exists");

  File file;
  file = fs->open(CONFIG_FILE, FILE_READ);
  if(file) {
    // Deserialize the JSON document
    DeserializationError error;
    JsonObject setting;
    error = deserializeJson(settings, file);
    if (error) {
      log_i("Failed to read file, using default configuration");
      goto Default;
    } else log_i("getConfigs: deserialized correctly");

    setting = settings[0];
    if(setting.containsKey("bright"))    { bruceConfig.bright    = setting["bright"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("dimmerSet")) { bruceConfig.dimmerSet = setting["dimmerSet"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rot"))       { bruceConfig.rotation  = setting["rot"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("Bruce_FGCOLOR"))   { FGCOLOR   = setting["Bruce_FGCOLOR"].as<uint16_t>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("wuiUsr"))   { bruceConfig.wuiUsr   = setting["wuiUsr"].as<String>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("wuiPwd"))   { bruceConfig.wuiPwd   = setting["wuiPwd"].as<String>(); } else { count++; log_i("Fail"); }

    if(setting.containsKey("irTx"))        { bruceConfig.irTx        = setting["irTx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("irRx"))        { bruceConfig.irRx        = setting["irRx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfTx"))        { bruceConfig.rfTx        = setting["rfTx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfRx"))        { bruceConfig.rfRx        = setting["rfRx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("tmz"))         { bruceConfig.tmz         = setting["tmz"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfModule"))    { bruceConfig.rfModule    = setting["rfModule"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfFreq"))      { bruceConfig.rfFreq      = setting["rfFreq"].as<float>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfFxdFreq"))   { bruceConfig.rfFxdFreq   = setting["rfFxdFreq"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfScanRange")) { bruceConfig.rfScanRange = setting["rfScanRange"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rfidModule"))  { bruceConfig.rfidModule  = setting["rfidModule"].as<int>(); } else { count++; log_i("Fail"); }

    if(!setting.containsKey("wifi"))  { count++; log_i("Fail"); }

    if(setting.containsKey("wifi_ap")) {
      JsonObject wifiAp = setting["wifi_ap"].as<JsonObject>();
      if (wifiAp.containsKey("ssid")) { ap_ssid = wifiAp["ssid"].as<String>(); } else { count++; log_i("Fail"); }
      if (wifiAp.containsKey("pwd"))  { ap_pwd  = wifiAp["pwd"].as<String>(); } else { count++; log_i("Fail"); }
    } else {
      count++; log_i("Fail");
    }

    if(setting.containsKey("wigleBasicToken"))  { bruceConfig.wigleBasicToken  = setting["wigleBasicToken"].as<String>(); } else { count++; log_i("Fail"); }

    if(setting.containsKey("devMode"))  { bruceConfig.devMode  = setting["devMode"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("soundEnabled"))  { bruceConfig.soundEnabled = setting["soundEnabled"].as<int>(); } else { count++; log_i("Fail"); }

    log_i("Brightness: %d", bruceConfig.bright);
    setBrightness(bruceConfig.bright);
    if(bruceConfig.dimmerSet<0) bruceConfig.dimmerSet=10;
    file.close();
    if(count>0) saveConfigs();

    sync_eeprom_values();
    log_i("Using config.conf setup file");
  } else {
      goto Default;
      log_i("Using settings stored on EEPROM");
  }

Default:
    //saveConfigs();
    //Serial.println("Sd Unmounted. Using settings stored on EEPROM");
    //closeSdCard();
    Serial.println("End of Config");
}

/*********************************************************************
**  Function: saveConfigs
**  save configs into JSON config.conf file
**********************************************************************/
void saveConfigs() {
  // Delete existing file, otherwise the configuration is appended to the file
  FS* fs = &LittleFS;
  if(setupSdCard()) fs = &SD;  // prefer SD card if available

  JsonObject setting = settings[0];
  setting["bright"] = bruceConfig.bright;
  setting["dimmerSet"] = bruceConfig.dimmerSet;
  setting["rot"] = bruceConfig.rotation;
  setting["Bruce_FGCOLOR"] = FGCOLOR;
  setting["wuiUsr"] = bruceConfig.wuiUsr;
  setting["wuiPwd"] = bruceConfig.wuiPwd;
  setting["irTx"] = bruceConfig.irTx;
  setting["irRx"] = bruceConfig.irRx;
  setting["rfTx"] = bruceConfig.rfTx;
  setting["rfRx"] = bruceConfig.rfRx;
  setting["rfModule"] = bruceConfig.rfModule;
  setting["rfFreq"] = bruceConfig.rfFreq;
  setting["rfFxdFreq"] = bruceConfig.rfFxdFreq;
  setting["rfScanRange"] = bruceConfig.rfScanRange;
  setting["rfidModule"] = bruceConfig.rfidModule;
  setting["tmz"] = bruceConfig.tmz;
  if(!setting.containsKey("wifi")) {
    JsonArray WifiList = setting["wifi"].to<JsonArray>();
    if(WifiList.size()<1) {
      JsonObject WifiObj = WifiList.add<JsonObject>();
      WifiObj["ssid"] = "myNetSSID";
      WifiObj["pwd"] = "myNetPassword";
    }
  }
  if(!setting.containsKey("wifi_ap")) {
    JsonObject WifiAp = setting["wifi_ap"].to<JsonObject>();
    WifiAp["ssid"] = ap_ssid;
    WifiAp["pwd"] = ap_pwd;
  }
  setting["wigleBasicToken"] = bruceConfig.wigleBasicToken;
  setting["devMode"] = bruceConfig.devMode;
  setting["soundEnabled"] = bruceConfig.soundEnabled;
  // Open file for writing
  File file = fs->open(CONFIG_FILE, FILE_WRITE);
  if (!file) {
    log_i("Failed to create file");
    file.close();
    return;
  } else log_i("config.conf created");
  // Serialize JSON to file
  serializeJsonPretty(settings,Serial);
  if (serializeJsonPretty(settings, file) < 5) {
    log_i("Failed to write to file");
  } else log_i("config.conf written successfully");

  // Close the file
  file.close();
}
