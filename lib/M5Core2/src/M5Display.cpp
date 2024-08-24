#if defined (CORE2)
#include "M5Display.h"

#ifdef M5Stack_M5Core2
#include <M5Touch.h>
#endif /* M5Stack_M5Core2 */

#define BLK_PWM_CHANNEL 7  // LEDC_CHANNEL_7

// So we can use this instance without including all of M5Core2 / M5Stack
M5Display *M5Display::instance;

M5Display::M5Display() : TFT_eSPI() {
    if (!instance) instance = this;
}

void M5Display::begin() {
    TFT_eSPI::begin();
    setRotation(1);
    fillScreen(0);

    // Init the back-light LED PWM
    ledcSetup(BLK_PWM_CHANNEL, 44100, 8);
    // ledcAttachPin(TFT_BL, BLK_PWM_CHANNEL);
    ledcWrite(BLK_PWM_CHANNEL, 80);
}

void M5Display::sleep() {
    startWrite();
    writecommand(ILI9341_SLPIN);  // Software reset
    endWrite();
}

void M5Display::wakeup() {
    startWrite();
    writecommand(ILI9341_SLPOUT);
    endWrite();
}

void M5Display::setBrightness(uint8_t brightness) {
    ledcWrite(BLK_PWM_CHANNEL, brightness);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                           const uint16_t *data) {
    bool swap = getSwapBytes();
    setSwapBytes(true);
    pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data);
    setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                           uint16_t *data) {
    bool swap = getSwapBytes();
    setSwapBytes(true);
    pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data);
    setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                           const uint16_t *data, uint16_t transparent) {
    bool swap = getSwapBytes();
    setSwapBytes(true);
    pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data,
              transparent);
    setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                           const uint8_t *data) {
    bool swap = getSwapBytes();
    setSwapBytes(true);
    pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h,
              (const uint16_t *)data);
    setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h,
                           uint8_t *data) {
    bool swap = getSwapBytes();
    setSwapBytes(true);
    pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h,
              (uint16_t *)data);
    setSwapBytes(swap);
}

void M5Display::progressBar(int x, int y, int w, int h, uint8_t val) {
    drawRect(x, y, w, h, 0x09F1);
    fillRect(x + 1, y + 1, w * (((float)val) / 100.0), h - 1, 0x09F1);
}

#include "utility/qrcode.h"
void M5Display::qrcode(const char *string, uint16_t x, uint16_t y,
                       uint8_t width, uint8_t version) {
    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, 0, string);

    // Top quiet zone
    uint8_t thickness   = width / qrcode.size;
    uint16_t lineLength = qrcode.size * thickness;
    uint8_t xOffset     = x + (width - lineLength) / 2;
    uint8_t yOffset     = y + (width - lineLength) / 2;
    fillRect(x, y, width, width, TFT_WHITE);

    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            uint8_t q = qrcode_getModule(&qrcode, x, y);
            if (q)
                fillRect(x * thickness + xOffset, y * thickness + yOffset,
                         thickness, thickness, TFT_BLACK);
        }
    }
}

void M5Display::qrcode(const String &string, uint16_t x, uint16_t y,
                       uint8_t width, uint8_t version) {
    int16_t len = string.length() + 2;
    char buffer[len];
    string.toCharArray(buffer, len);
    qrcode(buffer, x, y, width, version);
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read();  // LSB
    ((uint8_t *)&result)[1] = f.read();  // MSB
    return result;
}

uint32_t read32(fs::File &f) {
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read();  // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read();  // MSB
    return result;
}

