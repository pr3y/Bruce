#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

#define USB_VID 0x303a
#define USB_PID 0x1001

#define HAS_KEYBOARD     // has keyboard to use
#define HAS_KEYBOARD_HID // has keyboard to use
#define KB_HID_EXIT_MSG "fn + Ok to exit"
#define KB_I2C_ADDRESS 0x34
#define KB_ROWS 4
#define KB_COLS 14

#define TCA8418_REG_CFG 0x01             ///< Configuration register
#define TCA8418_REG_INT_STAT 0x02        ///< Interrupt status
#define TCA8418_REG_KEY_LCK_EC 0x03      ///< Key lock and event counter
#define TCA8418_REG_KEY_EVENT_A 0x04     ///< Key event register A
#define TCA8418_REG_KEY_EVENT_B 0x05     ///< Key event register B
#define TCA8418_REG_KEY_EVENT_C 0x06     ///< Key event register C
#define TCA8418_REG_KEY_EVENT_D 0x07     ///< Key event register D
#define TCA8418_REG_KEY_EVENT_E 0x08     ///< Key event register E
#define TCA8418_REG_KEY_EVENT_F 0x09     ///< Key event register F
#define TCA8418_REG_KEY_EVENT_G 0x0A     ///< Key event register G
#define TCA8418_REG_KEY_EVENT_H 0x0B     ///< Key event register H
#define TCA8418_REG_KEY_EVENT_I 0x0C     ///< Key event register I
#define TCA8418_REG_KEY_EVENT_J 0x0D     ///< Key event register J
#define TCA8418_REG_KP_LCK_TIMER 0x0E    ///< Keypad lock1 to lock2 timer
#define TCA8418_REG_UNLOCK_1 0x0F        ///< Unlock register 1
#define TCA8418_REG_UNLOCK_2 0x10        ///< Unlock register 2
#define TCA8418_REG_GPIO_INT_STAT_1 0x11 ///< GPIO interrupt status 1
#define TCA8418_REG_GPIO_INT_STAT_2 0x12 ///< GPIO interrupt status 2
#define TCA8418_REG_GPIO_INT_STAT_3 0x13 ///< GPIO interrupt status 3
#define TCA8418_REG_GPIO_DAT_STAT_1 0x14 ///< GPIO data status 1
#define TCA8418_REG_GPIO_DAT_STAT_2 0x15 ///< GPIO data status 2
#define TCA8418_REG_GPIO_DAT_STAT_3 0x16 ///< GPIO data status 3
#define TCA8418_REG_GPIO_DAT_OUT_1 0x17  ///< GPIO data out 1
#define TCA8418_REG_GPIO_DAT_OUT_2 0x18  ///< GPIO data out 2
#define TCA8418_REG_GPIO_DAT_OUT_3 0x19  ///< GPIO data out 3
#define TCA8418_REG_GPIO_INT_EN_1 0x1A   ///< GPIO interrupt enable 1
#define TCA8418_REG_GPIO_INT_EN_2 0x1B   ///< GPIO interrupt enable 2
#define TCA8418_REG_GPIO_INT_EN_3 0x1C   ///< GPIO interrupt enable 3
#define TCA8418_REG_KP_GPIO_1 0x1D       ///< Keypad/GPIO select 1
#define TCA8418_REG_KP_GPIO_2 0x1E       ///< Keypad/GPIO select 2
#define TCA8418_REG_KP_GPIO_3 0x1F       ///< Keypad/GPIO select 3
#define TCA8418_REG_GPI_EM_1 0x20        ///< GPI event mode 1
#define TCA8418_REG_GPI_EM_2 0x21        ///< GPI event mode 2
#define TCA8418_REG_GPI_EM_3 0x22        ///< GPI event mode 3
#define TCA8418_REG_GPIO_DIR_1 0x23      ///< GPIO data direction 1
#define TCA8418_REG_GPIO_DIR_2 0x24      ///< GPIO data direction 2
#define TCA8418_REG_GPIO_DIR_3 0x25      ///< GPIO data direction 3
#define TCA8418_REG_GPIO_INT_LVL_1 0x26  ///< GPIO edge/level detect 1
#define TCA8418_REG_GPIO_INT_LVL_2 0x27  ///< GPIO edge/level detect 2
#define TCA8418_REG_GPIO_INT_LVL_3 0x28  ///< GPIO edge/level detect 3
#define TCA8418_REG_DEBOUNCE_DIS_1 0x29  ///< Debounce disable 1
#define TCA8418_REG_DEBOUNCE_DIS_2 0x2A  ///< Debounce disable 2
#define TCA8418_REG_DEBOUNCE_DIS_3 0x2B  ///< Debounce disable 3
#define TCA8418_REG_GPIO_PULL_1 0x2C     ///< GPIO pull-up disable 1
#define TCA8418_REG_GPIO_PULL_2 0x2D     ///< GPIO pull-up disable 2
#define TCA8418_REG_GPIO_PULL_3 0x2E     ///< GPIO pull-up disable 3
// #define TCA8418_REG_RESERVED          0x2F

