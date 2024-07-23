#include "core/globals.h"

#include <EEPROM.h>
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include "esp32-hal-psram.h"

// Public Globals Variables
unsigned long previousMillis = millis();
int prog_handler;    // 0 - Flash, 1 - LittleFS, 3 - Download
int rotation;
int IrTx;
int IrRx;
int RfTx;
int RfRx;
int dimmerSet;
int bright=100;
int tmz=3;
bool sdcardMounted;
bool wifiConnected;
bool BLEConnected;
bool returnToMenu;
bool isSleeping = false;
bool isScreenOff = false;
bool dimmer = false;
char timeStr[10];
time_t localTime;
struct tm* timeInfo;
ESP32Time rtc;
bool clock_set = false;
JsonDocument settings;

String wui_usr="admin";
String wui_pwd="bruce";
String ssid;
String pwd;
std::vector<std::pair<std::string, std::function<void()>>> options;
const int bufSize = 4096;
uint8_t buff[4096] = {0};
// Protected global variables
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite draw = TFT_eSprite(&tft);

#if defined(CARDPUTER)
  Keyboard_Class Keyboard = Keyboard_Class();
#elif defined (STICK_C_PLUS)
  AXP192 axp192;
#endif

#include "Wire.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/settings.h"
#include "core/wg.h"
#include "core/wifi_common.h"

#include "modules/ble/ble_spam.h"
#include "modules/others/openhaystack.h"
#include "modules/others/tururururu.h"
#include "modules/others/TV-B-Gone.h"
#include "modules/others/webInterface.h"
#include "modules/rf/rf.h"
#include "modules/rfid/rfid.h"
#include "modules/rfid/tag_o_matic.h"
#include "modules/rfid/mfrc522_i2c.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/dpwo.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_atks.h"


#ifdef CARDPUTER
#include "modules/others/bad_usb.h"
#include "modules/others/led_control.h"
#endif


/*********************************************************************
**  Function: setup
**  Where the devices are started and variables set
*********************************************************************/
void setup() {
  Serial.begin(115200);

  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
  if(psramInit()) log_d("PSRAM Started");
  if(psramFound()) log_d("PSRAM Found");
  else log_d("PSRAM Not Found");
  log_d("Total PSRAM: %d", ESP.getPsramSize());
  log_d("Free PSRAM: %d", ESP.getFreePsram());

  // declare variables
  prog_handler=0;
  sdcardMounted=false;
  wifiConnected=false;
  BLEConnected=false;


  // Setup GPIOs and stuff
  #if  defined(STICK_C_PLUS2)
    pinMode(UP_BTN, INPUT);   // Sets the power btn as an INPUT
  #elif defined(STICK_C_PLUS)
    axp192.begin();           // Start the energy management of AXP192
  #endif


  #ifndef CARDPUTER
  pinMode(SEL_BTN, INPUT);
  pinMode(DW_BTN, INPUT);
  pinMode(4, OUTPUT);     // Keeps the Stick alive after take off the USB cable
  digitalWrite(4,HIGH);   // Keeps the Stick alive after take off the USB cable
  #else
  Keyboard.begin();
  pinMode(0, INPUT);
  pinMode(10, INPUT);     // Pin that reads the
  #endif

  tft.init();
  rotation = gsetRotation();
  tft.setRotation(rotation);
  resetTftDisplay();

  #if defined(BACKLIGHT)
  pinMode(BACKLIGHT, OUTPUT);
  #endif

  EEPROM.begin(EEPROMSIZE); // open eeprom
  rotation = EEPROM.read(0);
  dimmerSet = EEPROM.read(1);
  bright = EEPROM.read(2);
  IrTx = EEPROM.read(6);
  IrRx = EEPROM.read(7);
  RfTx = EEPROM.read(8);
  RfRx = EEPROM.read(9);
  tmz = EEPROM.read(10);
  FGCOLOR = EEPROM.read(11) << 8 | EEPROM.read(12);
  //log_i("EEPROM 0=%d, 1=%s, 2=%d, 6=%d, 7=%d, 8=%d, 9=%d, 10=%d, 11-12=%d", rotation, dimmerSet, bright,IrTx, IrRx, RfTx, RfRx, tmz, FGCOLOR);
  if (rotation>3 || dimmerSet>60 || bright>100 || IrTx>100 || IrRx>100 || RfRx>100 || RfTx>100 || tmz>24) {
    rotation = ROTATION;
    dimmerSet=10;
    bright=100;
    IrTx=LED;
    IrRx=GROVE_SCL;
    RfTx=GROVE_SDA;
    RfRx=GROVE_SCL;
    FGCOLOR=0xA80F;
    EEPROM.write(0, rotation);
    EEPROM.write(1, dimmerSet);
    EEPROM.write(2, bright);
    EEPROM.write(6, IrTx);
    EEPROM.write(7, IrRx);
    EEPROM.write(8, RfTx);
    EEPROM.write(9, RfRx);
    EEPROM.write(10, tmz);
    EEPROM.write(11, int((FGCOLOR >> 8) & 0x00FF));
    EEPROM.write(12, int(FGCOLOR & 0x00FF));
    EEPROM.writeString(20,"");
    EEPROM.commit();      // Store data to EEPROM
    EEPROM.end();    
    log_i("One of the eeprom values is invalid");
  }
  EEPROM.end();

  //Start Bootscreen timer

  bool change=false;
  tft.setTextColor(FGCOLOR, TFT_BLACK);
  tft.setTextSize(FM);
  tft.println("Bruce");
  tft.setTextSize(FP);
  tft.println(String(BRUCE_VERSION));
  tft.setTextSize(FM);
  if(!LittleFS.begin(true)) { LittleFS.format(), LittleFS.begin();}
  
  int i = millis();
  while(millis()<i+7000) { // boot image lasts for 5 secs
    if((millis()-i>2000) && (millis()-i)<2200) tft.fillScreen(TFT_BLACK);
    if((millis()-i>2200) && (millis()-i)<2700) tft.drawRect(160,50,2,2,FGCOLOR);
    if((millis()-i>2700) && (millis()-i)<2900) tft.fillScreen(TFT_BLACK);
    if((millis()-i>2900) && (millis()-i)<3400 && !change)  { tft.drawXBitmap(130,45,bruce_small_bits, bruce_small_width, bruce_small_height,TFT_BLACK,FGCOLOR); }
    if((millis()-i>3400) && (millis()-i)<3600) tft.fillScreen(TFT_BLACK);
    if((millis()-i>3600)) tft.drawXBitmap(1,1,bits, bits_width, bits_height,TFT_BLACK,FGCOLOR);
 
  #if defined (CARDPUTER)   // If any key is pressed, it'll jump the boot screen
    Keyboard.update();
    if(Keyboard.isPressed())
  #else
    if(digitalRead(SEL_BTN)==LOW)  // If M5 key is pressed, it'll jump the boot screen
  #endif
     {
        tft.fillScreen(TFT_BLACK);
        delay(10);
        goto Program;
      }
  }

  // If M5 or Enter button is pressed, continue from here
  Program:
  delay(200);
  previousMillis = millis();
}

