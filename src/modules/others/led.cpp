#ifdef HAS_RGB_LED
// By @IncursioHack - github.com/IncursioHack
#include <LiteLED.h>
#include "core/display.h"
#include "core/globals.h"
#include "led_control.h"

// Escolha o tipo de LED na lista abaixo.
// Comente todos menos um LED_TYPE.
#ifdef T_EMBED_1101
#define LED_TYPE    LED_STRIP_WS2812
#define LED_TYPE_IS_RGBW 0
#define LED_COUNT   8
#else
#define LED_TYPE    LED_STRIP_SK6812
#define LED_TYPE_IS_RGBW 1
#define LED_COUNT   1
#endif
// #define LED_STRIP_APA106
// #define LED_STRIP_SM16703

#define LED_BRIGHT_DEFAULT 245 

int brightness = 75;

LiteLED RGBLED( LED_TYPE, LED_TYPE_IS_RGBW );    // Create LiteLED Objected. Call it "RGBLED"

void ledrgb_setup() {
    RGBLED.begin( RGB_LED, LED_COUNT );         // Initialize the LED Object. Only 1 LED.
    RGBLED.brightness( brightness, 1 );  // Set LED Brightness

    options = {
        {"OFF", [=]() { RGBLED.brightness( 0, 1 ); }},
        {"PURPLE", [=]() { RGBLED.fill( L_PURPLE, 1 ); }},
        {"WHITE", [=]() { RGBLED.fill( L_WHITE, 1 ); }},
        {"RED", [=]() { RGBLED.fill( L_RED, 1 ); }},
        {"GREEN", [=]() { RGBLED.fill( L_GREEN, 1 ); }},
        {"BLUE", [=]() { RGBLED.fill( L_BLUE, 1 ); }},
    };
    delay(200);
    loopOptions(options);
    delay(200);
}

void ledrgb_brightness() {

    options = {
        {"10", [=]() { brightness = 10; }},
        {"25", [=]() { brightness = 20; }},
        {"50", [=]() { brightness = 50; }},
        {"75", [=]() { brightness = 75; }},
        {"100", [=]() { brightness = 100; }},
        {"150", [=]() { brightness = 150; }},
    };

    RGBLED.brightness( brightness, 1 );
    
    delay(200);
    loopOptions(options);
    delay(200);
}


void ledrgb_flash() {
    RGBLED.begin( RGB_LED, LED_COUNT );
    RGBLED.brightness( brightness, 1 );
    RGBLED.fill( L_PURPLE, 1 );
    delay(1000);
    RGBLED.brightness( 0, 1 );
    delay(1000);
    RGBLED.brightness( brightness, 1 );
    delay(1000);
    RGBLED.brightness( 0, 1 );
    delay(1000);
    RGBLED.brightness( brightness, 1 );
    delay(1000);
    RGBLED.brightness( 0, 1 );
    delay(1000);
    RGBLED.brightness( brightness, 1 );
    delay(1000);
    RGBLED.brightness( 0, 1 );
}
#endif