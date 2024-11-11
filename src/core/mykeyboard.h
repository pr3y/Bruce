#pragma once
#include "display.h"
#include "globals.h"

#if defined(T_DECK)

  // Setup for Trackball
  void IRAM_ATTR ISR_up();
  void IRAM_ATTR ISR_down();
  void IRAM_ATTR ISR_left();
  void IRAM_ATTR ISR_right();


  #include <Wire.h>

  #define LILYGO_KB_SLAVE_ADDRESS     0x55
  #define KB_I2C_SDA       18
  #define KB_I2C_SCL       8  
#endif

String  __attribute__((weak)) keyboard(String mytext, int maxSize = 76, String msg = "Type your message:");

bool __attribute__((weak)) checkNextPress();

bool __attribute__((weak)) checkPrevPress();

bool __attribute__((weak)) checkSelPress();

bool __attribute__((weak)) checkEscPress();

#ifdef CARDPUTER
void __attribute__((weak)) checkShortcutPress();
int __attribute__((weak)) checkNumberShortcutPress();
char __attribute__((weak)) checkLetterShortcutPress();
bool __attribute__((weak)) checkNextPagePress();
bool __attribute__((weak)) checkPrevPagePress();
#endif

bool __attribute__((weak)) checkAnyKeyPress();

void __attribute__((weak)) powerOff();

void __attribute__((weak)) checkReboot();
