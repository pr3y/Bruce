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
#include "Panel_SSD1306.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr uint8_t Bayer[] = { 8, 136, 40, 168, 200, 72, 232, 104, 56, 184, 24, 152, 248, 120, 216, 88, 8, 136, 40, 168, 200, 72, 232, 104, 56, 184, 24, 152, 248, 120, 216, 88 };

  inline static uint32_t to_gray(uint8_t r, uint8_t g, uint8_t b)
  {
    return (uint32_t)          // gamma2.0 convert and ITU-R BT.601 RGB to Y convert
          ( (r * r * 19749)    // R 0.299
          + (g * g * 38771)    // G 0.587
          + (b * b *  7530)    // B 0.114
          ) >> 24;
  }

  void Panel_1bitOLED::setTilePattern(uint_fast8_t i)
  {
    _bayer_offset = Bayer[i & 15] >> 4;
  }

  color_depth_t Panel_1bitOLED::setColorDepth(color_depth_t depth)
  {
    _write_depth = color_depth_t::rgb565_2Byte;
    _read_depth = color_depth_t::rgb565_2Byte;
    return color_depth_t::rgb565_2Byte;
  }

  void Panel_1bitOLED::waitDisplay(void)
  {
    _bus->wait();
  }

  bool Panel_1bitOLED::displayBusy(void)
  {
    return _bus->busy();
  }

  size_t Panel_1bitOLED::_get_buffer_length(void) const
  {
    return ((_cfg.panel_height + 7) >> 3) * _cfg.panel_width;
  }

  bool Panel_1bitOLED::init(bool use_reset)
  {
    if (!Panel_HasBuffer::init(use_reset))
    {
      return false;
    }

    startWrite(true);
    _bus->beginRead();
    uint8_t buf;
    bool res = _bus->readBytes(&buf, 1, true, true);
    _bus->endRead();

    if (res)
    {
      for (size_t i = 0; auto cmds = getInitCommands(i); i++)
      {
        size_t idx = 0;
        while (cmds[idx] != 0xFF || cmds[idx + 1] != 0xFF) ++idx;
        if (idx) { _bus->writeBytes(cmds, idx, false, true); }
      }

      setInvert(_invert);
      setRotation(_rotation);
    }

    endWrite();

    return res;
  }

  void Panel_1bitOLED::setInvert(bool invert)
  {
    _invert = invert;
    startWrite();
    _bus->writeCommand((invert ^ _cfg.invert) ? CMD_INVERTDISPLAY : CMD_NORMALDISPLAY, 8);
    endWrite();
  }

  void Panel_1bitOLED::setSleep(bool flg)
  {
    startWrite();
    _bus->writeCommand(flg ? CMD_DISP_OFF : CMD_DISP_ON, 8);
    endWrite();
  }

  void Panel_1bitOLED::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    uint_fast16_t xs = x, xe = x + w - 1;
    uint_fast16_t ys = y, ye = y + h - 1;
    _xs = xs;
    _ys = ys;
    _xe = xe;
    _ye = ye;
    _update_transferred_rect(xs, ys, xe, ye);

    swap565_t color;
    color.raw = rawcolor;
    uint32_t value = to_gray(color.R8(), color.G8(), color.B8());

    y = ys;
    do
    {
      x = xs;
      uint32_t idx = x + (y >> 3) * _cfg.panel_width;
      auto btbl = &Bayer[((y + (_bayer_offset >> 2)) & 3) << 2];
      uint32_t mask = 1 << (y&7);
      do
      {
        bool flg = 256 <= value + btbl[(x + _bayer_offset) & 3];
        if (flg) _buf[idx] |=   mask;
        else     _buf[idx] &= ~ mask;
        ++idx;
      } while (++x <= xe);
    } while (++y <= ye);
  }

  void Panel_1bitOLED::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    uint_fast16_t xs = x, xe = x + w - 1;
    uint_fast16_t ys = y, ye = y + h - 1;
    _update_transferred_rect(xs, ys, xe, ye);

    auto readbuf = (swap565_t*)alloca(w * sizeof(swap565_t));
    auto sx = param->src_x32;
    h += y;
    do
    {
      uint32_t prev_pos = 0, new_pos = 0;
      do
      {
        new_pos = param->fp_copy(readbuf, prev_pos, w, param);
        if (new_pos != prev_pos)
        {
          do
          {
            auto color = readbuf[prev_pos];
            _draw_pixel(x + prev_pos, y, to_gray(color.R8(), color.G8(), color.B8()));
          } while (new_pos != ++prev_pos);
        }
      } while (w != new_pos && w != (prev_pos = param->fp_skip(new_pos, w, param)));
      param->src_x32 = sx;
      param->src_y++;
    } while (++y < h);
  }

  void Panel_1bitOLED::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    {
      uint_fast16_t xs = _xs;
      uint_fast16_t xe = _xe;
      uint_fast16_t ys = _ys;
      uint_fast16_t ye = _ye;
      _update_transferred_rect(xs, ys, xe, ye);
    }
    uint_fast16_t xs   = _xs  ;
    uint_fast16_t ys   = _ys  ;
    uint_fast16_t xe   = _xe  ;
    uint_fast16_t ye   = _ye  ;
    uint_fast16_t xpos = _xpos;
    uint_fast16_t ypos = _ypos;

    static constexpr uint32_t buflen = 16;
    swap565_t colors[buflen];
    int bufpos = buflen;
    do
    {
      if (bufpos == buflen) {
        param->fp_copy(colors, 0, std::min(length, buflen), param);
        bufpos = 0;
      }
      auto color = colors[bufpos++];
      _draw_pixel(xpos, ypos, to_gray(color.R8(), color.G8(), color.B8()));
      if (++xpos > xe)
      {
        xpos = xs;
        if (++ypos > ye)
        {
          ypos = ys;
        }
      }
    } while (--length);
    _xpos = xpos;
    _ypos = ypos;
  }

  void Panel_1bitOLED::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    auto readbuf = (swap565_t*)alloca(w * sizeof(swap565_t));
    param->src_data = readbuf;
    int32_t readpos = 0;
    h += y;
    do
    {
      uint32_t idx = 0;
      do
      {
        readbuf[idx] = _read_pixel(x + idx, y) ? -1 : 0;
      } while (++idx != w);
      param->src_x32 = 0;
      readpos = param->fp_copy(dst, readpos, readpos + w, param);
    } while (++y < h);
  }

  void Panel_1bitOLED::_draw_pixel(uint_fast16_t x, uint_fast16_t y, uint32_t value)
  {
    _rotate_pos(x, y);
    uint32_t idx = x + (y >> 3) * _cfg.panel_width;
    uint32_t mask = 1 << (y&7);
    bool flg = 256 <= value + Bayer[ + (((x + _bayer_offset) & 3) | ((y + (_bayer_offset >> 2)) & 3) << 2)];
    if (flg) _buf[idx] |=  mask;
    else     _buf[idx] &= ~mask;
  }

  bool Panel_1bitOLED::_read_pixel(uint_fast16_t x, uint_fast16_t y)
  {
    _rotate_pos(x, y);
    uint32_t idx = x + (y >> 3) * _cfg.panel_width;
    return _buf[idx] & (1 << (y&7));
  }

  void Panel_1bitOLED::_update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye)
  {
    _rotate_pos(xs, ys, xe, ye);
    _range_mod.left   = std::min<int32_t>(xs, _range_mod.left);
    _range_mod.right  = std::max<int32_t>(xe, _range_mod.right);
    _range_mod.top    = std::min<int32_t>(ys, _range_mod.top);
    _range_mod.bottom = std::max<int32_t>(ye, _range_mod.bottom);
  }

