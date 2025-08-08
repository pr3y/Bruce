#include "mykeyboard.h"
#include "core/wifi/webInterface.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/rf/rf_send.h"
#include "powerSave.h"
#include "sd_functions.h"
#include <ArduinoJson.h>

const uint8_t max_chars = tftWidth / (LW * FM); // currently unused
const int max_FM_size = tftWidth / (LW * FM) - 1;
const int max_FP_size = tftWidth / (LW)-2;
const int keyboard_width = 12;
const int keyboard_height = 4;
char keys[keyboard_height][keyboard_width][2] = {
    //  4 lines, with 12 characters, capital and lowercase letters
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
char hex_keys[keyboard_height][keyboard_width][1] = {
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
    //  4 lines, with 4 characters, high caps only
    {'0', '1', '2', '3'},
    {'4', '5', '6', '7'},
    {'8', '9', 'A', 'B'},
    {'C', 'D', 'E', 'F'},
};

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

static constexpr std::size_t box_count = 53;

#endif

// Retrieves the current keyStroke from InputHandler, resets it after use.
// This function is used in loopTask to get the latest key press.
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
} // Returns a keyStroke that the keyboards won't recognize by default

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
** return the pressed number
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
** return the pressed letter
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

/// Handles character deletion from the text string and screen
bool handleDelete(String &current_text, int &cursor_x, int &cursor_y) {
    if (current_text.length() == 0) return false;

    // remove from string
    current_text.remove(current_text.length() - 1);
    // delete from screen:
    int fontSize = FM;
    if (current_text.length() > max_FP_size) {
        tft.setTextSize(FP);
        fontSize = FP;
    } else tft.setTextSize(FM);
    tft.setCursor((cursor_x - fontSize * LW), cursor_y);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.print(" ");
    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
    tft.setCursor(cursor_x - fontSize * LW, cursor_y);
    cursor_x = tft.getCursorX();
    cursor_y = tft.getCursorY();
    return true;
}

/// Handles adding a character to the text string
bool handleCharacterAdd(
    String &current_text, char character, int &cursor_x, int &cursor_y, const int max_size
) {
    if (current_text.length() >= max_size) return false;

    current_text += character;
    if (current_text.length() != (max_FP_size + 1)) tft.print(character);
    cursor_x = tft.getCursorX();
    cursor_y = tft.getCursorY();
    return true;
}

/// Handles adding space to the text string
bool handleSpaceAdd(String &current_text, const int max_size) {
    if (current_text.length() >= max_size) return false;
    current_text += ' ';
    return true;
}

// Enum for keyboard action results
enum KeyboardAction { KEYBOARD_CONTINUE, KEYBOARD_OK, KEYBOARD_CANCEL, KEYBOARD_REDRAW };

/// Handles keyboard selection logic for regular keyboard
KeyboardAction handleKeyboardSelection(
    int &x, int &y, String &current_text, bool &caps, int &cursor_x, int &cursor_y, const int max_size
) {
    tft.setCursor(cursor_x, cursor_y);
    int z = caps ? 1 : 0;

    if (y == -1) {
        switch (x) {
            case 0: // OK button
                return KEYBOARD_OK;
            case 1: // CAP button
                caps = !caps;
                return KEYBOARD_REDRAW;
            case 2: // DEL button
                if (handleDelete(current_text, cursor_x, cursor_y)) return KEYBOARD_REDRAW;
                break;
            case 3: // SPACE button
                if (handleSpaceAdd(current_text, max_size)) return KEYBOARD_REDRAW;
                break;
            case 4: // BACK button
                current_text = "\x1B";
                return KEYBOARD_CANCEL;
            default: break;
        }

    } else if (y > -1 && current_text.length() < max_size) {
        // add a letter to current_text
        if (handleCharacterAdd(current_text, keys[x][y][z], cursor_x, cursor_y, max_size)) {
            if (current_text.length() >= max_size) { // put the Cursor at "Ok" when max size reached
                x = 0;
                y = -1;
            }

            return KEYBOARD_REDRAW;
        }
    }

    return KEYBOARD_CONTINUE;
}

