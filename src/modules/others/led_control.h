#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__
#include <globals.h>
#ifdef HAS_RGB_LED
#include <Arduino.h>
#include <FastLED.h>

CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v);
void setColor(CRGB c);
void setBrightness(int b);
void ledColorConfig();
void ledBrightnessConfig();

#endif
#endif