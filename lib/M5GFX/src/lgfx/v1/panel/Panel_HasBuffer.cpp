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
#include "Panel_HasBuffer.hpp"

#include "../platforms/common.hpp"
#include "../Bus.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  Panel_HasBuffer::~Panel_HasBuffer(void)
  {
    if (_buf) { heap_free(_buf); }
  }

  Panel_HasBuffer::Panel_HasBuffer(void) : Panel_Device()
  {
    _auto_display = true;
  }

  bool Panel_HasBuffer::init(bool use_reset)
  {
    _range_mod.top = INT16_MAX;
    _range_mod.left = INT16_MAX;
    _range_mod.right = 0;
    _range_mod.bottom = 0;

    auto len = _get_buffer_length();
    if (_buf) heap_free(_buf);
    _buf = static_cast<uint8_t*>(heap_alloc_dma(len));

    return ((_buf != nullptr) && (Panel_Device::init(use_reset)));
  }

  void Panel_HasBuffer::beginTransaction(void)
  {
    if (_in_transaction) return;
    _in_transaction = true;
    _bus->beginTransaction();
    cs_control(false);
  }

  void Panel_HasBuffer::endTransaction(void)
  {
    if (!_in_transaction) return;
    _in_transaction = false;
    _bus->endTransaction();
    cs_control(true);
  }

  void Panel_HasBuffer::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;
    if (_internal_rotation & 1) std::swap(_width, _height);
  }

  void Panel_HasBuffer::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    _xpos = xs;
    _ypos = ys;
    _xs = xs;
    _ys = ys;
    _xe = xe;
    _ye = ye;
  }

  void Panel_HasBuffer::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
/*
    bool need_transaction = !getStartCount();
    if (need_transaction) startWrite();
    writeFillRectPreclipped(x, y, 1, 1, rawcolor);
    if (need_transaction) endWrite();
*/
    startWrite();
    writeFillRectPreclipped(x, y, 1, 1, rawcolor);
    endWrite();
  }

  void Panel_HasBuffer::writeBlock(uint32_t rawcolor, uint32_t length)
  {
    uint32_t xs = _xs;
    uint32_t xe = _xe;
    uint32_t ys = _ys;
    uint32_t ye = _ye;
    if (xe < xs || ye < ys) { return; }
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      auto len = std::min<uint32_t>(length, xe + 1 - xpos);
      writeFillRectPreclipped(xpos, ypos, len, 1, rawcolor);
      xpos += len;
      if (xpos > xe)
      {
        xpos = xs;
        if (++ypos > ye)
        {
          ypos = ys;
        }
      }
      length -= len;
    } while (length);
    _xs = xs;
    _xe = xe;
    _ys = ys;
    _ye = ye;
    _xpos = xpos;
    _ypos = ypos;
  }

  void Panel_HasBuffer::_rotate_pos(uint_fast16_t &x, uint_fast16_t &y)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if (r & 1) { std::swap(x, y); }
      uint_fast8_t rb = 1 << r;
      if (rb & 0b11000110) { x = _cfg.panel_width  - 1 - x; } // case 1:2:6:7:
      if (rb & 0b10011100) { y = _cfg.panel_height - 1 - y; } // case 2:3:4:7:
    }
  }

  void Panel_HasBuffer::_rotate_pos(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if (r & 1) { std::swap(xs, ys); std::swap(xe, ye); }
      uint_fast8_t rb = 1 << r;
      if (rb & 0b11000110) // case 1:2:6:7:
      {
        std::swap(xs, xe);
        xs = _cfg.panel_width - 1 - xs;
        xe = _cfg.panel_width - 1 - xe;
      }
      if (rb & 0b10011100) // case 2:3:4:7:
      {
        std::swap(ys, ye);
        ys = _cfg.panel_height - 1 - ys;
        ye = _cfg.panel_height - 1 - ye;
      }
    }
  }

//----------------------------------------------------------------------------
 }
}
