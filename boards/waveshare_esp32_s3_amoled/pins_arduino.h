#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

// Waveshare ESP32-S3 AMOLED pins
#define LCD_CS 10
#define LCD_SCLK 12
#define LCD_SDIO0 11
#define LCD_SDIO1 13
#define LCD_SDIO2 14
#define LCD_SDIO3 15
#define IIC_SDA 6
#define IIC_SCL 7
#define IMU_SDA IIC_SDA
#define IMU_SCL IIC_SCL
#define RTC_ADDR 0x51  // PCF85063
#define PMIC_ADDR 0x34 // AXP2101
#define AUDIO_I2S_WS 4
#define AUDIO_I2S_BCK 5
#define AUDIO_I2S_DATA 3
#define TF_CS 1
#define PWR_BTN 21
#define BOOT_BTN 0
#define HAS_IMU true
#define HAS_RTC true
#define HAS_AUDIO true
#ifndef HAS_TOUCH
#define HAS_TOUCH true
#endif
#define HAS_CC1101 false
#define HAS_NRF24 false
#define HAS_PN532 false
#define DISPLAY_WIDTH 368
#define DISPLAY_HEIGHT 448
#define DISPLAY_ROTATION 0

// SPI pins for Waveshare ESP32-S3 AMOLED
#define SCK 12  // SPI Clock
#define MISO 13 // SPI Master In Slave Out
#define MOSI 11 // SPI Master Out Slave In
#define SS 1    // SPI Slave Select (Chip Select)

// I2C pins
static const uint8_t SDA = IIC_SDA;
static const uint8_t SCL = IIC_SCL;

static const uint8_t ADC1 = -1; // No ADC used here

#endif /* Pins_Arduino_h */
