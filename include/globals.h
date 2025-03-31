#ifndef __GLOBALS__
#define __GLOBALS__

#include <precompiler_flags.h>
#include <interface.h>
// Globals.h

#define ALCOLOR TFT_RED

#include <Arduino.h>
#include <functional>
#include <vector>
#include <SPI.h>
#include <LittleFS.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#include "core/config.h"
#include "core/startup_app.h"
#include "core/serial_commands/cli.h"

#if defined(HAS_RTC)
  #include "../lib/RTC/cplus_RTC.h"
  extern cplus_RTC _rtc;
  extern RTC_TimeTypeDef _time;
  extern RTC_DateTypeDef _date;
#endif

// Declaração dos objetos TFT
#if defined(HAS_SCREEN)
  #include <TFT_eSPI.h>
  extern TFT_eSPI tft;
  extern TFT_eSprite sprite;
  extern TFT_eSprite draw;
#else
    #include <VectorDisplay.h>
    extern SerialDisplayClass tft;
    extern SerialDisplayClass& sprite;
    extern SerialDisplayClass& draw;
#endif

#ifdef USE_BQ27220_VIA_I2C
  #include <bq27220.h>
  extern BQ27220 bq;
#endif

extern bool interpreter_start;

extern BruceConfig bruceConfig;
extern SerialCli serialCli;
extern StartupApp startupApp;

extern char timeStr[10];
extern SPIClass sdcardSPI;
extern SPIClass CC_NRF_SPI;
extern bool clock_set;
extern time_t localTime;
extern struct tm* timeInfo;
extern ESP32Time rtc;
extern NTPClient timeClient;
extern Timezone myTZ;

extern int prog_handler;    // 0 - Flash, 1 - LittleFS, 2 - Download

extern bool sdcardMounted;  // inform if SD Cardis active or not

extern bool wifiConnected;  // inform if wifi is active or not
extern bool isWebUIActive; // inform if WebUI is active or not

extern volatile int tftWidth;
extern volatile int tftHeight;

extern String wifiIP;

extern bool BLEConnected;  // inform if BLE is active or not

extern bool gpsConnected; // inform if GPS is active or not

struct Option {
  String label;
  std::function<void()> operation;
  bool selected = false;
  bool ( *hover )(void *hoverPointer, bool shouldRender);
  void *hoverPointer;

  Option(String lbl,
         const std::function<void()>& op,
         bool sel = false,
         bool ( *hov )(void *hoverPointer, bool shouldRender) = nullptr, // hover lambda returns true if it already handled rendering
         void *ptr = nullptr)
    : label(lbl), operation(op), selected(sel), hover(hov), hoverPointer(ptr) {}
};

struct keyStroke { // DO NOT CHANGE IT!!!!!
    bool pressed=false;
    bool exit_key=false;
    bool fn = false;
    bool del = false;
    bool enter = false;
    bool alt = false;
    bool ctrl = false;
    bool gui = false;
    uint8_t modifiers = 0;
    std::vector<char> word;
    std::vector<uint8_t> hid_keys;
    std::vector<uint8_t> modifier_keys;


    // Clear function
    void Clear() {
        pressed = false;
        exit_key = false;
        fn = false;
        del = false;
        enter = false;
        bool alt = false;
        bool ctrl = false;
        bool gui = false;
        modifiers = 0;
        word.clear();
        hid_keys.clear();
        modifier_keys.clear();
    }
};

struct TouchPoint {
    bool pressed = false;
    uint16_t x;
    uint16_t y;

    // clear touch to better handle tasks
    void Clear(void) {
        pressed = false;
        x=0;
        y=0;
    }
};

extern TouchPoint touchPoint;
extern keyStroke KeyStroke;
extern std::vector<Option> options;

template<typename R, typename... Args>
std::function<void()> lambdaHelper(R (*callback)(Args...), Args... args) {
  return [=]() { (void)callback(args...); };
}

extern String fileToCopy;

extern const int bufSize;

extern bool returnToMenu; // variable to check and break loops to return to main menu

extern String cachedPassword;

// Screen sleep control variables
extern unsigned long previousMillis;
extern bool isSleeping;
extern bool isScreenOff;
extern bool dimmer;

extern volatile bool NextPress;

extern volatile bool PrevPress;

extern volatile bool UpPress;

extern volatile bool DownPress;

extern volatile bool SelPress;

extern volatile bool EscPress;

extern volatile bool AnyKeyPress;

extern volatile bool NextPagePress;

extern volatile bool PrevPagePress;

extern TaskHandle_t xHandle;
extern inline bool check(volatile bool &btn) {

#ifndef USE_TFT_eSPI_TOUCH
  if(!btn) return false;
  vTaskSuspend( xHandle );
  btn=false;
  AnyKeyPress=false;
  delay(10);
  vTaskResume( xHandle );
  return true;
#else

  InputHandler();
  if(!btn) return false;
  btn=false;
  AnyKeyPress=false;
  return true;

#endif
}

#endif
