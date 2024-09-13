#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "powerSave.h"
#include <EEPROM.h>
#include "modules/rf/rf.h"  // for initRfModule

#ifdef USE_CC1101_VIA_SPI
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#endif

/*
EEPROM ADDRESSES MAP


0	Rotation	  16		    32	Pass	48	Pass	64	Pass	80	Pass	96		112
1	Dim(N/L)	  17		    33	Pass	49	Pass	65	Pass	81	Pass	97		113
2	Bright	    18		    34	Pass	50	Pass	66	Pass	82	Pass	98		114
3	-	          19		    35	Pass	51	Pass	67	Pass	83	Pass	99		115
4	-	          20	Pass	36	Pass	52	Pass	68	Pass	84	Pass	100		116
5	-	          21	Pass	37	Pass	53	Pass	69	Pass	85		    101		117
6	IrTx	      22	Pass	38	Pass	54	Pass	70	Pass	86		    102		118	(L-odd)
7	IrRx	      23	Pass	39	Pass	55	Pass	71	Pass	87		    103		119	(L-odd)
8	RfTx	      24	Pass	40	Pass	56	Pass	72	Pass	88		    104		120	(L-even)
9	RfRx	      25	Pass	41	Pass	57	Pass	73	Pass	89		    105		121	(L-even)
10 TimeZone	  26	Pass	42	Pass	58	Pass	74	Pass	90		    106		122	(L-BGCOLOR)
11 FGCOLOR    27	Pass	43	Pass	59	Pass	75	Pass	91		    107		123	(L-BGCOLOR)
12 FGCOLOR    28	Pass	44	Pass	60	Pass	76	Pass	92		    108		124	(L-FGCOLOR)
13 RfModule   29	Pass	45	Pass	61	Pass	77	Pass	93		    109		125	(L-FGCOLOR)
14 RfidModule 30	Pass	46	Pass	62	Pass	78	Pass	94		    110		126	(L-AskSpiffs)
15		        31	Pass	47	Pass	63	Pass	79	Pass	95		    111		127	(L-OnlyBins)

From 1 to 5: Nemo shared addresses
(L -*) stands for Launcher addresses



*/

/*********************************************************************
**  Function: setBrightness
**  save brightness value into EEPROM
**********************************************************************/
void setBrightness(int brightval, bool save) {
  if(appConfig.getBright()>100) appConfig.setBright(100);

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
  #endif

  if(save) appConfig.setBright(brightval);
}

/*********************************************************************
**  Function: getBrightness
**  get brightness value from EEPROM
**********************************************************************/
void getBrightness() {
  EEPROM.begin(EEPROMSIZE);
  int brightval = EEPROM.read(EEPROM_BRIGHT);
  EEPROM.end(); // Free EEPROM memory

  if (brightval > 100) {
    brightval = 100;
    setBrightness(brightval);
  }

  #if defined(STICK_C_PLUS2) || defined(CARDPUTER)
  int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval/100 ));
  analogWrite(BACKLIGHT, bl);
  #elif defined(STICK_C_PLUS)
  axp192.ScreenBreath(brightval);
  #elif defined(CORE2)
  M5.Axp.ScreenBreath(brightval);
  #elif defined(CORE)
  uint8_t _tmp = (255*brightval)/100;
  M5.Lcd.setBrightness(_tmp);
  #elif defined(M5STACK)
    M5.Display.setBrightness(brightval);
  #endif
}

/*********************************************************************
**  Function: gsetRotation
**  get orientation from EEPROM
**********************************************************************/
int gsetRotation(bool set){
  EEPROM.begin(EEPROMSIZE);
  int getRot = EEPROM.read(EEPROM_ROT);
  EEPROM.end(); // Free EEPROM memory
  Serial.printf("EEPROM read rotation=%d\n", getRot);

  int result = ROTATION;

  if(getRot==1 && set) result = 3;
  else if(getRot==3 && set) result = 1;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }

  if(set) {
    appConfig.setRotation(result);
    delay(10);
    Serial.printf("Applying tft rotation=%d\n", result);
    tft.setRotation(result);
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
  appConfig.setDimmer(dimmerTime);
}

