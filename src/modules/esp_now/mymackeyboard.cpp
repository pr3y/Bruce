//
// File: src/modules/esp_now/mymackeyboard.cpp
//

#include "mymackeyboard.h"
#include "core/powerSave.h"
#include "core/sd_functions.h"
#include "core/wifi/webInterface.h"
#include "modules/badusb_ble/ducky_typer.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/rf/rf_send.h"

#if defined(HAS_TOUCH)
struct box_t {
    int x;
    int y;
    int w;
    int h;
    std::uint16_t color;
    int touch_id = -1;
    char key;
    char key_sh;

    void clear(void) {
        for (int i = 0; i < 8; ++i) { tft.fillRect(x, y, w, h, bruceConfig.bgColor); }
    }
    void draw(void) {
        int ie = touch_id < 0 ? 4 : 8;
        for (int i = 0; i < ie; ++i) {
            tft.drawRect(x, y, w, h, color);
            tft.setTextColor(color);
            tft.drawChar(key, x + w / 2 - FM * LW / 2, y + h / 2 - FM * LH / 2);
        }
    }
    bool contain(int x, int y) {
        return this->x <= x && x < (this->x + this->w) && this->y <= y && y < (this->y + this->h);
    }
};

static constexpr std::size_t box_count = 52;
static box_t box_list[box_count];
#endif

keyStroke Mac_getKeyPress() {
#ifndef USE_TFT_eSPI_TOUCH
    vTaskSuspend(xHandle);
    keyStroke key = KeyStroke;
    KeyStroke.Clear();
    delay(10);
    vTaskResume(xHandle);
    return key;
#else
    keyStroke key = KeyStroke;
    KeyStroke.Clear();
    return key;
#endif
}

bool Mac_checkNextPagePress() {
    if (check(NextPagePress)) return true;
    else return false;
}

bool Mac_checkPrevPagePress() {
    if (check(PrevPagePress)) return true;
    else return false;
}

void Mac_checkShortcutPress() {
    keyStroke key = Mac_getKeyPress();
    if (key.pressed) {
        for (auto i : key.word) {
            if (i == 'i') {
                otherIRcodes();
                returnToMenu = true;
            }
            if (i == 'r' || i == 's') {
                sendCustomRF();
                returnToMenu = true;
            }
            if (i == 'b') {
                ducky_setup(hid_usb, false);
                returnToMenu = true;
            }
            if (i == 'w') {
                loopOptionsWebUi();
                returnToMenu = true;
            }
            if (i == 'f') {
                setupSdCard() ? loopSD(SD) : loopSD(LittleFS);
                returnToMenu = true;
            }
            if (i == 'l') {
                loopSD(LittleFS);
                returnToMenu = true;
            }
        }
    }
}

int Mac_checkNumberShortcutPress() {
    keyStroke key = Mac_getKeyPress();
    for (auto i : key.word) {
        char c;
        for (c = '1'; c <= '9'; c++) {
            if (i == c) return (c - '1');
        }
    }
    return -1;
}

char Mac_checkLetterShortcutPress() {
    keyStroke key = Mac_getKeyPress();
    for (auto i : key.word) {
        char c;
        for (c = 'a'; c <= 'z'; c++) {
            if (i == c) return (c);
        }
        for (c = 'A'; c <= 'Z'; c++) {
            if (i == c) return (c);
        }
    }
    return -1;
}

