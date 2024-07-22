#ifdef CARDPUTER
// By IncursioHack @github.com/IncursioHack
#include <Arduino.h>
#include <LiteLED.h>

// Escolha o tipo de LED na lista abaixo. Comente todos, exceto o que você está usando.
#define LED_TYPE        LED_STRIP_SK6812
// #define LED_STRIP_WS2812
// #define LED_STRIP_APA106
// #define LED_STRIP_SM16703

#define LED_TYPE_IS_RGBW 1  // Se o LED for do tipo RGBW, altere o 0 para 1

// Definição de intensidade de brilho
#define LED_BRIGHT_DEFAULT 245  // Define o brilho padrão do LED. "0" é desligado; 255 pode ser muito brilhante.

// Cores predefinidas para o LED
static const crgb_t L_RED = 0xff0000;
static const crgb_t L_GREEN = 0x00ff00;
static const crgb_t L_BLUE = 0x0000ff;
static const crgb_t L_WHITE = 0xe0e0e0;
static const crgb_t L_PURPLE = 0xff00ff;

// Declaração do objeto LiteLED
extern LiteLED myLED;

// Declarações de funções para controle do LED RGB
void ledrgb_setup();
void ledrgb_flash();

#endif