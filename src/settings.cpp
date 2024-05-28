#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h>




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



void setClock() {

  if(!wifiConnected) wifiConnectMenu();

  options = {
    {"Brasilia", [=]() { timeClient.setTimeOffset(-3 * 3600); }},
    {"Pernambuco", [=]() { timeClient.setTimeOffset(-2 * 3600); }},
    {"New York", [=]() { timeClient.setTimeOffset(-4 * 3600); }},
    {"Lisbon", [=]() { timeClient.setTimeOffset(1 * 3600); }},
    {"Hong Kong", [=]() { timeClient.setTimeOffset(8 * 3600); }},
    {"Sydney", [=]() { timeClient.setTimeOffset(10 * 3600); }},
    {"Tokyo", [=]() { timeClient.setTimeOffset(9 * 3600); }},
    {"Moscow", [=]() { timeClient.setTimeOffset(3 * 3600); }},
    {"Amsterdan", [=]() { timeClient.setTimeOffset(2 * 3600); }},
  };

  delay(200);
  loopOptions(options);
  delay(200);

  timeClient.begin();
  runClockLoop();
}

void runClockLoop() {


  for (;;){

  timeClient.update();
  time_t localTime = myTZ.toLocal(timeClient.getEpochTime());
  struct tm* timeInfo = localtime(&localTime);

  char timeStr[10];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeInfo->tm_hour, timeInfo->tm_min);

  Serial.print("Current time: ");
  Serial.println(timeStr);
  tft.fillScreen(BGCOLOR);
  tft.drawRect(45,40,150,45, FGCOLOR);
  tft.setCursor(60, 50);
  tft.setTextSize(4);
  tft.println(timeStr);
  
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
    }   // apertar ESC no Cardputer
    #endif

  delay(1000); 

  }
}

