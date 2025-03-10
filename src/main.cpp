#include <globals.h>
#include "core/main_menu.h"

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include "esp32-hal-psram.h"
#include "core/utils.h"
#include "core/powerSave.h"
#include "esp_task_wdt.h"

BruceConfig bruceConfig;

StartupApp startupApp;
MainMenu mainMenu;
SPIClass sdcardSPI;
SPIClass CC_NRF_SPI;

// Navigation Variables
volatile bool NextPress=false;
volatile bool PrevPress=false;
volatile bool UpPress=false;
volatile bool DownPress=false;
volatile bool SelPress=false;
volatile bool EscPress=false;
volatile bool AnyKeyPress=false;
volatile bool NextPagePress=false;
volatile bool PrevPagePress=false;

TouchPoint touchPoint;

keyStroke KeyStroke;

TaskHandle_t xHandle;
void __attribute__((weak)) taskInputHandler(void *parameter) {
  while (true) {
    checkPowerSaveTime();
    NextPress=false;
    PrevPress=false;
    UpPress=false;
    DownPress=false;
    SelPress=false;
    EscPress=false;
    AnyKeyPress=false;
    NextPagePress=false;
    PrevPagePress=false;
    touchPoint.pressed=false;
    InputHandler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
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
RTC_TimeTypeDef _time;
RTC_DateTypeDef _date;
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
volatile int tftWidth = TFT_HEIGHT;
#ifdef HAS_TOUCH
volatile int tftHeight = TFT_WIDTH-20; // 20px to draw the TouchFooter(), were the btns are being read in touch devices.
#else
volatile int tftHeight = TFT_WIDTH;
#endif
#else
SerialDisplayClass tft;
SerialDisplayClass& sprite = tft;
SerialDisplayClass& draw = tft;
volatile int tftWidth = VECTOR_DISPLAY_DEFAULT_HEIGHT;
volatile int tftHeight = VECTOR_DISPLAY_DEFAULT_WIDTH;
#endif


#include <Wire.h>
#include "core/display.h"
#include "core/led_control.h"
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
  bruceConfig.fromFile();
}

/*********************************************************************
 **  Function: _setup_gpio()
 **  Sets up a weak (empty) function to be replaced by /ports/* /interface.h
 *********************************************************************/
void _setup_gpio() __attribute__((weak));
void _setup_gpio() { }

/*********************************************************************
 **  Function: _post_setup_gpio()
 **  Sets up a weak (empty) function to be replaced by /ports/* /interface.h
 *********************************************************************/
void _post_setup_gpio() __attribute__((weak));
void _post_setup_gpio() { }

/*********************************************************************
 **  Function: setup_gpio
 **  Setup GPIO pins
 *********************************************************************/
void setup_gpio() {

  //init setup from /ports/*/interface.h
  _setup_gpio();

#ifdef USE_CC1101_VIA_SPI
  if(bruceConfig.CC1101_bus.mosi == (gpio_num_t)TFT_MOSI) initCC1101once(&tft.getSPIinstance());    // (T_EMBED), CORE2 and others
  else if(bruceConfig.CC1101_bus.mosi == bruceConfig.SDCARD_bus.mosi) initCC1101once(&sdcardSPI);   // (ARDUINO_M5STACK_CARDPUTER) and (ESP32S3DEVKITC1) and devices that share CC1101 pin with only SDCard
  else initCC1101once(NULL); // (ARDUINO_M5STICK_C_PLUS) || (ARDUINO_M5STICK_C_PLUS2) and others that doesn´t share SPI with other devices (need to change it when Bruce board comes to shore)
#endif

}

/*********************************************************************
 **  Function: begin_tft
 **  Config tft
 *********************************************************************/
void begin_tft(){
  tft.setRotation(bruceConfig.rotation); //sometimes it misses the first command
  tft.invertDisplay(bruceConfig.colorInverted);
  tft.setRotation(bruceConfig.rotation);
  tftWidth = tft.width();
#ifdef HAS_TOUCH
  tftHeight = tft.height() - 20;
#else
  tftHeight = tft.height();
#endif
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
  tft.drawCentreString("Bruce", tftWidth / 2, 10, 1);
  tft.setTextSize(FP);
  tft.drawCentreString(BRUCE_VERSION, tftWidth / 2, 25, 1);
  tft.setTextSize(FM);
  tft.drawCentreString("PREDATORY FIRMWARE", tftWidth / 2, tftHeight+2, 1); // will draw outside the screen on non touch devices
}

/*********************************************************************
 **  Function: boot_screen_anim
 **  Draw boot screen
 *********************************************************************/
void boot_screen_anim() {
  boot_screen();
  int i = millis();
  // checks for boot.jpg in SD and LittleFS for customization
  int boot_img=0;
  bool drawn=false;
  if(sdcardMounted) {
    if(SD.exists("/boot.jpg"))            boot_img = 1;
    else if(SD.exists("/boot.gif"))       boot_img = 3;
  }
  if(boot_img == 0 && LittleFS.exists("/boot.jpg")) boot_img = 2;
  else if(boot_img == 0 && LittleFS.exists("/boot.gif")) boot_img = 4;

  tft.drawPixel(0,0,0); // Forces back communication with TFT, to avoid ghosting
                        // Start image loop
  while(millis()<i+7000) { // boot image lasts for 5 secs
    if((millis()-i>2000) && !drawn) {
      tft.fillRect(0,45,tftWidth,tftHeight-45,bruceConfig.bgColor);
      if(boot_img > 0 && !drawn) {
        tft.fillScreen(bruceConfig.bgColor);
        if(boot_img==1)       { showJpeg(SD,"/boot.jpg",0,0,true);           Serial.println("Image from SD"); }
        else if (boot_img==2) { showJpeg(LittleFS,"/boot.jpg",0,0,true);     Serial.println("Image from LittleFS"); }
        else if (boot_img==3) { showGif(&SD,"/boot.gif",0,0,true,3600);       Serial.println("Image from SD"); }
        else if (boot_img==4) { showGif(&LittleFS,"/boot.gif",0,0,true,3600); Serial.println("Image from LittleFS"); }
        tft.drawPixel(0,0,0); // Forces back communication with TFT, to avoid ghosting
      }
      drawn=true;
    }
#if !defined(LITE_VERSION)
    if(!boot_img && (millis()-i>2200) && (millis()-i)<2700) tft.drawRect(2*tftWidth/3,tftHeight/2,2,2,bruceConfig.priColor);
    if(!boot_img && (millis()-i>2700) && (millis()-i)<2900) tft.fillRect(0,45,tftWidth,tftHeight-45,bruceConfig.bgColor);
    if(!boot_img && (millis()-i>2900) && (millis()-i)<3400) tft.drawXBitmap(2*tftWidth/3 - 30 ,5+tftHeight/2,bruce_small_bits, bruce_small_width, bruce_small_height,bruceConfig.bgColor,bruceConfig.priColor);
    if(!boot_img && (millis()-i>3400) && (millis()-i)<3600) tft.fillScreen(bruceConfig.bgColor);
    if(!boot_img && (millis()-i>3600)) tft.drawXBitmap((tftWidth-238)/2,(tftHeight-133)/2,bits, bits_width, bits_height,bruceConfig.bgColor,bruceConfig.priColor);
#endif
    if(check(AnyKeyPress))  // If any key or M5 key is pressed, it'll jump the boot screen
    {
      tft.fillScreen(TFT_BLACK);
      delay(10);
      return;
    }
  }

  // Clear splashscreen
  tft.fillScreen(TFT_BLACK);
}


/*********************************************************************
 **  Function: init_clock
 **  Clock initialisation for propper display in menu
 *********************************************************************/
void init_clock() {
#if defined(HAS_RTC)
  
  _rtc.begin();
  _rtc.GetBm8563Time();
  _rtc.GetTime(&_time);
#endif
}


/*********************************************************************
 **  Function: init_led
 **  Led initialisation
 *********************************************************************/
void init_led() {
#ifdef HAS_RGB_LED
  beginLed();
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
#ifndef CC1101_GDO2_PIN
#define CC1101_GDO2_PIN -1
#endif
  bruceConfig.CC1101_bus = { (gpio_num_t)CC1101_SCK_PIN,  (gpio_num_t)CC1101_MISO_PIN,  (gpio_num_t)CC1101_MOSI_PIN,  (gpio_num_t)CC1101_SS_PIN,  (gpio_num_t)CC1101_GDO0_PIN, (gpio_num_t)CC1101_GDO2_PIN };
  bruceConfig.NRF24_bus =  { (gpio_num_t)NRF24_SCK_PIN,   (gpio_num_t)NRF24_MISO_PIN,   (gpio_num_t)NRF24_MOSI_PIN,   (gpio_num_t)NRF24_SS_PIN,   (gpio_num_t)NRF24_CE_PIN };
  bruceConfig.SDCARD_bus = { (gpio_num_t)SDCARD_SCK,      (gpio_num_t)SDCARD_MISO,      (gpio_num_t)SDCARD_MOSI,      (gpio_num_t)SDCARD_CS };
  bruceConfig.bright=100; // theres is no value yet

#if defined(HAS_SCREEN)
  tft.init();
  tft.setRotation(ROTATION);
  tft.fillScreen(TFT_BLACK); // bruceConfig is not read yet.. just to show something on screen due to long boot time
  tft.setTextColor(TFT_PURPLE,TFT_BLACK);
  tft.drawCentreString("Booting",tft.width()/2, tft.height()/2,1);
#else
  tft.begin();
#endif
  begin_storage();
  begin_tft();
  init_clock();
  init_led();

  // Some GPIO Settings (such as CYD's brightness control must be set after tft and sdcard)
  _post_setup_gpio();
  // end of post gpio begin

#ifndef USE_TFT_eSPI_TOUCH
  // This task keeps running all the time, will never stop
  xTaskCreate(
      taskInputHandler,   // Task function
      "InputHandler",     // Task Name
      4096,               // Stack size
      NULL,               // Task parameters
      2,                  // Task priority (0 to 3), loopTask has priority 2.
      &xHandle            // Task handle (not used)
      );
#endif

  boot_screen_anim();

  startup_sound();

  if (bruceConfig.wifiAtStartup) {
    xTaskCreate(
        wifiConnectTask,    // Task function
        "wifiConnectTask",  // Task Name
        4096,               // Stack size
        NULL,               // Task parameters
        2,                  // Task priority (0 to 3), loopTask has priority 2.
        NULL                // Task handle (not used)
    );
  }

#if ! defined(HAS_SCREEN)
  // start a task to handle serial commands while the webui is running
  startSerialCommandsHandlerTask();
#endif

  delay(200);
  wakeUpScreen();

  if (bruceConfig.startupApp != "" && !startupApp.startApp(bruceConfig.startupApp)) {
    bruceConfig.setStartupApp("");
  }
}

/**********************************************************************
 **  Function: loop
 **  Main loop
 **********************************************************************/
#if defined(HAS_SCREEN)
void loop() {
  bool redraw = true;
  long clock_update=0;
  mainMenu.begin();

  // Interpreter must be ran in the loop() function, otherwise it breaks
  // called by 'stack canary watchpoint triggered (loopTask)'
#if !defined(LITE_VERSION)
#if !defined(ARDUINO_M5STACK_CORE) && !defined(ARDUINO_M5STACK_CORE2)
  if(interpreter_start) {
    interpreter_start=false;
    interpreter();
    previousMillis = millis(); // ensure that will not dim screen when get back to menu
  }
#endif
#endif
  tft.fillScreen(bruceConfig.bgColor);


  while(1){
    if(interpreter_start) break;
    if (returnToMenu) {
      returnToMenu = false;
      tft.fillScreen(bruceConfig.bgColor); //fix any problem with the mainMenu screen when coming back from submenus or functions
      redraw=true;
    }

    if (redraw) {
      if(bruceConfig.rotation & 0b01) mainMenu.draw(float((float)tftHeight/(float)135));
      else mainMenu.draw(float((float)tftWidth/(float)240));
      clock_update=0; // forces clock drawing
      redraw = false;
    }

    handleSerialCommands();
#ifdef HAS_KEYBOARD
    checkShortcutPress();  // shortctus to quickly start apps without navigating the menus
#endif

    if (check(PrevPress)) {
      checkReboot();
      mainMenu.previous();
      redraw = true;
    }
    /* DW Btn to next item */
    if (check(NextPress)) {
      mainMenu.next();
      redraw = true;
    }

    /* Select and run function */
    if (check(SelPress)) {
      mainMenu.openMenuOptions();
      drawMainBorder(true);
      redraw=true;
    }
    // update battery and clock once every 30 seconds
    // it was added to avoid delays in btns readings from Core and improves overall performance
    if(millis()-clock_update>30000) {
      uint8_t bat = getBattery();
      drawBatteryStatus(bat);
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
  startWebUi(true);  // MEMO: will quit when check(EscPress)
}
#endif
