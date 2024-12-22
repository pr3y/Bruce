#include "core/display.h"

class ScrollableTextArea {
public:
    ScrollableTextArea(const String& title = "");

    ScrollableTextArea(
        uint8_t fontSize,
        int16_t startX,
        int16_t startY,
        int32_t width,
        int32_t height
    );

    ~ScrollableTextArea();

    void scrollUp();

    void scrollDown();

    void addLine(const String& text);

    void fromString(const String& text);

    void fromFile(File file);

    void draw(bool force = false);

    void show(bool force = false);

private:
    String _title;
    uint16_t _startLine;
    bool _redraw;
    uint8_t _fontSize;
    int16_t _startX, _startY;
    int32_t _width, _height;
    int32_t _pxlsPerLine;
    int32_t _maxLinesInArea;
    uint16_t _maxCharsInLine;
    std::vector<String> _lines;

    #if defined(HAS_SCREEN)
        TFT_eSprite _scrollBuffer;
    #else
        SerialDisplayClass& _scrollBuffer = tft;
    #endif

    void setup();

    void update(bool force = false);

};