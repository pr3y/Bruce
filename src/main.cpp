#include "core/globals.h"
#include "core/main_menu.h"

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include "esp32-hal-psram.h"


BruceConfig bruceConfig;

MainMenu mainMenu;
SPIClass sdcardSPI;
SPIClass CC_NRF_SPI;

// Public Globals Variables
unsigned long previousMillis = millis();
int prog_handler;    // 0 - Flash, 1 - LittleFS, 3 - Download
String cachedPassword="";
bool interpreter_start = false;
bool sdcardMounted = false;
bool gpsConnected = false;

// wifi globals
// TODO put in a namespace
bool wifiConnected = false;
String wifiIP;

bool BLEConnected = false;
bool returnToMenu;
bool isSleeping = false;
bool isScreenOff = false;
bool dimmer = false;
char timeStr[10];
time_t localTime;
struct tm* timeInfo;
#if defined(HAS_RTC)
  cplus_RTC _rtc;
  bool clock_set = true;
#else
  ESP32Time rtc;
  bool clock_set = false;
#endif

std::vector<Option> options;
const int bufSize = 1024;
uint8_t buff[1024] = {0};
// Protected global variables
#if defined(HAS_SCREEN)
	TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
	TFT_eSprite sprite = TFT_eSprite(&tft);
	TFT_eSprite draw = TFT_eSprite(&tft);
#else
    SerialDisplayClass tft;
    SerialDisplayClass& sprite = tft;
    SerialDisplayClass& draw = tft;
#endif


#include "Wire.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/settings.h"
#include "core/serialcmds.h"
#include "core/wifi_common.h"
#include "modules/others/audio.h"  // for playAudioFile
#include "modules/rf/rf.h"  // for initCC1101once
#include "modules/bjs_interpreter/interpreter.h" // for JavaScript interpreter


/*********************************************************************
**  Function: begin_storage
**  Config LittleFS and SD storage
*********************************************************************/
void begin_storage() {
  if(!LittleFS.begin(true)) { LittleFS.format(), LittleFS.begin();}
  setupSdCard();
}

/*********************************************************************
**  Function: _setup_gpio()
**  Sets up a weak (empty) function to be replaced by /ports/* /interface.h
*********************************************************************/
void _setup_gpio() __attribute__((weak));
void _setup_gpio() { }

/*********************************************************************
**  Function: setup_gpio
**  Setup GPIO pins
*********************************************************************/
void setup_gpio() {

  #if defined(BACKLIGHT)
  pinMode(BACKLIGHT, OUTPUT);
  #endif

  //init setup from /ports/*/interface.h
  _setup_gpio();

  #ifdef USE_CC1101_VIA_SPI
    #if CC1101_MOSI_PIN==TFT_MOSI // (T_EMBED), CORE2 and others
        initCC1101once(&tft.getSPIinstance());
    #elif CC1101_MOSI_PIN==SDCARD_MOSI // (CARDPUTER) and (ESP32S3DEVKITC1) and devices that share CC1101 pin with only SDCard
        initCC1101once(&sdcardSPI);
    #else // (STICK_C_PLUS) || (STICK_C_PLUS2) and others that doesn´t share SPI with other devices (need to change it when Bruce board comes to shore)
        initCC1101once(NULL);
    #endif
  #endif

}

/*********************************************************************
**  Function: begin_tft
**  Config tft
*********************************************************************/
void begin_tft(){
#if defined(HAS_SCREEN)// && !defined(CORE) //Need to test if it will work on Core Fire etc..
  tft.init();
#endif
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(bruceConfig.rotation);
  resetTftDisplay();
  setBrightness(bruceConfig.bright);
}


/*********************************************************************
**  Function: boot_screen
**  Draw boot screen
*********************************************************************/
void boot_screen() {
  tft.setTextColor(bruceConfig.priColor, TFT_BLACK);
  tft.setTextSize(FM);
  tft.drawPixel(0,0,TFT_BLACK);
  tft.drawCentreString("Bruce", WIDTH / 2, 10, SMOOTH_FONT);
  tft.setTextSize(FP);
  tft.drawCentreString(BRUCE_VERSION, WIDTH / 2, 25, SMOOTH_FONT);
  tft.setTextSize(FM);
  tft.drawCentreString("PREDATORY FIRMWARE", WIDTH / 2, HEIGHT+2, SMOOTH_FONT); // will draw outside the screen on non touch devices
}

