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
#include "Panel_M5UnitLCD.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/colortype.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  bool Panel_M5UnitLCD::init(bool use_reset)
  {
    /// I2C接続のためGPIOによるRESET制御は不要なのでfalseで呼出す;
    if (!Panel_Device::init(false)) return false;

    startWrite(true);

    _bus->writeCommand(CMD_READ_ID, 8);
    uint8_t buf[4];
    bool res = _bus->readBytes(buf, 4, true)
            && buf[0] == 0x77 && buf[1] == 0x89;

    _buff_free_count = 0;
    if (res)
    {
      _check_repeat();
      if (use_reset)
      {
        _bus->writeCommand(CMD_RESET | 0x77 << 8 | 0x89 << 16 | CMD_RESET << 24, 32);
        // リセットコマンド後は300msec待つ;
        lgfx::delay(300);
        int retry = 8;
        do
        {
          _bus->writeCommand(CMD_NOP, 16);
          endWrite();
          lgfx::delay(32);
          startWrite(true);
          _bus->writeCommand(CMD_READ_ID, 8);
        } while (!(_bus->readBytes(buf, 4, true) && buf[0] == 0x77 && buf[1] == 0x89) && --retry);
      }
    }

    endWrite();

    return res;
  }

  void Panel_M5UnitLCD::beginTransaction(void)
  {
    if (_in_transaction) return;
    _in_transaction = true;
    _bus->beginTransaction();
    cs_control(false);
    _last_cmd = 0;
  }

  void Panel_M5UnitLCD::endTransaction(void)
  {
    if (!_in_transaction) return;
    _in_transaction = false;
    _bus->endTransaction();
    cs_control(true);
    _last_cmd = 0;
  }

  bool Panel_M5UnitLCD::_check_repeat(uint32_t cmd, uint_fast8_t limit)
  {
    switch (_last_cmd & ~7)
    {
    default:
      break;
    case CMD_WRITE_RAW:
    case CMD_WRITE_RLE:
      if ((_buff_free_count > limit) && (_last_cmd == cmd))
      {
        --_buff_free_count;
        return true;
      }
      _bus->endTransaction();
      cs_control(true);
      _bus->beginTransaction();
      cs_control(false);
      break;
    }

    _last_cmd = cmd;

    if (_buff_free_count > limit)
    {
      --_buff_free_count;
      return false;
    }
    limit = std::min<uint_fast8_t>(255, limit * 2);

    size_t retry = 16;
    _buff_free_count = 255;
    while (!_bus->writeCommand(CMD_READ_BUFCOUNT, 8) && --retry);
    if (retry)
    {
      retry = 255;
      do
      {
        if (_bus->readBytes((uint8_t*)&_buff_free_count, 1))
        {
          if (_buff_free_count >= limit)
          {
            break;
          }
          lgfx::delay(2);
        }
        else
        {
          _bus->endRead();
          _bus->beginRead();
        }
      } while (--retry);
    }
    _bus->endTransaction();
    _bus->beginTransaction();

    return false;
  }

  color_depth_t Panel_M5UnitLCD::setColorDepth(color_depth_t depth)
  {
    auto bits = (depth & color_depth_t::bit_mask);
    if      (bits > 16) { depth = color_depth_t::rgb888_3Byte; }
    else if (bits < 16) { depth = color_depth_t::rgb332_1Byte; }
    else                { depth = color_depth_t::rgb565_2Byte; }

    _read_depth = _write_depth = depth;

//    _update_colmod();
    return depth;
  }

  void Panel_M5UnitLCD::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r ^ _cfg.offset_rotation) & 4);

    auto pw = _cfg.panel_width;
    auto ph = _cfg.panel_height;
    if (_internal_rotation & 1)
    {
      std::swap(pw, ph);
    }
    _width  = pw;
    _height = ph;

    _xs = _xe = _ys = _ye = INT16_MAX;

    if (_bus == nullptr) return;

    startWrite();
    _bus->writeCommand(CMD_ROTATE | _internal_rotation << 8, 16);
    endWrite();
  }

  void Panel_M5UnitLCD::setInvert(bool invert)
  {
    _invert = invert;
    startWrite();
    _check_repeat();
    _bus->writeCommand((invert ^ _cfg.invert) ? CMD_INVON : CMD_INVOFF, 8);
    endWrite();
  }

  void Panel_M5UnitLCD::setSleep(bool flg)
  {
    startWrite();
    _check_repeat();
    // true : sleep in  /  false : sleep out
    _bus->writeCommand(CMD_SET_SLEEP | (flg ? 1 : 0) << 8, 16);
    endWrite();
  }

  void Panel_M5UnitLCD::setPowerSave(bool flg)
  {
    startWrite();
    _check_repeat();
    // true : low-power / false : nomal-power
    _bus->writeCommand(CMD_SET_POWER | (flg ? 0 : 1) << 8, 16);
    endWrite();
  }

  void Panel_M5UnitLCD::setBrightness(uint8_t brightness)
  {
    startWrite();
    _check_repeat();
    _bus->writeCommand(CMD_BRIGHTNESS | brightness << 8, 16);
    endWrite();
  }

  void Panel_M5UnitLCD::writeBlock(uint32_t rawcolor, uint32_t length)
  {
/*
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
/*/
    _raw_color = rawcolor;
    size_t bytes = (rawcolor == 0) ? 1 : (_write_bits >> 3);
    auto buf = (uint8_t*)alloca((length >> 8) * (bytes + 1) + 2);
    buf[0] = CMD_WRITE_RLE | bytes;
    size_t idx = _check_repeat(buf[0]) ? 0 : 1;
    //_check_repeat(buf[0]);
    //size_t idx = 1;
    do
    {
      uint32_t len = (length < 0x100)
                        ? length : 0xFF;
      buf[idx++] = len;
      auto color = rawcolor;
      for (int i = bytes; i > 0; --i)
      {
        buf[idx++] = color;
        color >>= 8;
      }
      length -= len;
    } while (length);
    _bus->writeBytes(buf, idx, false, true);
//*/
  }

  void Panel_M5UnitLCD::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    startWrite();
    _check_repeat();
    writeFillRectPreclipped(x, y, 1, 1, rawcolor);
    endWrite();
  }

  void Panel_M5UnitLCD::_fill_rect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint_fast8_t bytes)
  {
    _xs = x;
    _ys = y;
    _xe = x + w - 1;
    _ye = y + h - 1;

    _check_repeat();
    uint8_t buf[16];
    bool rect = (w > 1) || (h > 1);
    buf[0] = (rect ? CMD_FILLRECT : CMD_DRAWPIXEL) | bytes;
    size_t idx = 1;
    bool flg_large = (_cfg.memory_width >= 256) || (_cfg.memory_height >= 256);
    if (flg_large) { buf[idx++] = _xs >> 8; }  buf[idx++] = _xs;
    if (flg_large) { buf[idx++] = _ys >> 8; }  buf[idx++] = _ys;
    if (rect)
    {
      if (flg_large) { buf[idx++] = _xe >> 8; }  buf[idx++] = _xe;
      if (flg_large) { buf[idx++] = _ye >> 8; }  buf[idx++] = _ye;
    }
    auto rawcolor = _raw_color;
    for (size_t i = 0; i < bytes; ++i)
    {
      buf[idx++] = rawcolor;
      rawcolor >>= 8;
    }
    _bus->writeBytes(buf, idx, false, true);
  }

  void Panel_M5UnitLCD::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    size_t bytes = 0;
    if (_raw_color != rawcolor)
    {
      _raw_color = rawcolor;
      bytes = _write_bits >> 3;
    }
    _fill_rect(x, y, w, h, bytes);
  }

  void Panel_M5UnitLCD::writeFillRectAlphaPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t argb8888)
  {
    _raw_color = getSwap32(argb8888);
    _fill_rect(x, y, w, h, 4);
    _raw_color = ~0u;
  }

  void Panel_M5UnitLCD::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    _xpos = xs;
    _ypos = ys;
    startWrite();
    _set_window(xs, ys, xe, ye);
    endWrite();
  }
  void Panel_M5UnitLCD::_set_window(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    uint8_t buf[10];
    size_t idx = 0;
    bool flg_large = (_cfg.memory_width >= 256) || (_cfg.memory_height >= 256);
    if (!flg_large)
    {
      ys = std::min<uint_fast16_t>(ys, 255u);
      ye = std::min<uint_fast16_t>(ye, 255u);
      xs = std::min<uint_fast16_t>(xs, 255u);
      xe = std::min<uint_fast16_t>(xe, 255u);
    }
    if (xs != _xs || xe != _xe)
    {
      _xs = xs;
      _xe = xe;
      buf[idx++] = CMD_CASET;
      if (flg_large) { buf[idx++] = xs >> 8; }  buf[idx++] = xs;
      if (flg_large) { buf[idx++] = xe >> 8; }  buf[idx++] = xe;
    }
    if (ys != _ys || ye != _ye)
    {
      _ys = ys;
      _ye = ye;
      buf[idx++] = CMD_RASET;
      if (flg_large) { buf[idx++] = ys >> 8; }  buf[idx++] = ys;
      if (flg_large) { buf[idx++] = ye >> 8; }  buf[idx++] = ye;
    }
    if (idx)
    {
      _check_repeat();
      _bus->writeBytes(buf, idx, false, true);
    }
  }


  static uint8_t* store_encoded(uint8_t* dst, const uint8_t* src, size_t data_num, size_t bytes)
  {
    *dst++ = data_num;
    memmove(dst, src, bytes);
    dst += bytes;
    return dst;
  }

  static uint8_t* store_absolute(uint8_t* dst, const uint8_t* src, size_t src_size, size_t bytes)
  {
    if (src_size >= 3)  // 絶対モード;
    {
      *dst++ = 0x00;
      *dst++ = src_size;
      memmove(dst, src, src_size * bytes);
      dst += src_size * bytes;
    }
    else  // RLEモード;
    {
      for (size_t i = 0; i < src_size; i++)
      {
        dst = store_encoded(dst, src + i * bytes, 1, bytes);
      }
    }
    return dst;
  }