String generalKeyboard(String current_text, int max_size, String textbox_title, bool caps_enabled) {
    resetTftDisplay();
    touchPoint.Clear();

    /* SUPPORT VARIABLES */
    bool caps = false;
    bool selection_made = false; // used for detecting if an key or a button was selected
    bool redraw = true;
    long last_input_time = millis(); // used for input debouncing
    int buttons_number = 5;
    // cursor coordinates: kep track of where the next character should be printed (in screen pixels)
    int cursor_x = 0;
    int cursor_y = 0;
    // keyboard navigation coordinates: keep track of which key (or button) is currently selected
    int x = 0;
    int y = -1; // -1 is where the buttons are, out of the keys[][][] array
    int z = 0;
    int old_x = 0;
    int old_y = 0;
    //       [x][y] [z], old_x and old_y are the previous position of x and y, used to redraw only that spot
    //       on keyboard screen

#if FM > 1      // Normal keyboard size
#define KBLH 20 // Keyboard Buttons Line Height
    // { x coord of btn border, btn width, x coord of the inside text }
    int btns_layout[buttons_number][3] = {
        {7,   46, 18 }, // OK button
        {55,  52, 64 }, // CAP button
        {109, 52, 118}, // DEL button
        {163, 76, 172}, // SPACE button
        {241, 64, 250}, // BACK button
        // ~11 px per letter -> actually 10 px + 2 of padding between the letters
        // ((12 * n_letters) - 2 ) + 9*2 = width
    };
#else           // small keyboard size, for smaller screen, like Marauder Mini and others ;)
#define KBLH 10 // Keyboard Buttons Line Height
    buttons_number = 4;
    int btns_layout[buttons_number][3] = {
        {2,  20, 5 }, // OK button
        {22, 25, 25}, // CAP button
        {47, 25, 50}, // DEL button
        {72, 50, 75}, // SPACE button
        // {122, 40, 125}, // BACK button
    };
#endif
    const int key_width = tftWidth / keyboard_width;
    const int key_height = (tftHeight - (2 * KBLH + 14)) / keyboard_height;
    const int text_offset_x = key_width / 2 - 3;

#if defined(HAS_TOUCH) // filling touch box list
    int k = 0;
    // Setup keyboard touch boxes
    for (int i = 0; i < keyboard_width; i++) {      // x coord
        for (int j = 0; j < keyboard_height; j++) { // y coord
            box_list[k].key = keys[j][i][0];
            box_list[k].key_sh = keys[j][i][1];
            box_list[k].color = ~bruceConfig.bgColor;
            box_list[k].x = i * key_width;
            box_list[k].y = j * key_height + 54;
            box_list[k].w = key_width;
            box_list[k].h = key_height;
            k++;
        }
    }
    // Setup buttons touch boxes
    for (int i = 0; i < buttons_number; i++) {
        box_list[k].key = ' ';
        box_list[k].key_sh = ' ';
        box_list[k].color = ~bruceConfig.bgColor;
        box_list[k].x = btns_layout[i][0];
        box_list[k].y = 0;
        box_list[k].w = btns_layout[i][1];
        box_list[k].h = KBLH + 2;
        k++;
    }

    k = 0;
#endif

    tft.fillScreen(bruceConfig.bgColor); // reset the screen

#if defined(HAS_3_BUTTONS) // StickCs and Core for long press detection logic
    uint8_t longNextPress = 0;
    uint8_t longPrevPress = 0;
    unsigned long LongPressTmp = millis();
#endif

    // main loop
    while (1) {
        if (redraw) {
            // setup
            tft.setCursor(0, 0);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            // Draw the top row buttons
            if (y < 0 || old_y < 0) {
                tft.fillRect(0, 1, tftWidth, 22, bruceConfig.bgColor);
                // Draw the buttons borders
                for (int i = 0; i < buttons_number; ++i) {
                    tft.drawRect(
                        btns_layout[i][0],
                        2,
                        btns_layout[i][1],
                        KBLH,
                        getComplementaryColor2(bruceConfig.bgColor)
                    );
                }

                tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor); // typed string border

                /* Highlight the corresponding button when the user cursor is over it */
                // OK
                if (x == 0 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(
                        btns_layout[0][0],
                        2,
                        btns_layout[0][1],
                        KBLH,
                        getComplementaryColor2(bruceConfig.bgColor)
                    );
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("OK", btns_layout[0][2], 5);
                // CAP
                if (x == 1 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(
                        btns_layout[1][0],
                        2,
                        btns_layout[1][1],
                        KBLH,
                        getComplementaryColor2(bruceConfig.bgColor)
                    );
                } else if (caps) {
                    tft.fillRect(btns_layout[1][0], 2, btns_layout[1][1], KBLH, TFT_DARKGREY);
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), TFT_DARKGREY);
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("CAP", btns_layout[1][2], 5);
                // DEL
                if (x == 2 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(
                        btns_layout[2][0],
                        2,
                        btns_layout[2][1],
                        KBLH,
                        getComplementaryColor2(bruceConfig.bgColor)
                    );
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("DEL", btns_layout[2][2], 5);
                // SPACE
                if (x == 3 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(
                        btns_layout[3][0],
                        2,
                        btns_layout[3][1],
                        KBLH,
                        getComplementaryColor2(bruceConfig.bgColor)
                    );
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("SPACE", btns_layout[3][2], 5);
#if FM > 1 // draw only on large enough screens
           //   BACK
                if (x > 3 && y == -1) {
                    tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor));
                    tft.fillRect(
                        btns_layout[4][0],
                        2,
                        btns_layout[4][1],
                        KBLH,
                        getComplementaryColor2(bruceConfig.bgColor)
                    );
                } else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
                tft.drawString("BACK", btns_layout[4][2], 5);
#endif
            }

            // Prints the title of the textbox, it should report what the user has to write in it
            tft.setTextSize(FP);
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
            tft.drawString(textbox_title.substring(0, max_FP_size), 3, KBLH + 4);

            // Drawing the textbox and the currently typed string
            tft.setTextSize(FM);
            // reset the text box if needed
            if (current_text.length() == (max_FM_size) || current_text.length() == (max_FM_size + 1) ||
                current_text.length() == (max_FP_size) || current_text.length() == (max_FP_size + 1))
                tft.fillRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.bgColor);
            // write the text
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor));
            if (current_text.length() >
                max_FM_size) { // if the text is too long, we try to set the smaller font
                tft.setTextSize(FP);
                if (current_text.length() >
                    max_FP_size) { // if its still too long, we divide it into two lines
                    tft.drawString(current_text.substring(0, max_FP_size), 5, KBLH + LH + 6);
                    tft.drawString(
                        current_text.substring(max_FP_size, current_text.length()), 5, KBLH + 2 * LH + 6
                    );
                } else {
                    tft.drawString(current_text, 5, KBLH + 14);
                }
            } else {
                // else if it fits, just draw the text
                tft.drawString(current_text, 5, KBLH + 14);
            }
            // Draw the textbox border again(?)
            tft.drawRect(3, KBLH + 12, tftWidth - 3, KBLH, bruceConfig.priColor); // typed string border

            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
            tft.setTextSize(FM);

            // Draw the actual keyboard
            for (int i = 0; i < keyboard_height; i++) {
                for (int j = 0; j < keyboard_width; j++) {
                    // key coordinates
                    int key_x = j * key_width;
                    int key_y = i * key_height + KBLH * 2 + 14;

                    // Use the previous coordinates to redraw only the previous letter
                    if (old_x == j && old_y == i) {
                        tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor);
                        tft.fillRect(key_x, key_y, key_width, key_height, bruceConfig.bgColor);
                    }
                    // If selected, highlight it by changing font color and filling the back rectangle
                    if (x == j && y == i) {
                        tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor);
                        tft.fillRect(key_x, key_y, key_width, key_height, ~bruceConfig.bgColor);
                    }

                    // Print the letters
                    if (!caps) tft.drawString(String(keys[i][j][0]), key_x + text_offset_x, key_y + 2);
                    else tft.drawString(String(keys[i][j][1]), key_x + text_offset_x, key_y + 2);

                    // Return colors to normal to print the other letters
                    if (x == j && y == i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
                }
            }
            // backup key coordinates
            old_x = x;
            old_y = y;
            redraw = false;
        }

        // Cursor Handler
        if (current_text.length() > max_FM_size) {
            tft.setTextSize(FP);
            if (current_text.length() > (max_FP_size)) {
                cursor_y = KBLH + 2 * LH + 6;
                cursor_x = 5 + (current_text.length() - max_FP_size) * LW;
            } else {
                cursor_y = KBLH + LH + 6;
                cursor_x = 5 + current_text.length() * LW;
            }
        } else {
            cursor_y = KBLH + LH + 6;
            cursor_x = 5 + current_text.length() * LW * FM;
        }

        if (millis() - last_input_time > 250) { // INPUT DEBOUCING
            // waits at least 250ms before accepting another input, to prevent rapid involuntary repeats

#if defined(HAS_TOUCH) // CYD, Core2, CoreS3
#if defined(USE_TFT_eSPI_TOUCH)
            check(AnyKeyPress);
#endif
            if (touchPoint.pressed) {
                // If using touchscreen and buttons, reset the navigation states to avoid inconsistent
                // behavior, and reset the navigation coords to the OK button.
                SelPress = false;
                EscPress = false;
                NextPress = false;
                PrevPress = false;
                UpPress = false;
                DownPress = false;
                x = 0;
                y = -1;

                bool touchHandled = false;

                if (box_list[48].contain(touchPoint.x, touchPoint.y)) { // OK btn
                    break;
                }
                if (box_list[49].contain(touchPoint.x, touchPoint.y)) { // CAPS btn
                    caps = !caps;
                    tft.fillRect(0, 54, tftWidth, tftHeight - 54, bruceConfig.bgColor);
                    touchHandled = true;
                }
                if (box_list[50].contain(touchPoint.x, touchPoint.y)) { // DEL btn
                    if (current_text.length() > 0) {
                        handleDelete(current_text, cursor_x, cursor_y);
                        touchHandled = true;
                    }
                }
                if (box_list[51].contain(touchPoint.x, touchPoint.y)) { // SPACE btn
                    if (current_text.length() < max_size) {
                        handleSpaceAdd(current_text, max_size);
                        touchHandled = true;
                    }
                }
                if (box_list[52].contain(touchPoint.x, touchPoint.y)) { // BACK btn
                    current_text = "\x1B";                              // ASCII ESC CHARACTER
                    break;
                }
                for (k = 0; k < 48; k++) {
                    if (box_list[k].contain(touchPoint.x, touchPoint.y)) {
                        if (caps)
                            handleCharacterAdd(
                                current_text, box_list[k].key_sh, cursor_x, cursor_y, max_size
                            );
                        else handleCharacterAdd(current_text, box_list[k].key, cursor_x, cursor_y, max_size);
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
            if (check(SelPress)) { selection_made = true; }
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
                    continue;
                }
                LongPress = false;
                // delay(10);
                if (y < 0 && x >= buttons_number) x = 0;
                if (x >= keyboard_width) x = 0;
                else if (x < 0) x = keyboard_width - 1;
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
                    continue;
                }
                LongPress = false;
                if (y >= keyboard_height) {
                    y = -1;
                } else if (y < -1) y = keyboard_height - 1;
                redraw = true;
            }
#elif defined(HAS_5_BUTTONS) // Smoochie and Marauder-Mini
            if (check(SelPress)) { selection_made = true; }
            /* Down Btn to move in X axis (to the right) */
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x >= buttons_number) || x >= keyboard_width) x = 0;
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x >= buttons_number) x = buttons_number - 1;
                else if (x < 0) x = keyboard_width - 1;
                redraw = true;
            }
            /* UP Btn to move in Y axis (Downwards) */
            if (check(DownPress)) {
                y++;
                if (y > keyboard_height - 1) { y = -1; }
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = keyboard_height - 1;
                redraw = true;
            }

