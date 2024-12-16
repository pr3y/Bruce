#ifdef HAS_RGB_LED
// By IncursioHack @github.com/IncursioHack
#include <Arduino.h>
#include <LiteLED.h>

static const crgb_t L_RED = 0xff0000;
static const crgb_t L_GREEN = 0x00ff00;
static const crgb_t L_BLUE = 0x0000ff;
static const crgb_t L_WHITE = 0xe0e0e0;
static const crgb_t L_PURPLE = 0xff00ff;

extern LiteLED RGBLED;

void ledrgb_brightness();
void ledrgb_setup();
void ledrgb_flash();

#endif