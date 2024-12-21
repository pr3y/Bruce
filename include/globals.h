#pragma once
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

#if defined(HAS_RTC)
  #include "../lib/RTC/cplus_RTC.h"
#endif

// Declaração dos objetos TFT
#if defined(HAS_SCREEN)
  #include <TFT_eSPI.h>
  extern TFT_eSPI tft;
  extern TFT_eSprite sprite;
  extern TFT_eSprite draw;
#else
    #include "core/VectorDisplay.h"
    extern SerialDisplayClass tft;
    extern SerialDisplayClass& sprite;
    extern SerialDisplayClass& draw;
#endif

extern bool interpreter_start;

extern BruceConfig bruceConfig;
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

extern volatile int tftWidth;
extern volatile int tftHeight;

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

extern bool dimmer;

void setup_gpio();

