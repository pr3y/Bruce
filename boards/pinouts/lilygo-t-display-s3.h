#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

// Lite Version
// #define LITE_VERSION 1

// Main I2C Bus
#define SPI_SS_PIN      10
#define SPI_MOSI_PIN    11
#define SPI_MISO_PIN    13
#define SPI_SCK_PIN     12
static const uint8_t SS    = SPI_SS_PIN;
static const uint8_t MOSI  = SPI_MOSI_PIN;
static const uint8_t SCK   = SPI_MISO_PIN;
static const uint8_t MISO  = SPI_SCK_PIN;

// Set Main I2C Bus
#define GROVE_SDA 44
#define GROVE_SCL 43
static const uint8_t SDA = GROVE_SDA;
static const uint8_t SCL = GROVE_SCL;

// TFT_eSPI display
#define USER_SETUP_LOADED
#define ST7789_DRIVER   1
#define TFT_BACKLIGHT_ON 1
#define INIT_SEQUENCE_3
#define CGRAM_OFFSET
#define TFT_RGB_ORDER TFT_RGB
#define TFT_INVERSION_ON
#define TFT_PARALLEL_8_BIT
#define SMOOTH_FONT     1
#define TFT_WIDTH       170
#define TFT_HEIGHT      320
#define TFT_BL          38
#define TFT_MISO        -1   
#define TFT_MOSI        11
#define TFT_SCLK        12
#define TFT_CS          6 
#define TFT_DC          7
#define TFT_RST         5
#define TFT_D0          39
#define TFT_D1          40
#define TFT_D2          41
#define TFT_D3          42
#define TFT_D4          45
#define TFT_D5          46
#define TFT_D6          47
#define TFT_D7          48
#define TFT_WR          8
#define TFT_RD          9
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SPI_FREQUENCY   40000000
#define SPI_READ_FREQUENCY 20000000

// Display Setup#
#define HAS_SCREEN
#define ROTATION        3
#define MINBRIGHT       (uint8_t) 1

// Font Sizes#
#define FP  1
#define FM  2
#define FG  3

// Serial
#define SERIAL_TX   43
#define SERIAL_RX   44

// Battery PIN
#define PIN_POWER_ON    15
#define BAT_PIN         4

// Mic
#define PIN_CLK         39
#define PIN_DATA        42

#define BTN_ALIAS	'"OK"'
#define HAS_3_BUTTONS
#define SEL_BTN     1
#define UP_BTN      0
#define DW_BTN      14 
#define BK_BTN	    3
#define BTN_ACT     LOW

// IR
#define LED	        44
#define RXLED	    43
#define LED_ON	    HIGH
#define LED_OFF	    LOW

// BadUSB 
#define USB_as_HID 1

#endif /* Pins_Arduino_h */