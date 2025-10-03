#include "core/mykeyboard.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include "hw_config.h"

extern TimerHandle_t nes_timer;
extern int16_t frame_scaling;

extern "C" {

/* controller is GPIO */
#if defined(HW_CONTROLLER_GPIO)

extern void controller_init() {
#if defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK)
    pinMode(HW_CONTROLLER_GPIO_UP_DOWN, INPUT);
    pinMode(HW_CONTROLLER_GPIO_LEFT_RIGHT, INPUT);
#else  /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
    pinMode(HW_CONTROLLER_GPIO_UP, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_DOWN, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_LEFT, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_RIGHT, INPUT_PULLUP);
#endif /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
    pinMode(HW_CONTROLLER_GPIO_SELECT, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_START, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_A, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_B, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_X, INPUT_PULLUP);
    pinMode(HW_CONTROLLER_GPIO_Y, INPUT_PULLUP);
}

extern uint32_t controller_read_input() {
    uint32_t u, d, l, r, s, t, a, b, x, y;

#if defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK)

#if defined(HW_CONTROLLER_GPIO_REVERSE_UD)
    int joyY = 4095 - analogRead(HW_CONTROLLER_GPIO_UP_DOWN);
#else  /* !defined(HW_CONTROLLER_GPIO_REVERSE_UD) */
    int joyY = analogRead(HW_CONTROLLER_GPIO_UP_DOWN);
#endif /* !defined(HW_CONTROLLER_GPIO_REVERSE_UD) */

#if defined(HW_CONTROLLER_GPIO_REVERSE_LF)
    int joyX = 4095 - analogRead(HW_CONTROLLER_GPIO_LEFT_RIGHT);
#else  /* !defined(HW_CONTROLLER_GPIO_REVERSE_LF) */
    int joyX = analogRead(HW_CONTROLLER_GPIO_LEFT_RIGHT);
#endif /* !defined(HW_CONTROLLER_GPIO_REVERSE_LF) */

    // Serial.printf("joyX: %d, joyY: %d\n", joyX, joyY);
#if defined(ARDUINO_ODROID_ESP32)

    if (joyY > 2048 + 1024) {
        u = 0;
        d = 1;
    } else if (joyY > 1024) {
        u = 1;
        d = 0;
    } else {
        u = 1;
        d = 1;
    }
    if (joyX > 2048 + 1024) {
        l = 0;
        r = 1;
    } else if (joyX > 1024) {
        l = 1;
        r = 0;
    } else {
        l = 1;
        r = 1;
    }

#else /* !defined(ARDUINO_ODROID_ESP32) */

    if (joyY > 2048 + 1024) {
        u = 1;
        d = 0;
    } else if (joyY < 1024) {
        u = 0;
        d = 1;
    } else {
        u = 1;
        d = 1;
    }

    if (joyX > 2048 + 1024) {
        l = 1;
        r = 0;
    } else if (joyX < 1024) {
        l = 0;
        r = 1;
    } else {
        l = 1;
        r = 1;
    }

#endif /* !defined(ARDUINO_ODROID_ESP32) */
#else  /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
    u = digitalRead(HW_CONTROLLER_GPIO_UP);
    d = digitalRead(HW_CONTROLLER_GPIO_DOWN);
    l = digitalRead(HW_CONTROLLER_GPIO_LEFT);
    r = digitalRead(HW_CONTROLLER_GPIO_RIGHT);
#endif /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */

    s = digitalRead(HW_CONTROLLER_GPIO_SELECT);
    t = digitalRead(HW_CONTROLLER_GPIO_START);
    a = digitalRead(HW_CONTROLLER_GPIO_A);
    b = digitalRead(HW_CONTROLLER_GPIO_B);
    x = digitalRead(HW_CONTROLLER_GPIO_X);
    y = digitalRead(HW_CONTROLLER_GPIO_Y);

    return 0xFFFFFFFF ^ ((!u << 0) | (!d << 1) | (!l << 2) | (!r << 3) | (!s << 4) | (!t << 5) | (!a << 6) |
                         (!b << 7) | (!x << 8) | (!y << 9));
}

/* controller is I2C M5Stack CardKB */
#elif defined(HW_CONTROLLER_I2C_M5CARDKB)

#include <Wire.h>

#define I2C_M5CARDKB_ADDR 0x5f
#define READ_BIT I2C_MASTER_READ /*!< I2C master read */
#define ACK_CHECK_EN 0x1         /*!< I2C master will check ack from slave */
#define NACK_VAL 0x1             /*!< I2C nack value */

extern void controller_init() { Wire.begin(); }

extern uint32_t controller_read_input() {
    uint32_t value = 0xFFFFFFFF;

    Wire.requestFrom(I2C_M5CARDKB_ADDR, 1);
    while (Wire.available()) {
        char c = Wire.read(); // receive a byte as characterif
        if (c != 0) {
            switch (c) {
                case 181: // up
                    value ^= (1 << 0);
                    break;
                case 182: // down
                    value ^= (1 << 1);
                    break;
                case 180: // left
                    value ^= (1 << 2);
                    break;
                case 183: // right
                    value ^= (1 << 3);
                    break;
                case ' ': // select
                    value ^= (1 << 4);
                    break;
                case 13: // enter -> start
                    value ^= (1 << 5);
                    break;
                case 'k': // A
                    value ^= (1 << 6);
                    break;
                case 'l': // B
                    value ^= (1 << 7);
                    break;
                case 'o': // X
                    value ^= (1 << 8);
                    break;
                case 'p': // Y
                    value ^= (1 << 9);
                    break;
            }
        }
    }

    return value;
}

/* controller is I2C BBQ10Keyboard */
#elif defined(HW_CONTROLLER_I2C_BBQ10KB)

#include <BBQ10Keyboard.h>
#include <Wire.h>
#include <globals.h>
BBQ10Keyboard keyboard;
static uint32_t value = 0xFFFFFFFF;

extern void controller_init() {
    Wire.begin();
    keyboard.begin();
    keyboard.setBacklight(0.2f);
}

extern uint32_t controller_read_input() {

    int keyCount = keyboard.keyCount();
    while (keyCount--) {
        const BBQ10Keyboard::KeyEvent key = keyboard.keyEvent();
        String state = "pressed";
        if (key.state == BBQ10Keyboard::StateLongPress) state = "held down";
        else if (key.state == BBQ10Keyboard::StateRelease) state = "released";

        // Serial.printf("key: '%c' (dec %d, hex %02x) %s\r\n", key.key, key.key, key.key, state.c_str());

        uint32_t bit = 0;
        if (key.key != 0) {
            switch (key.key) {
                case 'w': // up
                    bit = (1 << 0);
                    break;
                case 'z': // down
                    bit = (1 << 1);
                    break;
                case 'a': // left
                    bit = (1 << 2);
                    break;
                case 'd': // right
                    bit = (1 << 3);
                    break;
                case ' ': // select
                    bit = (1 << 4);
                    break;
                case 10: // enter -> start
                    bit = (1 << 5);
                    break;
                case 'k': // A
                    bit = (1 << 6);
                    break;
                case 'l': // B
                    bit = (1 << 7);
                    break;
                case 'o': // X
                    bit = (1 << 8);
                    break;
                case 'p': // Y
                    bit = (1 << 9);
                    break;
            }
            if (key.state == BBQ10Keyboard::StatePress) {
                value ^= bit;
            } else if (key.state == BBQ10Keyboard::StateRelease) {
                value |= bit;
            }
        }
    }

    return value;
}

#else /* no controller defined */

static int16_t touch_value_max = 50;
static int8_t select_function = 0;

extern void controller_init() {
    Serial.printf("controller_init\n");
    touch_value_max = touchRead(32);
}

extern uint32_t controller_read_input() {
    uint32_t value = 0xFFFFFFFF;

    bool upPressed = (digitalRead(UP_BTN) == LOW);
    bool selPressed = (digitalRead(SEL_BTN) == LOW);
    bool dwPressed = (digitalRead(DW_BTN) == LOW);
    bool leftPressed = touchRead(32) < (touch_value_max - 10);

    if (upPressed) {
        xTimerStop(nes_timer, 0);
        delay(300);
        while (digitalRead(UP_BTN) != LOW) {
            if (digitalRead(SEL_BTN) == LOW) {
                value ^= (1 << 5);
                break;
            } else if (digitalRead(DW_BTN) == LOW) {
                frame_scaling = frame_scaling == 1 ? 0 : 1;
                break;
            } else if (touchRead(32) < (touch_value_max - 10)) {
                select_function = select_function == 0 ? 1 : 0;
                break;
            }
            delay(20);
        }
        delay(300);
        xTimerStart(nes_timer, 0);
    }

    if (false) { value ^= (1 << 0); }                              // up
    if (false) { value ^= (1 << 1); }                              // down
    if (leftPressed) { value ^= (1 << 2); }                        // left
    if (dwPressed) { value ^= (1 << 3); }                          // right
    if (false) { value ^= (1 << 4); }                              // select
    if (false) { value ^= (1 << 5); }                              // start
    if (selPressed && select_function == 0) { value ^= (1 << 6); } // A
    if (selPressed && select_function == 1) { value ^= (1 << 7); } // B
    if (false) { value ^= (1 << 8); }                              // X
    if (false) { value ^= (1 << 9); }                              // Y

    return value;
}

#endif /* no controller defined */
}