/*
  static size_t rleDecode(const uint8_t* src, size_t len, size_t bytes)
  {
    size_t res = 0;
    size_t idx = 0;
    while (idx < len)
    {
      if (src[idx++] == 0)
      {
        size_t abs_len = src[idx++];
        idx += abs_len * bytes;
        res += abs_len;
        continue;
      }
      res += src[idx-1];
      idx += bytes;
    }
    return res;
  }
*/
  static size_t rleEncode(uint8_t* dest, const uint8_t* src, size_t bytelen, size_t bytes)
  {
/*
Serial.printf("bytelen:%d bytes:%d\r\n", bytelen, bytes);
for (int i = 0; i < bytelen; ++i) { Serial.printf("%02x ",src[i]); } Serial.println();
//*/
    static constexpr size_t maxbyte = 255;

    uint8_t* pdest = dest;
    const uint8_t* pabs = src;
    const uint8_t* prev = src;
 
    int_fast16_t cont = 1;
    int_fast16_t abso = 0;

    for (size_t i = bytes; i < bytelen; i += bytes)
    {
      size_t byteidx = 0;
      while (src[i + byteidx] == src[i + byteidx - bytes] && ++byteidx != bytes);

      if (byteidx == bytes)
      {
        cont++;
        if (abso >= 1)
        {
          pdest = store_absolute(pdest, pabs, abso, bytes);
        }
        else if (cont == maxbyte)
        {
          pdest = store_encoded(pdest, prev, maxbyte, bytes);
          cont = 1;
          prev = src + i;
        }
        abso = -1;
      }
      else
      {
        abso++;
        if (cont >= 2)
        {
          pdest = store_encoded(pdest, prev, cont, bytes);
        }
        else if (abso == maxbyte)
        {
          pdest = store_absolute(pdest, pabs, maxbyte, bytes);
          abso = 0;
        }
        cont = 1;
        if (abso == 0) { pabs = src + i; }
        prev = src + i;
      }
    }
     if (abso >= 0)
    {
      pdest = store_absolute(pdest, pabs, abso + 1, bytes);
    }
    else if (cont >= 2)
    {
      pdest = store_encoded(pdest, prev, cont, bytes);
    }
/*
size_t res = pdest - dest;
if (bytelen != rleDecode(dest, res, bytes)*bytes) {
  Serial.printf("res:%d\r\n", res);
  for (int i = 0; i < res; ++i) { Serial.printf("%02x ",dest[i]); } Serial.println();
}
//*/
    return pdest - dest;
  }