// Bodmers BMP image rendering function
void M5Display::drawBmpFile(fs::FS &fs, const char *path, uint16_t x,
                            uint16_t y) {
    if ((x >= width()) || (y >= height())) return;

    // Open requested file on SD card
    File bmpFS = fs.open(path, "r");

    if (!bmpFS) {
        Serial.print("File not found");
        return;
    }

    uint32_t seekOffset;
    uint16_t w, h, row, col;
    uint8_t r, g, b;

    uint32_t startTime = millis();

    if (read16(bmpFS) == 0x4D42) {
        read32(bmpFS);
        read32(bmpFS);
        seekOffset = read32(bmpFS);
        read32(bmpFS);
        w = read32(bmpFS);
        h = read32(bmpFS);

        if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) &&
            (read32(bmpFS) == 0)) {
            y += h - 1;

            setSwapBytes(true);
            bmpFS.seek(seekOffset);

            uint16_t padding = (4 - ((w * 3) & 3)) & 3;
            uint8_t lineBuffer[w * 3 + padding];

            for (row = 0; row < h; row++) {
                bmpFS.read(lineBuffer, sizeof(lineBuffer));
                uint8_t *bptr  = lineBuffer;
                uint16_t *tptr = (uint16_t *)lineBuffer;
                // Convert 24 to 16 bit colours
                for (col = 0; col < w; col++) {
                    b       = *bptr++;
                    g       = *bptr++;
                    r       = *bptr++;
                    *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                }

                // Push the pixel row to screen, pushImage will crop the line if
                // needed y is decremented as the BMP image is drawn bottom up
                pushImage(x, y--, w, 1, (uint16_t *)lineBuffer);
            }
            Serial.print("Loaded in ");
            Serial.print(millis() - startTime);
            Serial.println(" ms");
        } else
            Serial.println("BMP format not recognized.");
    }
    bmpFS.close();
}

// void M5Display::drawBmp(fs::FS &fs, const char *path, uint16_t x, uint16_t y)
// {
//   drawBmpFile(fs, path, x, y);
// }
/***************************************************
  This library is written to be compatible with Adafruit's ILI9341
  library and automatically detects the display type on ESP_WROVER_KITs
  Earlier WROVERs had ILI9341, while newer releases have ST7789V

  MIT license, all text above must be included in any redistribution
 ****************************************************/

/*
 * JPEG
 */

#include "rom/tjpgd.h"

#define jpgColor(c)                                  \
    (((uint16_t)(((uint8_t *)(c))[0] & 0xF8) << 8) | \
     ((uint16_t)(((uint8_t *)(c))[1] & 0xFC) << 3) | \
     ((((uint8_t *)(c))[2] & 0xF8) >> 3))

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_ERROR
const char *jd_errors[] = {"Succeeded",
                           "Interrupted by output function",
                           "Device error or wrong termination of input stream",
                           "Insufficient memory pool for the image",
                           "Insufficient stream input buffer",
                           "Parameter error",
                           "Data format error",
                           "Right format but not supported",
                           "Not supported JPEG standard"};
#endif

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t maxWidth;
    uint16_t maxHeight;
    uint16_t offX;
    uint16_t offY;
    jpeg_div_t scale;
    const void *src;
    size_t len;
    size_t index;
    M5Display *tft;
    uint16_t outWidth;
    uint16_t outHeight;
} jpg_file_decoder_t;

static uint32_t jpgReadFile(JDEC *decoder, uint8_t *buf, uint32_t len) {
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    File *file               = (File *)jpeg->src;
    if (buf) {
        return file->read(buf, len);
    } else {
        file->seek(len, SeekCur);
    }
    return len;
}

static uint32_t jpgRead(JDEC *decoder, uint8_t *buf, uint32_t len) {
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    if (buf) {
        memcpy(buf, (const uint8_t *)jpeg->src + jpeg->index, len);
    }
    jpeg->index += len;
    return len;
}