#elif defined(HAS_ENCODER) // T-Embed
            if (check(SelPress)) { selection_made = true; }
            /* NEXT "Btn" to move forward on th X axis (to the right) */
            // if ESC is pressed while NEXT or PREV is received, then we navigate on the Y axis instead
            if (check(NextPress)) {
                if (check(EscPress)) {
                    y++;
                } else if ((x >= buttons_number - 1 && y <= -1) || x >= keyboard_width) {
                    // if we are at the end of the current line
                    y++;   // next line
                    x = 0; // reset to first key
                } else x++;

                if (y >= keyboard_height)
                    y = -1; // if we are at the end of the keyboard, then return to the top

                // If we move to a new line using the ESC-press navigation and the previous x coordinate is
                // greater than the number of available buttons on the new line, reset x to avoid
                // out-of-bounds behavior, this can only happen when switching to the first line, as the
                // others have all the same number of keys
                if (y == -1 && x >= buttons_number) x = 0;

                redraw = true;
            }
            /* PREV "Btn" to move backwards on th X axis (to the left) */
            if (check(PrevPress)) {
                if (check(EscPress)) {
                    y--;
                } else if (x <= 0) {
                    y--;
                    x = keyboard_width - 1;
                } else x--;

                if (y < -1) {
                    y = keyboard_height - 1;
                    x = keyboard_width - 1;
                } else if (y == -1 && x >= buttons_number) x = buttons_number - 1;
                else if (x < 0) x = keyboard_width - 1;

                redraw = true;
            }

