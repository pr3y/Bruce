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
#include "core/main_menu.h"
#include "core/serialcmds.h"


/*********************************************************************
**  Function: setup_gpio
**  Setup GPIO pins
*********************************************************************/
void setup_gpio() {
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

  #if defined(BACKLIGHT)
  pinMode(BACKLIGHT, OUTPUT);
  #endif
}


/*********************************************************************
**  Function: begin_tft
**  Config tft
*********************************************************************/
void begin_tft(){
  tft.init();
  rotation = gsetRotation();
  tft.setRotation(rotation);
  resetTftDisplay();
}


/*********************************************************************
**  Function: boot_screen
**  Draw boot screen
*********************************************************************/
void boot_screen() {
  tft.setTextColor(FGCOLOR, TFT_BLACK);
  tft.setTextSize(FM);
  tft.println("Bruce");
  tft.setTextSize(FP);
  tft.println(String(BRUCE_VERSION));
  tft.setTextSize(FM);

  int i = millis();
  while(millis()<i+7000) { // boot image lasts for 5 secs
    if((millis()-i>2000) && (millis()-i)<2200) tft.fillScreen(TFT_BLACK);
    if((millis()-i>2200) && (millis()-i)<2700) tft.drawRect(160,50,2,2,FGCOLOR);
    if((millis()-i>2700) && (millis()-i)<2900) tft.fillScreen(TFT_BLACK);
    if((millis()-i>2900) && (millis()-i)<3400) tft.drawXBitmap(130,45,bruce_small_bits, bruce_small_width, bruce_small_height,TFT_BLACK,FGCOLOR);
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
      return;
    }
  }
}


/*********************************************************************
**  Function: load_eeprom
**  Load EEPROM data
*********************************************************************/
void load_eeprom() {
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

  log_i("\
  \n*-*EEPROM Settings*-* \
  \n- rotation  =%03d, \
  \n- dimmerSet =%03d, \
  \n- Brightness=%03d, \
  \n- IR Tx Pin =%03d, \
  \n- IR Rx Pin =%03d, \
  \n- RF Tx Pin =%03d, \
  \n- RF Rx Pin =%03d, \
  \n- Time Zone =%03d, \
  \n- FGColor   =0x%04X \
  \n*-*-*-*-*-*-*-*-*-*-*", rotation, dimmerSet, bright,IrTx, IrRx, RfTx, RfRx, tmz, FGCOLOR);
  if (rotation>3 || dimmerSet>60 || bright>100 || IrTx>100 || IrRx>100 || RfRx>100 || RfTx>100 || tmz>24) {
    rotation = ROTATION;
    dimmerSet=10;
    bright=100;
    IrTx=LED;
    IrRx=GROVE_SCL;
    RfTx=GROVE_SDA;
    RfRx=GROVE_SCL;
    FGCOLOR=0xA80F;
    tmz=0;

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
    log_w("One of the eeprom values is invalid");
  }
  setBrightness(bright,false);
  EEPROM.end();
}


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

  setup_gpio();
  begin_tft();
  load_eeprom();
  boot_screen();

  if(!LittleFS.begin(true)) { LittleFS.format(), LittleFS.begin();}

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
  int opt = 7;

  tft.fillRect(0,0,WIDTH,HEIGHT,BGCOLOR);
  setupSdCard();
  getConfigs();

  while(1){
    if (returnToMenu) {
      returnToMenu = false;
      tft.fillScreen(BGCOLOR); //fix any problem with the mainMenu screen when coming back from submenus or functions
      redraw=true;
    }

    if (redraw) {
      drawMainMenu(index);
      redraw = false;
      delay(200);
    }

    handleSerialCommands();
    checkShortcutPress();  // shortctus to quickly start apps without navigating the menus
    
    if (checkPrevPress()) {
      if(index==0) index = opt - 1;
      else if(index>0) index--;
      redraw = true;
    }
    /* DW Btn to next item */
    if (checkNextPress()) {
      index++;
      if((index+1)>opt) index = 0;
      redraw = true;
    }

    /* Select and run function */
    if (checkSelPress()) {
      getMainMenuOptions(index);
      redraw=true;
    }

    if (clock_set) {
      updateTimeStr(rtc.getTimeStruct());
      setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);
      tft.print(timeStr);
    }
    else {
      setTftDisplay(12, 12, FGCOLOR, 1, BGCOLOR);
      tft.print("BRUCE " + String(BRUCE_VERSION));
    }
  }
}
