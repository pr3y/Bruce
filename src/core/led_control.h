#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__
#include <globals.h>

#ifdef HAS_RGB_LED
#include <Arduino.h>
#include <FastLED.h>
#define LED_COLOR_WHEEL 1

CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v);

void beginLed();
void blinkLed(int blinkTime = 50);

void setLedColor(CRGB color);
void setLedBrightness(int value);

void setLedColorConfig();
void setLedBrightnessConfig();

#else
inline void blinkLed(int blinkTime = 50) {};
#endif

#endif
