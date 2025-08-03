#include "mykeyboard.h"
#include "core/wifi/webInterface.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/rf/rf_send.h"
#include "powerSave.h"
#include "sd_functions.h"
#include <ArduinoJson.h>

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
            tft.drawString(String(key), x + w / 2 - FM * LW / 2, y + h / 2 - FM * LH / 2);
        }
    }
    bool contain(int x, int y) {
        return this->x <= x && x < (this->x + this->w) && this->y <= y && y < (this->y + this->h);
    }
};

static constexpr std::size_t box_count = 52;
static box_t box_list[box_count];

#endif

// This will get the value from InputHandler and read add into loopTask,
// reseting the value after used
keyStroke _getKeyPress() {
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
} // must return something that the keyboards won´t recognize by default

/*********************************************************************
** Function: checkShortcutPress
** location: mykeyboard.cpp
** runs a function called by the shortcut action
**********************************************************************/
void checkShortcutPress() {
    static StaticJsonDocument<512> shortcutsJson; // parsed only once

    // lazy init
    if (shortcutsJson.size() == 0) {
        FS *fs;
        if (!getFsStorage(fs)) return;
        File file = fs->open("/shortcuts.json", FILE_READ);
        if (!file) {
            log_e("Shortcuts Config file not found. Using default values");
            JsonObject shortcuts = shortcutsJson.to<JsonObject>(); // root
            shortcuts["i"] = "loader open ir";
            shortcuts["r"] = "loader open rf";
            shortcuts["s"] = "loader open rf";
            shortcuts["b"] = "loader open badusb";
            shortcuts["w"] = "loader open webui";
            shortcuts["f"] = "loader open files";
            return;
        }
        // else
        if (deserializeJson(shortcutsJson, file)) {
            log_e("Failed to parse shortcuts.json");
            file.close();
            return;
        }
        file.close();
    }

    keyStroke key = _getKeyPress();

    // parse shortcutsJson and check the keys
    for (JsonPair kv : shortcutsJson.as<JsonObject>()) {
        const char *shortcut_key = kv.key().c_str();
        const char *shortcut_value = kv.value().as<const char *>();

        // check for matching keys
        for (auto i : key.word) {
            if (i == *shortcut_key) { // compare the 1st char of the key string
                // execute the associated action
                serialCli.parse(String(shortcut_value));
            }
        }
    }
}

/*********************************************************************
** Function: checkNumberShortcutPress
** location: mykeyboard.cpp
** return the number pressed
**********************************************************************/
int checkNumberShortcutPress() {
    // shortctus to quickly select options
    keyStroke key = _getKeyPress();
    for (auto i : key.word) {
        char c;
        for (c = '1'; c <= '9'; c++)
            if (i == c) return (c - '1');
    }
    // else
    return -1;
}

/*********************************************************************
** Function: checkLetterShortcutPress
** location: mykeyboard.cpp
** return the letter pressed
**********************************************************************/
char checkLetterShortcutPress() {
    // shortctus to quickly select options
    keyStroke key = _getKeyPress();
    for (auto i : key.word) {
        char c;
        for (c = 'a'; c <= 'z'; c++)
            if (i == c) return (c);
        for (c = 'A'; c <= 'Z'; c++)
            if (i == c) return (c);
    }
    // else
    return -1;
}

/*********************************************************************
** Shared keyboard helper functions
**********************************************************************/

// Handles character deletion from the text string and screen
bool handleDelete(String &mytext, int &cX, int &cY, const int maxFPSize) {
    if (mytext.length() == 0) return false;

    // remove from string
    mytext.remove(mytext.length() - 1);
    // delete from screen:
    int fontSize = FM;
    if (mytext.length() > maxFPSize) {
        tft.setTextSize(FP);
        fontSize = FP;
    } else tft.setTextSize(FM);
    tft.setCursor((cX - fontSize * LW), cY);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.print(" ");
    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
    tft.setCursor(cX - fontSize * LW, cY);
    cX = tft.getCursorX();
    cY = tft.getCursorY();
    return true;
}

// Handles adding a character to the text string
bool handleCharacterAdd(
    String &mytext, char character, int &cX, int &cY, const int maxSize, const int maxFMSize
) {
    if (mytext.length() >= maxSize) return false;

    mytext += character;
    if (mytext.length() != (maxFMSize + 1) && mytext.length() != (maxFMSize + 1)) tft.print(character);
    cX = tft.getCursorX();
    cY = tft.getCursorY();
    return true;
}

// Handles adding space to the text string
bool handleSpaceAdd(String &mytext, const int maxSize) {
    if (mytext.length() >= maxSize) return false;
    mytext += " ";
    return true;
}

// Enum for keyboard action results
enum KeyboardAction { KEYBOARD_CONTINUE, KEYBOARD_OK, KEYBOARD_CANCEL, KEYBOARD_REDRAW };

