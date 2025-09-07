#include "core/powerSave.h"
#include <Wire.h>
#include <globals.h>
#include <interface.h>

#include <Adafruit_TCA8418.h>
Adafruit_TCA8418 *keyboard;

// Keyboard state variables
bool fn_key_pressed = false;
bool shift_key_pressed = false;
bool caps_lock = false;

// Key value mapping for 4x14 keyboard
struct KeyValue_t {
    const char value_first;
    const char value_second;
    const char value_third;
};

const KeyValue_t _key_value_map[KB_ROWS][KB_COLS] = {
    {{'`', '~', '`'},
     {'1', '!', '1'},
     {'2', '@', '2'},
     {'3', '#', '3'},
     {'4', '$', '4'},
     {'5', '%', '5'},
     {'6', '^', '6'},
     {'7', '&', '7'},
     {'8', '*', '8'},
     {'9', '(', '9'},
     {'0', ')', '0'},
     {'-', '_', '-'},
     {'=', '+', '='},
     {'\b', '\b', '\b'}}, // Backspace

    {{'\t', '\t', '\t'}, // Tab
     {'q', 'Q', 'q'},
     {'w', 'W', 'w'},
     {'e', 'E', 'e'},
     {'r', 'R', 'r'},
     {'t', 'T', 't'},
     {'y', 'Y', 'y'},
     {'u', 'U', 'u'},
     {'i', 'I', 'i'},
     {'o', 'O', 'o'},
     {'p', 'P', 'p'},
     {'[', '{', '['},
     {']', '}', ']'},
     {'\\', '|', '\\'}},

    {{0xFF, 0xFF, 0xFF}, // FN key (special)
     {0x81, 0x81, 0x81}, // Shift key (special)
     {'a', 'A', 'a'},
     {'s', 'S', 's'},
     {'d', 'D', 'd'},
     {'f', 'F', 'f'},
     {'g', 'G', 'g'},
     {'h', 'H', 'h'},
     {'j', 'J', 'j'},
     {'k', 'K', 'k'},
     {'l', 'L', 'l'},
     {';', ':', ';'},
     {'\'', '\"', '\''},
     {'\r', '\r', '\r'}}, // Enter

    {{0x80, 0x80, 0x80}, // Ctrl key (special)
     {0x83, 0x83, 0x83}, // OPT key (special)
     {0x82, 0x82, 0x82}, // Alt key (special)
     {'z', 'Z', 'z'},
     {'x', 'X', 'x'},
     {'c', 'C', 'c'},
     {'v', 'V', 'v'},
     {'b', 'B', 'b'},
     {'n', 'N', 'n'},
     {'m', 'M', 'm'},
     {',', '<', ','},
     {'.', '>', '.'},
     {'/', '?', '/'},
     {' ', ' ', ' '}}
};

int handleSpecialKeys(uint8_t row, uint8_t col, bool pressed);
void mapRawKeyToPhysical(uint8_t rawValue, uint8_t &row, uint8_t &col);

char getKeyChar(uint8_t row, uint8_t col) {
    char keyVal;
    if (fn_key_pressed) {
        keyVal = _key_value_map[row][col].value_third;
    } else if (shift_key_pressed ^ caps_lock) {
        keyVal = _key_value_map[row][col].value_second;
    } else {
        keyVal = _key_value_map[row][col].value_first;
    }
    return keyVal;
}

int handleSpecialKeys(uint8_t row, uint8_t col, bool pressed) {
    char keyVal = _key_value_map[row][col].value_first;
    switch (keyVal) {
        case 0xFF: 
            if (pressed) fn_key_pressed = !fn_key_pressed;
            return 1;
        case 0x81: 
            shift_key_pressed = pressed;
            if (pressed && fn_key_pressed) caps_lock = !caps_lock;
            return 1;
        default: break;
    }
    return 0;
}

