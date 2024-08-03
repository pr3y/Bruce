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

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_FrameBufferBase : public Panel_Device
  {
  public:

    bool init(bool use_reset) override;
    void beginTransaction(void) override {}
    void endTransaction(void) override {}
    void setRotation(uint_fast8_t r) override;

    void initDMA(void) override {}
    void waitDMA(void) override {}
    bool dmaBusy(void) override { return false; }
    void waitDisplay(void) override {}
    bool displayBusy(void) override { return false; }
    color_depth_t setColorDepth(color_depth_t depth) override { _write_depth = depth; _read_depth = depth; return depth; }

    void setInvert(bool invert) override { _invert = invert; }
    void setSleep(bool flg) override {}
    void setPowerSave(bool flg) override {}

    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override {}

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeBlock(uint32_t rawcolor, uint32_t length) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param) override;
    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }

    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;

  protected:
    uint8_t** _lines_buffer = nullptr;
    uint16_t _xpos, _ypos;

    void _rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty);

  };

//----------------------------------------------------------------------------
 }
}