/*********************************************************************
**  Function: setBrightnessMenu
**  Handles Menu to set brightness
**********************************************************************/
void setBrightnessMenu() {
  int idx=0;
  int bright = appConfig.getBright();
  if(bright==100) idx=0;
  else if(bright==75) idx=1;
  else if(bright==50) idx=2;
  else if(bright==25) idx=3;
  else if(bright== 1) idx=4;

  options = {
    {"100%", [=]() { setBrightness(100); }, bright == 100 },
    {"75 %", [=]() { setBrightness(75);  }, bright == 75 },
    {"50 %", [=]() { setBrightness(50);  }, bright == 50 },
    {"25 %", [=]() { setBrightness(25);  }, bright == 25 },
    {" 0 %", [=]() { setBrightness(1);   }, bright == 1 },
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
  int dimmerSet = appConfig.getDimmer();
  if(dimmerSet==10) idx=0;
  else if(dimmerSet==20) idx=1;
  else if(dimmerSet==30) idx=2;
  else if(dimmerSet==60) idx=3;
  else if(dimmerSet== 0) idx=4;
  options = {
    {"10s", [=]() { setDimmerTime(10); }, dimmerSet == 10 },
    {"20s", [=]() { setDimmerTime(20); }, dimmerSet == 20 },
    {"30s", [=]() { setDimmerTime(30); }, dimmerSet == 30 },
    {"60s", [=]() { setDimmerTime(60); }, dimmerSet == 60 },
    {"Disabled", [=]() { setDimmerTime(0); }, dimmerSet == 0 },
  };
  delay(200);
  loopOptions(options,idx);
  delay(200);
}

/*********************************************************************
**  Function: setUIColor
**  Set and store main UI color
**********************************************************************/
void setUIColor(){
    int idx=0;
    if(FGCOLOR==0xA80F) idx=0;
    else if(FGCOLOR==TFT_WHITE) idx=1;
    else if(FGCOLOR==TFT_RED) idx=2;
    else if(FGCOLOR==TFT_DARKGREEN) idx=3;
    else if(FGCOLOR==TFT_BLUE) idx=4;
    else if(FGCOLOR==TFT_YELLOW) idx=5;
    else if(FGCOLOR==TFT_MAGENTA) idx=6;
    else if(FGCOLOR==TFT_ORANGE) idx=7;

    options = {
      {"Default",   [&]() { FGCOLOR=0xA80F;        }, FGCOLOR==0xA80F        },
      {"White",     [&]() { FGCOLOR=TFT_WHITE;     }, FGCOLOR==TFT_WHITE     },
      {"Red",       [&]() { FGCOLOR=TFT_RED;       }, FGCOLOR==TFT_RED       },
      {"Green",     [&]() { FGCOLOR=TFT_DARKGREEN; }, FGCOLOR==TFT_DARKGREEN },
      {"Blue",      [&]() { FGCOLOR=TFT_BLUE;      }, FGCOLOR==TFT_BLUE      },
      {"Yellow",    [&]() { FGCOLOR=TFT_YELLOW;    }, FGCOLOR==TFT_YELLOW    },
      {"Magenta",   [&]() { FGCOLOR=TFT_MAGENTA;   }, FGCOLOR==TFT_MAGENTA   },
      {"Orange",    [&]() { FGCOLOR=TFT_ORANGE;    }, FGCOLOR==TFT_ORANGE    },
      {"Main Menu", [=]() { backToMenu(); }},
    };
    delay(200);
    loopOptions(options, idx);

    tft.setTextColor(TFT_BLACK, FGCOLOR);
    appConfig.setBruceFgColor(FGCOLOR);
}

/*********************************************************************
**  Function: setSoundConfig
**  Enable or disable sound
**********************************************************************/
void setSoundConfig() {
  int result = 0;

  options = {
    {"Sound off", [&]() { result = 0; }, appConfig.getSoundEnabled() == 0},
    {"Sound on",  [&]() { result = 1; }, appConfig.getSoundEnabled() == 1},
  };
  delay(200);
  loopOptions(options, appConfig.getSoundEnabled());
  delay(200);

  appConfig.setSoundEnabled(result);
}

/*********************************************************************
**  Function: setRFModuleMenu
**  Handles Menu to set the RF module in use
**********************************************************************/
void setRFModuleMenu() {
  // TODO: save the setting in the EEPROM too?
  int result = 0;
  int idx=0;

  options = {
    {"M5 RF433T/R",    [&]() { result = M5_RF_MODULE; }, appConfig.getRfModule() == M5_RF_MODULE},
#ifdef USE_CC1101_VIA_SPI
    {"CC1101 on SPI",  [&]() { result = CC1101_SPI_MODULE; }, appConfig.getRfModule() == CC1101_SPI_MODULE},
#endif
/* WIP:
 * #ifdef USE_CC1101_VIA_PCA9554
 * {"CC1101+PCA9554",  [&]() { result = 2; }},
 * #endif
*/
  };
  delay(200);
  loopOptions(options, appConfig.getRfModule());  // 2fix: idx highlight not working?
  delay(200);

  if(result == CC1101_SPI_MODULE) {
    #ifdef USE_CC1101_VIA_SPI
    ELECHOUSE_cc1101.Init();
    if (ELECHOUSE_cc1101.getCC1101()){
      appConfig.setRfModule(result);
      return;
    }
    #endif
    // else display an error
    result = M5_RF_MODULE;
    displayError("CC1101 not found");
    while(!checkAnyKeyPress());
  }

  // fallback to "M5 RF433T/R" on errors
  appConfig.setRfModule(result);
}

/*********************************************************************
**  Function: setRFFreqMenu
**  Handles Menu to set the default frequency for the RF module
**********************************************************************/
void setRFFreqMenu() {
  // TODO: save the setting in the EEPROM too?
  float result = 433.92;
  String freq_str = keyboard(String(appConfig.getRfFreq()), 10, "Default frequency:");
  if(freq_str.length() > 1) result = freq_str.toFloat();  // returns 0 if not valid

  if(result < 300 || result > 928) { // TODO: check valid freq according to current module?
    displayError("Invalid frequency");
    result = 433.92;
  }

  appConfig.setRfFreq(result);
  delay(1000);
}

/*********************************************************************
**  Function: setRFIDModuleMenu
**  Handles Menu to set the RFID module in use
**********************************************************************/
void setRFIDModuleMenu() {
  int result = 0;

  options = {
    {"M5 RFID2",      [&]() { result = M5_RFID2_MODULE; },  appConfig.getRfidModule() == M5_RFID2_MODULE},
    {"PN532 on I2C",  [&]() { result = PN532_I2C_MODULE; }, appConfig.getRfidModule() == PN532_I2C_MODULE},
    {"PN532 on SPI",  [&]() { result = PN532_SPI_MODULE; }, appConfig.getRfidModule() == PN532_SPI_MODULE},
  };
  delay(200);
  loopOptions(options, appConfig.getRfidModule());
  delay(200);

  appConfig.setRfidModule(result);
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
  int tmz = appConfig.getTmz();

  #if defined(HAS_RTC)
    RTC_TimeTypeDef TimeStruct;
    cplus_RTC _rtc;
    _rtc.GetBm8563Time();
  #endif

  options = {
    {"NTP adjust", [&]() { auto_mode=true; }},
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
              {"Brasilia",  [&]() { timeClient.setTimeOffset(-3 * 3600); tmz=0; }, tmz==0 },
              {"Pernambuco",[&]() { timeClient.setTimeOffset(-2 * 3600); tmz=1; }, tmz==1 },
              {"New York",  [&]() { timeClient.setTimeOffset(-4 * 3600); tmz=2; }, tmz==2 },
              {"Lisbon",    [&]() { timeClient.setTimeOffset(1 * 3600);  tmz=3; }, tmz==3 },
              {"Hong Kong", [&]() { timeClient.setTimeOffset(8 * 3600);  tmz=4; }, tmz==4 },
              {"Sydney",    [&]() { timeClient.setTimeOffset(10 * 3600); tmz=5; }, tmz==5 },
              {"Tokyo",     [&]() { timeClient.setTimeOffset(9 * 3600);  tmz=6; }, tmz==6 },
              {"Moscow",    [&]() { timeClient.setTimeOffset(3 * 3600);  tmz=7; }, tmz==7 },
              {"Amsterdan", [&]() { timeClient.setTimeOffset(2 * 3600);  tmz=8; }, tmz==8 },
              {"Main Menu", [=]() { backToMenu(); }},
            };
            if (!returnToMenu) {
                delay(200);
                loopOptions(options);

                appConfig.setTmz(tmz);

                delay(200);
                timeClient.begin();
                timeClient.update();
                localTime = myTZ.toLocal(timeClient.getEpochTime());
                #if !defined(HAS_RTC)
                  rtc.setTime(timeClient.getEpochTime());
                #endif

                clock_set=true;
                runClockLoop();
            }
         }
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
  }

   // Checks para sair do loop
    if(checkSelPress() or checkEscPress()) { // Apertar o botão power dos sticks
      tft.fillScreen(BGCOLOR);
      returnToMenu=true;
      break;
      //goto Exit;
    }
  }
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Pin from EEPROM
**********************************************************************/
int gsetIrTxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(EEPROM_IR_TX);
  EEPROM.end();

  if(result>50) appConfig.setIrTx(LED);

  int irTx = appConfig.getIrTx();

  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = IR_TX_PINS;
    int idx=100;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==irTx && idx==100) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [&]() { result = pin.second; }, pin.second == irTx });
    }
    delay(200);
    loopOptions(options, idx);
    delay(200);

    appConfig.setIrTx(result);
  }

  returnToMenu=true;
  return appConfig.getIrTx();
}