// Handles keyboard selection logic for regular keyboard
KeyboardAction handleRegularKeyboardSelection(
    int x, int y, String &mytext, bool &caps, int &cX, int &cY, const int maxSize, const int maxFMSize,
    const int maxFPSize, char keys[4][12][2]
) {
    tft.setCursor(cX, cY);
    int z = caps ? 1 : 0;

    if (x == 0 && y == -1) {
        return KEYBOARD_OK; // OK button
    } else if (x == 1 && y == -1) {
        caps = !caps; // CAP button
        return KEYBOARD_REDRAW;
    } else if (x == 2 && y == -1 && mytext.length() > 0) {
        handleDelete(mytext, cX, cY, maxFPSize); // DEL button
        return KEYBOARD_REDRAW;
    } else if (x == 3 && y == -1 && mytext.length() < maxSize) {
        handleSpaceAdd(mytext, maxSize); // SPACE button
        return KEYBOARD_REDRAW;
    } else if (x == 4 && y == -1) {
        mytext = ""; // BACK button - return empty string to cancel
        return KEYBOARD_CANCEL;
    } else if (y > -1 && mytext.length() < maxSize) {
        handleCharacterAdd(mytext, keys[y][x][z], cX, cY, maxSize, maxFMSize); // add letters to mytext
        if (mytext.length() >= maxSize) {
            // Put the Cursor at "Ok" when max size reached
            // This would need to be handled by the caller
        }
        return KEYBOARD_REDRAW;
    }
    return KEYBOARD_CONTINUE;
}

// Handles keyboard selection logic for hex keyboard
KeyboardAction handleHexKeyboardSelection(
    int x, int y, String &mytext, int &cX, int &cY, const int maxSize, const int maxFMSize,
    const int maxFPSize, char keys[4][4]
) {
    tft.setCursor(cX, cY);

    if (x == 0 && y == -1) {
        return KEYBOARD_OK; // OK button
    } else if (x == 1 && y == -1 && mytext.length() > 0) {
        handleDelete(mytext, cX, cY, maxFPSize); // DEL button
        return KEYBOARD_REDRAW;
    } else if (x == 2 && y == -1 && mytext.length() < maxSize) {
        handleSpaceAdd(mytext, maxSize); // SPACE button
        return KEYBOARD_REDRAW;
    } else if (x == 3 && y == -1) {
        mytext = "\x1B"; // BACK button - return ESC to cancel
        return KEYBOARD_CANCEL;
    } else if (y > -1 && mytext.length() < maxSize) {
        handleCharacterAdd(mytext, keys[y][x], cX, cY, maxSize, maxFMSize); // add hex letters to mytext
        if (mytext.length() >= maxSize) {
            // Put the Cursor at "Ok" when max size reached
            // This would need to be handled by the caller
        }
        return KEYBOARD_REDRAW;
    }
    return KEYBOARD_CONTINUE;
}

