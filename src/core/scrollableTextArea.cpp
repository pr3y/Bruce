#include "scrollableTextArea.h"

ScrollableTextArea::ScrollableTextArea(const String& title) :
    _startLine{0},
    _redraw{true},
    _title(title),
    _fontSize(FP),
    _startX(BORDER_PAD_X),
    _startY(BORDER_PAD_Y),
    _width(WIDTH - 2*BORDER_PAD_X),
    _height(HEIGHT - BORDER_PAD_X - BORDER_PAD_Y)
#ifdef HAS_SCREEN
    ,_scrollBuffer(&tft)
#endif
{
    drawMainBorder();

    if (!_title.isEmpty()) {
        printTitle(_title);
        _startY = tft.getCursorY();
        _height -= (_startY - BORDER_PAD_Y);
    }

    setup();
}

ScrollableTextArea::ScrollableTextArea(
    uint8_t fontSize, int16_t startX, int16_t startY, int32_t width, int32_t height
) : _startLine{0},
    _redraw{true},
    _title(""),
    _fontSize(fontSize),
    _startX(startX),
    _startY(startY),
    _width(width),
    _height(height)
#ifdef HAS_SCREEN
    ,_scrollBuffer(&tft)
#endif
{
    drawMainBorder();
    setup();
}

ScrollableTextArea::~ScrollableTextArea() {
#ifdef HAS_SCREEN
    _scrollBuffer.deleteSprite();
#endif
}

void ScrollableTextArea::setup() {
#ifdef HAS_SCREEN
    _scrollBuffer.createSprite(_width, _height);
    _scrollBuffer.setTextColor(bruceConfig.priColor);
    _scrollBuffer.setTextSize(_fontSize);
    _scrollBuffer.fillSprite(bruceConfig.bgColor);

    _maxCharsInLine = floor(_width / _scrollBuffer.textWidth("w", _fontSize));
    _pxlsPerLine = _scrollBuffer.fontHeight() + 2;
    _maxLinesInArea = floor(_height / _pxlsPerLine);
#endif
}

void ScrollableTextArea::scrollUp() {
    if (_startLine) {
        _startLine--;
        _redraw = true;
    }
}

void ScrollableTextArea::scrollDown() {
    if (_startLine + _maxLinesInArea <= _lines.size()) {
        if (_startLine == 0) _startLine++;
        _startLine++;
        _redraw = true;
    }
}

void ScrollableTextArea::show(bool force) {
    draw(force);

    delay(100);

    while(checkSelPress())  { update(force); yield(); }
    while(!checkSelPress()) { update(force); yield(); }
}

void ScrollableTextArea::update(bool force) {
    if (checkPrevPress()) scrollUp();
    else if (checkNextPress()) scrollDown();

    draw(force);
}

void ScrollableTextArea::fromFile(File file) {
    while (file.available()) addLine(file.readStringUntil('\n'));

    draw(true);
    delay(100);
    draw(true);
}

void ScrollableTextArea::fromString(const String& text) {
    int startIdx = 0;
    int endIdx = 0;

    while (endIdx < text.length()) {
        if (text[endIdx] == '\n') {
            addLine(text.substring(startIdx, endIdx));
            startIdx = endIdx + 1;
        }

        endIdx++;
    }
}

// for devices it will act as a scrollable text area
#ifdef HAS_SCREEN
void ScrollableTextArea::addLine(const String& text) {
    if (text.isEmpty()) {
        _lines.emplace_back("");
        return;
    }

    String buff;
    size_t start{0};

    // automatically split into multiple lines
    while( !(buff = text.substring(start, start + _maxCharsInLine)).isEmpty() ){
        _lines.emplace_back(buff);
        start += buff.length();
    }

    _redraw = true;
}

void ScrollableTextArea::draw(bool force) {
    if (!_redraw && !force) return;

    _scrollBuffer.fillSprite(bruceConfig.bgColor);

    uint16_t yOffset = 0;
    uint16_t lines = 0;

    // if there is text above
    if (_startLine) {
        _scrollBuffer.drawString("...", 0, yOffset);
        yOffset += _pxlsPerLine;
        lines++;
    }

    int32_t tmpHeight = _height;
    // if there is text below
    if (_lines.size() - _startLine >= _maxLinesInArea) {
        _scrollBuffer.drawString("...", 0, _height - _pxlsPerLine);
        tmpHeight -= _pxlsPerLine;
        lines++;
    }

    size_t idx{_startLine};
    while( yOffset < tmpHeight && lines < _maxLinesInArea && idx < _lines.size() ){
        _scrollBuffer.drawString(_lines[idx], 0, yOffset);
        yOffset += _pxlsPerLine;
        lines++;
        idx++;
    }

    _scrollBuffer.pushSprite(_startX, _startY);
    _redraw = false;
}

// for webui as a regular text area
#else
void ScrollableTextArea::addLine(const String& text){
    if( !text.isEmpty() ) _lines.emplace_back(text);
}

void ScrollableTextArea::draw(bool force){
    uint16_t yOffset = 0;
    for( const auto& str : _lines ){
        _scrollBuffer.drawString(str, 0, yOffset);
        yOffset += 12;
    }
}
#endif