static uint32_t jpgWrite(JDEC *decoder, void *bitmap, JRECT *rect) {
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    uint16_t x               = rect->left;
    uint16_t y               = rect->top;
    uint16_t w               = rect->right + 1 - x;
    uint16_t h               = rect->bottom + 1 - y;
    uint16_t oL = 0, oR = 0;
    uint8_t *data = (uint8_t *)bitmap;

    if (rect->right < jpeg->offX) {
        return 1;
    }
    if (rect->left >= (jpeg->offX + jpeg->outWidth)) {
        return 1;
    }
    if (rect->bottom < jpeg->offY) {
        return 1;
    }
    if (rect->top >= (jpeg->offY + jpeg->outHeight)) {
        return 1;
    }
    if (rect->top < jpeg->offY) {
        uint16_t linesToSkip = jpeg->offY - rect->top;
        data += linesToSkip * w * 3;
        h -= linesToSkip;
        y += linesToSkip;
    }
    if (rect->bottom >= (jpeg->offY + jpeg->outHeight)) {
        uint16_t linesToSkip =
            (rect->bottom + 1) - (jpeg->offY + jpeg->outHeight);
        h -= linesToSkip;
    }
    if (rect->left < jpeg->offX) {
        oL = jpeg->offX - rect->left;
    }
    if (rect->right >= (jpeg->offX + jpeg->outWidth)) {
        oR = (rect->right + 1) - (jpeg->offX + jpeg->outWidth);
    }

    uint16_t pixBuf[32];
    uint8_t pixIndex = 0;
    uint16_t line;

    jpeg->tft->startWrite();
    // jpeg->tft->setAddrWindow(x - jpeg->offX + jpeg->x + oL, y - jpeg->offY +
    // jpeg->y, w - (oL + oR), h);
    jpeg->tft->setWindow(x - jpeg->offX + jpeg->x + oL,
                         y - jpeg->offY + jpeg->y,
                         x - jpeg->offX + jpeg->x + oL + w - (oL + oR) - 1,
                         y - jpeg->offY + jpeg->y + h - 1);

    while (h--) {
        data += 3 * oL;
        line = w - (oL + oR);
        while (line--) {
            pixBuf[pixIndex++] = jpgColor(data);
            data += 3;
            if (pixIndex == 32) {
                jpeg->tft->writePixels(pixBuf, 32);
                // SPI.writePixels((uint8_t *)pixBuf, 64);
                pixIndex = 0;
            }
        }
        data += 3 * oR;
    }
    if (pixIndex) {
        jpeg->tft->writePixels(pixBuf, pixIndex);
        // SPI.writePixels((uint8_t *)pixBuf, pixIndex * 2);
    }
    jpeg->tft->endWrite();
    return 1;
}

static bool jpgDecode(jpg_file_decoder_t *jpeg,
                      uint32_t (*reader)(JDEC *, uint8_t *, uint32_t)) {
    static uint8_t work[3100];
    JDEC decoder;

    JRESULT jres = jd_prepare(&decoder, reader, work, 3100, jpeg);
    if (jres != JDR_OK) {
        log_e("jd_prepare failed! %s", jd_errors[jres]);
        return false;
    }

    uint16_t jpgWidth  = decoder.width / (1 << (uint8_t)(jpeg->scale));
    uint16_t jpgHeight = decoder.height / (1 << (uint8_t)(jpeg->scale));

    if (jpeg->offX >= jpgWidth || jpeg->offY >= jpgHeight) {
        log_e("Offset Outside of JPEG size");
        return false;
    }

    size_t jpgMaxWidth  = jpgWidth - jpeg->offX;
    size_t jpgMaxHeight = jpgHeight - jpeg->offY;

    jpeg->outWidth =
        (jpgMaxWidth > jpeg->maxWidth) ? jpeg->maxWidth : jpgMaxWidth;
    jpeg->outHeight =
        (jpgMaxHeight > jpeg->maxHeight) ? jpeg->maxHeight : jpgMaxHeight;

    jres = jd_decomp(&decoder, jpgWrite, (uint8_t)jpeg->scale);
    if (jres != JDR_OK) {
        log_e("jd_decomp failed! %s", jd_errors[jres]);
        return false;
    }

    return true;
}

void M5Display::drawJpg(const uint8_t *jpg_data, size_t jpg_len, uint16_t x,
                        uint16_t y, uint16_t maxWidth, uint16_t maxHeight,
                        uint16_t offX, uint16_t offY, jpeg_div_t scale) {
    if ((x + maxWidth) > width() || (y + maxHeight) > height()) {
        log_e("Bad dimensions given");
        return;
    }

    jpg_file_decoder_t jpeg;

    if (!maxWidth) {
        maxWidth = width() - x;
    }
    if (!maxHeight) {
        maxHeight = height() - y;
    }

    jpeg.src       = jpg_data;
    jpeg.len       = jpg_len;
    jpeg.index     = 0;
    jpeg.x         = x;
    jpeg.y         = y;
    jpeg.maxWidth  = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX      = offX;
    jpeg.offY      = offY;
    jpeg.scale     = scale;
    jpeg.tft       = this;

    jpgDecode(&jpeg, jpgRead);
}

