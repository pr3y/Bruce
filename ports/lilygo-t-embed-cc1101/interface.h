#pragma once

#include <RotaryEncoder.h>
//extern RotaryEncoder encoder;
extern RotaryEncoder *encoder;
IRAM_ATTR void checkPosition();
#include <Wire.h>
#include <XPowersLib.h>
extern XPowersPPM PPM;

int getBattery();

void _setup_gpio();



void _setBrightness(uint8_t brightval);

String keyboard(String mytext, int maxSize, String msg);

bool checkNextPress();

bool checkPrevPress();

bool checkSelPress();

bool checkEscPress();

bool checkAnyKeyPress();

void powerOff();

void checkReboot();
