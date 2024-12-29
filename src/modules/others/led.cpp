#ifdef HAS_RGB_LED
#include <FastLED.h>
#include "core/display.h"
#include <globals.h>
#include "led_control.h"

#define LED_BRIGHT_DEFAULT 245

int brightness = 75;

CRGB leds[LED_COUNT];
CRGB color = CRGB::Red;

CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch ((h / 60) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }

    CRGB c;
    c.red = r;
    c.green = g;
    c.blue = b;
    return c;
}

void setColor(CRGB c)
{
    color = c;
    for(int i = 0; i < LED_COUNT; i++){
        leds[i] = color;
    }
    FastLED.show();
}

void setBrightness(int b)
{
    brightness = b;
    FastLED.setBrightness(brightness);
    FastLED.show(); 
}


void ledColorConfig()
{
    FastLED.addLeds<LED_TYPE, RGB_LED, LED_ORDER>(leds, LED_COUNT); // Initialize the LED Object. Only 1 LED.
    setBrightness(brightness); // Set LED Brightness

    options = {
        {"OFF", [=]()
         { setBrightness(0); }},
        {"PURPLE", [=]()
         { setColor(CRGB::Purple); }},
        {"WHITE", [=]()
         { setColor(CRGB::White); }},
        {"RED", [=]()
         { setColor(CRGB::Red); }},
        {"GREEN", [=]()
         { setColor(CRGB::Green); }},
        {"BLUE", [=]()
         { setColor(CRGB::Blue); }},
    };
    delay(200);
    loopOptions(options);
    delay(200);
}

void ledBrightnessConfig()
{

    options = {
        {"10", [=]()
         { setBrightness(10); }},
        {"25", [=]()
         { setBrightness(20); }},
        {"50", [=]()
         { setBrightness(50); }},
        {"75", [=]()
         { setBrightness(75); }},
        {"100", [=]()
         { setBrightness(100); }},
    };

    delay(200);
    loopOptions(options);
    delay(200);
}
#endif