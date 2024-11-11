#pragma once
#include <Arduino.h>

int getBattery();   //core/display.h

void _setup_gpio(); //main.cpp

void _setBrightness(uint8_t brightval); //core/settings.h

// mykeyboard
String keyboard(String mytext, int maxSize, String msg);

bool checkNextPress();

bool checkPrevPress();

bool checkSelPress();

bool checkEscPress();

bool checkAnyKeyPress();

void powerOff();

void checkReboot();