/*********************************************************************
**  Function: boot_screen_anim
**  Draw boot screen
*********************************************************************/
void boot_screen_anim() {
  boot_screen();
  int i = millis();
  // checks for boot.jpg in SD and LittleFS for customization
  bool boot_img=false;
  if(SD.exists("/boot.jpg")) boot_img = true;
  else if(LittleFS.exists("/boot.jpg")) boot_img = true;
  else if(SD.exists("/boot.gif")) boot_img = true;
  else if(LittleFS.exists("/boot.gif")) boot_img = true;
  // Start image loop
  while(millis()<i+7000) { // boot image lasts for 5 secs
  #if !defined(LITE_VERSION)
    bool drawn=false;
    if((millis()-i>2000) && (millis()-i)<2200){
      tft.fillRect(0,45,WIDTH,HEIGHT-45,bruceConfig.bgColor);
      if(boot_img && !drawn) {
        if(showJpeg(SD,"/boot.jpg") && (millis()-i>2000) && (millis()-i<2200)) { boot_img=true; Serial.println("Image from SD"); }
        else if (showJpeg(LittleFS,"/boot.jpg") && (millis()-i>2000) && (millis()-i<2100)) { boot_img=true; Serial.println("Image from LittleFS"); }
        else if (showGIF(SD,"/boot.gif") && (millis()-i>2000) && (millis()-i<2200)) { boot_img=true; Serial.println("Image from SD"); }
        else if (showGIF(LittleFS,"/boot.gif") && (millis()-i>2000) && (millis()-i<2100)) { boot_img=true; Serial.println("Image from LittleFS"); }
        drawn=true;
      }
    }
    if(!boot_img && (millis()-i>2200) && (millis()-i)<2700) tft.drawRect(2*WIDTH/3,HEIGHT/2,2,2,bruceConfig.priColor);
    if(!boot_img && (millis()-i>2700) && (millis()-i)<2900) tft.fillRect(0,45,WIDTH,HEIGHT-45,bruceConfig.bgColor);
    if(!boot_img && (millis()-i>2900) && (millis()-i)<3400) tft.drawXBitmap(2*WIDTH/3 - 30 ,5+HEIGHT/2,bruce_small_bits, bruce_small_width, bruce_small_height,TFT_BLACK,bruceConfig.priColor);
    if(!boot_img && (millis()-i>3400) && (millis()-i)<3600) tft.fillRect(0,0,WIDTH,HEIGHT,bruceConfig.bgColor);
    if(!boot_img && (millis()-i>3600)) tft.drawXBitmap((WIDTH-238)/2,(HEIGHT-133)/2,bits, bits_width, bits_height,TFT_BLACK,bruceConfig.priColor);
  #endif
    if(checkAnyKeyPress())  // If any key or M5 key is pressed, it'll jump the boot screen
    {
      tft.fillScreen(TFT_BLACK);
      tft.fillScreen(TFT_BLACK);
      delay(10);
      return;
    }
  }

  // Clear splashscreen
  tft.fillScreen(TFT_BLACK);

  // Clear splashscreen
  tft.fillScreen(TFT_BLACK);
}


/*********************************************************************
**  Function: init_clock
**  Clock initialisation for propper display in menu
*********************************************************************/
void init_clock() {
  #if defined(HAS_RTC)
    RTC_TimeTypeDef _time;
    cplus_RTC _rtc;
    _rtc.begin();
    _rtc.GetBm8563Time();
    _rtc.GetTime(&_time);
  #endif
}


/*********************************************************************
**  Function: startup_sound
**  Play sound or tone depending on device hardware
*********************************************************************/
void startup_sound() {
#if !defined(LITE_VERSION)
  #if defined(BUZZ_PIN)
    // Bip M5 just because it can. Does not bip if splashscreen is bypassed
    _tone(5000, 50);
    delay(200);
    _tone(5000, 50);
  /*  2fix: menu infinite loop */
  #elif defined(HAS_NS4168_SPKR)
    // play a boot sound
    if(SD.exists("/boot.wav")) playAudioFile(&SD, "/boot.wav");
    else if(LittleFS.exists("/boot.wav")) playAudioFile(&LittleFS, "/boot.wav");
    setup_gpio(); // temp fix for menu inf. loop
  #endif
#endif
}


