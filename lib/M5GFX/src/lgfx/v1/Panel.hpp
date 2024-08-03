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

#include <stdint.h>

#include "../internal/alloca.h"
#include "misc/enum.hpp"
#include "misc/colortype.hpp"
#include "misc/pixelcopy.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct pixelcopy_t;

  struct IPanel
  {
  protected:
    uint32_t _start_count = 0;
    union
    {
      uint32_t _xsxe = ~0;
      struct
      {
        uint16_t _xs;
        uint16_t _xe;
      };
    };
    union
    {
      uint32_t _ysye = ~0;
      struct
      {
        uint16_t _ys;
        uint16_t _ye;
      };
    };
    
    uint16_t _width = 0;
    uint16_t _height = 0;
    union
    {
      color_depth_t _write_depth = color_depth_t::rgb565_2Byte;
      struct
      {
        uint8_t _write_bits;
        uint8_t _write_attrib;
      };
    };
    union
    {
      color_depth_t _read_depth  = color_depth_t::rgb565_2Byte;
      struct
      {
        uint8_t _read_bits;
        uint8_t _read_attrib;
      };
    };
    uint8_t _rotation = 0;
    epd_mode_t _epd_mode = (epd_mode_t)0;  // EPDでない場合は0。それ以外の場合はEPD描画モード;
    bool _invert = false;
    bool _auto_display = false;

  public:
    IPanel(void) = default;
    virtual ~IPanel(void) = default;

    void startWrite(bool transaction = true) { if (1 == ++_start_count && transaction) { beginTransaction(); } }
    void endWrite(void) { if (_start_count) {  if (0 == --_start_count) { if (_auto_display) { display(0,0,0,0); } endTransaction(); } } }
    uint32_t getStartCount(void) const { return _start_count; }
    color_depth_t getWriteDepth(void) const { return _write_depth; }
    color_depth_t getReadDepth(void) const { return _read_depth; }
    uint8_t getRotation(void) const { return _rotation; }
    bool getInvert(void) const { return _invert; }

    uint16_t width(void) const { return _width; }
    uint16_t height(void) const { return _height; }
    epd_mode_t getEpdMode(void) const { return _epd_mode; }
    void setEpdMode(epd_mode_t epd_mode) { if (_epd_mode && epd_mode) _epd_mode = epd_mode; }
    bool isEpd(void) const { return _epd_mode; }
    bool getAutoDisplay(void) const { return _auto_display; }
    void setAutoDisplay(bool auto_display) { _auto_display = auto_display; }

    virtual void beginTransaction(void) = 0;
    virtual void endTransaction(void) = 0;

    virtual void setBrightness(uint8_t brightness) { (void)brightness; };

    virtual color_depth_t setColorDepth(color_depth_t depth) = 0;

    virtual void setInvert(bool invert) = 0;
    virtual void setRotation(uint_fast8_t r) = 0;
    virtual void setSleep(bool flg_sleep) = 0;
    virtual void setPowerSave(bool flg_idle) = 0;

    virtual void writeCommand(uint32_t cmd, uint_fast8_t length) = 0;
    virtual void writeData(uint32_t data, uint_fast8_t length) = 0;

    virtual void initDMA(void) = 0;
    virtual void waitDMA(void) = 0;
    virtual bool dmaBusy(void) = 0;
    virtual void waitDisplay(void) = 0;
    virtual bool displayBusy(void) = 0;
    virtual void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) = 0;
    virtual bool isReadable(void) const = 0;
    virtual bool isBusShared(void) const = 0;

    virtual void writeBlock(uint32_t rawcolor, uint32_t len) = 0;
    virtual void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) = 0;
    virtual void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) = 0;
    virtual void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) = 0;
    virtual void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) = 0;
    virtual void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param) = 0;
    virtual void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) = 0;

    virtual uint32_t readCommand(uint_fast16_t cmd, uint_fast8_t index = 0, uint_fast8_t length = 4) = 0;
    virtual uint32_t readData(uint_fast8_t index = 0, uint_fast8_t length = 4) = 0;
    virtual void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) = 0;
    virtual void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) = 0;

    /// Obtains the current scanning line position.
    /// @return -1=unsupported. / 0~height= current scanline position.
    virtual int32_t getScanLine(void) { return -1; }

    virtual void writeFillRectAlphaPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t argb8888)
    {
      effect(x, y, w, h, effect_fill_alpha ( argb8888_t { argb8888 } ) );
    }

    template<typename TFunc>
    void effect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, TFunc&& effector)
    {
      auto ye = y + h;
      auto buf = (RGBColor*)alloca(w * sizeof(RGBColor));
#pragma GCC diagnostic push
#if defined(__has_warning)
#if __has_warning("-Wmaybe-uninitialized")
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#else
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
      /// Not actually used uninitialized. Just grabbing a copy of the pointer before we start the loop that fills it.
      pixelcopy_t pc_write(buf    ,_write_depth, RGBColor::depth, false);
#pragma GCC diagnostic pop
      pixelcopy_t pc_read( nullptr, RGBColor::depth, _read_depth, false);
      startWrite();
      do
      {
        readRect(x, y, w, 1, buf, &pc_read);
        size_t i = 0;
        do
        {
          effector(x + i, y, buf[i]);
        } while (++i < w);
        writeImage(x, y, w, 1, &pc_write, true);
        pc_write.src_x32 = 0;
      } while (++y < ye);
      endWrite();
    }
 };

//----------------------------------------------------------------------------
 }
}