void M5Display::drawJpgFile(fs::FS &fs, const char *path, uint16_t x,
                            uint16_t y, uint16_t maxWidth, uint16_t maxHeight,
                            uint16_t offX, uint16_t offY, jpeg_div_t scale) {
    if ((x + maxWidth) > width() || (y + maxHeight) > height()) {
        log_e("Bad dimensions given");
        return;
    }

    File file = fs.open(path);
    if (!file) {
        log_e("Failed to open file for reading");
        return;
    }

    jpg_file_decoder_t jpeg;

    if (!maxWidth) {
        maxWidth = width() - x;
    }
    if (!maxHeight) {
        maxHeight = height() - y;
    }

    jpeg.src       = &file;
    jpeg.len       = file.size();
    jpeg.index     = 0;
    jpeg.x         = x;
    jpeg.y         = y;
    jpeg.maxWidth  = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX      = offX;
    jpeg.offY      = offY;
    jpeg.scale     = scale;
    jpeg.tft       = this;

    jpgDecode(&jpeg, jpgReadFile);

    file.close();
}

/*
 * PNG
 */

#include "utility/pngle.h"
#include <HTTPClient.h>

typedef struct _png_draw_params {
    uint16_t x;
    uint16_t y;
    uint16_t maxWidth;
    uint16_t maxHeight;
    uint16_t offX;
    uint16_t offY;
    double scale;
    uint8_t alphaThreshold;

    M5Display *tft;
} png_file_decoder_t;

static void pngle_draw_callback(pngle_t *pngle, uint32_t x, uint32_t y,
                                uint32_t w, uint32_t h, uint8_t rgba[4]) {
    png_file_decoder_t *p = (png_file_decoder_t *)pngle_get_user_data(pngle);
    uint16_t color        = jpgColor(rgba);  // XXX: It's PNG ;)

    if (x < p->offX || y < p->offY) return;
    x -= p->offX;
    y -= p->offY;

    // An interlaced file with alpha channel causes disaster, so use 1 here for
    // simplicity
    w = 1;
    h = 1;

    if (p->scale != 1.0) {
        x = (uint32_t)ceil(x * p->scale);
        y = (uint32_t)ceil(y * p->scale);
        w = (uint32_t)ceil(w * p->scale);
        h = (uint32_t)ceil(h * p->scale);
    }

    if (x >= p->maxWidth || y >= p->maxHeight) return;
    if (x + w >= p->maxWidth) w = p->maxWidth - x;
    if (y + h >= p->maxHeight) h = p->maxHeight - y;

    x += p->x;
    y += p->y;

    if (rgba[3] >= p->alphaThreshold) {
        p->tft->fillRect(x, y, w, h, color);
    }
}

void M5Display::drawPngFile(fs::FS &fs, const char *path, uint16_t x,
                            uint16_t y, uint16_t maxWidth, uint16_t maxHeight,
                            uint16_t offX, uint16_t offY, double scale,
                            uint8_t alphaThreshold) {
    File file = fs.open(path);
    if (!file) {
        log_e("Failed to open file for reading");
        return;
    }

    pngle_t *pngle = pngle_new();

    png_file_decoder_t png;

    if (!maxWidth) {
        maxWidth = width() - x;
    }
    if (!maxHeight) {
        maxHeight = height() - y;
    }

    png.x              = x;
    png.y              = y;
    png.maxWidth       = maxWidth;
    png.maxHeight      = maxHeight;
    png.offX           = offX;
    png.offY           = offY;
    png.scale          = scale;
    png.alphaThreshold = alphaThreshold;
    png.tft            = this;

    pngle_set_user_data(pngle, &png);
    pngle_set_draw_callback(pngle, pngle_draw_callback);

    // Feed data to pngle
    uint8_t buf[1024];
    int remain = 0;
    int len;
    while ((len = file.read(buf + remain, sizeof(buf) - remain)) > 0) {
        int fed = pngle_feed(pngle, buf, remain + len);
        if (fed < 0) {
            log_e("[pngle error] %s", pngle_error(pngle));
            break;
        }

        remain = remain + len - fed;
        if (remain > 0) memmove(buf, buf + fed, remain);
    }

    pngle_destroy(pngle);
    file.close();
}

