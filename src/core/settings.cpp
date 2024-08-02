#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "powerSave.h"
#include <EEPROM.h>
/*
EEPROM ADDRESSES MAP


0	Rotation	16		    32	Pass	48	Pass	64	Pass	80	Pass	96		112
1	Dim(N/L)	17		    33	Pass	49	Pass	65	Pass	81	Pass	97		113
2	Bright	  18		    34	Pass	50	Pass	66	Pass	82	Pass	98		114
3	-	        19		    35	Pass	51	Pass	67	Pass	83	Pass	99		115
4	-	        20	Pass	36	Pass	52	Pass	68	Pass	84	Pass	100		116
5	-	        21	Pass	37	Pass	53	Pass	69	Pass	85		    101		117
6	IrTX	    22	Pass	38	Pass	54	Pass	70	Pass	86		    102		118	(L-odd)
7	IrRx	    23	Pass	39	Pass	55	Pass	71	Pass	87		    103		119	(L-odd)
8	RfTX	    24	Pass	40	Pass	56	Pass	72	Pass	88		    104		120	(L-even)
9	RfRx	    25	Pass	41	Pass	57	Pass	73	Pass	89		    105		121	(L-even)
10 TimeZone	26	Pass	42	Pass	58	Pass	74	Pass	90		    106		122	(L-BGCOLOR)
11 FGCOLOR  27	Pass	43	Pass	59	Pass	75	Pass	91		    107		123	(L-BGCOLOR)
12 FGCOLOR  28	Pass	44	Pass	60	Pass	76	Pass	92		    108		124	(L-FGCOLOR)
13		      29	Pass	45	Pass	61	Pass	77	Pass	93		    109		125	(L-FGCOLOR)
14		      30	Pass	46	Pass	62	Pass	78	Pass	94		    110		126	(L-AskSpiffs)
15		      31	Pass	47	Pass	63	Pass	79	Pass	95		    111		127	(L-OnlyBins)

From 1 to 5: Nemo shared addresses
(L -*) stands for Launcher addresses



*/



/*********************************************************************
**  Function: setBrightness
**  save brightness value into EEPROM
**********************************************************************/
void setBrightness(int brightval, bool save) {
  if(bright>100) bright=100;

  #if !defined(STICK_C_PLUS)
  int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval/100 ));
  analogWrite(BACKLIGHT, bl);
  #else
  axp192.ScreenBreath(brightval);
  #endif

  if(save){
    bright=brightval;
    EEPROM.begin(EEPROMSIZE); // open eeprom
    EEPROM.write(2, brightval); //set the byte
    EEPROM.commit(); // Store data to EEPROM
    EEPROM.end(); // Free EEPROM memory
  }
}

/*********************************************************************
**  Function: getBrightness
**  save brightness value into EEPROM
**********************************************************************/
void getBrightness() {
  EEPROM.begin(EEPROMSIZE);
  bright = EEPROM.read(2);
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
**  Function: setDimmerTime
**  Set a timer for screen dimmer
**********************************************************************/
void setDimmerTime(int dimmerTime) {
  if(dimmerTime>60 || dimmerTime<0) dimmerTime = 0;

  dimmerSet=dimmerTime;
  EEPROM.begin(EEPROMSIZE); // open eeprom
  EEPROM.write(1, dimmerSet); //set the byte
  EEPROM.commit(); // Store data to EEPROM
  EEPROM.end(); // Free EEPROM memory
}

/*********************************************************************
**  Function: getDimmerSet
**  Get dimmerSet value from EEPROM
**********************************************************************/
void getDimmerSet() {
  EEPROM.begin(EEPROMSIZE);
  dimmerSet = EEPROM.read(1);
  EEPROM.end(); // Free EEPROM memory
  if(dimmerSet>60 || dimmerSet<0) setDimmerTime(0);
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
    {"Main Menu", [=]() { backToMenu(); }},
  };
  delay(200);
  loopOptions(options, true);
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
  options = {
    {"10s", [=]() { setDimmerTime(10); }},
    {"20s", [=]() { setDimmerTime(20); }},
    {"30s", [=]() { setDimmerTime(30); }},
    {"60s", [=]() { setDimmerTime(60); }},
    {"Disabled", [=]() { setDimmerTime(0); }},
  };
  delay(200);
  loopOptions(options);
  delay(200);
}

