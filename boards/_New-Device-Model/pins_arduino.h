#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

#define USB_VID 0x303a
#define USB_PID 0x1001

// دبابيس TX/RX الافتراضية للـ ESP32 (إذا لم تستخدم 43/44)
// سنتركها كما هي إذا كانت لوحة ESP32-S3 هي القاعدة، لكن نركز على SPI/TFT
static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t TXD2 = 1;
static const uint8_t RXD2 = 3; // تم التعديل من 2 إلى 3 (افتراضي لـ UART2)

// دبابيس I2C الافتراضية للوحة CrowPanel
static const uint8_t SDA = 21; // تم التعديل من 13 إلى 21
static const uint8_t SCL = 22; // تم التعديل من 15 إلى 22

// دبابيس SPI الافتراضية - موحدة مع دبابيس الشاشة
static const uint8_t SS = 5;    // (GPIO 5 شائع لـ SPI CS)
static const uint8_t MOSI = 13; // تم التعديل من -1 إلى 13 (TFT)
static const uint8_t MISO = 12; // تم التعديل من -1 إلى 12 (TFT)
static const uint8_t SCK = 14;  // تم التعديل من -1 إلى 14 (TFT)

// ... (بقية الدبابيس العامة G0-G46 لم تتغير) ...

// تعريفات خاصة بالكود
#define RGB_LED	25 // (GPIO 25 شائع لـ RGB/LED)

// ... (بقية تعريفات الأجهزة الطرفية الأخرى لم تتغير) ...

// ==========================================================
// تعريفات الشاشة المخصصة لـ ELECROW CrowPanel 2.8" (ILI9341V)
// ==========================================================

#define HAS_SCREEN	1
#define ROTATION	1
#define MINBRIGHT	160

#define USER_SETUP_LOADED	1
#define USE_HSPI_PORT	    1
#define ILI9341_2_DRIVER	1   // تم التعديل من ST7789 إلى ILI9341
#define TFT_RGB_ORDER	    1
#define TFT_WIDTH	        320 // تم التعديل من 135 إلى 320
#define TFT_HEIGHT	        240 // تم التعديل من 240 إلى 240 (مقلوب)
#define TFT_BACKLIGHT_ON	HIGH // تم التعديل من 1 إلى HIGH
#define TFT_BL	            27  // [CrowPanel Pin: 27] - تم التعديل
#define TFT_RST	            4   // [CrowPanel Pin: 4] - تم التعديل
#define TFT_DC	            2   // [CrowPanel Pin: 2] - تم التعديل
#define TFT_MOSI	        13  // [CrowPanel Pin: 13] - تم التعديل
#define TFT_SCLK	        14  // [CrowPanel Pin: 14] - تم التعديل
#define TFT_CS	            15  // [CrowPanel Pin: 15] - تم التعديل
#define TOUCH_CS	        33  // [CrowPanel Pin: 33] - تم التعديل (لتفعيل اللمس)
#define SMOOTH_FONT	        1
#define SPI_FREQUENCY	    40000000 // زيادة السرعة للتوافق
#define SPI_READ_FREQUENCY	20000000
#define SPI_TOUCH_FREQUENCY	2500000

// دبابيس بطاقة الذاكرة (SD Card)
#define SDCARD_CS	21    // [CrowPanel Pin: 21] - تم التعديل
#define SDCARD_SCK	14
#define SDCARD_MISO	12
#define SDCARD_MOSI	13

// دبابيس GROVE (I2C)
#define GROVE_SDA	21
#define GROVE_SCL	22

// دبابيس SPI العامة
#define SPI_SCK_PIN	    14
#define SPI_MOSI_PIN	13
#define SPI_MISO_PIN	12
#define SPI_SS_PIN	    10

#endif /* Pins_Arduino_h */