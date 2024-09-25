#include "display.h"
#include "globals.h"


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