/**********************************************************************
**  Function: loop
**  Main loop
**********************************************************************/
void loop() {
  bool redraw = true;
  int index = 0;
  int opt = 6; // there are 3 options> 1 list SD files, 2 OTA and 3 Config
  tft.fillRect(0,0,WIDTH,HEIGHT,BGCOLOR);
  setupSdCard();
  getConfigs();  
  while(1){
    if(returnToMenu) {
      returnToMenu = false;
      tft.fillScreen(BGCOLOR); //fix any problem with the mainMenu screen when coming back from submenus or functions
      redraw=true;
    }

    if (redraw) {
      drawMainMenu(index);
      redraw = false;
      delay(200);
    }

    if(checkPrevPress()) {
      if(index==0) index = opt - 1;
      else if(index>0) index--;
      redraw = true;
    }
    /* DW Btn to next item */
    if(checkNextPress()) {
      index++;
      if((index+1)>opt) index = 0;
      redraw = true;
    }

    /* Select and run function */
    if(checkSelPress()) {
      switch(index) {
        case 0:   // WiFi
          if(!wifiConnected) {
            options = {
              {"Connect Wifi", [=]()  { wifiConnectMenu(); }},    //wifi_common.h
              {"WiFi AP", [=]()       { wifiConnectMenu(true); }},//wifi_common.h
            };
          } else {
            options = {
              {"Disconnect", [=]()  { wifiDisconnect(); }},    //wifi_common.h
            };
          }
          options.push_back({"Wifi Atks", [=]()     { wifi_atk_menu(); }});
        #ifndef STICK_C_PLUS
          options.push_back({"TelNET", [=]()        { telnet_setup(); }});
          options.push_back({"SSH", [=]()           { ssh_setup(); }});
        #endif
          options.push_back({"Raw Sniffer", [=]()   { sniffer_setup(); }});
          options.push_back({"DPWO", [=]()          { dpwo_setup(); }});
          options.push_back({"Evil Portal", [=]()   { startEvilPortal(); }});
          options.push_back({"Scan Hosts", [=]()    { local_scan_setup(); }});
        #ifndef STICK_C_PLUS
          options.push_back({"Wireguard", [=]()     { wg_setup(); }});
        #endif
          options.push_back({"Main Menu", [=]()     { backToMenu(); }});
          delay(200);
          loopOptions(options,false,true,"WiFi");
          break;
        case 1: // BLE
          options = {
            {"AppleJuice", [=]()   { aj_adv(0); }},
            {"SwiftPair", [=]()    { aj_adv(1); }},
            {"Samsung Spam", [=]() { aj_adv(2);}},
            {"SourApple", [=]()    { aj_adv(3); }},
            {"Android Spam", [=]() { aj_adv(4); }},
            {"BT Maelstrom", [=]() { aj_adv(5); }},
            {"Main Menu", [=]()    { backToMenu(); }},
          };
          delay(200);
          loopOptions(options,false,true,"Bluetooth");
          break;
        case 2: // RF
          options = {
            //{"Scan/copy", [=]()   { displayRedStripe("Scan/Copy"); }},
            //{"Replay", [=]()      { displayRedStripe("Replay"); }},
            {"Spectrum", [=]()            { rf_spectrum(); }}, //@IncursioHack
            {"Jammer Itmt", [=]() { rf_jammerIntermittent(); }}, //@IncursioHack
            {"Jammer Full", [=]()         { rf_jammerFull(); }}, //@IncursioHack
            {"Main Menu", [=]()   { backToMenu(); }},
          };
          delay(200);
          loopOptions(options,false,true,"Radio Frequency");
          break;
        case 3: // RFID
          options = {
            {"Tag-O-Matic", [=]()   { TagOMatic(); }}, //@RennanCockles
            {"Copy/Write", [=]()   { rfid_setup(); }}, //@IncursioHack
            //{"Replay", [=]()      { displayRedStripe("Replay"); }},
            {"Main Menu", [=]()    { backToMenu(); }},
          };
          delay(200);
          loopOptions(options,false,true,"RFID");
          break;
        case 4: //Other
          options = {
            {"TV-B-Gone", [=]()     { StartTvBGone(); }},
            {"Custom IR", [=]()     { otherIRcodes(); }},
            {"SD Card", [=]()       { loopSD(SD); }},
            {"LittleFS", [=]()      { loopSD(LittleFS); }},
            {"WebUI", [=]()         { loopOptionsWebUi(); }},
            {"Megalodon", [=]()     { shark_setup(); }},
          };
          #ifdef CARDPUTER
          options.push_back({"BadUSB", [=]()        { usb_setup(); }});
          options.push_back({"LED Control", [=]()   { ledrgb_setup(); }}); //IncursioHack
          options.push_back({"LED FLash", [=]()     { ledrgb_flash(); }}); // IncursioHack

          #endif
          options.push_back({"Openhaystack", [=]()  { openhaystack_setup(); }});
          options.push_back({"Main Menu", [=]()     { backToMenu(); }});
          delay(200);
          loopOptions(options,false,true,"Others");
          break;
        case 5: //Config
          options = {
            {"Brightness",    [=]() { setBrightnessMenu();   saveConfigs();}},                 //settings.h
            {"Dim Time",      [=]() { setDimmerTimeMenu();   saveConfigs();}},             
            {"Clock",         [=]() { setClock();            saveConfigs();}},                      //settings.h
            {"Orientation",   [=]() { gsetRotation(true);    saveConfigs();}},               //settings.h
            {"UI Color",      [=]() { setUIColor();          saveConfigs();}},
            {"Ir TX Pin",     [=]() { gsetIrTxPin(true);     saveConfigs();}},                 //settings.h
            {"Ir RX Pin",     [=]() { gsetIrRxPin(true);     saveConfigs();}},                 //settings.h
            #ifndef CARDPUTER
            {"RF TX Pin",     [=]() { gsetRfTxPin(true);     saveConfigs();}},                 //settings.h
            {"RF RX Pin",     [=]() { gsetRfRxPin(true);     saveConfigs();}},                 //settings.h
            #endif
            {"Sleep",    [=]() { setSleepMode(); }},
            {"Restart",       [=]() { ESP.restart(); }},
            {"Main Menu",     [=]() { backToMenu(); }},
          };
          delay(200);
          loopOptions(options,false,true,"Config");
          break;
      }
      redraw=true;
    }
    if(clock_set) {
      updateTimeStr(rtc.getTimeStruct());
      setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);
      tft.print(timeStr);
    }
   else{
      setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);
      tft.print("BRUCE " + String(BRUCE_VERSION));
    }
  }
}