/*********************************************************************
**  Function: setUIColor
**  Set and store main UI color
**********************************************************************/
void setUIColor(){
    EEPROM.begin(EEPROMSIZE);
    //int color = EEPROM.read(5);

    options = {
      {"Default",   [&]() { FGCOLOR=0xA80F;     }},
      {"White",     [&]() { FGCOLOR=TFT_WHITE;  }},
      {"Red",       [&]() { FGCOLOR=TFT_RED;    }},
      {"Green",     [&]() { FGCOLOR=TFT_DARKGREEN; }},
      {"Blue",      [&]() { FGCOLOR=TFT_BLUE;  }},
      {"Yellow",    [&]() { FGCOLOR=TFT_YELLOW;  }},
      {"Magenta",   [&]() { FGCOLOR=TFT_MAGENTA;  }},
      {"Orange",    [&]() { FGCOLOR=TFT_ORANGE; }},
      {"Main Menu", [=]() { backToMenu(); }},
    };
    delay(200);
    loopOptions(options);
    tft.setTextColor(TFT_BLACK, FGCOLOR);

    EEPROM.begin(EEPROMSIZE);
    EEPROM.write(11, int((FGCOLOR >> 8) & 0x00FF));
    EEPROM.write(12, int(FGCOLOR & 0x00FF));
    EEPROM.commit();
    EEPROM.end();
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
              {"Brasilia",  [&]() { timeClient.setTimeOffset(-3 * 3600); tmz=0; }},
              {"Pernambuco",[&]() { timeClient.setTimeOffset(-2 * 3600); tmz=1; }},
              {"New York",  [&]() { timeClient.setTimeOffset(-4 * 3600); tmz=2; }},
              {"Lisbon",    [&]() { timeClient.setTimeOffset(1 * 3600);  tmz=3; }},
              {"Hong Kong", [&]() { timeClient.setTimeOffset(8 * 3600);  tmz=4; }},
              {"Sydney",    [&]() { timeClient.setTimeOffset(10 * 3600); tmz=5; }},
              {"Tokyo",     [&]() { timeClient.setTimeOffset(9 * 3600);  tmz=6; }},
              {"Moscow",    [&]() { timeClient.setTimeOffset(3 * 3600);  tmz=7; }},
              {"Amsterdan", [&]() { timeClient.setTimeOffset(2 * 3600);  tmz=8; }},
              {"Main Menu", [=]() { backToMenu(); }},
            };
            if (!returnToMenu) {
                delay(200);
                loopOptions(options);
                EEPROM.begin(EEPROMSIZE); // open eeprom
                EEPROM.write(10, tmz);     // set the byte
                EEPROM.commit();          // Store data to EEPROM
                EEPROM.end();             // Free EEPROM memory

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
    tft.drawRect(10, 10, tft.width()-16,118, FGCOLOR);
    tft.setCursor(64, tft.height()/3+5);
    tft.setTextSize(4);
    #if defined(HAS_RTC)
      _rtc.GetBm8563Time();
      _rtc.GetTime(&_time);
      tft.printf("%02d:%02d:%02d", _time.Hours, _time.Minutes, _time.Seconds);
    #else
      tft.println(timeStr);
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
  int result = EEPROM.read(7);
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
    EEPROM.write(7, result);
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
  int result = EEPROM.read(8);
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
    EEPROM.write(8, result);
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
  int result = EEPROM.read(9);
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
    EEPROM.write(9, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  RfRx = result;
  return result;
}

void getConfigs() {
  bool EEPROMSave=false;
  int count=0;
  if(setupSdCard()) {
    if(!SD.exists(CONFIG_FILE)) {
      File file;
      file = SD.open(CONFIG_FILE, FILE_WRITE);
      if(file) {
        #if ROTATION >1
        file.print("[{\"rot\":3,\"dimmerSet\":10,\"bright\":100,\"wui_usr\":\"admin\",\"wui_pwd\":\"bruce\",\"Bruce_FGCOLOR\":43023,\"IrTx\":"+String(LED)+",\"IrRx\":"+String(GROVE_SCL)+",\"RfTx\":"+String(GROVE_SDA)+",\"RfRx\":"+String(GROVE_SCL)+",\"tmz\":3,\"wifi\":[{\"ssid\":\"myNetSSID\",\"pwd\":\"myNetPassword\"}]}]");
        #else
        file.print("[{\"rot\":1,\"dimmerSet\":10,\"bright\":100,\"wui_usr\":\"admin\",\"wui_pwd\":\"bruce\",\"Bruce_FGCOLOR\":43023,\"IrTx\":"+String(LED)+",\"IrRx\":"+String(GROVE_SCL)+",\"RfTx\":"+String(GROVE_SDA)+",\"RfRx\":"+String(GROVE_SCL)+",\"tmz\":3,\"wifi\":[{\"ssid\":\"myNetSSID\",\"pwd\":\"myNetPassword\"}]}]");
        #endif
      }
      file.close();
      delay(50);
    } else log_i("getConfigs: config.conf exists");

    File file;
    file = SD.open(CONFIG_FILE, FILE_READ);
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
        if(setting.containsKey("bright"))    { bright    = setting["bright"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("dimmerSet")) { dimmerSet = setting["dimmerSet"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("rot"))       { rotation  = setting["rot"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("Bruce_FGCOLOR"))   { FGCOLOR   = setting["Bruce_FGCOLOR"].as<uint16_t>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("wui_usr"))   { wui_usr   = setting["wui_usr"].as<String>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("wui_pwd"))   { wui_pwd   = setting["wui_pwd"].as<String>(); } else { count++; log_i("Fail"); }

        if(setting.containsKey("IrTx"))    { IrTx    = setting["IrTx"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("IrRx"))    { IrRx    = setting["IrRx"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("RfTx"))    { RfTx    = setting["RfTx"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("RfRx"))    { RfRx    = setting["RfRx"].as<int>(); } else { count++; log_i("Fail"); }
        if(setting.containsKey("tmz"))     { tmz    = setting["tmz"].as<int>(); } else { count++; log_i("Fail"); }

        if(!setting.containsKey("wifi"))  { count++; log_i("Fail"); }

        log_i("Brightness: %d", bright);
        setBrightness(bright);
        if(dimmerSet<10) dimmerSet=10;
        file.close();
        if(count>0) saveConfigs();

        count=0;
        EEPROM.begin(EEPROMSIZE); // open eeprom
        if(EEPROM.read(0)!= rotation) { EEPROM.write(0, rotation); count++; }
        if(EEPROM.read(1)!= dimmerSet) { EEPROM.write(1, dimmerSet); count++; }
        if(EEPROM.read(2)!= bright) { EEPROM.write(2, bright);  count++; }
        if(EEPROM.read(6)!= IrTx) { EEPROM.write(6, IrTx); count++; }
        if(EEPROM.read(7)!= IrRx) { EEPROM.write(7, IrRx); count++; }
        if(EEPROM.read(8)!= RfTx) { EEPROM.write(8, RfTx); count++; }
        if(EEPROM.read(9)!= RfRx) { EEPROM.write(9, RfRx); count++; }
        if(EEPROM.read(10)!= tmz) { EEPROM.write(10, tmz); count++; }
        if(EEPROM.read(11)!=(int((FGCOLOR >> 8) & 0x00FF))) {EEPROM.write(11, int((FGCOLOR >> 8) & 0x00FF));  count++; }
        if(EEPROM.read(12)!= int(FGCOLOR & 0x00FF)) { EEPROM.write(12, int(FGCOLOR & 0x00FF)); count++; }
        //If something changed, saves the changes on EEPROM.
        if(count>0) {
          if(!EEPROM.commit()) log_i("fail to write EEPROM");      // Store data to EEPROM
        } else log_i("Wrote new conf to EEPROM");
        EEPROM.end();
        log_i("Using config.conf setup file");
    } else {
        goto Default;
        log_i("Using settings stored on EEPROM");
    }
  }
  else {
Default:
      //saveConfigs();
    Serial.println("Sd Unmounted. Using settings stored on EEPROM");
    }
  //closeSdCard();
  Serial.println("Enf o Config");
}
/*********************************************************************
**  Function: saveConfigs
**  save configs into JSON config.conf file
**********************************************************************/
void saveConfigs() {
  // Delete existing file, otherwise the configuration is appended to the file
  if(setupSdCard()) {
    JsonObject setting = settings[0];
    setting["bright"] = bright;
    setting["dimmerSet"] = dimmerSet;
    setting["rot"] = rotation;
    setting["Bruce_FGCOLOR"] = FGCOLOR;
    setting["wui_usr"] = wui_usr;
    setting["wui_pwd"] = wui_pwd;
    setting["IrTx"] = IrTx;
    setting["IrRx"] = IrRx;
    setting["RfTx"] = RfTx;
    setting["RfRx"] = RfRx;
    setting["tmz"] = tmz;
    if(!setting.containsKey("wifi")) {
      JsonArray WifiList = setting["wifi"].to<JsonArray>();
      if(WifiList.size()<1) {
        JsonObject WifiObj = WifiList.add<JsonObject>();
        WifiObj["ssid"] = "myNetSSID";
        WifiObj["pwd"] = "myNetPassword";
      }
    }
    // Open file for writing
    File file = SD.open(CONFIG_FILE, FILE_WRITE);
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
  } else log_i("saveConfig: SdCard Unmounted.");
}