/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** keyboard interface.
**********************************************************************/
String keyboard(String mytext, int maxSize, String msg) {
    resetTftDisplay();
    touchPoint.Clear();
    String _mytext = mytext;
    const uint8_t max_chars = tftWidth / (LW * FM);
    const int maxFMSize = tftWidth / (LW * FM) - 1;
    const int maxFPSize = tftWidth / (LW)-2;
    bool caps = false;
    bool redraw = true;
    long holdCode = millis(); // to hold the inputs for 250ms before adding other letter
    int cX = 0;               // Cursor position
    int cY = 0;               // Cursor position
    int x = 0;
    int y = -1; // -1 is where buttons are, out of keys[][][] array
    int z = 0;
    int x2 = 0;
    int y2 = 0;
    //       [x][y] [z], x2 and y2 are the previous position of x and y, used to redraw only that spot on
    //       keyboard screen
    char keys[4][12][2] = {
        //  4 lines, with 12 characteres, low and high caps
        {
         {'1', '!'},  // 1
            {'2', '@'},  // 2
            {'3', '#'},  // 3
            {'4', '$'},  // 4
            {'5', '%'},  // 5
            {'6', '^'},  // 6
            {'7', '&'},  // 7
            {'8', '*'},  // 8
            {'9', '('},  // 9
            {'0', ')'},  // 10
            {'-', '_'},  // 11
            {'=', '+'}  // 12
        },
        {
         {'q', 'Q'},  // 1
            {'w', 'W'},  // 2
            {'e', 'E'},  // 3
            {'r', 'R'},  // 4
            {'t', 'T'},  // 5
            {'y', 'Y'},  // 6
            {'u', 'U'},  // 7
            {'i', 'I'},  // 8
            {'o', 'O'},  // 9
            {'p', 'P'},  // 10
            {'[', '{'},  // 11
            {']', '}'}  // 12
        },
        {
         {'a', 'A'},  // 1
            {'s', 'S'},  // 2
            {'d', 'D'},  // 3
            {'f', 'F'},  // 4
            {'g', 'G'},  // 5
            {'h', 'H'},  // 6
            {'j', 'J'},  // 7
            {'k', 'K'},  // 8
            {'l', 'L'},  // 9
            {';', ':'},  // 10
            {'"', '\''}, // 11
            {'|', '\\'}  // 12
        },
        {
         {'\\', '|'}, // 1
            {'z', 'Z'}, // 2
            {'x', 'X'}, // 3
            {'c', 'C'}, // 4
            {'v', 'V'}, // 5
            {'b', 'B'}, // 6
            {'n', 'N'}, // 7
            {'m', 'M'}, // 8
            {',', '<'}, // 9
            {'.', '>'}, // 10
            {'?', '/'}, // 11
            {'/', '/'}   // 12
        }
    };
#if FM > 1      // Normal keyboard size
#define KBLH 20 // Keyboard Buttons Line Height
    // { x coord of btn border, btn width, x coord of the inside text }
    int ofs[5][3] = {
        {7,   46, 18 }, // OK button
        {55,  50, 64 }, // CAP button
        {107, 50, 115}, // DEL button
        {159, 74, 168}, // SPACE button
        {235, 62, 244}, // BACK button
    };
#else // small keyboard size, for small letters (smaller screen, like Marauder Mini and others ;) )
#define KBLH 10
    int ofs[5][3] = {
        {7,   20, 10 }, // OK button
        {27,  25, 30 }, // CAP button
        {52,  25, 55 }, // DEL button
        {77,  50, 80 }, // SPACE button
        {127, 40, 130}, // BACK button
    };
#endif
    const int _x = tftWidth / 12;
    const int _y = (tftHeight - (2 * KBLH + 14)) / 4;
    const int _xo = _x / 2 - 3;

#if defined(HAS_TOUCH) // filling touch box list
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
    // OK
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = 0;
    box_list[k].y = 0;
    box_list[k].w = 53;
    box_list[k].h = 22;
    k++;
    // CAP
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = 55;
    box_list[k].y = 0;
    box_list[k].w = 50;
    box_list[k].h = 22;
    k++;
    // DEL
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = 107;
    box_list[k].y = 0;
    box_list[k].w = 50;
    box_list[k].h = 22;
    k++;
    // SPACE
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = 159;
    box_list[k].y = 0;
    box_list[k].w = tftWidth - 164;
    box_list[k].h = 22;

    k = 0;
    x2 = 0;
    y2 = 0;
#endif

    tft.fillScreen(bruceConfig.bgColor);

#if defined(HAS_3_BUTTONS) // StickCs and Core for long press detection logic
    uint8_t longNextPress = 0;
    uint8_t longPrevPress = 0;
    unsigned long LongPressTmp = millis();
#endif

    while (1) {
        if (redraw) {
            tft.setCursor(0, 0);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            // Draw the top row buttons
            if (y < 0 || y2 < 0) {
                tft.fillRect(0, 1, tftWidth, 22, bruceConfig.bgColor);
                // Draw the borders
                tft.drawRect(
                    ofs[0][0], 2, ofs[0][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // OK btn border
                tft.drawRect(
                    ofs[1][0], 2, ofs[1][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // CAP btn border
                tft.drawRect(
                    ofs[2][0], 2, ofs[2][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // DEL btn border
                tft.drawRect(
                    ofs[3][0], 2, ofs[3][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // SPACE btn border
                tft.drawRect(
                    ofs[4][0], 2, ofs[4][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // BACK btn border

                tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor); // typed string border

                // Highlight the corresponding button when the user cursor is over it

                // OK
                if (x == 0 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[0][0], 2, ofs[0][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("OK", ofs[0][2], 4);

                // CAP
                if (x == 1 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[1][0], 2, ofs[1][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else if (caps) {
                    tft.fillRect(ofs[1][0], 2, ofs[1][1], KBLH, TFT_DARKGREY);
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), TFT_DARKGREY);
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("CAP", ofs[1][2], 4);

                // DEL
                if (x == 2 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[2][0], 2, ofs[2][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("DEL", ofs[2][2], 4);

                // SPACE
                if (x == 3 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[3][0], 2, ofs[3][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("SPACE", ofs[3][2], 4);

                // BACK
                if (x > 3 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[4][0], 2, ofs[4][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("BACK", ofs[4][2], 4);
            }

            // prints the title of the textbox, it should report what the user has to write in it
            tft.setTextSize(FP);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
            tft.drawString(msg.substring(0, maxFPSize), 3, KBLH + 4);

            // drawing the textbox and the currently typed string
            tft.setTextSize(FM);
            // reset the text box
            if (mytext.length() == (maxFMSize) || mytext.length() == (maxFMSize + 1) ||
                mytext.length() == (maxFPSize) || mytext.length() == (maxFPSize + 1))
                tft.fillRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.bgColor);
            // write the text
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor));
            if (mytext.length() > maxFMSize) { // if the text is too long, we try to set the smaller font
                tft.setTextSize(FP);
                if (mytext.length() > maxFPSize) { // if its still too long, we divide it into two lines
                    tft.drawString(mytext.substring(0, maxFPSize), 5, KBLH + LH + 6);
                    tft.drawString(mytext.substring(maxFPSize, mytext.length()), 5, KBLH + 2 * LH + 6);
                } else {
                    tft.drawString(mytext, 5, KBLH + 14);
                }
            } else {
                // else if it fits, just draw the text
                tft.drawString(mytext, 5, KBLH + 14);
            }
            // Draw the textbox border again(?)
            tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor); // typed string border

            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            // Draw the actual keyboard
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 12; j++) {
                    // Use the previous coordinates to repaint only the previous letter
                    if (x2 == j && y2 == i) {
                        tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor);
                        tft.fillRect(j * _x, i * _y + KBLH * 2 + 14, _x, _y, bruceConfig.bgColor);
                    }
                    // If selected, highlight it by changing font color and filling the back rectangle
                    if (x == j && y == i) {
                        tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor);
                        tft.fillRect(j * _x, i * _y + KBLH * 2 + 14, _x, _y, ~bruceConfig.bgColor);
                    }

                    // Print the letters
                    if (!caps)
                        tft.drawString(String(keys[i][j][0]), (j * _x + _xo), (i * _y + KBLH * 2 + 16));
                    else tft.drawString(String(keys[i][j][1]), (j * _x + _xo), (i * _y + KBLH * 2 + 16));

                    // Return colors to normal to print the other letters
                    if (x == j && y == i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
                }
            }
            // Save actual key coordinates
            x2 = x;
            y2 = y;
            redraw = false;
        }

        // Cursor Handler
        if (mytext.length() > maxFMSize) {
            tft.setTextSize(FP);
            if (mytext.length() > (maxFPSize)) {
                cY = KBLH + 2 * LH + 6;
                cX = 5 + (mytext.length() - maxFPSize) * LW;
            } else {
                cY = KBLH + LH + 6;
                cX = 5 + mytext.length() * LW;
            }
        } else {
            cY = KBLH + LH + 6;
            cX = 5 + mytext.length() * LW * FM;
        }

        if (millis() - holdCode > 250) { // allow reading inputs

#if defined(HAS_TOUCH) // CYD, Core2, CoreS3
#if defined(USE_TFT_eSPI_TOUCH)
            check(AnyKeyPress);
#endif
            if (touchPoint.pressed) {
                // If using Touchscreen and buttons, reset the navigation states to not
                // act weirdly, and put the cursor on Ok again.
                SelPress = false;
                EscPress = false;
                NextPress = false;
                PrevPress = false;
                UpPress = false;
                DownPress = false;
                x = 0;
                y = -1;

                bool touchHandled = false;

                if (box_list[48].contain(touchPoint.x, touchPoint.y)) {
                    break; // Ok
                }
                if (box_list[49].contain(touchPoint.x, touchPoint.y)) {
                    caps = !caps;
                    tft.fillRect(0, 54, tftWidth, tftHeight - 54, bruceConfig.bgColor);
                    touchHandled = true;
                }
                if (box_list[50].contain(touchPoint.x, touchPoint.y)) {
                    if (mytext.length() > 0) {
                        handleDelete(mytext, cX, cY, maxFPSize);
                        touchHandled = true;
                    }
                }
                if (box_list[51].contain(touchPoint.x, touchPoint.y)) {
                    if (mytext.length() < maxSize) {
                        handleSpaceAdd(mytext, maxSize);
                        touchHandled = true;
                    }
                }
                for (k = 0; k < 48; k++) {
                    if (box_list[k].contain(touchPoint.x, touchPoint.y)) {
                        if (caps) handleCharacterAdd(mytext, box_list[k].key_sh, cX, cY, maxSize, maxFMSize);
                        else handleCharacterAdd(mytext, box_list[k].key, cX, cY, maxSize, maxFMSize);
                        touchHandled = true;
                        break;
                    }
                }

                if (touchHandled) {
                    wakeUpScreen();
                    touchPoint.Clear();
                    redraw = true;
                }
            }
#endif

#if defined(HAS_3_BUTTONS) // StickCs and Core
            if (check(SelPress)) { goto SELECT; }
            /* Down Btn to move in X axis (to the right) */
            if (longNextPress || NextPress) {
                unsigned long now = millis();
                if (!longNextPress) {
                    longNextPress = 1;
                    LongPress = true;
                    LongPressTmp = now;
                }
                delay(1); // does not work without it
                // Check if the button is held long enough (long press)
                if (now - LongPressTmp > 300) {
                    x--; // Long press action
                    longNextPress = 2;
                    LongPress = false;
                    check(NextPress);
                    LongPressTmp = now;
                } else if (!NextPress) {
                    if (longNextPress != 2) x++; // Short press action
                    longNextPress = 0;
                } else {
                    goto WAITING;
                }
                LongPress = false;
                // delay(10);
                if (y < 0 && x > 4) x = 0; // Changed from 3 to 4 to include BACK button
                if (x > 11) x = 0;
                else if (x < 0) x = 11;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (longPrevPress || PrevPress) {
                unsigned long now = millis();
                if (!longPrevPress) {
                    longPrevPress = 1;
                    LongPress = true;
                    LongPressTmp = now;
                }
                delay(1); // does not work without it
                // Check if the button is held long enough (long press)
                if (now - LongPressTmp > 300) {
                    y--; // Long press action
                    longPrevPress = 2;
                    LongPress = false;
                    check(PrevPress);
                    LongPressTmp = now;
                } else if (!PrevPress) {
                    if (longPrevPress != 2) y++; // Short press action
                    longPrevPress = 0;
                } else {
                    goto WAITING;
                }
                LongPress = false;
                if (y > 3) {
                    y = -1;
                } else if (y < -1) y = 3;
                redraw = true;
            }
#elif defined(HAS_5_BUTTONS) // Smoochie and Marauder-Mini
            if (check(SelPress)) { goto SELECT; }
            /* Down Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x > 4) || x > 11) x = 0; // Changed from 3 to 4 to include BACK button
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x > 4) x = 4; // Changed from 3 to 4 to include BACK button
                else if (x < 0) x = 11;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(DownPress)) {
                y++;
                if (y > 3) { y = -1; }
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = 3;
                redraw = true;
            }

#elif defined(HAS_ENCODER) // T-Embed
            if (check(SelPress)) { goto SELECT; }
            /* DOWN Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                if (check(EscPress)) {
                    y++;
                } else if ((x >= 4 && y < 0) || x == 11) {
                    y++;
                    x = 0;
                } else x++;

                if (y > 3) y = -1;
                if (y == -1 && x > 4) x = 0;

                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(PrevPress)) {
                if (check(EscPress)) {
                    y--;
                    if (y == -1 && x > 4) x = 4;
                } else if (x == 0) {
                    y--;
                    x--;
                } else x--;

                if (y < -1) {
                    y = 3;
                    x = 11;
                } else if (y < 0 && x < 0) x = 4;
                else if (x < 0) x = 11;

                redraw = true;
            }

#elif defined(HAS_KEYBOARD) // Cardputer and T-Deck
            if (KeyStroke.pressed) {
                wakeUpScreen();
                tft.setCursor(cX, cY);
                String keyStr = "";
                for (auto i : KeyStroke.word) {
                    if (keyStr != "") {
                        keyStr = keyStr + "+" + i;
                    } else {
                        keyStr += i;
                    }
                }

                if (mytext.length() < maxSize && !KeyStroke.enter && !KeyStroke.del) {
                    mytext += keyStr;
                    if (mytext.length() != (maxFMSize + 1) && mytext.length() != (maxFMSize + 1))
                        tft.print(keyStr.c_str());
                    cX = tft.getCursorX();
                    cY = tft.getCursorY();
                    if (mytext.length() == (maxFMSize + 1)) redraw = true;
                    if (mytext.length() == (maxFPSize + 1)) redraw = true;
                }
                if (KeyStroke.del && mytext.length() > 0) { // delete 0x08
                    // Handle backspace key
                    mytext.remove(mytext.length() - 1);
                    int fontSize = FM;
                    if (mytext.length() > maxFPSize) {
                        tft.setTextSize(FP);
                        fontSize = FP;
                    } else tft.setTextSize(FM);
                    tft.setCursor((cX - fontSize * LW), cY);
                    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                    tft.print(" ");
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
                    tft.setCursor(cX - fontSize * LW, cY);
                    cX = tft.getCursorX();
                    cY = tft.getCursorY();
                    if (mytext.length() == maxFMSize) redraw = true;
                    if (mytext.length() == maxFPSize) redraw = true;
                }
                if (KeyStroke.enter) { break; }
                KeyStroke.Clear();
            }
            if (check(SelPress)) break;

#endif
        } // end of holdCode detection

        if (SerialCmdPress) { // only for Remote Control, if no input was detected on device
            if (check(SelPress)) { goto SELECT; }
            /* Down Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x > 4) || x > 11) x = 0; // Changed from 3 to 4 to include BACK button
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x > 4) x = 4; // Changed from 3 to 4 to include BACK button
                else if (x < 0) x = 11;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(DownPress)) {
                y++;
                if (y > 3) { y = -1; }
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = 3;
                redraw = true;
            }
        }

        if (false) { // When selecting some letter or something, use these helper functions instead of goto
        SELECT:      // so when we "select", so when we click something, this is executed:
            KeyboardAction action = handleRegularKeyboardSelection(
                x, y, mytext, caps, cX, cY, maxSize, maxFMSize, maxFPSize, keys
            );

            if (action == KEYBOARD_OK || action == KEYBOARD_CANCEL) {
                break;
            } else if (action == KEYBOARD_REDRAW) {
                redraw = true;
            }

            holdCode = millis();
        }
    WAITING: // Used in long press detection
        yield();
    }

    // Resets screen when finished writing
    tft.fillScreen(bruceConfig.bgColor);
    resetTftDisplay();

    return mytext;
}

/*********************************************************************
** Function: hex_keyboard
** location: mykeyboard.cpp
** keyboard interface with hex only characters.
**********************************************************************/
String hex_keyboard(String mytext, int maxSize, String msg) {
    resetTftDisplay();
    touchPoint.Clear();
    String _mytext = mytext;
    const uint8_t max_chars = tftWidth / (LW * FM); // Display Width / (Letter Width * Medium Font)
    const int maxFMSize = tftWidth / (LW * FM) - 1; //
    const int maxFPSize = tftWidth / (LW)-2;
    bool redraw = true;
    long holdCode = millis(); // to hold the inputs for 250ms before adding other letter
    int cX = 0;               // Cursor position
    int cY = 0;               // Cursor position
    int x = 0;
    int y = -1; // -1 is where buttons are, out of keys[][][] array
    int x2 = 0;
    int y2 = 0;
    //       [x][y], x2 and y2 are the previous position of x and y, used to redraw only that spot on
    //       keyboard screen
    char keys[4][4] = {
        //  2 lines, with 10 characters, high caps
        // {
        //  '0', '1',
        //  '2', '3',
        //  '4', '5',
        //  '6', '7',
        //  '8', '9',
        //  },
        // {
        //  'A', 'B',
        //  'C', 'D',
        //  'E', 'F',
        //  },
        //  4 lines, with 4 characters, high caps
        {'0', '1', '2', '3'},
        {'4', '5', '6', '7'},
        {'8', '9', 'A', 'B'},
        {'C', 'D', 'E', 'F'},
    };
    /****************TOP-BUTTONS****************/
#if FM > 1      // Normal keyboard size
#define KBLH 20 // Keyboard Buttons Line Height
    // { x coord of btn border, btn width, x coord of the inside text }
    int ofs[4][3] = {
        {7,   46, 18 }, // OK button
        {55,  50, 64 }, // DEL button
        {107, 74, 115}, // SPACE button
        {183, 62, 192}, // BACK button
    };
#else // small keyboard size, for small letters (smaller screen, like Marauder Mini and others ;) )
#define KBLH 10
    // { x coord of btn border, btn width, x coord of the inside text }
    int ofs[4][3] = {
        {7,   20, 10 }, // OK button
        {27,  25, 30 }, // DEL button
        {52,  50, 55 }, // SPACE button
        {102, 40, 105}, // BACK button
    };
#endif
    const int _x = tftWidth / 12;
    const int _y = (tftHeight - (2 * KBLH + 14)) / 4;
    const int _xo = _x / 2 - 3;

#if defined(HAS_TOUCH)
    int k = 0;
    // Setup touch boxes for the 4x4 hex keyboard
    for (x2 = 0; x2 < 4; x2++) {
        for (y2 = 0; y2 < 4; y2++) {
            box_list[k].key = keys[y2][x2];
            box_list[k].key_sh = keys[y2][x2]; // hex chars don't have shift variants
            box_list[k].color = ~bruceConfig.bgColor;
            box_list[k].x = x2 * (_x * 3); // spread out 4 keys across 12-key width
            box_list[k].y = y2 * _y + 54;
            box_list[k].w = _x * 3;
            box_list[k].h = _y;
            k++;
        }
    }
    // OK button (index 16)
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = ofs[0][0];
    box_list[k].y = 0;
    box_list[k].w = ofs[0][1];
    box_list[k].h = 22;
    k++;
    // DEL button (index 17)
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = ofs[1][0];
    box_list[k].y = 0;
    box_list[k].w = ofs[1][1];
    box_list[k].h = 22;
    k++;
    // SPACE button (index 18)
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = ofs[2][0];
    box_list[k].y = 0;
    box_list[k].w = ofs[2][1];
    box_list[k].h = 22;
    k++;
    // BACK button (index 19)
    box_list[k].key = ' ';
    box_list[k].key_sh = ' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x = ofs[3][0];
    box_list[k].y = 0;
    box_list[k].w = ofs[3][1];
    box_list[k].h = 22;

    k = 0;
    x2 = 0;
    y2 = 0;
#endif

    tft.fillScreen(bruceConfig.bgColor);

#if defined(HAS_3_BUTTONS) // StickCs and Core for long press detection logic
    uint8_t longNextPress = 0;
    uint8_t longPrevPress = 0;
    unsigned long LongPressTmp = millis();
#endif

    // main loop, display keyboard, manage keystrokes and displays the string
    while (1) {
        if (redraw) {
            tft.setCursor(0, 0);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            // Draw the top row buttons
            if (y < 0 || y2 < 0) {
                tft.fillRect(0, 1, tftWidth, 22, bruceConfig.bgColor);
                // Draw the borders
                tft.drawRect(
                    ofs[0][0], 2, ofs[0][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // OK btn border
                tft.drawRect(
                    ofs[1][0], 2, ofs[1][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // DEL btn border
                tft.drawRect(
                    ofs[2][0], 2, ofs[2][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // SPACE btn border
                tft.drawRect(
                    ofs[3][0], 2, ofs[3][1], KBLH, getComplementaryColor2(bruceConfig.bgColor)
                ); // BACK btn border

                tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor); // typed string border

                // Highlight the corresponding button when the user cursor is over it

                // OK
                if (x == 0 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[0][0], 2, ofs[0][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("OK", ofs[0][2], 4);

                // DEL
                if (x == 1 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[1][0], 2, ofs[1][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("DEL", ofs[1][2], 4);

                // SPACE
                if (x == 2 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[2][0], 2, ofs[2][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("SPACE", ofs[2][2], 4);

                // BACK
                if (x > 2 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(ofs[3][0], 2, ofs[3][1], KBLH, getComplementaryColor2(bruceConfig.bgColor));
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("BACK", ofs[3][2], 4);
            }

            // prints the title of the textbox, it should report what the user has to write in it
            tft.setTextSize(FP);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
            tft.drawString(msg.substring(0, maxFPSize), 3, KBLH + 4);

            // drawing the textbox and the currently typed string
            tft.setTextSize(FM);
            // reset the text box
            if (mytext.length() == (maxFMSize) || mytext.length() == (maxFMSize + 1) ||
                mytext.length() == (maxFPSize) || mytext.length() == (maxFPSize + 1))
                tft.fillRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.bgColor);
            // write the text
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor));
            if (mytext.length() > maxFMSize) { // if the text is too long, we try to set the smaller font
                tft.setTextSize(FP);
                if (mytext.length() > maxFPSize) { // if its still too long, we divide it into two lines
                    tft.drawString(mytext.substring(0, maxFPSize), 5, KBLH + LH + 6);
                    tft.drawString(mytext.substring(maxFPSize, mytext.length()), 5, KBLH + 2 * LH + 6);
                } else {
                    tft.drawString(mytext, 5, KBLH + 14);
                }
            } else {
                // else if it fits, just draw the text
                tft.drawString(mytext, 5, KBLH + 14);
            }
            // Draw the textbox border again(?)
            tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor); // typed string border

            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            // Draw the actual keyboard (4x4 hex layout)
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    int key_x = j * (_x * 3); // spread 4 keys across 12-key width
                    int key_y = i * _y + KBLH * 2 + 16;
                    int key_w = _x * 3; // key width

                    // Use the previous coordinates to repaint only the previous letter
                    if (x2 == j && y2 == i) {
                        tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor);
                        tft.fillRect(key_x, key_y, key_w, _y, bruceConfig.bgColor);
                    }
                    // If selected, highlight it by changing font color and filling the back rectangle
                    if (x == j && y == i) {
                        tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor);
                        tft.fillRect(key_x, key_y, key_w, _y, ~bruceConfig.bgColor);
                    }

                    // Print the hex characters
                    tft.drawString(String(keys[i][j]), key_x + key_w / 2 - FM * LW / 2, key_y + 2);

                    // Return colors to normal to print the other letters
                    if (x == j && y == i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
                }
            }
            // Save actual key coordinates
            x2 = x;
            y2 = y;
            redraw = false;
        }

        // Cursor Handler
        if (mytext.length() > maxFMSize) {
            tft.setTextSize(FP);
            if (mytext.length() > (maxFPSize)) {
                cY = KBLH + 2 * LH + 6;
                cX = 5 + (mytext.length() - maxFPSize) * LW;
            } else {
                cY = KBLH + LH + 6;
                cX = 5 + mytext.length() * LW;
            }
        } else {
            cY = KBLH + LH + 6;
            cX = 5 + mytext.length() * LW * FM;
        }

        if (millis() - holdCode > 250) { // allow reading inputs

#if defined(HAS_TOUCH) // CYD, Core2, CoreS3
#if defined(USE_TFT_eSPI_TOUCH)
            check(AnyKeyPress);
#endif
            if (touchPoint.pressed) {
                // If using Touchscreen and buttons, reset the navigation states to not
                // act weirdly, and put the cursor on Ok again.
                SelPress = false;
                EscPress = false;
                NextPress = false;
                PrevPress = false;
                UpPress = false;
                DownPress = false;
                x = 0;
                y = -1;

                bool touchHandled = false;

                // Check buttons (indices 16-19 for hex keyboard)
                if (box_list[16].contain(touchPoint.x, touchPoint.y)) {
                    break; // OK button
                }
                if (box_list[17].contain(touchPoint.x, touchPoint.y)) {
                    if (mytext.length() > 0) {
                        handleDelete(mytext, cX, cY, maxFPSize);
                        touchHandled = true;
                    }
                } // DEL button
                if (box_list[18].contain(touchPoint.x, touchPoint.y)) {
                    if (mytext.length() < maxSize) {
                        handleSpaceAdd(mytext, maxSize);
                        touchHandled = true;
                    }
                } // SPACE button
                if (box_list[19].contain(touchPoint.x, touchPoint.y)) {
                    mytext = "\x1B"; // BACK button - ESC
                    break;
                }

                // Check hex character keys (indices 0-15)
                for (k = 0; k < 16; k++) {
                    if (box_list[k].contain(touchPoint.x, touchPoint.y)) {
                        handleCharacterAdd(mytext, box_list[k].key, cX, cY, maxSize, maxFMSize);
                        touchHandled = true;
                        break;
                    }
                }

                if (touchHandled) {
                    wakeUpScreen();
                    touchPoint.Clear();
                    redraw = true;
                }
            }
#endif

#if defined(HAS_3_BUTTONS) // StickCs and Core
            if (check(SelPress)) { goto SELECT; }
            /* Down Btn to move in X axis (to the right) */
            if (longNextPress || NextPress) {
                unsigned long now = millis();
                if (!longNextPress) {
                    longNextPress = 1;
                    LongPress = true;
                    LongPressTmp = now;
                }
                delay(1); // does not work without it
                // Check if the button is held long enough (long press)
                if (now - LongPressTmp > 300) {
                    x--; // Long press action
                    longNextPress = 2;
                    LongPress = false;
                    check(NextPress);
                    LongPressTmp = now;
                } else if (!NextPress) {
                    if (longNextPress != 2) x++; // Short press action
                    longNextPress = 0;
                } else {
                    goto WAITING;
                }
                LongPress = false;
                // delay(10);
                if (y < 0 && x > 4) x = 0; // Changed from 3 to 4 to include BACK button
                if (x > 11) x = 0;
                else if (x < 0) x = 11;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (longPrevPress || PrevPress) {
                unsigned long now = millis();
                if (!longPrevPress) {
                    longPrevPress = 1;
                    LongPress = true;
                    LongPressTmp = now;
                }
                delay(1); // does not work without it
                // Check if the button is held long enough (long press)
                if (now - LongPressTmp > 300) {
                    y--; // Long press action
                    longPrevPress = 2;
                    LongPress = false;
                    check(PrevPress);
                    LongPressTmp = now;
                } else if (!PrevPress) {
                    if (longPrevPress != 2) y++; // Short press action
                    longPrevPress = 0;
                } else {
                    goto WAITING;
                }
                LongPress = false;
                if (y > 3) {
                    y = -1;
                } else if (y < -1) y = 3;
                redraw = true;
            }
#elif defined(HAS_5_BUTTONS) // Smoochie and Marauder-Mini
            if (check(SelPress)) { goto SELECT; }
            /* Down Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x > 4) || x > 11) x = 0; // Changed from 3 to 4 to include BACK button
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x > 4) x = 4; // Changed from 3 to 4 to include BACK button
                else if (x < 0) x = 11;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(DownPress)) {
                y++;
                if (y > 3) { y = -1; }
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = 3;
                redraw = true;
            }

#elif defined(HAS_ENCODER) // T-Embed
            if (check(SelPress)) { goto SELECT; }
            /* DOWN Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                if (check(EscPress)) {
                    y++;
                } else if ((x >= 4 && y < 0) || x == 3) {
                    y++;
                    x = 0;
                } else x++;

                if (y > 3) y = -1;
                if (y == -1 && x > 4) x = 0;

                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(PrevPress)) {
                if (check(EscPress)) {
                    y--;
                    if (y == -1 && x > 4) x = 4;
                } else if (x == 0) {
                    y--;
                    x--;
                } else x--;

                if (y < -1) {
                    y = 3;
                    x = 3;
                } else if (y < 0 && x < 0) x = 4;
                else if (x < 0) x = 3;

                redraw = true;
            }

#elif defined(HAS_KEYBOARD) // Cardputer and T-Deck
            if (KeyStroke.pressed) {
                wakeUpScreen();
                tft.setCursor(cX, cY);
                String keyStr = "";
                for (auto i : KeyStroke.word) {
                    if (keyStr != "") {
                        keyStr = keyStr + "+" + i;
                    } else {
                        keyStr += i;
                    }
                }

                if (mytext.length() < maxSize && !KeyStroke.enter && !KeyStroke.del) {
                    mytext += keyStr;
                    if (mytext.length() != (maxFMSize + 1) && mytext.length() != (maxFMSize + 1))
                        tft.print(keyStr.c_str());
                    cX = tft.getCursorX();
                    cY = tft.getCursorY();
                    if (mytext.length() == (maxFMSize + 1)) redraw = true;
                    if (mytext.length() == (maxFPSize + 1)) redraw = true;
                }
                if (KeyStroke.del && mytext.length() > 0) { // delete 0x08
                    // Handle backspace key
                    mytext.remove(mytext.length() - 1);
                    int fontSize = FM;
                    if (mytext.length() > maxFPSize) {
                        tft.setTextSize(FP);
                        fontSize = FP;
                    } else tft.setTextSize(FM);
                    tft.setCursor((cX - fontSize * LW), cY);
                    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                    tft.print(" ");
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
                    tft.setCursor(cX - fontSize * LW, cY);
                    cX = tft.getCursorX();
                    cY = tft.getCursorY();
                    if (mytext.length() == maxFMSize) redraw = true;
                    if (mytext.length() == maxFPSize) redraw = true;
                }
                if (KeyStroke.enter) { break; }
                KeyStroke.Clear();
            }
            if (check(SelPress)) break;

#endif
        } // end of holdCode detection

        if (SerialCmdPress) { // only for Remote Control, if no input was detected on device
            if (check(SelPress)) { goto SELECT; }
            /* Down Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x > 4) || x > 11) x = 0; // Changed from 3 to 4 to include BACK button
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x > 4) x = 4; // Changed from 3 to 4 to include BACK button
                else if (x < 0) x = 11;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(DownPress)) {
                y++;
                if (y > 3) { y = -1; }
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = 3;
                redraw = true;
            }
        }

        if (false) { // When selecting some letter or something, use these helper functions instead of goto
        SELECT:      // so when we "select", so when we click something, this is executed:
            KeyboardAction action =
                handleHexKeyboardSelection(x, y, mytext, cX, cY, maxSize, maxFMSize, maxFPSize, keys);

            if (action == KEYBOARD_OK || action == KEYBOARD_CANCEL) {
                break;
            } else if (action == KEYBOARD_REDRAW) {
                redraw = true;
            }

            holdCode = millis();
        WAITING: // Used in long press detection
            yield();
        }
    }

    // Resets screen when finished writing
    tft.fillScreen(bruceConfig.bgColor);
    resetTftDisplay();

    return mytext;
}

void powerOff() { displayWarning("Not available", true); }
void goToDeepSleep() {
#if DEEPSLEEP_WAKEUP_PIN >= 0
    esp_sleep_enable_ext0_wakeup((gpio_num_t)DEEPSLEEP_WAKEUP_PIN, DEEPSLEEP_PIN_ACT);
    esp_deep_sleep_start();
#else
    displayWarning("Not available", true);
#endif
}

void checkReboot() {}
