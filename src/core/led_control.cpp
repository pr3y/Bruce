#include "led_control.h"
#ifdef HAS_RGB_LED
#include <globals.h>
#include "core/display.h"
#include "core/utils.h"
#include <FastLED.h>

CRGB leds[LED_COUNT];

CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
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


void beginLed() {
#ifdef RGB_LED_CLK
    FastLED.addLeds<LED_TYPE, RGB_LED, RGB_LED_CLK, LED_ORDER>(leds, LED_COUNT);
#else
    FastLED.addLeds<LED_TYPE, RGB_LED, LED_ORDER>(leds, LED_COUNT); // Initialize the LED Object. Only 1 LED.
#endif
    setLedColor(bruceConfig.ledColor);
    setLedBrightness(bruceConfig.ledBright);
}


void setLedColor(CRGB color) {
    for (int i = 0; i < LED_COUNT; i++) leds[i] = color;
    FastLED.show();
}


void setLedBrightness(int value) {
    value = max(0, min(255, value));
    int bright = 255 * value/100;
    FastLED.setBrightness(bright);
    FastLED.show();
}


void setLedColorConfig() {
    int idx;
    if (bruceConfig.ledColor==CRGB::Black) idx=0;
    else if (bruceConfig.ledColor==CRGB::Purple) idx=1;
    else if (bruceConfig.ledColor==CRGB::White) idx=2;
    else if (bruceConfig.ledColor==CRGB::Red) idx=3;
    else if (bruceConfig.ledColor==CRGB::Green) idx=4;
    else if (bruceConfig.ledColor==CRGB::Blue) idx=5;
    else idx=6;  // custom color

    options = {
        {"OFF",    [=]() { bruceConfig.setLedColor(CRGB::Black); }, bruceConfig.ledColor == CRGB::Black },
        {"Purple", [=]() { bruceConfig.setLedColor(CRGB::Purple); }, bruceConfig.ledColor == CRGB::Purple},
        {"White",  [=]() { bruceConfig.setLedColor(CRGB::White); }, bruceConfig.ledColor == CRGB::White},
        {"Red",    [=]() { bruceConfig.setLedColor(CRGB::Red); }, bruceConfig.ledColor == CRGB::Red},
        {"Green",  [=]() { bruceConfig.setLedColor(CRGB::Green); }, bruceConfig.ledColor == CRGB::Green},
        {"Blue",   [=]() { bruceConfig.setLedColor(CRGB::Blue); }, bruceConfig.ledColor == CRGB::Blue},
    };

    if (idx == 6) options.emplace_back("Custom Color", [=]() { backToMenu(); }, true);
    options.emplace_back("Main Menu", [=]() { backToMenu(); });

    loopOptions(options, idx);
    setLedColor(bruceConfig.ledColor);
}


void setLedBrightnessConfig() {
    int idx;
    if (bruceConfig.ledBright==10) idx=0;
    else if (bruceConfig.ledBright==25) idx=1;
    else if (bruceConfig.ledBright==50) idx=2;
    else if (bruceConfig.ledBright==75) idx=3;
    else if (bruceConfig.ledBright==100) idx=4;

    options = {
        {"10 %", [=]() { bruceConfig.setLedBright(10);  }, bruceConfig.ledBright == 10 },
        {"25 %", [=]() { bruceConfig.setLedBright(25);  }, bruceConfig.ledBright == 25 },
        {"50 %", [=]() { bruceConfig.setLedBright(50);  }, bruceConfig.ledBright == 50 },
        {"75 %", [=]() { bruceConfig.setLedBright(75);  }, bruceConfig.ledBright == 75 },
        {"100%", [=]() { bruceConfig.setLedBright(100); }, bruceConfig.ledBright == 100 },
        {"Main Menu", [=]() { backToMenu(); }},
    };

    loopOptions(options, idx);
    setLedBrightness(bruceConfig.ledBright);
}
#endif