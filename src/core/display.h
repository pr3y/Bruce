#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <globals.h>
#include "sd_functions.h" // to catch FileList Struct
#include <SD.h>
#include <FS.h>
#include <LittleFS.h>
#define BORDER_PAD_X 10
#define BORDER_PAD_Y 28

struct Opt_Coord {
  uint16_t x=0;
  uint16_t y=0;
  uint16_t size=10;
  uint16_t fgcolor=bruceConfig.priColor;
  uint16_t bgcolor=bruceConfig.bgColor;
};
void displayScrollingText(const String& text, Opt_Coord& coord);

#include <AnimatedGIF.h>

struct GifPosition {
    int x;
    int y;

    GifPosition(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
};

class Gif {
public:
    Gif();

    ~Gif();

    bool openGIF(FS *fs, const char *filename);

    int playFrame(int x = 0, int y = 0, bool bSync = true);

    int getInfo(GIFINFO *pInfo) {
      return gif->getInfo(pInfo);
    }

    void reset(){
      return gif->reset();
    }

    void close() {
      return gif->close();
    }

    int getCanvasWidth() {
      return gif->getCanvasWidth();
    }

    int getCanvasHeight() {
      return gif->getCanvasHeight();
    }

    int getLastError();

    AnimatedGIF *gif;

private:
    unsigned long lTime = millis();

    static FS *GifFs;

    int zero = 0;
    int *delayMilliseconds = &zero;

    GifPosition gifPosition;

    static void *openFile(const char *fname, int32_t *pSize);

    static void closeFile(void *pHandle);

    static int32_t readFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);

    static int32_t seekFile(GIFFILE *pFile, int32_t iPosition);

    static void GIFDraw(GIFDRAW *pDraw);

};

bool showGif(FS *fs, const char *filename, int x=0, int y=0, bool center = false, int playDurationMs = 0);
bool showJpeg(FS fs,String filename, int x=0, int y=0, bool center = false);

uint16_t getComplementaryColor(uint16_t color);
uint16_t getComplementaryColor2(uint16_t color);
uint16_t getColorVariation(uint16_t color, int delta = 10, int direction = 0);

void resetTftDisplay(int x = 0, int y = 0, uint16_t fc = bruceConfig.priColor, int size = FM, uint16_t bg = bruceConfig.bgColor, uint16_t screen = bruceConfig.bgColor);
void setTftDisplay(int x = 0, int y = 0, uint16_t fc = tft.textcolor, int size = tft.textsize, uint16_t bg = tft.textbgcolor);

void turnOffDisplay();
bool wakeUpScreen();

void displayRedStripe(String text, uint16_t fgcolor = TFT_WHITE, uint16_t bgcolor = TFT_RED);

void displayError(String txt, bool waitKeyPress = false);     // Red Stripe
void displayWarning(String txt, bool waitKeyPress = false);   // Yellow Stripe
void displayInfo(String txt, bool waitKeyPress = false);      // Blue Stripe
void displaySuccess(String txt, bool waitKeyPress = false);   // Green Strupe
void displayTextLine(String txt, bool waitKeyPress = false); // UI Colored stripe
void setPadCursor(int16_t padx=1, int16_t pady=0);

void padprintf(int16_t padx, const char *format, ...);
void padprintf(const char *format, ...);

void padprint(const String &s, int16_t padx=1);
void padprint(const char str[], int16_t padx=1);
void padprint(char c, int16_t padx=1);
void padprint(unsigned char b, int base=DEC, int16_t padx=1);
void padprint(int n, int base=DEC, int16_t padx=1);
void padprint(unsigned int n, int base=DEC, int16_t padx=1);
void padprint(long n, int base=DEC, int16_t padx=1);
void padprint(unsigned long n, int base=DEC, int16_t padx=1);
void padprint(long long n, int base=DEC, int16_t padx=1);
void padprint(unsigned long long n, int base=DEC, int16_t padx=1);
void padprint(double n, int digits, int16_t padx=1);

void padprintln(const String &s, int16_t padx=1);
void padprintln(const char str[], int16_t padx=1);
void padprintln(char c, int16_t padx=1);
void padprintln(unsigned char b, int base=DEC, int16_t padx=1);
void padprintln(int n, int base=DEC, int16_t padx=1);
void padprintln(unsigned int n, int base=DEC, int16_t padx=1);
void padprintln(long n, int base=DEC, int16_t padx=1);
void padprintln(unsigned long n, int base=DEC, int16_t padx=1);
void padprintln(long long n, int base=DEC, int16_t padx=1);
void padprintln(unsigned long long n, int base=DEC, int16_t padx=1);
void padprintln(double n, int digits, int16_t padx=1);

//loopOptions will now return the last index used in the function
int loopOptions(std::vector<Option>& options, bool bright, bool submenu, String subText,int index = 0);
inline int loopOptions(std::vector<Option>& options, int _index) { return loopOptions(options, false, false, "", _index); }
inline int loopOptions(std::vector<Option>& options) { return loopOptions(options, false, false, "", 0); }

Opt_Coord drawOptions(int index,std::vector<Option>& options, uint16_t fgcolor, uint16_t bgcolor);

void drawSubmenu(int index,std::vector<Option>& options, String system);

void drawStatusBar();
void drawMainBorder(bool clear = true);
void drawMainBorderWithTitle(String title, bool clear = true);
void printTitle(String title);
void printSubtitle(String subtitle, bool withLine = true);
void printFootnote(String text);

Opt_Coord listFiles(int index, std::vector<FileList> fileList);

void drawWireguardStatus(int x, int y);

void progressHandler(int progress, size_t total, String message = "Running, Wait");

int getBattery() __attribute__((weak));

void drawBatteryStatus(uint8_t bat);

void drawWifiSmall(int x, int y);

void drawBLESmall(int x, int y);

void drawBLE_beacon(int x, int y, uint16_t color);

void drawGPS(int x, int y);

void drawGpsSmall(int x, int y);

void TouchFooter(uint16_t color = bruceConfig.priColor);

void MegaFooter(uint16_t color = bruceConfig.priColor);

#if !defined(LITE_VERSION)
#define bruce_small_width 60
#define bruce_small_height 34
PROGMEM const unsigned char bruce_small_bits[] = {
  0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0x7F, 0x0F,
  // skrocone zeby nie zajmowalo tak duzo miejsca
  // i tak jest zastepowane przez boot.gif
 };


#define bits_width 237
#define bits_height 133
PROGMEM const unsigned char bits[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F,
  // skrocone zeby nie zajmowalo tak duzo miejsca
  // i tak jest zastepowane przez boot.gif
};


#endif
#endif