//----------------------------------------------------------------------------

  void Panel_SSD1306::setComPins(uint8_t data)
  {
    _compins = data;
    if (_buf)
    { /// 初期化済みの場合はここでコマンド送信する;
      startWrite();
      _bus->writeCommand(CMD_SETCOMPINS | data << 8, 16);
      endWrite();
    }
  }

  bool Panel_SSD1306::init(bool use_reset)
  {
    if (!Panel_1bitOLED::init(use_reset)) { return false; }
    setComPins(_compins);
    return true;
  }

  void Panel_SSD1306::setBrightness(uint8_t brightness)
  {
    startWrite();
    _bus->writeCommand(CMD_SETPRECHARGE | ((brightness+15)/17)*0x11 << 8, 16);
    _bus->writeCommand(CMD_SETVCOMDETECT | (brightness>>1) << 8, 16);
    _bus->writeCommand(CMD_SETCONTRAST | brightness << 8, 16);
    endWrite();
  }

  void Panel_SSD1306::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      _range_mod.left   = std::min<int_fast16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int_fast16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int_fast16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int_fast16_t>(_range_mod.bottom, y + h - 1);
    }
    if (_range_mod.empty()) { return; }

    uint_fast8_t xs = _range_mod.left;
    uint_fast8_t xe = _range_mod.right;
    uint_fast8_t ys = _range_mod.top    >> 3;
    uint_fast8_t ye = _range_mod.bottom >> 3;
    int retry = 3;
    while (!(_bus->writeCommand(CMD_COLUMNADDR| (xs +  _cfg.offset_x      ) << 8 | (xe +  _cfg.offset_x      ) << 16, 24)
          && _bus->writeCommand(CMD_PAGEADDR  | (ys + (_cfg.offset_y >> 3)) << 8 | (ye + (_cfg.offset_y >> 3)) << 16, 24)) && --retry)
    {
      _bus->endTransaction();
      _bus->beginTransaction();
    }
    if (retry)
    {
      do
      {
        auto buf = &_buf[xs + ys * _cfg.panel_width];
        _bus->writeBytes(buf, xe - xs + 1, true, true);
      } while (++ys <= ye);
      _range_mod.top    = INT16_MAX;
      _range_mod.left   = INT16_MAX;
      _range_mod.right  = 0;
      _range_mod.bottom = 0;
    }
  }