//  FIELDS CONFIG REGISTER  1
#define TCA8418_REG_CFG_AI 0x80           ///< Auto-increment for read/write
#define TCA8418_REG_CFG_GPI_E_CGF 0x40    ///< Event mode config
#define TCA8418_REG_CFG_OVR_FLOW_M 0x20   ///< Overflow mode enable
#define TCA8418_REG_CFG_INT_CFG 0x10      ///< Interrupt config
#define TCA8418_REG_CFG_OVR_FLOW_IEN 0x08 ///< Overflow interrupt enable
#define TCA8418_REG_CFG_K_LCK_IEN 0x04    ///< Keypad lock interrupt enable
#define TCA8418_REG_CFG_GPI_IEN 0x02      ///< GPI interrupt enable
#define TCA8418_REG_CFG_KE_IEN 0x01       ///< Key events interrupt enable

//  FIELDS INT_STAT REGISTER  2
#define TCA8418_REG_STAT_CAD_INT 0x10      ///< Ctrl-alt-del seq status
#define TCA8418_REG_STAT_OVR_FLOW_INT 0x08 ///< Overflow interrupt status
#define TCA8418_REG_STAT_K_LCK_INT 0x04    ///< Key lock interrupt status
#define TCA8418_REG_STAT_GPI_INT 0x02      ///< GPI interrupt status
#define TCA8418_REG_STAT_K_INT 0x01        ///< Key events interrupt status

//  FIELDS  KEY_LCK_EC REGISTER 3
#define TCA8418_REG_LCK_EC_K_LCK_EN 0x40 ///< Key lock enable
#define TCA8418_REG_LCK_EC_LCK_2 0x20    ///< Keypad lock status 2
#define TCA8418_REG_LCK_EC_LCK_1 0x10    ///< Keypad lock status 1
#define TCA8418_REG_LCK_EC_KLEC_3 0x08   ///< Key event count bit 3
#define TCA8418_REG_LCK_EC_KLEC_2 0x04   ///< Key event count bit 2
#define TCA8418_REG_LCK_EC_KLEC_1 0x02   ///< Key event count bit 1
#define TCA8418_REG_LCK_EC_KLEC_0 0x01   ///< Key event count bit 0

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t TXD2 = 1;
static const uint8_t RXD2 = 2;

static const uint8_t SDA = 8;
static const uint8_t SCL = 9;

static const uint8_t SS = 12;
static const uint8_t MOSI = 14;
static const uint8_t MISO = 39;
static const uint8_t SCK = 40;

static const uint8_t G0 = 0;
static const uint8_t G1 = 1;
static const uint8_t G2 = 2;
static const uint8_t G3 = 3;
static const uint8_t G4 = 4;
static const uint8_t G5 = 5;
static const uint8_t G6 = 6;
static const uint8_t G7 = 7;
static const uint8_t G8 = 8;
static const uint8_t G9 = 9;
static const uint8_t G10 = 10;
static const uint8_t G11 = 11;
static const uint8_t G12 = 12;
static const uint8_t G13 = 13;
static const uint8_t G14 = 14;
static const uint8_t G15 = 15;
static const uint8_t G39 = 39;
static const uint8_t G40 = 40;
static const uint8_t G41 = 41;
static const uint8_t G42 = 42;
static const uint8_t G43 = 43;
static const uint8_t G44 = 44;
static const uint8_t G46 = 46;

static const uint8_t ADC1 = 7;
static const uint8_t ADC2 = 8;

#define SHIFT 0x80
#define KEY_LEFT_CTRL 0x80
#define KEY_LEFT_SHIFT 0x81
#define KEY_LEFT_ALT 0x82
#define KEY_FN 0xff
#define KEY_OPT 0x83
#define KEY_BACKSPACE 0x2a
#define KEY_TAB 0x2b
#define KEY_ENTER 0x42