String MacKeyboard(String mytext, int maxSize, String msg) {
    resetTftDisplay();
    touchPoint.Clear();
    String _mytext = mytext;

    const int maxFMSize = tftWidth / (LW * FM) - 1;
    const int maxFPSize = tftWidth / (LW)-2;
    bool caps = false;
    bool redraw = true;
    long holdCode = millis();

    int x = 0;
    int y = -1;
    int z = 0;
    int x2 = 0;
    int y2 = 0;

    char keys[2][9][2] = {
        {{'0', '0'},
         {'1', '1'},
         {'2', '2'},
         {'3', '3'},
         {'4', '4'},
         {'5', '5'},
         {'6', '6'},
         {'7', '7'},
         {':', ':'}},
        {{'A', 'A'},
         {'B', 'B'},
         {'C', 'C'},
         {'D', 'D'},
         {'E', 'E'},
         {'F', 'F'},
         {'8', '8'},
         {'9', '9'},
         {':', ':'}}
    };

#if FM > 1
#define KBLH 20
    int ofs[4][3] = {
        {7,   46, 18 },
        {55,  50, 64 },
        {107, 50, 115},
        {159, 74, 168}
    };
#else
#define KBLH 10
    int ofs[4][3] = {
        {7,  20, 10},
        {27, 25, 30},
        {52, 25, 55},
        {77, 50, 80}
    };
#endif

    const int _x = tftWidth / 12;
    const int _y = (tftHeight - (2 * KBLH + 14)) / 4;
    const int _xo = _x / 2 - 3;

#if defined(HAS_TOUCH)
    int k = 0;
    for (x2 = 0; x2 < 12; x2++) {
        for (y2 = 0; y2 < 4; y2++) {
            box_list[k].key = keys[y2][x2][0];
            box_list[k].key_sh = keys[y2][x2][1];
            box_list[k].color = ~bruceConfig.bgColor;
            box_list[k].x = x2 * _x;
            box_list[k].y = y2 * _y + 54;
            box_list[k].w = _x;
            box_list[k].h = _y;
            k++;
        }
    }
    box_list[48].key = ' ';
    box_list[48].key_sh = ' ';
    box_list[48].color = ~bruceConfig.bgColor;
    box_list[48].x = 0;
    box_list[48].y = 0;
    box_list[48].w = 53;
    box_list[48].h = 22;
    k++;
    box_list[49].key = ' ';
    box_list[49].key_sh = ' ';
    box_list[49].color = ~bruceConfig.bgColor;
    box_list[49].x = 55;
    box_list[49].y = 0;
    box_list[49].w = 50;
    box_list[49].h = 22;
    k++;
    box_list[50].key = ' ';
    box_list[50].key_sh = ' ';
    box_list[50].color = ~bruceConfig.bgColor;
    box_list[50].x = 107;
    box_list[50].y = 0;
    box_list[50].w = 50;
    box_list[50].h = 22;
    k++;
    box_list[51].key = ' ';
    box_list[51].key_sh = ' ';
    box_list[51].color = ~bruceConfig.bgColor;
    box_list[51].x = 159;
    box_list[51].y = 0;
    box_list[51].w = tftWidth - 164;
    box_list[51].h = 22;

    k = 0;
    x2 = 0;
    y2 = 0;
#endif

    tft.fillScreen(bruceConfig.bgColor);

#if defined(HAS_3_BUTTONS)
    uint8_t longNextPress = 0;
    uint8_t longPrevPress = 0;
    unsigned long LongPressTmp = millis();
#endif

    const unsigned long startTime = millis();

    while (true) {
        if (millis() - startTime > 10000) { return String(""); }

        if (redraw) {
            tft.setCursor(0, 0);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            if (y < 0 || y2 < 0) {
                tft.fillRect(0, 1, tftWidth, 22, bruceConfig.bgColor);
                tft.drawRect(ofs[0][0], 2, ofs[0][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                tft.drawRect(ofs[1][0], 2, ofs[1][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                tft.drawRect(ofs[2][0], 2, ofs[2][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                tft.drawRect(ofs[3][0], 2, ofs[3][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));

                if (x == 0 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[0][0], 2, ofs[0][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else {
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                }
                tft.drawString("OK", ofs[0][2], 4);

                if (x == 1 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[1][0], 2, ofs[1][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else if (caps) {
                    tft.fillRect(ofs[1][0], 2, ofs[1][1], KBLH, TFT_DARKGREY);
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), TFT_DARKGREY);
                } else {
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                }
                tft.drawString("CAP", ofs[1][2], 4);

                if (x == 2 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[2][0], 2, ofs[2][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else {
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                }
                tft.drawString("DEL", ofs[2][2], 4);

                if (x > 2 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[3][0], 2, ofs[3][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else {
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                }
                tft.drawString("SPACE", ofs[3][2], 4);
            }

            tft.setTextSize(FP);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
            tft.drawString(msg.substring(0, maxFPSize), 3, KBLH + 4);

            tft.setTextSize(FM);
            if (_mytext.length() == maxFMSize || _mytext.length() == (maxFMSize + 1) ||
                _mytext.length() == maxFPSize || _mytext.length() == (maxFPSize + 1)) {
                tft.fillRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.bgColor);
            }

            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor));
            if (_mytext.length() > maxFMSize) {
                tft.setTextSize(FP);
                if (_mytext.length() > maxFPSize) {
                    tft.drawString(_mytext.substring(0, maxFPSize), 5, KBLH + LH + 6);
                    tft.drawString(_mytext.substring(maxFPSize, _mytext.length()), 5, KBLH + 2 * LH + 6);
                } else {
                    tft.drawString(_mytext, 5, KBLH + 14);
                }
            } else {
                tft.drawString(_mytext, 5, KBLH + 14);
            }

            tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor);

            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 12; j++) {
                    if (x2 == j && y2 == i) {
                        tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor);
                        tft.fillRect(j * _x, i * _y + KBLH * 2 + 14, _x, _y, bruceConfig.bgColor);
                    }
                    if (x == j && y == i) {
                        tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor);
                        tft.fillRect(j * _x, i * _y + KBLH * 2 + 14, _x, _y, ~bruceConfig.bgColor);
                    }
                    if (!caps) {
                        tft.drawChar(keys[i][j][0], (j * _x + _xo), (i * _y + KBLH * 2 + 16));
                    } else {
                        tft.drawChar(keys[i][j][1], (j * _x + _xo), (i * _y + KBLH * 2 + 16));
                    }
                    if (x == j && y == i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
                }
            }
            x2 = x;
            y2 = y;
            redraw = false;
        }

        if (millis() - holdCode > 250) {
#if defined(HAS_TOUCH)
#if defined(USE_TFT_eSPI_TOUCH)
            check(AnyKeyPress);
#endif
            if (touchPoint.pressed) {
                SelPress = false;
                EscPress = false;
                NextPress = false;
                PrevPress = false;
                UpPress = false;
                DownPress = false;
                x = 0;
                y = -1;

                if (box_list[48].contain(touchPoint.x, touchPoint.y)) { break; }
                if (box_list[49].contain(touchPoint.x, touchPoint.y)) {
                    caps = !caps;
                    tft.fillRect(0, 54, tftWidth, tftHeight - 54, bruceConfig.bgColor);
                    goto THIS_END;
                }
                if (box_list[50].contain(touchPoint.x, touchPoint.y)) goto DEL;
                if (box_list[51].contain(touchPoint.x, touchPoint.y)) {
                    if (_mytext.length() < maxSize) _mytext += box_list[51].key;
                    goto THIS_END;
                }
                for (k = 0; k < 48; k++) {
                    if (box_list[k].contain(touchPoint.x, touchPoint.y)) {
                        if (caps) _mytext += box_list[k].key_sh;
                        else _mytext += box_list[k].key;
                    }
                }
                wakeUpScreen();
            THIS_END:
                touchPoint.Clear();
                redraw = true;
            }
#endif

#if defined(HAS_3_BUTTONS)
            if (Mac_checkNextPagePress() || NextPress) {
                unsigned long now = millis();
                static uint8_t longNextPress = 0;
                static unsigned long LongPressTmp = 0;

                if (!longNextPress) {
                    longNextPress = 1;
                    LongPressTmp = now;
                }
                delay(1);
                if (now - LongPressTmp > 300) {
                    x--;
                    longNextPress = 2;
                    LongPressTmp = now;
                } else if (!NextPress) {
                    if (longNextPress != 2) x++;
                    longNextPress = 0;
                } else {
                    goto WAITING;
                }
                if (y < 0 && x > 3) x = 0;
                if (x > 11) x = 0;
                else if (x < 0) x = 11;
                redraw = true;
            }

            if (Mac_checkPrevPagePress() || PrevPress) {
                unsigned long now = millis();
                static uint8_t longPrevPress = 0;
                static unsigned long LongPressTmp = 0;

                if (!longPrevPress) {
                    longPrevPress = 1;
                    LongPressTmp = now;
                }
                delay(1);
                if (now - LongPressTmp > 300) {
                    y--;
                    longPrevPress = 2;
                    LongPressTmp = now;
                } else if (!PrevPress) {
                    if (longPrevPress != 2) y++;
                    longPrevPress = 0;
                } else {
                    goto WAITING;
                }
                if (y > 3) {
                    y = -1;
                } else if (y < -1) {
                    y = 3;
                }
                redraw = true;
            }
#elif defined(HAS_5_BUTTONS)
            if (check(SelPress)) goto SELECT;
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x > 3) || x > 11) x = 0;
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x > 3) x = 3;
                else if (x < 0) x = 11;
                redraw = true;
            }
            if (check(DownPress)) {
                y++;
                if (y > 3) y = -1;
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = 3;
                redraw = true;
            }
#elif defined(HAS_ENCODER)
            if (check(SelPress)) goto SELECT;
            if (check(NextPress)) {
                if (check(EscPress)) {
                    y++;
                } else if ((x >= 3 && y < 0) || x == 11) {
                    y++;
                    x = 0;
                } else {
                    x++;
                }
                if (y > 3) y = -1;
                if (y == -1 && x > 3) x = 0;
                redraw = true;
            }
            if (check(PrevPress)) {
                if (check(EscPress)) {
                    y--;
                    if (y == -1 && x > 3) x = 3;
                } else if (x == 0) {
                    y--;
                    x--;
                } else {
                    x--;
                }
                if (y < -1) {
                    y = 3;
                    x = 11;
                } else if (y < 0 && x < 0) x = 3;
                else if (x < 0) x = 11;
                redraw = true;
            }
#elif defined(HAS_KEYBOARD)
            if (KeyStroke.pressed) {
                wakeUpScreen();
                if (KeyStroke.enter) { break; }
                KeyStroke.Clear();
            }
            if (check(SelPress)) { break; }
#endif
        }

#if false
    SELECT:
      tft.setCursor(cX, cY);
      if (caps) z = 1;
      else     z = 0;
      if (x == 0 && y == -1) break;
      else if (x == 1 && y == -1) caps = !caps;
      else if (x == 2 && y == -1 && _mytext.length() > 0) {
    DEL:
        _mytext.remove(_mytext.length() - 1);
        int fS = FM;
        if (_mytext.length() > maxFPSize) {
          tft.setTextSize(FP);
          fS = FP;
        } else {
          tft.setTextSize(FM);
        }
        tft.setCursor((cX - fS * LW), cY);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.print(" ");
        tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
        tft.setCursor(cX - fS * LW, cY);
        cX = tft.getCursorX();
        cY = tft.getCursorY();
      } else if (x > 2 && y == -1 && _mytext.length() < maxSize) {
        _mytext += " ";
      } else if (y > -1 && _mytext.length() < maxSize) {
    ADD:
        _mytext += keys[y][x][z];
        if (_mytext.length() != (maxFMSize + 1) && _mytext.length() != (maxFMSize + 1)) {
          tft.print(keys[y][x][z]);
        }
        cX = tft.getCursorX();
        cY = tft.getCursorY();
        if (_mytext.length() >= maxSize) {
          x = 0; y = -1;
        }
      }
      redraw   = true;
      holdCode = millis();
#endif

    WAITING:
        yield();
    }

    tft.fillScreen(bruceConfig.bgColor);
    resetTftDisplay();
    return _mytext;
}
