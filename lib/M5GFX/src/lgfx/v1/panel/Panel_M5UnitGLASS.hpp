/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include "Panel_Device.hpp"
#include "Panel_SSD1306.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_M5UnitGlass : public Panel_1bitOLED
  {
    static constexpr const uint8_t REG_INDEX_CLEAR            = 0x00;
    static constexpr const uint8_t REG_INDEX_SHOW             = 0x10;
    static constexpr const uint8_t REG_INDEX_DRAW_STRING      = 0x20;
    static constexpr const uint8_t REG_INDEX_DRAW_POINT       = 0x30;
    static constexpr const uint8_t REG_INDEX_DRAW_LINE        = 0x40;
    static constexpr const uint8_t REG_INDEX_DRAW_CIRCLE      = 0x50;
    static constexpr const uint8_t REG_INDEX_INVERT           = 0x60;
    static constexpr const uint8_t REG_INDEX_DISPLAY_ON_OFF   = 0x70;
    static constexpr const uint8_t REG_INDEX_STRING_BUFFER    = 0x80;
    static constexpr const uint8_t REG_INDEX_PICTURE_BUFFER   = 0x90;
    static constexpr const uint8_t REG_INDEX_COLOR_INVERT     = 0xA0;
    static constexpr const uint8_t REG_INDEX_DRAW_PICTURE     = 0xB0;
    static constexpr const uint8_t REG_INDEX_BUZZER           = 0xC0;
    static constexpr const uint8_t REG_INDEX_READ_KEY         = 0xD0;
    static constexpr const uint8_t REG_INDEX_FIRMWARE_VERSION = 0xFE;

    static constexpr const uint8_t PANEL_HEIGHT_MAX = 64;

    Panel_M5UnitGlass() : Panel_1bitOLED()
    {
      _cfg.bus_shared = false;
      _cfg.panel_width = 128;
      _cfg.memory_width = 128;
      _cfg.panel_height = 64;
      _cfg.memory_height = 64;
    }

    void beginTransaction(void) override {}
    void endTransaction(void) override {}

    bool init(bool use_reset) override;
    void setSleep(bool flg) override;
    void setInvert(bool invert) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

    uint8_t getKey(uint_fast8_t idx);
    uint8_t getFirmwareVersion(void);
    void setBuzzer(uint16_t freq, uint8_t duty);
    void setBuzzerEnable(bool enable);

  protected:

    // UnitGLASS側が応答可能になるまでの待機終了予定時間(Showの後、数msec応答できなくなるため)
    uint32_t _msec_busy = 0;

    // 変更のあった箇所を記録するビットフラグ配列。
    // 1ビットあたり 8x8 pixelを割当。
    // 配列1要素で 16x8 = 128pixel 。
    // 要素数が8あるので 縦 8x8 = 64pixel。これで 128x64全体をカバーできる。
    uint16_t _modified_flags[PANEL_HEIGHT_MAX >> 3];

    bool _enable_buzzer_flg = false;

    void _update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye) override;

    void waitBusy(void);
  };

//----------------------------------------------------------------------------
 }
}
