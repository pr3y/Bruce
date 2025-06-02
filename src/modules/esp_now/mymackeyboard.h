#pragma once

#include "core/display.h"
#include <globals.h> // if you need keyStroke, etc.

//
// In this header, we declare **only** the new “Mac_…”–prefixed functions.
//
String MacKeyboard(String mytext, int maxSize = 76, String msg = "Type your message:");

// If you need to power off or deep sleep from this module:
void Mac_powerOff();
void Mac_goToDeepSleep();
void Mac_checkReboot();

// Shortcut logic (new Mac_ versions)
keyStroke Mac_getKeyPress();
bool Mac_checkNextPagePress();
bool Mac_checkPrevPagePress();
void Mac_checkShortcutPress();
int Mac_checkNumberShortcutPress();
char Mac_checkLetterShortcutPress();
