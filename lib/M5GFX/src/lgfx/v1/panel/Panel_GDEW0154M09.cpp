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
#include "Panel_GDEW0154M09.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/colortype.hpp"

#ifdef min
#undef min
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr uint8_t Bayer[16] = { 8, 200, 40, 232, 72, 136, 104, 168, 56, 248, 24, 216, 120, 184, 88, 152 };

  Panel_GDEW0154M09::Panel_GDEW0154M09(void)
  {
    _cfg.dummy_read_bits = 0;
    _epd_mode = epd_mode_t::epd_quality;
  }

  color_depth_t Panel_GDEW0154M09::setColorDepth(color_depth_t depth)
  {
    _write_depth = color_depth_t::rgb565_2Byte;
    _read_depth = color_depth_t::rgb565_2Byte;
    return color_depth_t::rgb565_2Byte;
  }

  size_t Panel_GDEW0154M09::_get_buffer_length(void) const
  {
    return ((_cfg.panel_width + 7) & ~7) * _cfg.panel_height >> 3;
  }

  bool Panel_GDEW0154M09::init(bool use_reset)
  {
    pinMode(_cfg.pin_busy, pin_mode_t::input);

    if (!Panel_HasBuffer::init(use_reset))
    {
      return false;
    }

    //memset(_buf, 255, len);

    _wait_busy();

    startWrite(true);

    for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
    {
      command_list(cmds);
    }

    setInvert(_invert);

    setRotation(_rotation);

    _range_old.top = 0;
    _range_old.left = 0;
    _range_old.right = _width - 1;
    _range_old.bottom = _height - 1;
    _exec_transfer(0x13, _range_old);
    _close_transfer();
    _range_mod = _range_old;

    endWrite();

    return true;
  }

  void Panel_GDEW0154M09::waitDisplay(void)
  {
    _wait_busy();
  }

  bool Panel_GDEW0154M09::displayBusy(void)
  {
    return _cfg.pin_busy >= 0 && !gpio_in(_cfg.pin_busy);
  }

  void Panel_GDEW0154M09::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      _range_mod.left   = std::min<int16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int16_t>(_range_mod.bottom, y + h - 1);
    }
    if (_range_mod.empty()) { return; }
    _close_transfer();
    _range_old = _range_mod;
    while (millis() - _send_msec < _refresh_msec) delay(1);
    if (getEpdMode() == epd_mode_t::epd_quality)
    {
      _exec_transfer(0x13, _range_mod, true);
      _wait_busy();
      _bus->writeCommand(0x12, 8);
      auto send_msec = millis();
      delay(300);
      while (millis() - send_msec < _refresh_msec) delay(1);
      _exec_transfer(0x10, _range_mod, true);
    }
    _exec_transfer(0x13, _range_mod);
    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;

    _wait_busy();
    _bus->writeCommand(0x12, 8);
    _send_msec = millis();
  }

  void Panel_GDEW0154M09::setInvert(bool invert)
  {
    startWrite();
    _invert = invert;
    _wait_busy();
    _bus->writeCommand(0x50, 8);  // VCOM and DATA Interval Setting (CDI)
    _bus->writeData((invert ^ _cfg.invert) ? 0xC7 : 0xD7, 8);
    endWrite();
  }

  void Panel_GDEW0154M09::setSleep(bool flg)
  {
    if (flg)
    {
      startWrite();
      _bus->writeCommand(0x50, 8);  // VCOM and DATA Interval Setting (CDI)
      _bus->writeData((_invert ^ _cfg.invert) ? 0xE7 : 0xF7, 8); // without OLD data
      _wait_busy();
      _bus->writeCommand(0x02, 8); // Power OFF

      _bus->writeCommand(0x07, 8); // Deep Sleep(DSLP)
      _bus->writeData(   0xA5, 8); // Deep Sleep(DSLP)
      endWrite();
    }
    else
    {
      init(true);
    }
  }

  void Panel_GDEW0154M09::setPowerSave(bool flg)
  {
    startWrite();
    _bus->writeCommand(0x50, 8);
    if (flg)
    {
      _bus->writeData((_invert ^ _cfg.invert) ? 0xE7 : 0xF7, 8); // without OLD data
      _wait_busy();
      _bus->writeCommand(0x02, 8); // Power OFF(POF)
    }
    else
    {
      _bus->writeData((_invert ^ _cfg.invert) ? 0xC7 : 0xD7, 8); // without OLD data
      _wait_busy();
      _bus->writeCommand(0x04, 8); // Power ON(PON)
    }
    endWrite();
  }

  void Panel_GDEW0154M09::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
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
    uint32_t value = (color.R8() + (color.G8() << 1) + color.B8()) >> 2;

    y = ys;
    do
    {
      x = xs;
      uint32_t idx = ((_cfg.panel_width + 7) & ~7) * y + x;
      auto btbl = &Bayer[(y & 3) << 2];
      do
      {
        bool flg = 256 <= value + btbl[x & 3];
        if (flg) _buf[idx >> 3] |=   0x80 >> (idx & 7);
        else     _buf[idx >> 3] &= ~(0x80 >> (idx & 7));
        ++idx;
      } while (++x <= xe);
    } while (++y <= ye);
  }

  void Panel_GDEW0154M09::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
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
            _draw_pixel(x + prev_pos, y, (color.R8() + (color.G8() << 1) + color.B8()) >> 2);
          } while (new_pos != ++prev_pos);
        }
      } while (w != new_pos && w != (prev_pos = param->fp_skip(new_pos, w, param)));
      param->src_x32 = sx;
      param->src_y++;
    } while (++y < h);
  }

  void Panel_GDEW0154M09::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
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
      _draw_pixel(xpos, ypos, (color.R8() + (color.G8() << 1) + color.B8()) >> 2);
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

  void Panel_GDEW0154M09::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
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

  bool Panel_GDEW0154M09::_wait_busy(uint32_t timeout)
  {
    if (_cfg.pin_busy >= 0 && !gpio_in(_cfg.pin_busy))
    {
      uint32_t start_time = millis();
      do
      {
        if (millis() - start_time > timeout) return false;
        delay(1);
      } while (!gpio_in(_cfg.pin_busy));
    }
    return true;
  }

  void Panel_GDEW0154M09::_draw_pixel(uint_fast16_t x, uint_fast16_t y, uint32_t value)
  {
    _rotate_pos(x, y);
    uint32_t idx = ((_cfg.panel_width + 7) & ~7) * y + x;
    bool flg = 256 <= value + Bayer[(x & 3) | (y & 3) << 2];
    if (flg) _buf[idx >> 3] |=   0x80 >> (idx & 7);
    else     _buf[idx >> 3] &= ~(0x80 >> (idx & 7));
  }

  bool Panel_GDEW0154M09::_read_pixel(uint_fast16_t x, uint_fast16_t y)
  {
    _rotate_pos(x, y);
    uint32_t idx = ((_cfg.panel_width + 7) & ~7) * y + x;
    return _buf[idx >> 3] & (0x80 >> (idx & 7));
  }

  void Panel_GDEW0154M09::_exec_transfer(uint32_t cmd, const range_rect_t& range, bool invert)
  {
    int32_t xs = range.left & ~7;
    int32_t xe = range.right & ~7;

    _wait_busy();

    //_bus->writeCommand(0x91, 8);
    //_bus->writeCommand(0x90, 8);
    _bus->writeCommand(0x91 | 0x90 << 8, 16);

    _bus->writeData(xs | xe << 8, 16);
    _bus->writeData(getSwap16(range.top) | getSwap16(range.bottom)<<16, 32);
    _bus->writeData(1, 8);

    _wait_busy();

    _bus->writeCommand(cmd, 8);
    int32_t w = ((xe - xs) >> 3) + 1;
    int32_t y = range.top;
    int32_t add = ((_cfg.panel_width + 7) & ~7) >> 3;
    auto b = &_buf[xs >> 3];
    if (invert)
    {
      b += y * add;
      do
      {
        int32_t i = 0;
        do
        {
          _bus->writeData(~b[i], 8);
        } while (++i != w);
        b += add;
      } while (++y <= range.bottom);
    }
    else
    {
      do
      {
        _bus->writeBytes(&b[add * y], w, true, true);
      } while (++y <= range.bottom);
    }
    /*
    range->top = INT_MAX;
    range->left = INT_MAX;
    range->right = 0;
    range->bottom = 0;
    //*/
  }

  void Panel_GDEW0154M09::_close_transfer(void)
  {
    if (_range_old.empty()) { return; }
    while (millis() - _send_msec < _refresh_msec) delay(1);
    _exec_transfer(0x10, _range_old);
    _range_old.top    = INT16_MAX;
    _range_old.left   = INT16_MAX;
    _range_old.right  = 0;
    _range_old.bottom = 0;

    _bus->wait();
  }

  void Panel_GDEW0154M09::_update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye)
  {
    _rotate_pos(xs, ys, xe, ye);

    int32_t x1 = xs & ~7;
    int32_t x2 = (xe & ~7) + 7;

    if (_range_old.horizon.intersectsWith(x1, x2) && _range_old.vertical.intersectsWith(ys, ye))
    {
      _close_transfer();
    }
    _range_mod.top    = std::min<int32_t>(ys, _range_mod.top);
    _range_mod.left   = std::min<int32_t>(x1, _range_mod.left);
    _range_mod.right  = std::max<int32_t>(x2, _range_mod.right);
    _range_mod.bottom = std::max<int32_t>(ye, _range_mod.bottom);
  }

//----------------------------------------------------------------------------
 }
}