static const uint8_t _kb_asciimap[128] = {
    0x00,          // NUL
    0x00,          // SOH
    0x00,          // STX
    0x00,          // ETX
    0x00,          // EOT
    0x00,          // ENQ
    0x00,          // ACK
    0x00,          // BEL
    KEY_BACKSPACE, // BS	Backspace
    KEY_TAB,       // TAB	Tab
    KEY_ENTER,     // LF	Enter
    0x00,          // VT
    0x00,          // FF
    0x00,          // CR
    0x00,          // SO
    0x00,          // SI
    0x00,          // DEL
    0x00,          // DC1
    0x00,          // DC2
    0x00,          // DC3
    0x00,          // DC4
    0x00,          // NAK
    0x00,          // SYN
    0x00,          // ETB
    0x00,          // CAN
    0x00,          // EM
    0x00,          // SUB
    0x00,          // ESC
    0x00,          // FS
    0x00,          // GS
    0x00,          // RS
    0x00,          // US

    0x2c,         //  ' '
    0x1e | SHIFT, // !
    0x34 | SHIFT, // "
    0x20 | SHIFT, // #
    0x21 | SHIFT, // $
    0x22 | SHIFT, // %
    0x24 | SHIFT, // &
    0x34,         // '
    0x26 | SHIFT, // (
    0x27 | SHIFT, // )
    0x25 | SHIFT, // *
    0x2e | SHIFT, // +
    0x36,         // ,
    0x2d,         // -
    0x37,         // .
    0x38,         // /
    0x27,         // 0
    0x1e,         // 1
    0x1f,         // 2
    0x20,         // 3
    0x21,         // 4
    0x22,         // 5
    0x23,         // 6
    0x24,         // 7
    0x25,         // 8
    0x26,         // 9
    0x33 | SHIFT, // :
    0x33,         // ;
    0x36 | SHIFT, // <
    0x2e,         // =
    0x37 | SHIFT, // >
    0x38 | SHIFT, // ?
    0x1f | SHIFT, // @
    0x04 | SHIFT, // A
    0x05 | SHIFT, // B
    0x06 | SHIFT, // C
    0x07 | SHIFT, // D
    0x08 | SHIFT, // E
    0x09 | SHIFT, // F
    0x0a | SHIFT, // G
    0x0b | SHIFT, // H
    0x0c | SHIFT, // I
    0x0d | SHIFT, // J
    0x0e | SHIFT, // K
    0x0f | SHIFT, // L
    0x10 | SHIFT, // M
    0x11 | SHIFT, // N
    0x12 | SHIFT, // O
    0x13 | SHIFT, // P
    0x14 | SHIFT, // Q
    0x15 | SHIFT, // R
    0x16 | SHIFT, // S
    0x17 | SHIFT, // T
    0x18 | SHIFT, // U
    0x19 | SHIFT, // V
    0x1a | SHIFT, // W
    0x1b | SHIFT, // X
    0x1c | SHIFT, // Y
    0x1d | SHIFT, // Z
    0x2f,         // [
    0x31,         // bslash
    0x30,         // ]
    0x23 | SHIFT, // ^
    0x2d | SHIFT, // _
    0x35,         // `
    0x04,         // a
    0x05,         // b
    0x06,         // c
    0x07,         // d
    0x08,         // e
    0x09,         // f
    0x0a,         // g
    0x0b,         // h
    0x0c,         // i
    0x0d,         // j
    0x0e,         // k
    0x0f,         // l
    0x10,         // m
    0x11,         // n
    0x12,         // o
    0x13,         // p
    0x14,         // q
    0x15,         // r
    0x16,         // s
    0x17,         // t
    0x18,         // u
    0x19,         // v
    0x1a,         // w
    0x1b,         // x
    0x1c,         // y
    0x1d,         // z
    0x2f | SHIFT, // {
    0x31 | SHIFT, // |
    0x30 | SHIFT, // }
    0x35 | SHIFT, // ~
    0             // DEL
};

#define HAS_RGB_LED 1
#define LED_TYPE SK6812
#define LED_ORDER GRB
#define LED_TYPE_IS_RGBW 1
#define LED_COUNT 1
#define LED_COLOR_STEP 15

// Deepsleep
#define DEEPSLEEP_WAKEUP_PIN 0
#define DEEPSLEEP_PIN_ACT LOW

#endif /* Pins_Arduino_h */