//*
  void Panel_M5UnitLCD::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    (void)use_dma;
    auto bytes = _write_bits >> 3;
    uint32_t wb = length * bytes;
    auto dmabuf = _bus->getDMABuffer(wb + (wb >> 7) + 128);
    dmabuf[0] = CMD_WRITE_RLE | bytes;
    size_t idx = _check_repeat(dmabuf[0]) ? 0 : 1;

    auto buf = &dmabuf[(wb >> 7) + 128];
    param->fp_copy(buf, 0, length, param);
    size_t writelen = idx + rleEncode(&dmabuf[idx], buf, length * bytes, bytes);
    _bus->writeBytes(dmabuf, writelen, false, true);
    _raw_color = ~0u;
  }
/*/
  void Panel_M5UnitLCD::writePixels(pixelcopy_t* param, uint32_t length)
  {
    auto bytes = _write_bits >> 3;
    uint32_t wb = length * bytes;
    auto dmabuf = _bus->getDMABuffer(wb + (wb >> 7) + 1);
    dmabuf[0] = CMD_WRITE_RAW | _write_bits >> 3;
    size_t idx = _check_repeat(dmabuf[0]) ? 0 : 1;

    auto buf = &dmabuf[idx];
    param->fp_copy(buf, 0, length, param);
    size_t writelen = idx + wb;
    _bus->writeBytes(dmabuf, writelen, false, true);
    _raw_color = ~0u;
  }
//*/
/*
  void Panel_M5UnitLCD::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    uint32_t sx32 = param->src_x32;
    auto bytes = _write_bits >> 3;
    uint32_t y_add = 1;
    bool transp = (param->transp != pixelcopy_t::NON_TRANSP);
    if (!transp) { _set_window(x, y, x+w-1, y+h-1); }
    uint32_t wb = w * bytes;
    do
    {
      uint32_t i = 0;
      while (w != (i = param->fp_skip(i, w, param)))
      {
        auto dmabuf = _bus->getDMABuffer(wb + 1);
        dmabuf[0] = CMD_WRITE_RAW | ((_write_bits >> 3) & 3);
        auto buf = &dmabuf[1];
        int32_t len = param->fp_copy(buf, 0, w - i, param);
        if (transp) { _set_window(x + i, y, x + i + len - 1, y); }
        _bus->writeBytes(dmabuf, 1 + wb, false, true);
        if (w == (i += len)) break;
      }
      param->src_x32 = sx32;
      param->src_y++;
      y += y_add;
    } while (--h);
    _raw_color = ~0u;
  }
/*/
  void Panel_M5UnitLCD::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    (void)use_dma;
    // _xs_raw = ~0u;
    // _ys_raw = ~0u;

    uint32_t sx32 = param->src_x32;
    auto bytes = _write_bits >> 3;
    uint32_t y_add = 1;
    uint32_t cmd = CMD_WRITE_RLE | bytes;
    bool transp = (param->transp != pixelcopy_t::NON_TRANSP);
    if (!transp)
    {
      _set_window(x, y, x+w-1, y+h-1);
    }
    uint32_t wb = w * bytes;
    do
    {
      uint32_t i = 0;
      while (w != (i = param->fp_skip(i, w, param)))
      {
        auto sub = (w - i) >> 2;
        _buff_free_count = (_buff_free_count > sub)
                         ? (_buff_free_count - sub)
                         : 0;
        auto dmabuf = _bus->getDMABuffer(wb + (wb >> 7) + 128);
        dmabuf[0] = cmd;
        auto buf = &dmabuf[(wb >> 7) + 128];
        int32_t len = param->fp_copy(buf, 0, w - i, param);
        if (transp)
        {
          _set_window(x + i, y, x + i + len - 1, y);
        }
        if (!_check_repeat(cmd))
        {
          _bus->writeCommand(cmd, 8);
        }
        size_t idx = 0;
        size_t writelen = rleEncode(&dmabuf[idx], buf, len * bytes, bytes);
        _bus->writeBytes(dmabuf, writelen, false, true);
        if (w == (i += len)) break;
      }
      param->src_x32 = sx32;
      param->src_y++;
      y += y_add;
    } while (--h);
    _raw_color = ~0u;
  }

  void Panel_M5UnitLCD::writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param)
  {
    _set_window(x, y, x + w - 1, y);
    auto buf = (uint32_t*)param->src_data;
    if (!_check_repeat(CMD_WRITE_RAW_32))
    {
      writeCommand(CMD_WRITE_RAW_32, 1);
    }
    for (size_t i = 0; i < w; ++i)
    {
      _bus->writeCommand(getSwap32(buf[i]), 32);
    }
    _raw_color = ~0u;
  }

  void Panel_M5UnitLCD::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    startWrite();
    int retry = 4;
    do {
      _check_repeat(0, 255);
    } while (_buff_free_count < 255 && --retry);
    _set_window(x, y, x+w-1, y+h-1);

    _bus->writeCommand(CMD_READ_RAW | ((_read_bits >> 3) & 3), 8);
    if (param->no_convert)
    {
      _bus->readBytes((uint8_t*)dst, w * h * _read_bits >> 3, true);
    }
    else
    {
      _bus->readPixels(dst, param, w * h);
    }
    endWrite();
    if (_start_count)
    {
      _bus->endTransaction();
      _bus->beginTransaction();
    }
  }

  void Panel_M5UnitLCD::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    uint8_t buf[16];
    size_t idx = 0;
    buf[idx++] = CMD_COPYRECT;
    auto xe = src_x + w - 1;
    auto ye = src_y + h - 1;

    if (_cfg.memory_width >= 256) buf[idx++] = src_x >> 8;
    buf[idx++] = src_x;

    if (_cfg.memory_height >= 256) buf[idx++] = src_y >> 8;
    buf[idx++] = src_y;

    if (_cfg.memory_width >= 256) buf[idx++] = xe >> 8;
    buf[idx++] = xe;

    if (_cfg.memory_height >= 256) buf[idx++] = ye >> 8;
    buf[idx++] = ye;

    if (_cfg.memory_width >= 256) buf[idx++] = dst_x >> 8;
    buf[idx++] = dst_x;

    if (_cfg.memory_height >= 256) buf[idx++] = dst_y >> 8;
    buf[idx++] = dst_y;

    startWrite();
    _check_repeat();
    _bus->writeBytes(buf, idx, false, true);
    endWrite();
  }

//----------------------------------------------------------------------------
 }
}
