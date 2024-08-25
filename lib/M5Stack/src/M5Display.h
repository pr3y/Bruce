#if defined (CORE)
#ifndef _M5DISPLAY_H_
#define _M5DISPLAY_H_

#include <Arduino.h>
#include <FS.h>
#include <SPI.h>

#include <TFT_eSPI.h>
//#include "utility/In_eSPI.h"
//#include "utility/Sprite.h"

typedef enum {
    JPEG_DIV_NONE,
    JPEG_DIV_2,
    JPEG_DIV_4,
    JPEG_DIV_8,
    JPEG_DIV_MAX
} jpeg_div_t;

class M5Display : public TFT_eSPI {
   public:
    M5Display();
    void begin();
    void sleep();
    void wakeup();
    void setBrightness(uint8_t brightness);
    void clearDisplay(uint32_t color = ILI9341_BLACK) {
        fillScreen(color);
    }
    void clear(uint32_t color = ILI9341_BLACK) {
        fillScreen(color);
    }
    void display() {
    }

    inline void startWrite(void) {
#if defined(SPI_HAS_TRANSACTION) && defined(SUPPORT_TRANSACTIONS) && \
    !defined(ESP32_PARALLEL)
        if (locked) {
            locked = false;
            SPI.beginTransaction(
                SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
        }
#endif
        CS_L;
    }
    inline void endWrite(void) {
#if defined(SPI_HAS_TRANSACTION) && defined(SUPPORT_TRANSACTIONS) && \
    !defined(ESP32_PARALLEL)
        if (!inTransaction) {
            if (!locked) {
                locked = true;
                SPI.endTransaction();
            }
        }
#endif
        CS_H;
    }
    inline void writePixel(uint16_t color) {
        SPI.write16(color);
    }
    inline void writePixels(uint16_t *colors, uint32_t len) {
        SPI.writePixels((uint8_t *)colors, len * 2);
    }
    void progressBar(int x, int y, int w, int h, uint8_t val);

#define setFont setFreeFont

    void qrcode(const char *string, uint16_t x = 50, uint16_t y = 10,
                uint8_t width = 220, uint8_t version = 6);
    void qrcode(const String &string, uint16_t x = 50, uint16_t y = 10,
                uint8_t width = 220, uint8_t version = 6);

    void drawBmp(fs::FS &fs, const char *path, uint16_t x, uint16_t y);
    void drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);

    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                    const uint16_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                    const uint8_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                    uint16_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                    uint8_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                    const uint16_t *data, uint16_t transparent);

    void drawJpg(const uint8_t *jpg_data, size_t jpg_len, uint16_t x = 0,
                 uint16_t y = 0, uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                 uint16_t offX = 0, uint16_t offY = 0,
                 jpeg_div_t scale = JPEG_DIV_NONE);

    void drawJpg(fs::FS &fs, const char *path, uint16_t x = 0, uint16_t y = 0,
                 uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                 uint16_t offX = 0, uint16_t offY = 0,
                 jpeg_div_t scale = JPEG_DIV_NONE);

    void drawJpgFile(fs::FS &fs, const char *path, uint16_t x = 0,
                     uint16_t y = 0, uint16_t maxWidth = 0,
                     uint16_t maxHeight = 0, uint16_t offX = 0,
                     uint16_t offY = 0, jpeg_div_t scale = JPEG_DIV_NONE);

    void drawPngFile(fs::FS &fs, const char *path, uint16_t x = 0,
                     uint16_t y = 0, uint16_t maxWidth = 0,
                     uint16_t maxHeight = 0, uint16_t offX = 0,
                     uint16_t offY = 0, double scale = 1.0,
                     uint8_t alphaThreshold = 127);

    void drawPngUrl(const char *url, uint16_t x = 0, uint16_t y = 0,
                    uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                    uint16_t offX = 0, uint16_t offY = 0, double scale = 1.0,
                    uint8_t alphaThreshold = 127);

   private:
};
#endif
#endif