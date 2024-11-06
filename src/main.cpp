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
#if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
SPIClass CC_NRF_SPI;
#endif
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
  #if defined(M5STACK) && !defined(CORE2) && !defined(CORE)
  #define tft M5.Lcd
  M5Canvas sprite(&M5.Lcd);
  M5Canvas draw(&M5.Lcd);
  #else
	TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
	TFT_eSprite sprite = TFT_eSprite(&tft);
	TFT_eSprite draw = TFT_eSprite(&tft);
  #endif
#else
    SerialDisplayClass tft;
    SerialDisplayClass& sprite = tft;
    SerialDisplayClass& draw = tft;
#endif

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
**  Function: setup_gpio
**  Setup GPIO pins
*********************************************************************/
void setup_gpio() {
  #if  defined(STICK_C_PLUS2)
    pinMode(UP_BTN, INPUT);   // Sets the power btn as an INPUT
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
    pinMode(4, OUTPUT);     // Keeps the Stick alive after take off the USB cable
    digitalWrite(4,HIGH);   // Keeps the Stick alive after take off the USB cable
  #elif defined(STICK_C_PLUS)
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
    axp192.begin();           // Start the energy management of AXP192
  #elif defined(CARDPUTER)
    Keyboard.begin();
    pinMode(0, INPUT);
    pinMode(10, INPUT);     // Pin that reads the
  #elif ! defined(HAS_SCREEN)
    // do nothing
  #elif defined(M5STACK) // init must be done after tft, to make SDCard work
    //M5.begin();
  #elif defined(CYD)
    pinMode(XPT2046_CS, OUTPUT);
    //touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    if(!touch.begin()) {
        Serial.println("Touch IC not Started");
        log_i("Touch IC not Started");
    } else log_i("Touch IC Started");
    digitalWrite(XPT2046_CS, LOW);
    // Brightness control -> Not working yet, don't know why! @Pirata
    pinMode(TFT_BL,OUTPUT);
    ledcSetup(TFT_BRIGHT_CHANNEL,TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits); //Channel 0, 10khz, 8bits
    ledcAttachPin(TFT_BL, TFT_BRIGHT_CHANNEL);
    ledcWrite(TFT_BRIGHT_CHANNEL,255);

  #elif defined(T_EMBED)
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    #ifdef T_EMBED_1101
      // T-Embed CC1101 has a antenna circuit optimized to each frequency band, controlled by SW0 and SW1
      //Set antenna frequency settings
      pinMode(BOARD_LORA_SW1, OUTPUT);
      pinMode(BOARD_LORA_SW0, OUTPUT);

      // Chip Select CC1101 to HIGH State
      pinMode(CC1101_SS_PIN, OUTPUT);
      digitalWrite(CC1101_SS_PIN,HIGH);

      // Power chip pin
      pinMode(PIN_POWER_ON, OUTPUT);
      digitalWrite(PIN_POWER_ON, HIGH);  // Power on CC1101 and LED
      bool pmu_ret = false;
      Wire.begin(GROVE_SDA, GROVE_SCL);
      pmu_ret = PPM.init(Wire, GROVE_SDA, GROVE_SCL, BQ25896_SLAVE_ADDRESS);
      if(pmu_ret) {
          PPM.setSysPowerDownVoltage(3300);
          PPM.setInputCurrentLimit(3250);
          Serial.printf("getInputCurrentLimit: %d mA\n",PPM.getInputCurrentLimit());
          PPM.disableCurrentLimitPin();
          PPM.setChargeTargetVoltage(4208);
          PPM.setPrechargeCurr(64);
          PPM.setChargerConstantCurr(832);
          PPM.getChargerConstantCurr();
          Serial.printf("getChargerConstantCurr: %d mA\n",PPM.getChargerConstantCurr());
          PPM.enableADCMeasure();
          PPM.enableCharge();
      }
    #else
      pinMode(BAT_PIN,INPUT); // Battery value
    #endif
    
    pinMode(BK_BTN, INPUT);
    pinMode(ENCODER_KEY, INPUT);
    // use TWO03 mode when PIN_IN1, PIN_IN2 signals are both LOW or HIGH in latch position.
    encoder = new RotaryEncoder(ENCODER_INA, ENCODER_INB, RotaryEncoder::LatchMode::TWO03);

    // register interrupt routine
    attachInterrupt(digitalPinToInterrupt(ENCODER_INA), checkPosition, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_INB), checkPosition, CHANGE);

  #elif defined(T_DECK)
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(SEL_BTN, INPUT);

    // Setup for Trackball
    pinMode(UP_BTN, INPUT_PULLUP);
    attachInterrupt(UP_BTN, ISR_up, FALLING);
    pinMode(DW_BTN, INPUT_PULLUP);
    attachInterrupt(DW_BTN, ISR_down, FALLING);
    pinMode(L_BTN, INPUT_PULLUP);
    attachInterrupt(L_BTN, ISR_left, FALLING);
    pinMode(R_BTN, INPUT_PULLUP);
    attachInterrupt(R_BTN, ISR_right, FALLING);
    //pinMode(BACKLIGHT, OUTPUT);
    //digitalWrite(BACKLIGHT,HIGH);

      // PWM backlight setup
    // ledcSetup(TFT_BRIGHT_CHANNEL,TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits); //Channel 0, 10khz, 8bits
    // ledcAttachPin(TFT_BL, TFT_BRIGHT_CHANNEL);
    // ledcWrite(TFT_BRIGHT_CHANNEL,125);    
  #else
    pinMode(UP_BTN, INPUT);   // Sets the power btn as an INPUT
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
  #endif

  #if defined(BACKLIGHT)
  pinMode(BACKLIGHT, OUTPUT);
  #endif

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
#if defined(HAS_SCREEN) && !defined(M5STACK)
  tft.init();
