#include "display.h"
#include "globals.h"

#if defined(CYD)
  #include "CYD28_TouchscreenR.h"
  #define CYD28_DISPLAY_HOR_RES_MAX 320
  #define CYD28_DISPLAY_VER_RES_MAX 240  
  extern CYD28_TouchR touch;
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
