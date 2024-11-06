#include "scrollableTextArea.h"

// god, it's ugly
ScrollableTextArea::ScrollableTextArea(uint8_t fontSize,
                                        int16_t startX,
                                        int16_t startY,
                                        int32_t width,
                                        int32_t height)
        : _startLine{0},
        _redraw{true},
        _fontSize(fontSize),
        _startX(startX),
        _startY(startY),
        _width(width),
        _height(height)
        #if defined(HAS_SCREEN)
            ,_scrollBuffer(&tft)
        #endif
{
    #if defined(HAS_SCREEN)
        _scrollBuffer.createSprite(_width, _height);
        _scrollBuffer.setTextColor(bruceConfig.priColor);
        _scrollBuffer.setTextSize(_fontSize);
        _scrollBuffer.fillSprite(TFT_BLACK);

        _maxCharsInLine = floor(width / _scrollBuffer.textWidth("w", _fontSize));
        _pxlsPerLine = _scrollBuffer.fontHeight() + 2;
        _maxLinesInArea = floor(_height / _pxlsPerLine);
    #endif
}

ScrollableTextArea::~ScrollableTextArea(){
    #if defined(HAS_SCREEN)
        _scrollBuffer.deleteSprite();
    #endif
}

void ScrollableTextArea::scrollUp() {
    if( _startLine ){
        --_startLine;
        _redraw = true;
    }
}

void ScrollableTextArea::scrollDown() {
    if (_startLine + _maxLinesInArea < _lines.size()) {
        ++_startLine;
        _redraw = true;
    }
}

// for devices it will act as a scrollable text area
#if defined(HAS_SCREEN)
void ScrollableTextArea::addLine(const String& text) {
    if( text.isEmpty() ) return;

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
    if( !_redraw && !force ) return;

    _scrollBuffer.fillSprite(TFT_BLACK);

    uint16_t yOffset = 0;
    uint16_t lines = 0;

    // if there is text above
    if( _startLine ){
        _scrollBuffer.drawString("...", 0, yOffset);
        yOffset += _pxlsPerLine;
        ++lines;
    }

    int32_t tmpHeight = _height;
    // if there is text below
    if( _lines.size() - _startLine > _maxLinesInArea ){
        _scrollBuffer.drawString("...", 0, _height - _pxlsPerLine);
        tmpHeight -= _pxlsPerLine;
        ++lines;
    }

    size_t idx{_startLine};
    while( yOffset < tmpHeight && lines < _maxLinesInArea && idx < _lines.size() ){
        _scrollBuffer.drawString(_lines[idx], 0, yOffset);
        yOffset += _pxlsPerLine;
        ++lines;
        ++idx;
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