//----------------------------------------------------------------------------

  void Panel_SH110x::setBrightness(uint8_t brightness)
  {
    startWrite();
    _bus->writeCommand(CMD_SETCONTRAST | brightness << 8, 16);
    endWrite();
  }

  void Panel_SH110x::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      _range_mod.left   = std::min<int_fast16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int_fast16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int_fast16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int_fast16_t>(_range_mod.bottom, y + h - 1);
    }
    if (_range_mod.empty()) { return; }

    uint_fast8_t xs = _range_mod.left ;
    uint_fast8_t xe = _range_mod.right;
    uint_fast8_t ys = _range_mod.top    >> 3;
    uint_fast8_t ye = _range_mod.bottom >> 3;

    uint_fast8_t offset_y = _cfg.offset_y >> 3;
    uint_fast8_t offset_x = _cfg.offset_x + xs;

    int retry = 3;
    do
    {
      while (!_bus->writeCommand(  CMD_SETPAGEADDR | (ys + offset_y)
                                | (CMD_SETHIGHCOLUMN + (offset_x >> 4)) << 8
                                | (CMD_SETLOWCOLUMN  + (offset_x & 0x0F)) << 16
                                , 24) && --retry)
      {
        _bus->endTransaction();
        _bus->beginTransaction();
      }
      if (!retry) { break; }

      auto buf = &_buf[xs + ys * _cfg.panel_width];
      _bus->writeBytes(buf, xe - xs + 1, true, true);
    } while (++ys <= ye);

    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;
  }

//----------------------------------------------------------------------------

  // void Panel_ST7565::setBrightness(uint8_t brightness) {}

  void Panel_ST7565::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      _range_mod.left   = std::min<int_fast16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int_fast16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int_fast16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int_fast16_t>(_range_mod.bottom, y + h - 1);
    }
    if (_range_mod.empty()) { return; }

    // xeの位置を2ライン単位の位置にしないと次の描画位置がずれる事があったため調整
    uint_fast8_t xs = _range_mod.left     ;
    uint_fast8_t xe = (_range_mod.right+2) & ~1;
    uint_fast8_t ys = _range_mod.top    >> 3;
    uint_fast8_t ye = _range_mod.bottom >> 3;

    uint_fast8_t offset_y = _cfg.offset_y >> 3;
    uint_fast8_t offset_x = _cfg.offset_x + xs;

    int retry = 3;
    do
    {
      while (!_bus->writeCommand(  CMD_SETPAGEADDR | (ys + offset_y)
                                | (CMD_SETHIGHCOLUMN + (offset_x >> 4)) << 8
                                | (CMD_SETLOWCOLUMN  + (offset_x & 0x0F)) << 16
                                , 24) && --retry)
      {
        _bus->endTransaction();
        _bus->beginTransaction();
      }
      if (!retry) { break; }

      auto buf = &_buf[xs + ys * _cfg.panel_width];
      _bus->writeBytes(buf, xe - xs, true, true);
    } while (++ys <= ye);

    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;
  }

//----------------------------------------------------------------------------
 }
}
