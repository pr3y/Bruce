#include "core/powerSave.h"
#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include "hal/inputs/touch.h"
#include <Arduino.h>
#include <Wire.h>
#include <esp_sleep.h>
#include <interface.h>

// Seeed Studio XIAO 1.47" Touch Display, on a XIAO ESP32-S3 (Plus).
// GPIOs below are the ESP32-S3 numbers; the wiki documents the board in XIAO
// D-numbers, and connections.md carries the full D-number <-> GPIO table.
#define TOUCH_SDA 5  // D4
#define TOUCH_SCL 6  // D5
#define TOUCH_INT 44 // D7
#define BTN_USR1 11  // D19
#define BTN_USR2 42  // D15

// --- AXS5106L capacitive touch ------------------------------------------
// Kept board-local: src/hal/inputs/touch.cpp has no TOUCH_CTRL_* case for this
// chip, and the whole driver is a single register read -- a 14-byte report
// from 0x01, whose byte 1 is the touch count and whose first point is a 12-bit
// X/Y packed as low nibble + low byte. Its RST line is the panel's RST, so it
// is never pulsed here (that would blank an already-initialized panel).
#define AXS5106L_ADDRESS 0x63
#define AXS5106L_DATA_REG 0x01
#define AXS5106L_REPORT_LEN 14

static bool touch_OK = false;

static bool axsReadReport(uint8_t *buf) {
    Wire.beginTransmission(AXS5106L_ADDRESS);
    Wire.write(AXS5106L_DATA_REG);
    if (Wire.endTransmission() != 0) return false;
    if (Wire.requestFrom((uint8_t)AXS5106L_ADDRESS, (size_t)AXS5106L_REPORT_LEN) < AXS5106L_REPORT_LEN)
        return false;
    for (uint8_t i = 0; i < AXS5106L_REPORT_LEN; ++i) buf[i] = (uint8_t)Wire.read();
    return true;
}

// Reports the panel's native (portrait, 172x320) coordinates, so the caller
// rotates them. INT is an active-low hint only: some revisions of this module
// leave it high even with a finger down, so a high INT only defers the read to
// the ~60Hz poll below rather than skipping it (the vendor driver falls back
// the same way).
static bool axsGetPoint(int16_t &x, int16_t &y) {
    static uint32_t lastPoll = 0;
    const uint32_t now = millis();
    if (digitalRead(TOUCH_INT) != LOW && (uint32_t)(now - lastPoll) < 16) return false;
    lastPoll = now;

    uint8_t b[AXS5106L_REPORT_LEN];
    if (!axsReadReport(b)) return false;
    if (b[1] == 0) return false; // no finger down
    x = (int16_t)(((b[2] & 0x0F) << 8) | b[3]);
    y = (int16_t)(((b[4] & 0x0F) << 8) | b[5]);
    return true;
}

// Native portrait point -> the screen frame of the current rotation. Same
// swap-then-mirror order (and the same per-rotation table) the shared touch HAL
// applies for the other host-rotated controllers.
static bool axsReadRotated(BruceTouchPoint &out) {
    int16_t x = 0, y = 0;
    if (!axsGetPoint(x, y)) return false;

    const uint8_t r = bruceConfigPins.rotation & 0x03;
    //                            rot: 0      1      2      3
    static const bool swapXY[4] = {false, true, false, true};
    static const bool mirrorX[4] = {false, false, true, true};
    static const bool mirrorY[4] = {false, true, true, false};

    // Panel size for this rotation, without the touch footer main.cpp carves
    // out of tftHeight -- the controller knows nothing about that band.
    const int16_t screenW = (r & 1) ? TFT_HEIGHT : TFT_WIDTH;
    const int16_t screenH = (r & 1) ? TFT_WIDTH : TFT_HEIGHT;

    if (swapXY[r]) {
        const int16_t t = x;
        x = y;
        y = t;
    }
    if (mirrorX[r]) x = screenW - x;
    if (mirrorY[r]) y = screenH - y;

    out.x = x;
    out.y = y;
    out.pressed = true;
    return true;
}

/***************************************************************************************
** Function:    _setup_gpio()
** Location:    main.cpp
** Description: initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // The microSD shares the panel's SPI bus: park its CS high before the
    // panel starts talking, so it can't answer the panel's traffic. GPIO43 is
    // also UART0 TX, which is why the console is on USB CDC on this board.
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);

    // Attach here, not in _post_setup_gpio(): main.cpp sets the brightness
    // right after tft.init() and before _post_setup_gpio(), and leaving the
    // backlight at duty 0 until then hides the panel's init garbage.
    hal_bright_attach(TFT_BL);

    hal_buttons_init_2(DeviceButtons{BTN_USR1, BTN_USR2}, 600);
}

/***************************************************************************************
** Function:    _post_setup_gpio()
** Location:    main.cpp
** Description: second stage gpio setup, run after TFT and storage initialization
***************************************************************************************/
void _post_setup_gpio() {
    // After the panel: touch shares its RST line, so it only works once the
    // panel's own reset has run.
    pinMode(TOUCH_INT, INPUT_PULLUP);
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    bruceConfigPins.sys_i2c = {(gpio_num_t)TOUCH_SDA, (gpio_num_t)TOUCH_SCL};
    // Probe with a real register read: the chip doesn't ACK an empty write on
    // every revision, so beginTransmission/endTransmission alone would report
    // a working controller as missing.
    uint8_t probe[AXS5106L_REPORT_LEN];
    touch_OK = axsReadReport(probe);
    Serial.printf("AXS5106L touch: %s\n", touch_OK ? "started" : "not found");
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { hal_bright_set(TFT_BL, brightval); }

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    if (touch_OK) {
        static unsigned long tm = 0;
        BruceTouchPoint t;
        if (axsReadRotated(t)) {
            if ((millis() - tm) > 200 || LongPress) {
                tm = millis();
                if (!hal_touch_apply(t)) return;
            }
        }
    }

    // USR1 short click -> Next, double click/hold -> Sel;
    // USR2 short click -> Prev, double click/hold -> Esc.
    hal_buttons_poll_2();
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    _setBrightness(0);
    // No PMIC on this board -- deep sleep, waking on USR1 (active low).
    esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BTN_USR1), LOW);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_deep_sleep_start();
}
