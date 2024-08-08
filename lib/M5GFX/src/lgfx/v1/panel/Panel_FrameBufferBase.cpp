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

#include "Panel_FrameBufferBase.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/common_function.hpp"

#if defined (ESP_PLATFORM)
 #include <sdkconfig.h>
 #if defined (CONFIG_IDF_TARGET_ESP32S3)
  #if __has_include(<esp32s3/rom/cache.h>)
   #include <esp32s3/rom/cache.h>
   extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);
   #define LGFX_USE_CACHE_WRITEBACK_ADDR
  #endif
 #endif
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

#if defined ( LGFX_USE_CACHE_WRITEBACK_ADDR )
  void cacheWriteBack(const void* ptr, uint32_t size)
  {
    if (!isEmbeddedMemory(ptr))
    {
      Cache_WriteBack_Addr((uint32_t)ptr, size);
    }
  }
#else
  static inline void cacheWriteBack(const void*, uint32_t) {}
#endif

  bool Panel_FrameBufferBase::init(bool use_reset)
  {
    setInvert(_invert);
    setRotation(_rotation);

    if (!Panel_Device::init(use_reset))
    {
      return false;
    }
    return true;
  }

  void Panel_FrameBufferBase::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    auto pw = _cfg.panel_width;
    auto ph = _cfg.panel_height;
    if (_internal_rotation & 1)
    {
      std::swap(pw, ph);
    }
    _width  = pw;
    _height = ph;
    _xe = pw-1;
    _ye = ph-1;
    _xs = 0;
    _ys = 0;
  }

  void Panel_FrameBufferBase::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    xs = std::max<uint_fast16_t>(0u, std::min<uint_fast16_t>(_width  - 1, xs));
    xe = std::max<uint_fast16_t>(0u, std::min<uint_fast16_t>(_width  - 1, xe));
    ys = std::max<uint_fast16_t>(0u, std::min<uint_fast16_t>(_height - 1, ys));
    ye = std::max<uint_fast16_t>(0u, std::min<uint_fast16_t>(_height - 1, ye));
    _xpos = xs;
    _xs = xs;
    _xe = xe;
    _ypos = ys;
    _ys = ys;
    _ye = ye;
  }

  void Panel_FrameBufferBase::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
      if (r & 2)                  { x = _width  - (x + 1); }
      if (r & 1) { std::swap(x, y); }
    }
    if (_write_bits >= 8)
    {
      size_t bytes = _write_bits >> 3;
      auto ptr = &_lines_buffer[y][x * bytes];
      memcpy(ptr, &rawcolor, bytes);
      cacheWriteBack(ptr, bytes);
    }
  }

  void Panel_FrameBufferBase::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + h); }
      if (r & 2)                  { x = _width  - (x + w); }
      if (r & 1) { std::swap(x, y);  std::swap(w, h); }
    }
    h += y;
    if (_write_bits >= 8)
    {
      size_t bytes = _write_bits >> 3;
      do
      {
        auto ptr = &_lines_buffer[y][x * bytes];
        memset_multi(ptr, rawcolor, bytes, w);
        cacheWriteBack(ptr, bytes * w);
      } while (++y < h);
    }
  }

  void Panel_FrameBufferBase::writeBlock(uint32_t rawcolor, uint32_t length)
  {
    do
    {
      uint32_t h = 1;
      auto w = std::min<uint32_t>(length, _xe + 1 - _xpos);
      if (length >= (w << 1) && _xpos == _xs)
      {
        h = std::min<uint32_t>(length / w, _ye + 1 - _ypos);
      }
      writeFillRectPreclipped(_xpos, _ypos, w, h, rawcolor);
      if ((_xpos += w) <= _xe) return;
      _xpos = _xs;
      if (_ye < (_ypos += h)) { _ypos = _ys; }
      length -= w * h;
    } while (length);
  }

  void Panel_FrameBufferBase::_rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty)
  {
    uint32_t addx = param->src_x32_add;
    uint32_t addy = param->src_y32_add;
    uint_fast8_t r = _internal_rotation;
    uint_fast8_t bitr = 1u << r;
    // if (bitr & 0b10011100)
    // {
    //   nextx = -nextx;
    // }
    if (bitr & 0b10010110) // case 1:2:4:7:
    {
      param->src_y32 += nexty * (h - 1);
      nexty = -(int32_t)nexty;
      y = _height - (y + h);
    }
    if (r & 2)
    {
      param->src_x32 += addx * (w - 1);
      param->src_y32 += addy * (w - 1);
      addx = -(int32_t)addx;
      addy = -(int32_t)addy;
      x = _width  - (x + w);
    }
    if (r & 1)
    {
      std::swap(x, y);
      std::swap(w, h);
      std::swap(nextx, addx);
      std::swap(nexty, addy);
    }
    param->src_x32_add = addx;
    param->src_y32_add = addy;
  }

  void Panel_FrameBufferBase::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    (void)use_dma;
    uint_fast16_t xs = _xs;
    uint_fast16_t xe = _xe;
    uint_fast16_t ys = _ys;
    uint_fast16_t ye = _ye;
    uint_fast16_t x = _xpos;
    uint_fast16_t y = _ypos;
    const size_t bytes = _write_bits >> 3;
    // auto k = _bitwidth * bits >> 3;

    uint_fast8_t r = _internal_rotation;
    if (!r)
    {
      uint_fast16_t linelength;
      do {
        linelength = std::min<uint_fast16_t>(xe - x + 1, length);
        auto ptr = &_lines_buffer[y][x * bytes];
        param->fp_copy(ptr, 0, linelength, param);
        cacheWriteBack(ptr, bytes * linelength);

        if ((x += linelength) > xe)
        {
          x = xs;
          y = (y != ye) ? (y + 1) : ys;
        }
      } while (length -= linelength);
      _xpos = x;
      _ypos = y;
      return;
    }

    int_fast16_t ax = 1;
    int_fast16_t ay = 1;
    if ((1u << r) & 0b10010110) { y = _height - (y + 1); ys = _height - (ys + 1); ye = _height - (ye + 1); ay = -1; }
    if (r & 2)                  { x = _width  - (x + 1); xs = _width  - (xs + 1); xe = _width  - (xe + 1); ax = -1; }

    if (r & 1)
    {
      do
      {
        param->fp_copy(_lines_buffer[x], y, y + 1, param); /// xとyを入れ替えて処理する;
        if (x != xe)
        {
          x += ax;
        }
        else
        {
          x = xs;
          y = (y != ye) ? (y + ay) : ys;
        }
      } while (--length);
    }
    else
    {
      int w = abs((int)(xe - xs)) + 1;
      do
      {
        param->fp_copy(_lines_buffer[y], x, x + 1, param);
        if (x != xe)
        {
          x += ax;
        }
        else
        {
          x = xs;
          cacheWriteBack(&_lines_buffer[y][x], bytes * w);
          y = (y != ye) ? (y + ay) : ys;
        }
      } while (--length);
    }

    if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
    if (r & 2)                  { x = _width  - (x + 1); }
    _xpos = x;
    _ypos = y;
  }

  void Panel_FrameBufferBase::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool)
  {
    uint_fast8_t r = _internal_rotation;
    if (r == 0 && param->transp == pixelcopy_t::NON_TRANSP && param->no_convert)
    {
      auto bits = _write_bits;
      x = x * bits >> 3;
      w = w * bits >> 3;
      auto sw = param->src_bitwidth * bits >> 3;
      auto src = &((uint8_t*)param->src_data)[param->src_y * sw + (param->src_x * bits >> 3)];
      h += y;
      do
      {
        memcpy(&_lines_buffer[y][x], src, w);
        cacheWriteBack(&_lines_buffer[y][x], w);
        src += sw;
      } while (++y != h);
      return;
    }

    uint32_t nextx = 0;
    uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
    if (r)
    {
      _rotate_pixelcopy(x, y, w, h, param, nextx, nexty);
    }
    uint32_t sx32 = param->src_x32;
    uint32_t sy32 = param->src_y32;
    uint_fast8_t bytes = _write_bits >> 3;
    h += y;
    do
    {
      int32_t pos = x;
      int32_t end = pos + w;
      while (end != (pos = param->fp_copy(_lines_buffer[y], pos, end, param))
         &&  end != (pos = param->fp_skip(                  pos, end, param)));
      param->src_x32 = (sx32 += nextx);
      param->src_y32 = (sy32 += nexty);
      auto ptr = &_lines_buffer[y][x * bytes];
      cacheWriteBack(ptr, bytes * end);
    } while (++y != h);
  }

  void Panel_FrameBufferBase::writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param)
  {
    uint32_t nextx = 0;
    uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
    if (_internal_rotation)
    {
      _rotate_pixelcopy(x, y, w, h, param, nextx, nexty);
    }
    uint32_t sx32 = param->src_x32;
    uint32_t sy32 = param->src_y32;

    uint32_t pos = x;
    uint32_t end = pos + w;
    h += y;
    uint_fast16_t wbytes = (w * _write_bits) >> 3;
    do
    {
      param->fp_copy(_lines_buffer[y], pos, end, param);
      cacheWriteBack(&_lines_buffer[y][pos], wbytes);
      param->src_x32 = (sx32 += nextx);
      param->src_y32 = (sy32 += nexty);
    } while (++y < h);
  }

  void Panel_FrameBufferBase::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    uint_fast8_t r = _internal_rotation;
    if (r == 0 && param->no_convert)
    {
      h += y;
      auto bytes = _write_bits >> 3;
      auto d = (uint8_t*)dst;
      w *= bytes;
      do
      {
        memcpy(d, &_lines_buffer[y][x * bytes], w);
        d += w;
      } while (++y != h);
      return;
    }

    int addx = 1;
    int addy = 1;
    uint_fast16_t wlen = 1;
    if (r)
    {
      if (r & 2)
      {
        x = _width - (x + 1);
        param->src_x32_add = -param->src_x32_add;
        addx = -1;
      }
      if ((1 << r) & 0b10010110)
      {
        y = _height - (y + 1);
        addy = -1;
      }
      if (r & 1)
      {
        std::swap(x, y);
        std::swap(w, h);
        std::swap(addx, addy);
        std::swap(wlen, w);
        std::swap(param->src_x32_add, param->src_y32_add);
      }
    }

    h = y + (h * addy);
    uint_fast32_t pos = 0;
    uint_fast16_t ybak = y;
    do
    {
      y = ybak;
      uint32_t x32 = x << pixelcopy_t::FP_SCALE;
      do
      {
        param->src_y32 = 0;
        param->src_x32 = x32;
        param->src_data = _lines_buffer[y];
        param->fp_copy(dst, pos, pos + w, param);
        pos += w;
      } while (h != (y += addy));
      x += addx;
    } while (--wlen);
  }

  void Panel_FrameBufferBase::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { src_y = _height - (src_y + h); dst_y = _height - (dst_y + h); }
      if (r & 2)                  { src_x = _width  - (src_x + w); dst_x = _width  - (dst_x + w); }
      if (r & 1) { std::swap(src_x, src_y);  std::swap(dst_x, dst_y);  std::swap(w, h); }
    }
    size_t bytes = _write_bits >> 3;
    size_t len = w * bytes;
    int32_t add = 1;
    if (src_y < dst_y) add = -add;
    int32_t pos = (src_y < dst_y) ? h - 1 : 0;

    /// PSRAMを使用している場合、PSRAM to PSRAMのmemcpyがデータ破損を起こす場合があるため、一旦ローカルの配列を経由してコピーを行う;
    auto buf = (uint8_t*)alloca(len);
    do
    {
      uint8_t* src = &_lines_buffer[src_y + pos][src_x * bytes];
      uint8_t* dst = &_lines_buffer[dst_y + pos][dst_x * bytes];
      memcpy(buf, src, len);
      memcpy(dst, buf, len);
      cacheWriteBack(dst, len);
      pos += add;
    } while (--h);
  }

//----------------------------------------------------------------------------
 }
}
