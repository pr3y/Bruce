#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

#define USB_VID 0x303a
#define USB_PID 0x1001

static const uint8_t TX = 1;
static const uint8_t RX = 2;

static const uint8_t SDA = 47;
static const uint8_t SCL = 48;

// Modified elsewhere
static const uint8_t SS = 43;
static const uint8_t MOSI = 12;
static const uint8_t MISO = 11;
static const uint8_t SCK = 13;

static const uint8_t G0 = 0;
static const uint8_t G1 = 1;
static const uint8_t G2 = 2;
static const uint8_t G3 = 3;
static const uint8_t G4 = 4;
static const uint8_t G5 = 5;
static const uint8_t G6 = 6;
static const uint8_t G7 = 7;
static const uint8_t G8 = 8;
static const uint8_t G9 = 9;
static const uint8_t G10 = 10;
static const uint8_t G11 = 11;
static const uint8_t G12 = 12;
static const uint8_t G13 = 13;
static const uint8_t G14 = 14;
static const uint8_t G15 = 15;
static const uint8_t G39 = 39;
static const uint8_t G40 = 40;
static const uint8_t G41 = 41;
static const uint8_t G42 = 42;
static const uint8_t G43 = 43;
static const uint8_t G44 = 44;
static const uint8_t G46 = 46;

static const uint8_t ADC1 = 7;
static const uint8_t ADC2 = 8;

#define SERIAL_RX 2
#define SERIAL_TX 1
#define BAD_RX SERIAL_RX
#define BAD_TX SERIAL_TX
#define USB_as_HID 1

#define BTN_ALIAS	"\"OK\""
#define HAS_5_BUTTONS
#define SEL_BTN     0
#define UP_BTN      41
#define DW_BTN      40
#define R_BTN       38
#define L_BTN       39
#define BTN_ACT     LOW

#define RXLED	4
#define LED     5
#define LED_ON	HIGH
#define LED_OFF	LOW

#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN	9
#define CC1101_GDO2_PIN 10
#define CC1101_SS_PIN	46
#define CC1101_MOSI_PIN	SPI_MOSI_PIN
#define CC1101_SCK_PIN	SPI_SCK_PIN
#define CC1101_MISO_PIN	SPI_MISO_PIN

#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN    21
#define NRF24_SS_PIN	14
#define NRF24_MOSI_PIN	SPI_MOSI_PIN
#define NRF24_SCK_PIN	SPI_SCK_PIN
#define NRF24_MISO_PIN	SPI_MISO_PIN

#define FP	1
#define FM	2
#define FG	3

#define HAS_SCREEN	1
#define ROTATION	1
#define MINBRIGHT       (uint8_t) 1

#define USER_SETUP_LOADED	1
#define USE_HSPI_PORT	    1
#define ST7789_DRIVER	    1
#define TFT_RGB_ORDER	    0
#define TFT_WIDTH	        170
#define TFT_HEIGHT	        320
#define TFT_BACKLIGHT_ON	1
#define TFT_BL	            6
#define TFT_RST	            16
#define TFT_DC	            15
#define TFT_MOSI	        17
#define TFT_SCLK	        18
#define TFT_CS	            7
#define TOUCH_CS	        -1
#define SMOOTH_FONT	        1
#define SPI_FREQUENCY	    20000000
#define SPI_READ_FREQUENCY	20000000
#define SPI_TOUCH_FREQUENCY	2500000

#define SDCARD_CS	3
#define SDCARD_SCK	18
#define SDCARD_MISO	8
#define SDCARD_MOSI	17

#define GROVE_SDA	47
#define GROVE_SCL	48

#define SPI_SCK_PIN	    13
#define SPI_MOSI_PIN	12
#define SPI_MISO_PIN	11
#define SPI_SS_PIN	    43

// RGB LED

#define HAS_RGB_LED         1
#define RGB_LED             45
#define LED_TYPE            WS2812B
#define LED_ORDER           GRB
#define LED_TYPE_IS_RGBW    0
#define LED_COUNT          16

#define USE_BQ25896 

#endif /* Pins_Arduino_h */
