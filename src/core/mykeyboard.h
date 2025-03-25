#pragma once
#include "display.h"
#include <globals.h>


String keyboard(String mytext, int maxSize = 76, String msg = "Type your message:");

void __attribute__((weak)) powerOff();

void __attribute__((weak)) checkReboot();

// Shortcut logic

keyStroke _getKeyPress(); // This function must be implemented in the interface.h of the device, in order to 
                                                // return the key pressed to use as shortcut or input in keyboard environment
                                                // by using the flag HAS_KEYBOARD

// Core functions, depends on the implementation of the funtions above in the interface.h
void checkShortcutPress();
int  checkNumberShortcutPress();
char checkLetterShortcutPress();

bool checkNextPagePress();
bool checkPrevPagePress();



