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

bool __attribute__((weak)) checkAnyKeyPress();

void __attribute__((weak)) powerOff();

void __attribute__((weak)) checkReboot();

// Shortcut logic

keyStroke __attribute__((weak)) _getKeyPress(); // This function must be implemented in the interface.h of the device, in order to 
                                                // return the key pressed to use as shortcut or input in keyboard environment
                                                // by using the flag HAS_KEYBOARD
bool __attribute__((weak)) _checkNextPagePress();
bool __attribute__((weak)) _checkPrevPagePress();


// Core functions, depends on the implementation of the funtions above in the interface.h
void checkShortcutPress();
int  checkNumberShortcutPress();
char checkLetterShortcutPress();

bool checkNextPagePress();
bool checkPrevPagePress();



