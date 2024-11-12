#include "core/display.h"

class ScrollableTextArea {
public:
    ScrollableTextArea(
        uint8_t fontSize, 
        int16_t startX, 
        int16_t startY, 
        int32_t width, 
        int32_t height);

    ~ScrollableTextArea();

    void scrollUp();

    void scrollDown();

    void addLine(const String& text);

    void draw(bool force = false);
private:
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

};