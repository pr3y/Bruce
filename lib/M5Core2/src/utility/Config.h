#include "../M5Display.h"
#if defined (CORE2)
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define TFT     M5Display::instance
#define BUTTONS M5Buttons::instance

// Screen
#define TFT_LED_PIN  32
#define TFT_DC_PIN   27
#define TFT_CS_PIN   14
#define TFT_MOSI_PIN 23
#define TFT_CLK_PIN  18
#define TFT_RST_PIN  33
#define TFT_MISO_PIN 19

// SD card
#define TFCARD_CS_PIN 4

// UART
#define USE_SERIAL Serial

// Core2 defines
#define M5Stack_M5Core2
#define TFT_eSPI_TOUCH_EMULATION
#define TOUCH M5Touch::instance

#endif /* CONFIG_H */
#endif