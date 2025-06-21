#ifndef __DISPLAY_LOGER
#define __DISPLAY_LOGER
#include <precompiler_flags.h> //need to fetch the device Settings that are not in platformio.ini file
#include <vector>
#ifdef HAS_SCREEN
#include <TFT_eSPI.h>
#define BRUCE_TFT_DRIVER TFT_eSPI
#else
#include <VectorDisplay.h>
#define BRUCE_TFT_DRIVER SerialDisplayClass
#endif
enum tftFuncs : uint8_t { // DO NOT CHANGE THE ORDER, ADD NEW FUNCTIONS TO THE END!!!
    FILLSCREEN,           // 0
    DRAWRECT,             // 1
    FILLRECT,             // 2
    DRAWROUNDRECT,        // 3
    FILLROUNDRECT,        // 4
    DRAWCIRCLE,           // 5
    FILLCIRCLE,           // 6
    DRAWTRIAGLE,          // 7
    FILLTRIANGLE,         // 8
    DRAWELIPSE,           // 9
    FILLELIPSE,           // 10
    DRAWLINE,             // 11
    DRAWARC,              // 12
    DRAWWIDELINE,         // 13
    DRAWCENTRESTRING,     // 14
    DRAWRIGHTSTRING,      // 15
    DRAWSTRING,           // 16
    PRINT,                // 17
    DRAWIMAGE,            // 18
    DRAWPIXEL,            // 19
    DRAWFASTVLINE,        // 20
    DRAWFASTHLINE,        // 21
    // Add new ones here

    SCREEN_INFO = 99 // 99
};

struct tftLog {
    tftFuncs function;
    String info;
};
class tft_logger : public BRUCE_TFT_DRIVER {
private:
    std::vector<tftLog> log;
    bool logging = false;
    bool _logging = false;
    void addScreenInfo();

public:
    tft_logger(int16_t w = TFT_WIDTH, int16_t h = TFT_HEIGHT);
    virtual ~tft_logger();
    void setLogging(bool _log = true);
    void pauseLogging(bool p = true);
    bool inline getLogging(void) { return logging; };
    String getJSONLog();
    void fillScreen(uint32_t color);
    void removeLogEntriesInsideRect(int rx, int ry, int rw, int rh);
    void removeOverlappedImages(int x, int y, int center, int ms);
    void imageToJson(String fs, String file, int x, int y, bool center = false, int Ms = 0);
    void drawLine(uint32_t x, uint32_t y, uint32_t x1, uint32_t y1, uint32_t color);
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
    void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
    void drawArc(
        int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle,
        uint32_t fg_color, uint32_t bg_color, bool smoothArc = true
    );
    void
    drawWideLine(float ax, float ay, float bx, float by, float wd, uint32_t fg, uint32_t bg = 0x00FFFFFF);
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t fg);
    void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t fg);
    void log_drawString(String s, tftFuncs fn, int32_t x, int32_t y);
    int16_t drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1);
    int16_t drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font = 1);
    int16_t drawRightString(const String &string, int32_t x, int32_t y, uint8_t font = 1);

    void log_print(String s);

    size_t print(const String &s);
    size_t println(const String &s);
    size_t println(void);

    size_t println(char c);
    size_t println(unsigned char b, int base);
    size_t println(int n, int base);
    size_t println(unsigned int n, int base);
    size_t println(long n, int base);
    size_t println(unsigned long n, int base);
    size_t println(long long n, int base);
    size_t println(unsigned long long n, int base);
    size_t println(double n, int digits);

    size_t print(char c);
    size_t print(unsigned char b, int base);
    size_t print(int n, int base);
    size_t print(unsigned int n, int base);
    size_t print(long n, int base);
    size_t print(unsigned long n, int base);
    size_t print(long long n, int base);
    size_t print(unsigned long long n, int base);
    size_t print(double n, int digits);
    size_t printf(const char *format, ...);

protected:
    bool isLogEqual(const tftLog &a, const tftLog &b);
    void checkAndLog(tftFuncs f, String s);
    void restoreLogger();
    void pushLogIfUnique(const tftLog &l);
};

#endif //__DISPLAY_LOGER
