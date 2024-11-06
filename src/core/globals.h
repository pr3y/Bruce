#pragma once
#include "precompiler_flags.h"

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
#include "config.h"

#if defined (STICK_C_PLUS) || defined (STICK_C)
  #include <AXP192.h>
  extern AXP192 axp192;
#endif

#if defined(HAS_RTC)
  #include "../lib/RTC/cplus_RTC.h"
#endif

#if defined(CARDPUTER)
  #include <Keyboard.h>
  extern Keyboard_Class Keyboard;
#endif

#if defined(CORE2)
  #include <M5Core2.h>
#elif defined(CORE)
  #include <M5Stack.h>
#elif defined(M5STACK)
  #include <M5Unified.h>

#endif

// Declaração dos objetos TFT
#if defined(HAS_SCREEN)
  #if defined(M5STACK) && !defined(CORE2) && !defined(CORE)
    #define tft M5.Lcd
    extern M5Canvas sprite;
    extern M5Canvas draw;
  #else
    #include <TFT_eSPI.h>
    extern TFT_eSPI tft;
    extern TFT_eSprite sprite;
    extern TFT_eSprite draw;
  #endif
#else
    #include "VectorDisplay.h"
    extern SerialDisplayClass tft;
    extern SerialDisplayClass& sprite;
    extern SerialDisplayClass& draw;
#endif

extern bool interpreter_start;

extern BruceConfig bruceConfig;

extern char timeStr[10];
extern SPIClass sdcardSPI;
#if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
extern SPIClass CC_NRF_SPI;
#endif
extern bool clock_set;
extern time_t localTime;
extern struct tm* timeInfo;
extern ESP32Time rtc;
extern NTPClient timeClient;
extern Timezone myTZ;

extern int prog_handler;    // 0 - Flash, 1 - LittleFS, 2 - Download

extern bool sdcardMounted;  // inform if SD Cardis active or not

extern bool wifiConnected;  // inform if wifi is active or not

extern String wifiIP;

extern bool BLEConnected;  // inform if BLE is active or not

extern bool gpsConnected; // inform if GPS is active or not

struct Option {
  std::string label;
  std::function<void()> operation;
  bool selected = false;

  Option(const std::string& lbl, const std::function<void()>& op, bool sel = false)
    : label(lbl), operation(op), selected(sel) {}
};

extern std::vector<Option> options;

extern String fileToCopy;

extern uint8_t buff[1024];

extern const int bufSize;

extern bool returnToMenu; // variable to check and break loops to return to main menu

extern String cachedPassword;

// Screen sleep control variables
extern unsigned long previousMillis;
extern bool isSleeping;
extern bool isScreenOff;

void backToMenu();

void updateTimeStr(struct tm timeInfo);

extern bool dimmer;

void setup_gpio();

