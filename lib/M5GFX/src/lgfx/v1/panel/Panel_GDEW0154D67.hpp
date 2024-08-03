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

#include "Panel_HasBuffer.hpp"
#include "../misc/range.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_GDEW0154D67 : public Panel_HasBuffer
  {
    Panel_GDEW0154D67(void);

    bool init(bool use_reset) override;

    color_depth_t setColorDepth(color_depth_t depth) override;

    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool flg) override;

    void waitDisplay(void) override;
    bool displayBusy(void) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }

    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;

  private:

    static constexpr unsigned long _refresh_msec = 256;

    range_rect_t _range_old;
    unsigned long _send_msec = 0;
    epd_mode_t _last_epd_mode;
    bool _initialize_seq;
    bool _need_flip_draw;
    bool _epd_frame_switching = false;
    bool _epd_frame_back = false;

    size_t _get_buffer_length(void) const override;

    bool _wait_busy(uint32_t timeout = 2048);
    void _draw_pixel(uint_fast16_t x, uint_fast16_t y, uint32_t value);
    bool _read_pixel(uint_fast16_t x, uint_fast16_t y);
    void _update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye);
    void _exec_transfer(uint32_t cmd, const range_rect_t& range, bool invert = false);
    void _after_wake(void);

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
          0x12, 0 + CMD_INIT_DELAY, 10, // SW Reset + 10 msec delay.
          0x01, 3, 199, 0, 0,   // set gate driver output  199+1=200gate
          0x11, 1, 0x03,        // Define data entry sequence x+ y+
          0x3C, 1, 0x05,        // BorderWavefrom
          0x18, 1, 0x80,        // Read built-in temperature sensor
          0x0C, 4, 0x8B, 0x9C, 0x96, 0x0F,  // booster enable with phase 1
          0x21, 1, 0x00,
          0x22, 1, 0xF8,        // Display update seq opt
          // 0x47, 1 + CMD_INIT_DELAY, 0xF7, 100,
          // 0x20, 0 + CMD_INIT_DELAY, 100,
          // 0x47, 1 + CMD_INIT_DELAY, 0xF7, 100,
          0xFF, 0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------
 }
}