/***************************************************************************************
** Function name: mapRawKeyToPhysical()
** Location: interface.cpp
** Description:   initial mapping for keyboard
***************************************************************************************/
void mapRawKeyToPhysical(uint8_t rawValue, uint8_t &row, uint8_t &col) {
    switch(rawValue) {
        case 1: row = 0; col = 0; break; // ESC/`
        case 2: row = 1; col = 0; break; // Tab
        case 3: row = 2; col = 0; break; // FN
        case 4: row = 3; col = 0; break; // Ctrl
        case 5: row = 0; col = 1; break; // 1
        case 6: row = 1; col = 1; break; // Q
        case 7: row = 2; col = 1; break; // Shift
        case 8: row = 3; col = 1; break; // Opt
        case 11: row = 0; col = 2; break; // 2
        case 12: row = 1; col = 2; break; // W
        case 13: row = 2; col = 2; break; // A
        case 14: row = 3; col = 2; break; // Alt
        case 15: row = 0; col = 3; break; // 3
        case 16: row = 1; col = 3; break; // E
        case 17: row = 2; col = 3; break; // S
        case 18: row = 3; col = 3; break; // Z
        case 21: row = 0; col = 4; break; // 4
        case 22: row = 1; col = 4; break; // R
        case 23: row = 2; col = 4; break; // D
        case 24: row = 3; col = 4; break; // X
        case 25: row = 0; col = 5; break; // 5
        case 26: row = 1; col = 5; break; // T
        case 27: row = 2; col = 5; break; // F
        case 28: row = 3; col = 5; break; // C
        case 31: row = 0; col = 6; break; // 6
        case 32: row = 1; col = 6; break; // Y
        case 33: row = 2; col = 6; break; // G
        case 34: row = 3; col = 6; break; // V
        case 35: row = 0; col = 7; break; // 7
        case 36: row = 1; col = 7; break; // U
        case 37: row = 2; col = 7; break; // H
        case 38: row = 3; col = 7; break; // B
        case 41: row = 0; col = 8; break; // 8
        case 42: row = 1; col = 8; break; // I
        case 43: row = 2; col = 8; break; // J
        case 44: row = 3; col = 8; break; // N
        case 45: row = 0; col = 9; break; // 9
        case 46: row = 1; col = 9; break; // O
        case 47: row = 2; col = 9; break; // K
        case 48: row = 3; col = 9; break; // M
        case 51: row = 0; col = 10; break; // 0
        case 52: row = 1; col = 10; break; // P
        case 53: row = 2; col = 10; break; // L
        case 54: row = 3; col = 10; break; // ,
        case 55: row = 0; col = 11; break; // -
        case 56: row = 1; col = 11; break; // [
        case 57: row = 2; col = 11; break; // ;
        case 58: row = 3; col = 11; break; // .
        case 61: row = 0; col = 12; break; // =
        case 62: row = 1; col = 12; break; // ]
        case 63: row = 2; col = 12; break; // '
        case 64: row = 3; col = 12; break; // /
        case 65: row = 0; col = 13; break; // Backspace
        case 66: row = 1; col = 13; break; // 
        case 67: row = 2; col = 13; break; // Enter
        case 68: row = 3; col = 13; break; // Space
        default:
            row = 0; col = 0;
            Serial.printf("Unknown raw value: %d\n", rawValue);
            break;
    }
}

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    Serial.println("=== SETTING UP 14x4 KEYBOARD ===");
    
    Wire.begin(SDA, SCL);
    delay(100);
    
    // Check if device exists
    Wire.beginTransmission(KB_I2C_ADDRESS);
    byte error = Wire.endTransmission();
    if (error != 0) {
        Serial.printf("Keyboard not found at 0x%02X! Error: %d\n", KB_I2C_ADDRESS, error);
        return;
    }
    
    keyboard = new Adafruit_TCA8418();
    if (!keyboard->begin(KB_I2C_ADDRESS, &Wire)) {
        Serial.println("Failed to initialize keyboard library");
        return;
    }

    // Reset the device to ensure clean state
    keyboard->writeRegister(TCA8418_REG_CFG, 0x00);
    delay(10);
    
    // Configure for 4 rows and 14 columns
    // Rows 0-3 as outputs, columns 4-17 as inputs
    keyboard->writeRegister(TCA8418_REG_GPIO_DIR_1, 0x0F);  // GPIO0-3: outputs, GPIO4-7: inputs
    keyboard->writeRegister(TCA8418_REG_GPIO_DIR_2, 0xFF);  // GPIO8-15: inputs
    keyboard->writeRegister(TCA8418_REG_GPIO_DIR_3, 0x03);  // GPIO16-17: inputs
    
    // Set all used pins as keypad
    keyboard->writeRegister(TCA8418_REG_KP_GPIO_1, 0xFF);  // GPIO0-7 as keypad
    keyboard->writeRegister(TCA8418_REG_KP_GPIO_2, 0xFF);  // GPIO8-15 as keypad
    keyboard->writeRegister(TCA8418_REG_KP_GPIO_3, 0x03);  // GPIO16-17 as keypad
    
    // Enable pull-ups on all inputs
    keyboard->writeRegister(TCA8418_REG_GPIO_PULL_1, 0xF0);  // Pull-ups on GPIO4-7
    keyboard->writeRegister(TCA8418_REG_GPIO_PULL_2, 0xFF);  // Pull-ups on GPIO8-15
    keyboard->writeRegister(TCA8418_REG_GPIO_PULL_3, 0x03);  // Pull-ups on GPIO16-17
    
    // Configure interrupts
    keyboard->writeRegister(TCA8418_REG_CFG, 
        TCA8418_REG_CFG_KE_IEN |    // Enable key event interrupt
        TCA8418_REG_CFG_AI          // Auto-increment
    );
    
    // Clear interrupts
    keyboard->writeRegister(TCA8418_REG_INT_STAT, 0xFF);
    
    Serial.println("14x4 keyboard configured successfully!");
}

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/adc_channel.h>
#include <soc/soc_caps.h>
/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    uint8_t percent;
    uint8_t _batAdcCh = ADC1_GPIO10_CHANNEL;
    uint8_t _batAdcUnit = 1;
    static uint32_t lastVolt = 5000;
    static unsigned long lastTime = 0;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten((adc1_channel_t)_batAdcCh, ADC_ATTEN_DB_12);
    static esp_adc_cal_characteristics_t *adc_chars = nullptr;
    static constexpr int BASE_VOLATAGE = 3600;
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(
        (adc_unit_t)_batAdcUnit, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, BASE_VOLATAGE, adc_chars
    );
    int raw;
    raw = adc1_get_raw((adc1_channel_t)_batAdcCh);
    uint32_t volt = esp_adc_cal_raw_to_voltage(raw, adc_chars);

    float mv = volt * 2;
    percent = (mv - 3300) * 100 / (float)(4150 - 3350);

    return (percent < 0) ? 0 : (percent >= 100) ? 100 : percent;
}
bool isCharging() { return false; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval == 0) {
        analogWrite(TFT_BL, brightval);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    static unsigned long lastKeyTime = 0;
    static uint8_t lastKeyValue = 0;
    
    if (millis() - tm < 200 && !LongPress) return;

    bool shoulder = digitalRead(0);

    bool arrow_up = false;
    bool arrow_dw = false;
    bool arrow_ry = false;
    bool arrow_le = false;

    if (keyboard->available() > 0) {
        int keyEvent = keyboard->getEvent();
        bool pressed = !(keyEvent & 0x80);  // Bit 7: 0=pressed, 1=released
        uint8_t value = keyEvent & 0x7F;    // Bits 0-6: key value
        
        // Debounce check
        if (millis() - lastKeyTime < 50 && value == lastKeyValue) {
            return;
        }
        lastKeyTime = millis();
        lastKeyValue = value;
        
        // Map raw value to physical position
        uint8_t row, col;
        mapRawKeyToPhysical(value, row, col);
        
        Serial.printf("Key event: raw=%d, pressed=%d, row=%d, col=%d\n",
                     value, pressed, row, col);

        if (row >= KB_ROWS || col >= KB_COLS) {
            Serial.printf("Invalid position: row=%d, col=%d\n", row, col);
            return;
        }

        if (!wakeUpScreen()) {
            AnyKeyPress = true;
            
            if (handleSpecialKeys(row, col, pressed) > 0) goto END;
            
            if (pressed) {
                keyStroke key;
                char keyVal = getKeyChar(row, col);

                Serial.printf("Key pressed: %c (0x%02X) at row=%d, col=%d\n", 
                             keyVal, keyVal, row, col);

                if (keyVal == 0x08) {
                    key.del = true;
                    key.word.emplace_back(KEY_BACKSPACE);
                    EscPress = true;
                } else if (keyVal == 0x60) {
                    EscPress = true;
                } else if (keyVal == 0x0D) {
                    key.enter = true;
                    key.word.emplace_back(KEY_ENTER);
                    SelPress = true;
                } else if (keyVal == 0x09) {
                    key.word.emplace_back(KEY_TAB);
                } else if (keyVal == 0xFF) {
                    key.fn = true;
                } else if (keyVal == 0x81) {
                    key.modifier_keys.emplace_back(KEY_LEFT_SHIFT);
                } else if (keyVal == 0x3B) {
                    arrow_up = true;
                    PrevPress = true;
                } else if (keyVal == 0x80) {
                    key.modifier_keys.emplace_back(KEY_LEFT_CTRL);
                } else if (keyVal == 0x82) {
                    key.modifier_keys.emplace_back(KEY_LEFT_ALT);
                } else if (keyVal == 0x83) {
                    key.gui = true;
                } else if (keyVal == 0x2E) {
                    arrow_dw = true;
                    NextPress = true;
                } else if (keyVal == 0x2C) {
                    arrow_le = true;
                    PrevPress = true;
                    PrevPagePress = true;
                    key.word.emplace_back(keyVal);
                } else if (keyVal == 0x13) {
                    PrevPagePress = true;
                    key.word.emplace_back(keyVal);
                } else if (keyVal == 0x2F) {
                    arrow_ry = true;
                    NextPress = true;
                    NextPagePress = true;
                    key.word.emplace_back(keyVal);
                } else {
                    key.word.emplace_back(keyVal);
                }

                key.pressed = true;
                KeyStroke = key;
                tm = millis();
            }
        }
    } else if (shoulder == LOW) {
        if (!wakeUpScreen()) {
            AnyKeyPress = true;
            SelPress = true;
            tm = millis();
        }
    } else {
        KeyStroke.Clear();
    }

END:
    if (shoulder == LOW) tm = millis();
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
