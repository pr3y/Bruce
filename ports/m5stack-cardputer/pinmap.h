#include <FastLED.h>
// Lite Version
// #define LITE_VERSION 1

// Main I2C Bus
#define SPI_MISO_PIN    39
#define SPI_MOSI_PIN    14
#define SPI_SCK_PIN     40
#define SPI_SS_PIN      GROVE_SCL

// Set Main I2C Bus
#define GROVE_SDA 2
#define GROVE_SCL 1

// TFT_eSPI display
#define ST7789_2_DRIVER   1
#define USER_SETUP_LOADED
#define USE_HSPI_PORT
#define SMOOTH_FONT
#define TFT_RGB_ORDER
#define TFT_BACKLIGHT_ON
#define TFT_WIDTH       135
#define TFT_HEIGHT      240
#define TFT_BL          38
#define TFT_MOSI        35
#define TFT_SCLK        36
#define TFT_CS          37 
#define TFT_DC          34
#define TFT_RST         33
#define TOUCH_CS        -1
#define SPI_FREQUENCY   20000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

// Display Setup
#define HAS_SCREEN
#define ROTATION        1
#define WIDTH           TFT_HEIGHT
#define HEIGHT          TFT_WIDTH
#define BACKLIGHT       TFT_BL
#define MINBRIGHT       160

// Font Sizes
#define FP  1
#define FM  2
#define FG  3

// SD Card
#define SDCARD_MISO SPI_MISO_PIN
#define SDCARD_MOSI SPI_MOSI_PIN
#define SDCARD_SCK  SPI_SCK_PIN
#define SDCARD_CS   13

// NRF24 - Over QWIIC Port #2
#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN    GROVE_SDA
#define NRF24_SS_PIN    SPI_SS_PIN
#define NRF24_MOSI_PIN  SDCARD_MOSI
#define NRF24_SCK_PIN   SDCARD_SCK
#define NRF24_MISO_PIN  SDCARD_MISO

// CC1101
#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN GROVE_SDA
#define CC1101_SS_PIN   SPI_SS_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#define CC1101_SCK_PIN  SPI_SCK_PIN


// Mic
#define MIC_SPM1423
#define PIN_CLK         43
#define PIN_DATA        46

//Speaker
#define HAS_NS4168_SPKR
#define BCLK    46
#define WCLK    40
#define DOUT    7
#define MCLK    PIN_CLK

// Serial
#define SERIAL_TX 43 
#define SERIAL_RX 44

// Fuel Gauge
#define BQ27220_I2C_ADDRESS	0x55
#define BQ27220_I2C_SDA	GROVE_SDA
#define BQ27220_I2C_SCL	GROVE_SCL

// Encoder
#define ENCODER_INA	4
#define ENCODER_INB	5
#define ENCODER_KEY	0
#define HAS_BTN	    1
#define BTN_ALIAS	'"Mid"'
#define SEL_BTN	    ENCODER_KEY
#define UP_BTN	    -1
#define DW_BTN	    -1
#define BK_BTN	    6
#define BTN_ACT	    LOW

// IR
#define IR_TX_PINS	'{{"Default", 2}, {"Pin 43", 43}, {"Pin 44", 44}}'
#define IR_RX_PINS	'{{"Default", 1}, {"Pin 43", 43}, {"Pin 44", 44}}'
#define LED	        44
#define LED_ON	    HIGH
#define LED_OFF	    LOW

// RF Module
#define RF_TX_PINS	'{{"Pin 43", 43}, {"Pin 44", 44}}'
#define RF_RX_PINS	'{{"Pin 43", 43}, {"Pin 44", 44}}'

// FM
// #define FM_SI4713
// #define FM_RSTPIN 40

// RTC
// #define HAS_RTC

// RGB LED
// #define HAS_RGB_LED
// #define RGB_LED=21

// BadUSB 
#define USB_as_HID=1
// #define BAD_TX=GROVE_SDA
//#define BAD_RX=GROVE_SCL