/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin from EEPROM
**********************************************************************/
int gsetIrRxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(EEPROM_IR_RX);
  EEPROM.end();

  if(result>45) appConfig.setIrRx(GROVE_SCL);

  int irRx = appConfig.getIrRx();

  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = IR_RX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==irRx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [&]() { result = pin.second; }, pin.second == irRx});
    }
    delay(200);
    loopOptions(options);
    delay(200);

    appConfig.setIrRx(result);
  }

  returnToMenu=true;
  return appConfig.getIrRx();
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin from EEPROM
**********************************************************************/
int gsetRfTxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(EEPROM_RF_TX);
  EEPROM.end();

  if(result>45) appConfig.setRfTx(GROVE_SDA);

  int rfTx = appConfig.getRfTx();

  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = RF_TX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==rfTx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [&]() { result = pin.second; }, pin.second==rfTx});
    }
    delay(200);
    loopOptions(options);
    delay(200);

    appConfig.setRfTx(result);
  }

  returnToMenu=true;
  return appConfig.getRfTx();
}

/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin from EEPROM
**********************************************************************/
int gsetRfRxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(EEPROM_RF_RX);
  EEPROM.end();

  if(result>36) appConfig.setRfRx(GROVE_SCL);

  int rfRx = appConfig.getRfRx();

  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = RF_RX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==rfRx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [&]() { result = pin.second; }, pin.second == rfRx});
    }
    delay(200);
    loopOptions(options);
    delay(200);

    appConfig.setRfRx(result);
  }

  returnToMenu=true;
  return appConfig.getRfRx();
}
