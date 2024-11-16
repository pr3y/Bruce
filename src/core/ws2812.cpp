#ifdef HAS_WS2812_LED

#include "core/display.h"
#include "core/globals.h"
#include <FastLED.h>

CRGB leds[WS2812_NUM_LEDS];

void ws2812_set_color(CRGB c)
{
    for(int i = 0; i < WS2812_NUM_LEDS; i++){
        leds[i] = c;
    }
    FastLED.show();
    bruceConfig.set_WS2812_Color(c.r, c.g, c.b);
}

void ws2812_set_light(uint8_t light)
{
    FastLED.setBrightness(light);
    FastLED.show();
}

void ws2812_TurnOff(void)
{
    for(int i = 0; i < WS2812_NUM_LEDS; i++){
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void ws2812_TurnOn(void)
{
    ws2812_set_color(CRGB(bruceConfig.ws2812_r, bruceConfig.ws2812_g, bruceConfig.ws2812_b));
}

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

void ws2812_init(void)
{
    FastLED.addLeds<WS2812, WS2812_DATA_PIN, GRB>(leds, WS2812_NUM_LEDS);
    FastLED.setBrightness(WS2812_DEFAULT_LIGHT);   
}

void ws2812_setup(void)
{
    ws2812_init();

    options = {
        {"OFF", [=]() { ws2812_set_color(CRGB::Black); }},
        {"PURPLE", [=]() { ws2812_set_color(CRGB::Purple); }},
        {"WHITE", [=]() { ws2812_set_color(CRGB::White); }},
        {"RED", [=]() { ws2812_set_color(CRGB::Red); }},
        {"GREEN", [=]() { ws2812_set_color(CRGB::Green); }},
        {"BLUE", [=]() { ws2812_set_color(CRGB::Blue); }},
    };
    delay(200);
    loopOptions(options);
    delay(200);
}



#endif