#elif defined(CORE2)
  M5.begin();
  tft.init();
#elif defined(CORE)
  tft.init();
  M5.begin();
#elif defined(M5STACK)
  M5.begin();

#endif
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
    if((millis()-i>2000) && (millis()-i)<2200){
      tft.fillRect(0,45,WIDTH,HEIGHT-45,bruceConfig.bgColor);
      if(showJpeg(SD,"/boot.jpg") && (millis()-i>2000) && (millis()-i<2200)) { boot_img=true; Serial.println("Image from SD"); }
      else if (showJpeg(LittleFS,"/boot.jpg") && (millis()-i>2000) && (millis()-i<2100)) { boot_img=true; Serial.println("Image from LittleFS"); }
      else if (showGIF(SD,"/boot.gif") && (millis()-i>2000) && (millis()-i<2200)) { boot_img=true; Serial.println("Image from SD"); }
      else if (showGIF(LittleFS,"/boot.gif") && (millis()-i>2000) && (millis()-i<2100)) { boot_img=true; Serial.println("Image from LittleFS"); }
    }
    if(!boot_img && (millis()-i>2200) && (millis()-i)<2700) tft.drawRect(2*WIDTH/3,HEIGHT/2,2,2,bruceConfig.priColor);
    if(!boot_img && (millis()-i>2700) && (millis()-i)<2900) tft.fillRect(0,45,WIDTH,HEIGHT-45,bruceConfig.bgColor);
    #if defined(M5STACK)
      char16_t bgcolor = bruceConfig.bgColor;  // Conversion tor M5GFX variable
      char16_t priColor = bruceConfig.priColor;// Conversion tor M5GFX variable
      if(!boot_img && (millis()-i>2900) && (millis()-i)<3400) tft.drawXBitmap(2*WIDTH/3 - 30 ,5+HEIGHT/2,bruce_small_bits, bruce_small_width, bruce_small_height,bgcolor,priColor);
      if(!boot_img && (millis()-i>3400) && (millis()-i)<3600) tft.fillRect(0,0,WIDTH,HEIGHT,bruceConfig.bgColor);
      if(!boot_img && (millis()-i>3600)) tft.drawXBitmap((WIDTH-238)/2,(HEIGHT-133)/2,bits, bits_width, bits_height,bgcolor,priColor);
    #else
      if(!boot_img && (millis()-i>2900) && (millis()-i)<3400) tft.drawXBitmap(2*WIDTH/3 - 30 ,5+HEIGHT/2,bruce_small_bits, bruce_small_width, bruce_small_height,TFT_BLACK,bruceConfig.priColor);
      if(!boot_img && (millis()-i>3400) && (millis()-i)<3600) tft.fillRect(0,0,WIDTH,HEIGHT,bruceConfig.bgColor);
      if(!boot_img && (millis()-i>3600)) tft.drawXBitmap((WIDTH-238)/2,(HEIGHT-133)/2,bits, bits_width, bits_height,TFT_BLACK,bruceConfig.priColor);
    #endif
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
  begin_storage();

  bruceConfig.fromFile();

  begin_tft();
  init_clock();

  boot_screen();

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
#ifdef CARDPUTER
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
    wifiApConnect();  // TODO: read mode from config file
  }
  Serial.println("startWebUi");
  startWebUi(true);  // MEMO: will quit when checkEscPress
}
#endif