void M5Display::drawPngUrl(const char *url, uint16_t x, uint16_t y,
                           uint16_t maxWidth, uint16_t maxHeight, uint16_t offX,
                           uint16_t offY, double scale,
                           uint8_t alphaThreshold) {
    HTTPClient http;

    if (WiFi.status() != WL_CONNECTED) {
        log_e("Not connected");
        return;
    }

    http.begin(url);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        log_e("HTTP ERROR: %d\n", httpCode);
        http.end();
        return;
    }

    WiFiClient *stream = http.getStreamPtr();

    pngle_t *pngle = pngle_new();

    png_file_decoder_t png;

    if (!maxWidth) {
        maxWidth = width() - x;
    }
    if (!maxHeight) {
        maxHeight = height() - y;
    }

    png.x              = x;
    png.y              = y;
    png.maxWidth       = maxWidth;
    png.maxHeight      = maxHeight;
    png.offX           = offX;
    png.offY           = offY;
    png.scale          = scale;
    png.alphaThreshold = alphaThreshold;
    png.tft            = this;

    pngle_set_user_data(pngle, &png);
    pngle_set_draw_callback(pngle, pngle_draw_callback);

    // Feed data to pngle
    uint8_t buf[1024];
    int remain = 0;
    int len;
    while (http.connected()) {
        size_t size = stream->available();
        if (!size) {
            delay(1);
            continue;
        }

        if (size > sizeof(buf) - remain) size = sizeof(buf) - remain;
        if ((len = stream->readBytes(buf + remain, size)) > 0) {
            int fed = pngle_feed(pngle, buf, remain + len);
            if (fed < 0) {
                log_e("[pngle error] %s", pngle_error(pngle));
                break;
            }

            remain = remain + len - fed;
            if (remain > 0) memmove(buf, buf + fed, remain);
        }
    }

    pngle_destroy(pngle);
    http.end();
}

// Saves and restores font properties, datum, cursor, colors

void M5Display::pushState() {
    DisplayState s;
    s.textfont    = textfont;
    s.textsize    = textsize;
    s.textcolor   = textcolor;
    s.textbgcolor = textbgcolor;
    s.cursor_x    = cursor_x;
    s.cursor_y    = cursor_y;
    s.padX        = padX;
    _displayStateStack.push_back(s);
}

void M5Display::popState() {
    if (_displayStateStack.empty()) return;
    DisplayState s = _displayStateStack.back();
    _displayStateStack.pop_back();
    textfont    = s.textfont;
    textsize    = s.textsize;
    textcolor   = s.textcolor;
    textbgcolor = s.textbgcolor;
    cursor_x    = s.cursor_x;
    cursor_y    = s.cursor_y;
    padX        = s.padX;
}

#ifdef M5Stack_M5Core2

#ifdef TFT_eSPI_TOUCH_EMULATION

// Emulates the native (resistive) TFT_eSPI touch interface using M5.Touch

uint8_t M5Display::getTouchRaw(uint16_t *x, uint16_t *y) {
    return getTouch(x, y);
}

uint16_t M5Display::getTouchRawZ(void) {
    return (TOUCH->ispressed()) ? 1000 : 0;
}

void M5Display::convertRawXY(uint16_t *x, uint16_t *y) {
    return;
}

uint8_t M5Display::getTouch(uint16_t *x, uint16_t *y,
                            uint16_t threshold /* = 600 */) {
    TOUCH->read();
    if (TOUCH->points) {
        *x = TOUCH->point[0].x;
        *y = TOUCH->point[0].y;
        return true;
    }
    return false;
}

void M5Display::calibrateTouch(uint16_t *data, uint32_t color_fg,
                               uint32_t color_bg, uint8_t size) {
    return;
}

void M5Display::setTouch(uint16_t *data) {
    return;
}

#endif /* TFT_eSPI_TOUCH_EMULATION */

#endif /* M5Stack_M5Core2 */
#endif