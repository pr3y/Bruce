#ifdef CARDPUTER
// By @IncursioHack - github.com/IncursioHack
#include "display.h"
#include "globals.h"
#include "led_control.h"
#include <LiteLED.h>

// Escolha o tipo de LED na lista abaixo.
// Comente todos menos um LED_TYPE.
// #define LED_TYPE        LED_STRIP_WS2812
#define LED_TYPE        LED_STRIP_SK6812
// #define LED_STRIP_APA106
// #define LED_STRIP_SM16703

#define LED_TYPE_IS_RGBW 1  // Se o LED for do tipo RGBW, altere o 0 para 1
#define LED_BRIGHT 245  // Define o brilho do LED. "0" está desligado; 255 pode queimar seus olhos (não recomendado)

LiteLED myLED( LED_TYPE, LED_TYPE_IS_RGBW );    // Cria o objeto LiteLED com o nome "myLED"

void ledrgb_setup() {
    myLED.begin( RGB_LED, 1 );         // Inicialize o objeto myLED. Aqui temos 1 LED conectado ao pino RGB_LED
    myLED.brightness( LED_BRIGHT, 1 );  // Ligue o LED 

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
    myLED.begin( RGB_LED, 1 );         // Inicialize o objeto myLED. Aqui temos 1 LED conectado ao pino RGB_LED
    myLED.brightness( LED_BRIGHT_DEFAULT, 1 );  // Ligue o LED
    myLED.setPixel( 0, L_PURPLE, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );
    delay(1000);
    myLED.brightness( LED_BRIGHT_DEFAULT, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );    
    delay(1000);
    myLED.brightness( LED_BRIGHT_DEFAULT, 1 );
    delay(1000);
    myLED.brightness( 0, 1 ); 
    delay(1000);
    myLED.brightness( LED_BRIGHT_DEFAULT, 1 );
    delay(1000);
    myLED.brightness( 0, 1 );     
}
#endif