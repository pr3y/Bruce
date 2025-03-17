#ifndef __GLOBALS__
#define __GLOBALS__

#include <interface.h>
#include <precompiler_flags.h>
// Globals.h

#define ALCOLOR TFT_RED

#include "core/config.h"
#include "core/serial_commands/cli.h"
#include "core/startup_app.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Time.h>
#include <LittleFS.h>
#include <NTPClient.h>
#include <SPI.h>
#include <Timezone.h>
#include <functional>
#include <vector>

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
extern SerialDisplayClass &sprite;
extern SerialDisplayClass &draw;
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
extern struct tm *timeInfo;
extern ESP32Time rtc;
extern NTPClient timeClient;
extern Timezone myTZ;

extern int prog_handler; // 0 - Flash, 1 - LittleFS, 2 - Download

extern bool sdcardMounted; // inform if SD Cardis active or not

extern bool wifiConnected; // inform if wifi is active or not
extern bool isWebUIActive; // inform if WebUI is active or not

extern volatile int tftWidth;
extern volatile int tftHeight;

extern String wifiIP;

extern bool BLEConnected; // inform if BLE is active or not

extern bool gpsConnected; // inform if GPS is active or not

struct Option {
  std::string label;
  std::function<void()> operation;
  bool selected = false;

  Option(const std::string &lbl, const std::function<void()> &op,
         bool sel = false)
      : label(lbl), operation(op), selected(sel) {}
};

struct keyStroke { // DO NOT CHANGE IT!!!!!
  bool pressed = false;
  bool exit_key = false;
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
    x = 0;
    y = 0;
  }
};

extern TouchPoint touchPoint;
extern keyStroke KeyStroke;
extern std::vector<Option> options;

extern String fileToCopy;

extern uint8_t buff[1024];

extern const int bufSize;

// variable to check and break loops to return to main menu
extern bool returnToMenu;

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
  if (!btn)
    return false;
  vTaskSuspend(xHandle);
  btn = false;
  AnyKeyPress = false;
  delay(10);
  vTaskResume(xHandle);
  return true;
#else

  InputHandler();
  if (!btn)
    return false;
  btn = false;
  AnyKeyPress = false;
  return true;

#endif
}

#endif