/*********************************************************************
**  Function: setup
**  Where the devices are started and variables set
*********************************************************************/
void setup() {
  Serial.setRxBufferSize(SAFE_STACK_BUFFER_SIZE);  // Must be invoked before Serial.begin(). Default is 256 chars
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

  bruceConfig.bright=100; // theres is no value yet
  begin_tft();
  boot_screen();

  begin_storage();
  bruceConfig.fromFile();
  resetTftDisplay();
  tft.setRotation(bruceConfig.rotation);
  setBrightness(bruceConfig.bright);

  init_clock();

  boot_screen_anim();

  startup_sound();

  if (bruceConfig.wifiAtStartup) {
    displayInfo("Connecting WiFi...");
    wifiConnectTask();
  }

  #if ! defined(HAS_SCREEN)
    // start a task to handle serial commands while the webui is running
    startSerialCommandsHandlerTask();
  #endif

  delay(200);
  previousMillis = millis();
  setBrightness(bruceConfig.bright);
}

/**********************************************************************
**  Function: loop
**  Main loop
**********************************************************************/
#if defined(HAS_SCREEN)
void loop() {
  #if defined(HAS_RTC)
    RTC_TimeTypeDef _time;
  #endif
  bool redraw = true;
  long clock_update=0;
  mainMenu.begin();

  // Interpreter must be ran in the loop() function, otherwise it breaks
  // called by 'stack canary watchpoint triggered (loopTask)'
#if !defined(LITE_VERSION)
  #if !defined(CORE) && !defined(CORE2)
    if(interpreter_start) {
      interpreter_start=false;
      interpreter();
      previousMillis = millis(); // ensure that will not dim screen when get back to menu
      //goto END;
    }
  #endif
#endif
  tft.fillRect(0,0,WIDTH,HEIGHT,bruceConfig.bgColor);
  bruceConfig.fromFile();


  while(1){
    if(interpreter_start) goto END;
    if (returnToMenu) {
      returnToMenu = false;
      tft.fillScreen(bruceConfig.bgColor); //fix any problem with the mainMenu screen when coming back from submenus or functions
      redraw=true;
    }

    if (redraw) {
      mainMenu.draw();
      clock_update=0; // forces clock drawing
      redraw = false;
      delay(REDRAW_DELAY);
    }

    handleSerialCommands();
#ifdef HAS_KEYBOARD
    checkShortcutPress();  // shortctus to quickly start apps without navigating the menus
#endif

    if (checkPrevPress()) {
      checkReboot();
      mainMenu.previous();
      redraw = true;
    }
    /* DW Btn to next item */
    if (checkNextPress()) {
      mainMenu.next();
      redraw = true;
    }

    /* Select and run function */
    if (checkSelPress()) {
      mainMenu.openMenuOptions();
      drawMainBorder(true);
      redraw=true;
    }
    // update battery and clock once every 30 seconds
    // it was added to avoid delays in btns readings from Core and improves overall performance
    if(millis()-clock_update>30000) {
      drawBatteryStatus();
      if (clock_set) {
        #if defined(HAS_RTC)
          _rtc.GetTime(&_time);
          setTftDisplay(12, 12, bruceConfig.priColor, 1, bruceConfig.bgColor);
          snprintf(timeStr, sizeof(timeStr), "%02d:%02d", _time.Hours, _time.Minutes);
          tft.print(timeStr);
        #else
          updateTimeStr(rtc.getTimeStruct());
          setTftDisplay(12, 12, bruceConfig.priColor, 1, bruceConfig.bgColor);
          tft.print(timeStr);
        #endif
      }
      else {
        setTftDisplay(12, 12, bruceConfig.priColor, 1, bruceConfig.bgColor);
        tft.print("BRUCE " + String(BRUCE_VERSION));
      }
      clock_update=millis();
    }
  }
  END:
  delay(1);
}
#else

// alternative loop function for headless boards
#include "modules/others/webInterface.h"

void loop() {
  setupSdCard();
  bruceConfig.fromFile();

  if(!wifiConnected) {
    Serial.println("wifiConnect");
    wifiConnectMenu(WIFI_AP);  // TODO: read mode from config file
  }
  Serial.println("startWebUi");
  startWebUi(true);  // MEMO: will quit when checkEscPress
}
#endif