#elif defined(HAS_KEYBOARD) // Cardputer and T-Deck
            if (KeyStroke.pressed) {
                wakeUpScreen();
                tft.setCursor(cursor_x, cursor_y);
                String keyStr = "";
                for (auto i : KeyStroke.word) {
                    if (keyStr != "") {
                        keyStr = keyStr + "+" + i;
                    } else {
                        keyStr += i;
                    }
                }

                if (current_text.length() < max_size && !KeyStroke.enter && !KeyStroke.del) {
                    current_text += keyStr;
                    if (current_text.length() != (max_FM_size + 1) &&
                        current_text.length() != (max_FM_size + 1))
                        tft.print(keyStr.c_str());
                    cursor_x = tft.getCursorX();
                    cursor_y = tft.getCursorY();
                    if (current_text.length() == (max_FM_size + 1)) redraw = true;
                    if (current_text.length() == (max_FP_size + 1)) redraw = true;
                }
                if (KeyStroke.del && current_text.length() > 0) { // delete 0x08
                    // Handle backspace key
                    current_text.remove(current_text.length() - 1);
                    int fontSize = FM;
                    if (current_text.length() > max_FP_size) {
                        tft.setTextSize(FP);
                        fontSize = FP;
                    } else tft.setTextSize(FM);
                    tft.setCursor((cursor_x - fontSize * LW), cursor_y);
                    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                    tft.print(" ");
                    tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
                    tft.setCursor(cursor_x - fontSize * LW, cursor_y);
                    cursor_x = tft.getCursorX();
                    cursor_y = tft.getCursorY();
                    if (current_text.length() == max_FM_size) redraw = true;
                    if (current_text.length() == max_FP_size) redraw = true;
                }
                if (KeyStroke.enter) { break; }
                KeyStroke.Clear();
            }
            if (check(SelPress)) break;

