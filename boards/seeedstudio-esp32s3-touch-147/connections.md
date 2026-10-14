# Seeed Studio XIAO 1.47" Touch Display (ESP32-S3)

Env: `seeedstudio-esp32s3-touch-147`

A display board for the XIAO series, sold mated to a **XIAO ESP32-S3 Plus**
(ESP32-S3R8 — 16 MB flash, 8 MB octal PSRAM). It is Plus-only: LCD RST,
backlight and both user buttons sit on pads D17/D18/D19/D15, which a plain
XIAO ESP32-S3 does not expose (for that module see
`boards/seeedstudio-xiao-esp32s3`, a headless build). The display board itself has no
MCU; everything below is a XIAO pad, so the pinout is only meaningful together
with the XIAO ESP32-S3 Plus pad map.

- Product wiki: <https://wiki.seeedstudio.com/getting_started_1.47_inch_touch_display_esp32s3/>
- Vendor library (source of the panel/touch facts below): [Seeed_GFX2](https://github.com/Seeed-Studio/Seeed_GFX2)

## D-number → GPIO

The wiki documents this board in XIAO **D-numbers**; the firmware needs raw
GPIOs. The mapping is from `variants/XIAO_ESP32S3_Plus/pins_arduino.h` in
arduino-esp32 and is **not** the same as the plain XIAO ESP32-S3 above D10.

| D | GPIO | | D | GPIO |
|---|---|---|---|---|
| D0 | 1 | | D10 | 9 |
| D1 | 2 | | D11 | 38 |
| D2 | 3 | | D12 | 39 |
| D3 | 4 | | D13 | 40 |
| D4 | 5 | | D14 | 41 |
| D5 | 6 | | D15 | 42 |
| D6 | 43 | | D16 | 10 |
| D7 | 44 | | D17 | 13 |
| D8 | 7 | | D18 | 12 |
| D9 | 8 | | D19 | 11 |

## Pinout

| Function | D | GPIO | Notes |
|---|---|---|---|
| LCD SCK | D8 | 7 | shared SPI bus with the microSD |
| LCD MOSI | D10 | 9 | shared |
| LCD MISO | D9 | 8 | not used by the panel; shared, the SD needs it |
| LCD CS | D2 | 3 | |
| LCD DC | D3 | 4 | |
| LCD RST | D17 | 13 | **shared with the touch controller's RST** |
| LCD backlight | D18 | 12 | plain PWM, `hal_bright_*` |
| Touch SDA | D4 | 5 | |
| Touch SCL | D5 | 6 | |
| Touch INT | D7 | 44 | active low; also UART0 RX, free because the console is USB CDC |
| microSD CS | D6 | 43 | also UART0 TX — see below |
| Battery sense | D16 | 10 | the XIAO's own `ADC_BAT`, 316K/160K divider |
| User button 1 | D19 | 11 | `USR1`, active low → Next / Sel |
| User button 2 | D15 | 42 | `USR2`, active low → Prev / Esc |
| LED | — | 21 | XIAO's `LED_BUILTIN`, unused by Bruce |

Present on the board but unused by Bruce: a LSM6DS3 6-axis IMU (same
I2C bus as touch, INT on D14/GPIO41), a PDM microphone (CLK D0/GPIO1, DATA
D1/GPIO2) and an I2S output (SD D11/GPIO38, SCK D12/GPIO39, WS D13/GPIO40).

## Bus sharing

**SPI** — the panel and the microSD share one bus (SCK 7, MOSI 9, MISO 8).
`sd_functions.cpp` detects this from `TFT_MOSI == SDCARD_MOSI` and mounts the
card on the already-initialized global `SPI` object instead of opening a second
`SPIClass`. `_setup_gpio()` parks `SDCARD_CS` high before the panel comes up so
the card can't answer the panel's traffic.

**I2C** — touch and the (unused) IMU share `Wire` on SDA 5 / SCL 6.
`hal_touch_init()` is what calls `Wire.begin()` on this board.

**Reset** — the panel and the touch controller share GPIO13. This is why
`hal_touch_init()` runs from `_post_setup_gpio()` (after `tft->begin()`) with
`DeviceTouch.pin_rst` left at `-1`: the panel's own reset has already pulsed
the line, and pulsing it again would blank the initialized panel.

**UART0** — GPIO43/44 (D6/D7) are UART0 TX/RX, and the board uses them as SD CS
and touch INT. That is fine only because the console is the ESP32-S3 native USB
(`ARDUINO_USB_CDC_ON_BOOT=1`); there is no usable serial console on this board.

## Display

JD9853A, 172×320 IPS, SPI, **BGR**, inversion **off**.

Bruce drives it with **`TFT_DISPLAY_DRIVER_N=2` (`Arduino_ST7796`)**
rather than the ST7789 you would expect from a 172×320 panel. Two reasons, both
from the vendor's own driver (`Seeed_GFX2/src/driver/tft/Driver_JD9853A.*`):

1. The glass is BGR. Arduino_GFX's `Arduino_ST7789` hard-codes
   `ST7789_MADCTL_RGB` (0x00) in every rotation and has no BGR path, so red and
   blue would swap. `Arduino_ST7796` sets `ST7796_MADCTL_BGR` (0x08) in all of
   them. `TFT_RGB_ORDER` does **not** help here — the Arduino_GFX backend
   ignores `displayConfig.rgbOrder` (only the LovyanGFX backend reads it).
2. Seeed's own driver notes that an `Arduino_ST7796` initialization is one of
   the two known-working paths on this glass (the other being their
   ST7789-compatible sequence). The ST7796 init table is panel-size agnostic —
   COLMOD, gamma, power, SLPOUT, DISPON — so it carries nothing 320×480
   specific into a 172×320 panel.

`TFT_IPS=0` because `Arduino_TFT::tftInit()` ends in `invertDisplay(false)`,
which sends `INVOFF` when `ips` is false — the state the vendor driver forces
explicitly for this glass.

### Window offsets

The controller RAM is 240 wide and the glass is the centre 172 columns, so the
column axis carries a 34 px offset ((240−172)/2) and the row axis none. With
`TFT_COL_OFS1=34 / TFT_ROW_OFS1=0 / TFT_COL_OFS2=34 / TFT_ROW_OFS2=0`,
`Arduino_TFT::setRotation()` lands the 34 on CASET at rotation 0 and on RASET
at rotations 1 and 3 — correct in each case, since MADCTL's MV bit swaps which
command addresses the narrow axis.

### Rotation: this glass wants Arduino_GFX's 4-7 family

Arduino_GFX offers eight rotations: 0-3 are the four 90 degrees steps measured
from `MADCTL = 0`, and 4-7 are the same four steps measured from
`MADCTL = MX`. Those are two different families -- one is the other mirrored --
and only one matches how the glass is actually bonded.

Seeed's driver pins the answer: it sets `MADCTL = MX | BGR` (0x48) for its
portrait and calls that value "verified" on the XIAO Display Board. A family
based at `MX` is Arduino_GFX's **4-7**, which map one-for-one onto the vendor's
rotations:

| vendor rotation | MADCTL | Arduino_GFX |
|---|---|---|
| 0 (portrait) | `MX` | 4 |
| 1 (landscape) | `MV` | **7** |
| 2 (portrait) | `MY` | 6 |
| 3 (landscape) | `MX\|MY\|MV` | 5 |

**Caveat, read this before blaming the panel:** Bruce only ever hands the
driver a rotation in `0..3` (`bruceConfigPins.rotation`, the four entries
`gsetRotation()` offers), so this build runs the glass in the *mirrored*
family. Landscape still fills the screen correctly at `ROTATION=3`, but the
image is mirrored versus the vendor orientation (text reads backwards).

Fixing it properly needs a rotation-family offset in the shared code -- an
`ROT_OFFSET` added to `TFT_ROTATION`, to `bruceConfigPins.rotation`'s default,
to `validateRotationValue()` and to `gsetRotation()`'s window, defaulting to 0
so no other board changes. That is deliberately **not** part of this board port;
until it lands, the rotation menu offers 1 and 3 instead of 7 and 5.

## Touch

AXS5106L at a fixed I2C address **0x63**. There is no `TOUCH_CTRL_*` case for
this chip in `src/hal/inputs/touch.cpp` and none is added by this port, so the
whole driver lives in this board's `interface.cpp` -- it is a single register
read: a 14-byte report from 0x01, where byte 1 is the touch count and each
point packs 12-bit X/Y as a low nibble plus a low byte. The point then goes to
the shared `hal_touch_apply()`, which is what publishes it to
`touchPoint`/`touchHeatMap()` like every other touch board.

INT (GPIO44) is treated as a hint, not a gate: the vendor driver notes some
revisions leave it high with a finger down, so a high INT only defers the read
to a ~60 Hz poll. Its RST line is the panel's RST, so touch is brought up in
`_post_setup_gpio()` (after the panel) and never pulses RST itself -- that
would blank an initialized panel.

Coordinates come out panel-native (portrait) and `axsReadRotated()` maps them
with the usual swap-then-mirror table, indexed by the current rotation.

## Verification status

Confirmed working on hardware **in the Launcher port this was ported from**:
panel (colour order, inversion, window offsets), touch, both buttons,
backlight, and microSD on the shared SPI bus. Not yet re-tested as a Bruce
build -- and see the rotation caveat above, which is a known difference.

Two numbers are still only derived, not measured — fix them here if they ever
look wrong:

- **Battery scale.** `ANALOG_BAT_MULTIPLIER=2.975f` comes from the wiki's
  stated 316K/160K divider ((316+160)/160), not from a meter.
- **SPI clock.** `TFT_PREF_SPEED=40000000` is the repo's conservative default
  for a shared SPI bus, not a figure from the JD9853A datasheet.

The portrait entries (indices 0 and 2) of the touch mirror/swap table are also
untested on hardware.

## Inputs

Two user buttons, via `HAS_2_BUTTONS` + `BUTTONS_IDF_COMPONENT` (the
vendored `lib/ESP32_Button` component, no `lib_deps` entry needed):

- **USR1** (GPIO11): short click → Next, double click or hold → Sel
- **USR2** (GPIO42): short click → Prev, double click or hold → Esc

plus the touchscreen. The XIAO's own BOOT button (GPIO0) is not used by the
Bruce — it may not be reachable once the boards are stacked.

## Power

No PMIC and no fuel gauge: a 2-pin JST LiPo header read straight off the
XIAO's `ADC_BAT` (GPIO10) through the 316K/160K divider, handled by the generic
`ANALOG_BAT_PIN`/`ANALOG_BAT_MULTIPLIER` path in `src/hal/power/gauge.cpp` (so
this board defines no `getBattery()` of its own). There is no charge-status line, so
`isCharging()` is left at its default.

`powerOff()` turns the backlight off and deep-sleeps with an ext0 wake on USR1
(GPIO11, active low).

## Flashing

ESP32-S3, so the bootloader goes at **0x0**:

```
esptool --chip esp32s3 write-flash \
  0x0     .pio/build/seeedstudio-esp32s3-touch-147/bootloader.bin \
  0x8000  .pio/build/seeedstudio-esp32s3-touch-147/partitions.bin \
  0x10000 .pio/build/seeedstudio-esp32s3-touch-147/firmware.bin
```
