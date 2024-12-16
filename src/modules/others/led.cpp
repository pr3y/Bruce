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
#define LED_COUNT   8
#else
#define LED_TYPE    LED_STRIP_SK6812
#define LED_COUNT   1
#endif
// #define LED_STRIP_APA106
// #define LED_STRIP_SM16703

#define LED_TYPE_IS_RGBW 1  // Set to 1 if LED is RGBW
#define LED_BRIGHT 245  // Set brightness. 0-255
#define LED_BRIGHT_DEFAULT 245 

LiteLED myLED( LED_TYPE, LED_TYPE_IS_RGBW );    // Create LiteLED Objected. Call it "myLED"

void ledrgb_setup() {
    myLED.begin( RGB_LED, LED_COUNT );         // Initialize the LED Object. Only 1 LED.
    myLED.brightness( LED_BRIGHT, 1 );  // Set LED Brightness

    options = {
        {"OFF", [=]() { myLED.brightness( 0, 1 ); }},
        {"PURPLE", [=]() { myLED.setPixel( 0, L_PURPLE, 1 ); }},
        {"WHITE", [=]() { myLED.setPixel( 0, L_WHITE, 1 ); }},
        {"RED", [=]() { myLED.setPixel( 0, L_RED, 1 ); }},
        {"GREEN", [=]() { myLED.setPixel( 0, L_GREEN, 1 ); }},
        {"BLUE", [=]() { myLED.setPixel( 0, L_BLUE, 1 ); }},
    };
    delay(200);
    loopOptions(options);
    delay(200);
}

void ledrgb_flash() {
    myLED.begin( RGB_LED, LED_COUNT );         // Inicialize o objeto myLED. Aqui temos 1 LED conectado ao pino RGB_LED
    myLED.brightness( LED_BRIGHT, 1 );  // Ligue o LED
    myLED.setPixel( 0, L_PURPLE, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );
    delay(1000);
    myLED.brightness( LED_BRIGHT, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );
    delay(1000);
    myLED.brightness( LED_BRIGHT, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );
    delay(1000);
    myLED.brightness( LED_BRIGHT, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );
}
#endif