#endif
        } // end of physical input detection

        if (SerialCmdPress) { // only for Remote Control, if no type of input was detected on device
            if (check(SelPress)) { selection_made = true; }
            /* Next-Prev Btns to move in X axis (right-left) */
            if (check(NextPress)) {
                x++;
                if ((y < 0 && x >= buttons_number) || x >= keyboard_width) x = 0;
                redraw = true;
            }
            if (check(PrevPress)) {
                x--;
                if (y < 0 && x >= buttons_number) x = buttons_number - 1;
                else if (x < 0) x = keyboard_width - 1;
                redraw = true;
            }
            /* Down-Up Btns to move in Y axis */
            if (check(DownPress)) {
                y++;
                if (y >= keyboard_height) { y = -1; }
                redraw = true;
            }
            if (check(UpPress)) {
                y--;
                if (y < -1) y = keyboard_height - 1;
                redraw = true;
            }
        }

        if (selection_made) { // if something was selected then handle it
            selection_made = false;
            KeyboardAction action =
                handleKeyboardSelection(x, y, current_text, caps, cursor_x, cursor_y, max_size);

            if (action == KEYBOARD_OK) { // OK BTN
                break;
            } else if (action == KEYBOARD_CANCEL) { // BACK BTN
                current_text = "\x1B";              // ASCII ESC CHARACTER
                break;
            } else if (action == KEYBOARD_REDRAW) {
                redraw = true;
            }

            last_input_time = millis();
        }
    }

    // Resets screen when finished writing
    tft.fillScreen(bruceConfig.bgColor);
    resetTftDisplay();

    return current_text;
}

/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** keyboard interface.
**********************************************************************/
String keyboard(String current_text, int max_size, String textbox_title) {
    return generalKeyboard(current_text, max_size, textbox_title, true);
}

String hex_keyboard(String current_text, int max_size, String textbox_title) {
    return generalKeyboard(current_text, max_size, textbox_title, false);
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
