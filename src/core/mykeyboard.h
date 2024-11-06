#include "display.h"
#include "globals.h"

#if defined(CYD)
  #include "CYD28_TouchscreenR.h"
  #define CYD28_DISPLAY_HOR_RES_MAX 320
  #define CYD28_DISPLAY_VER_RES_MAX 240  
  extern CYD28_TouchR touch;
#elif defined(T_EMBED)
  #include <RotaryEncoder.h>
  //extern RotaryEncoder encoder;
  extern RotaryEncoder *encoder;
  IRAM_ATTR void checkPosition();
  #include <Wire.h>
  #include <XPowersLib.h>
  extern XPowersPPM PPM;
#elif defined(T_DECK)

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

String keyboard(String mytext, int maxSize = 76, String msg = "Type your message:");

bool checkNextPress();

bool checkPrevPress();

bool checkSelPress();

bool checkEscPress();

#ifdef CARDPUTER
void checkShortcutPress();
int checkNumberShortcutPress();
char checkLetterShortcutPress();
bool checkNextPagePress();
bool checkPrevPagePress();
#endif

bool checkAnyKeyPress();

void checkReboot();
