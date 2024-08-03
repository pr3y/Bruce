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

#include "LGFX_Sprite.hpp"

#include "misc/common_function.hpp"

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

  void Panel_Sprite::setBuffer(void* buffer, int32_t w, int32_t h, color_conv_t* conv)
  {
    deleteSprite();

    _img.reset(buffer);
    uint32_t x_mask = 7 >> (conv->bits >> 1);
    _bitwidth = (w + x_mask) & (~x_mask);
    _panel_width = w;
    _xe = w - 1;
    _panel_height = h;
    _ye = h - 1;

    setRotation(_rotation);
  }

  void Panel_Sprite::deleteSprite(void)
  {
    _bitwidth = _panel_width = _panel_height = _width = _height = 0;
    setRotation(_rotation);
    _img.release();
  }

  void* Panel_Sprite::createSprite(int32_t w, int32_t h, color_conv_t* conv, bool psram)
  {
    if (w < 1 || h < 1)
    {
      deleteSprite();
      return nullptr;
    }
    if (!_img || (uint_fast16_t)w != _panel_width || (uint_fast16_t)h != _panel_height)
    {
      _panel_width = w;
      _panel_height = h;
      uint32_t x_mask = 7 >> (conv->bits >> 1);
      _bitwidth = (w + x_mask) & (~x_mask);
      size_t len = h * (_bitwidth * _write_bits >> 3) + std::max(1, _write_bits >> 3);

      _img.reset(len, psram ? AllocationSource::Psram : AllocationSource::Dma);

      if (!_img)
      {
        deleteSprite();
        return nullptr;
      }
    }
    memset(_img, 0, (_bitwidth * _write_bits >> 3) * _panel_height);

    setRotation(_rotation);

    return _img;
  }

  color_depth_t Panel_Sprite::setColorDepth(color_depth_t depth)
  {
    _write_depth = depth;
    _read_depth = depth;
    return depth;
  }

  void Panel_Sprite::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    auto pw = _panel_width;
    auto ph = _panel_height;
    if (r & 1)
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

  void Panel_Sprite::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
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

  void Panel_Sprite::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    uint_fast8_t r = _rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
      if (r & 2)                  { x = _width  - (x + 1); }
      if (r & 1) { std::swap(x, y); }
    }
    auto bits = _write_bits;
    uint32_t index = x + y * _bitwidth;
    if (bits >= 8)
    {
      if (bits == 8)
      {
        _img.img8()[index] = rawcolor;
      }
      else if (bits == 16)
      {
        _img.img16()[index] = rawcolor;
      }
      else
      {
        _img.img24()[index] = rawcolor;
      }
    }
    else
    {
      index *= bits;
      uint8_t* dst = &_img.img8()[index >> 3];
      uint8_t mask = (uint8_t)(~(0xFF >> bits)) >> (index & 7);
      *dst = (*dst & ~mask) | (rawcolor & mask);
    }
  }

  void Panel_Sprite::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    uint_fast8_t r = _rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + h); }
      if (r & 2)                  { x = _width  - (x + w); }
      if (r & 1) { std::swap(x, y);  std::swap(w, h); }
    }

    uint_fast8_t bits = _write_bits;
    if (bits >= 8)
    {
      if (w > 1)
      {
        uint_fast8_t bytes = bits >> 3;
        uint_fast16_t bw = _bitwidth;
        uint8_t* dst = &_img[(x + y * bw) * bytes];
        uint8_t* src = dst;
        uint_fast16_t add_dst = bw * bytes;
        uint_fast32_t len = w * bytes;
        uint_fast32_t w32 = w;

        if (_img.use_memcpy())
        {
          if (w32 != bw)
          {
            dst += add_dst;
          }
          else
          {
            w32 *= h;
            h = 1;
          }
        }
        else
        {
          src = (uint8_t*)alloca(len);
          ++h;
        }
        memset_multi(src, rawcolor, bytes, w32);
        while (--h)
        {
          memcpy(dst, src, len);
          dst += add_dst;
        }
      }
      else
      {
        uint_fast16_t bw = _bitwidth;
        uint32_t index = x + y * bw;
        if (bits == 8)
        {
          auto img = &_img[index];
          do { *img = rawcolor;  img += bw; } while (--h);
        }
        else if (bits == 16)
        {
          auto img = &_img.img16()[index];
          do { *img = rawcolor;  img += bw; } while (--h);
        }
        else if (bits == 32)
        {
          auto img = &_img.img32()[index];
          do { *img = rawcolor; img += bw; } while (--h);
        }
        else
        {
          auto img = &_img.img24()[index];
          do { *img = rawcolor; img += bw; } while (--h);
        }
      }
    }
    else
    {
      x *= bits;
      w *= bits;
      uint32_t add_dst = _bitwidth * bits >> 3;
      uint8_t* dst = &_img[y * add_dst + (x >> 3)];
      uint32_t len = ((x + w) >> 3) - (x >> 3);
      uint8_t mask = 0xFF >> (x & 7);
      if (len)
      {
        if (mask != 0xFF)
        {
          --len;
          auto d = dst++;
          uint8_t mc = rawcolor & mask;
          auto i = h;
          do { *d = (*d & ~mask) | mc; d += add_dst; } while (--i);
        }
        mask = ~(0xFF>>((x + w) & 7));
        if (len)
        {
          auto d = dst;
          auto i = h;
          do { memset(d, rawcolor, len); d += add_dst; } while (--i);
          dst += len;
        }
        if (mask == 0) return;
      }
      else
      {
        mask ^= mask >> w;
      }
      rawcolor &= mask;
      do { *dst = (*dst & ~mask) | rawcolor; dst += add_dst; } while (--h);
    }
  }

  void Panel_Sprite::writeBlock(uint32_t rawcolor, uint32_t length)
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

  void Panel_Sprite::_rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty)
  {
    uint32_t addx = param->src_x32_add;
    uint32_t addy = param->src_y32_add;
    uint_fast8_t r = _rotation;
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

  void Panel_Sprite::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    (void)use_dma;
    uint_fast16_t xs = _xs;
    uint_fast16_t xe = _xe;
    uint_fast16_t ys = _ys;
    uint_fast16_t ye = _ye;
    uint_fast16_t x = _xpos;
    uint_fast16_t y = _ypos;
    const size_t bits = _write_bits;
    auto k = _bitwidth * bits >> 3;

    uint_fast8_t r = _rotation;
    if (!r)
    {
      uint_fast16_t linelength;
      do {
        linelength = std::min<uint_fast16_t>(xe - x + 1, length);
        param->fp_copy(&_img.img8()[y * k], x, x + linelength, param);
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
    if (param->no_convert)
    {
      size_t bytes = bits >> 3;
      size_t xw = 1;
      size_t yw = _bitwidth;
      if (r & 1) std::swap(xw, yw);
      size_t idx = y * yw + x * xw;
      auto data = (uint8_t*)param->src_data;
      do
      {
        auto dst = &_img.img8()[idx * bytes];
        size_t b = 0;
        do
        {
          dst[b] = *data++;
        } while (++b < bytes);
        if (x != xe)
        {
          idx += xw * ax;
          x += ax;
        }
        else
        {
          x = xs;
          y = (y != ye) ? (y + ay) : ys;
          idx = y * yw + x * xw;
        }
      } while (--length);
    }
    else
    {
      if (r & 1)
      {
        do
        {
          param->fp_copy(&_img.img8()[x * k], y, y + 1, param); /// xとyを入れ替えて処理する;
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
        do
        {
          param->fp_copy(&_img.img8()[y * k], x, x + 1, param);
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
    }
    if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
    if (r & 2)                  { x = _width  - (x + 1); }
    _xpos = x;
    _ypos = y;
  }

  void Panel_Sprite::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool)
  {
    uint_fast8_t r = _rotation;
    if (r == 0 && param->transp == pixelcopy_t::NON_TRANSP && param->no_convert && _img.use_memcpy())
    {
      auto sx = param->src_x;
      auto bits = param->src_bits;
      bool flg_memcpy = (0 == (bits & 7));
      if (!flg_memcpy)
      {
        uint_fast8_t mask = (bits == 1) ? 7
                          : (bits == 2) ? 3
                                        : 1;
        flg_memcpy = (sx & mask) == (x & mask) && (w == this->_panel_width || 0 == (w & mask));
      }
      if (flg_memcpy)
      {
        auto bw = _bitwidth * bits >> 3;
        auto dst = &_img[bw * y];
        auto sw = param->src_bitwidth * bits >> 3;
        auto src = &((uint8_t*)param->src_data)[param->src_y * sw];
        if (sw == bw && this->_panel_width == w && sx == 0 && x == 0)
        {
          memcpy_P(dst, src, bw * h);
          return;
        }
        y = 0;
        dst +=  x * bits >> 3;
        src += sx * bits >> 3;
        w    =  w * bits >> 3;
        do
        {
          memcpy_P(&dst[y * bw], &src[y * sw], w);
        } while (++y != h);
        return;
      }
    }

    uint32_t nextx = 0;
    uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
    if (r)
    {
      _rotate_pixelcopy(x, y, w, h, param, nextx, nexty);
    }
    uint32_t sx32 = param->src_x32;
    uint32_t sy32 = param->src_y32;

    uint32_t yb = y * _bitwidth;
    do
    {
      int32_t pos = x + yb;
      int32_t end = pos + w;
      while (end != (pos = param->fp_copy(_img, pos, end, param))
         &&  end != (pos = param->fp_skip(      pos, end, param)));
      param->src_x32 = (sx32 += nextx);
      param->src_y32 = (sy32 += nexty);
      yb += _bitwidth;
    } while (--h);
  }

  void Panel_Sprite::writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param)
  {
    uint32_t nextx = 0;
    uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
    if (_rotation)
    {
      _rotate_pixelcopy(x, y, w, h, param, nextx, nexty);
    }
    uint32_t sx32 = param->src_x32;
    uint32_t sy32 = param->src_y32;

    uint32_t pos = x + y * _bitwidth;
    uint32_t end = pos + w;
    param->fp_copy(_img, pos, end, param);
    while (--h)
    {
      pos += _bitwidth;
      end = pos + w;
      param->src_x32 = (sx32 += nextx);
      param->src_y32 = (sy32 += nexty);
      param->fp_copy(_img, pos, end, param);
    }
  }

  uint32_t Panel_Sprite::readPixelValue(uint_fast16_t x, uint_fast16_t y)
  {
    uint_fast8_t r = _rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
      if (r & 2)                  { x = _width  - (x + 1); }
      if (r & 1) { std::swap(x, y); }
    }

    if (x >= _panel_width || y >= _panel_height) return 0;
    size_t index = x + y * _bitwidth;
    auto bits = _read_bits;
    if (bits >= 8)
    {
      if (bits == 8)
      {
        return _img.img8()[index];
      }
      else if (bits == 16)
      {
        return _img.img16()[index];
      }
      return (uint32_t)_img.img24()[index];
    }
    index *= bits;
    uint8_t mask = (1 << bits) - 1;
    return (_img.img8()[index >> 3] >> (-(int32_t)(index + bits) & 7)) & mask;
  }

  void Panel_Sprite::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    uint_fast8_t r = _rotation;
    if (0 == r && param->no_convert && _write_bits >= 8)
    {
      h += y;
      auto bytes = _write_bits >> 3;
      auto bw = _bitwidth;
      auto d = (uint8_t*)dst;
      w *= bytes;
      do {
        memcpy(d, &_img[(x + y * bw) * bytes], w);
        d += w;
      } while (++y != h);
    }
    else
    {
      param->src_bitwidth = _bitwidth;
      param->src_data = _img;
      uint32_t nextx = 0;
      uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
      if (r)
      {
        uint32_t addx = param->src_x32_add;
        uint32_t addy = param->src_y32_add;
        uint_fast8_t rb = 1 << r;
        if (rb & 0b10010110) // case 1:2:4:7:
        {
          nexty = -(int32_t)nexty;
          y = _height - (y + 1);
        }
        if (r & 2)
        {
          addx = -(int32_t)addx;
          x = _width - (x + 1);
        }
        if ((r+1) & 2)
        {
          addy  = -(int32_t)addy;
        }
        if (r & 1)
        {
          std::swap(x, y);
          std::swap(addx, addy);
          std::swap(nextx, nexty);
        }
        param->src_x32_add = addx;
        param->src_y32_add = addy;
      }
      size_t dstindex = 0;
      uint32_t x32 = x << pixelcopy_t::FP_SCALE;
      uint32_t y32 = y << pixelcopy_t::FP_SCALE;
      param->src_x32 = x32;
      param->src_y32 = y32;
      do
      {
        param->src_x32 = x32;
        x32 += nextx;
        param->src_y32 = y32;
        y32 += nexty;
        dstindex = param->fp_copy(dst, dstindex, dstindex + w, param);
      } while (--h);
    }
  }

  void Panel_Sprite::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    uint_fast8_t r = _rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { src_y = _height - (src_y + h); dst_y = _height - (dst_y + h); }
      if (r & 2)                  { src_x = _width  - (src_x + w); dst_x = _width  - (dst_x + w); }
      if (r & 1) { std::swap(src_x, src_y);  std::swap(dst_x, dst_y);  std::swap(w, h); }
    }

    if (_write_bits < 8) {
      pixelcopy_t param(_img, _write_depth, _write_depth);
      param.src_bitwidth = _bitwidth;
      int32_t add_y = (src_y < dst_y) ? -1 : 1;
      if (src_y != dst_y) {
        if (src_y < dst_y) {
          src_y += h - 1;
          dst_y += h - 1;
        }
        param.src_y = src_y;
        do
        {
          param.src_x = src_x;
          auto idx = dst_x + dst_y * _bitwidth;
          param.fp_copy(_img, idx, idx + w, &param);
          dst_y += add_y;
          param.src_y += add_y;
        } while (--h);
      } else {
        size_t len = (_bitwidth * _write_bits) >> 3;
        auto buf = (uint8_t*)alloca(len);
        param.src_data = buf;
        param.src_y32 = 0;
        do {
          memcpy(buf, &_img[src_y * len], len);
          param.src_x = src_x;
          auto idx = dst_x + dst_y * _bitwidth;
          param.fp_copy(_img, idx, idx + w, &param);
          dst_y += add_y;
          src_y += add_y;
        } while (--h);
      }
    }
    else
    {
      size_t bytes = _write_bits >> 3;
      size_t len = w * bytes;
      int32_t add = _bitwidth * bytes;
      if (src_y < dst_y) add = -add;
      int32_t pos = (src_y < dst_y) ? h - 1 : 0;
      uint8_t* src = &_img.img8()[(src_x + (src_y + pos) * _bitwidth) * bytes];
      uint8_t* dst = &_img.img8()[(dst_x + (dst_y + pos) * _bitwidth) * bytes];
      if (_img.use_memcpy())
      {
        do
        {
          memmove(dst, src, len);
          src += add;
          dst += add;
        } while (--h);
      }
      else
      {
        auto buf = (uint8_t*)alloca(len);
        do
        {
          memcpy(buf, src, len);
          memcpy(dst, buf, len);
          src += add;
          dst += add;
        } while (--h);
      }
    }
  }

//----------------------------------------------------------------------------

  bool LGFX_Sprite::create_from_bmp_file(DataWrapper* data, const char *path) {
    data->need_transaction = false;
    bool res = false;
    if (data->open(path)) {
      res = createFromBmp(data);
      data->close();
    }
    return res;
  }

  bool LGFX_Sprite::createFromBmp(DataWrapper* data)
  {
    bitmap_header_t bmpdata;

    if (!bmpdata.load_bmp_header(data)
      || ( bmpdata.biCompression > 3)) {
      return false;
    }
    uint32_t seekOffset = bmpdata.bfOffBits;
    uint_fast16_t bpp = bmpdata.biBitCount; // 24 bcBitCount 24=RGB24bit
    setColorDepth(bpp < 32 ? bpp : 24);
    uint32_t w = bmpdata.biWidth;
    int32_t h = bmpdata.biHeight;  // bcHeight Image height (pixels)
    if (!createSprite(w, h)) return false;

      //If the value of Height is positive, the image data is from bottom to top
      //If the value of Height is negative, the image data is from top to bottom.
    int32_t flow = (h < 0) ? 1 : -1;
    int32_t y = 0;
    if (h < 0) h = -h;
    else y = h - 1;

    if (bpp <= 8) {
      if (!_palette) createPalette();
      uint_fast16_t palettecount = 1 << bpp;
      argb8888_t *palette = (argb8888_t*)alloca(sizeof(argb8888_t*) * palettecount);
      data->seek(bmpdata.biSize + 14);
      data->read((uint8_t*)palette, (palettecount * sizeof(argb8888_t))); // load palette
      for (uint_fast16_t i = 0; i < _palette_count; ++i)
      {
        _palette.img24()[i].set(color_convert<bgr888_t, argb8888_t>(palette[i].get()));
      }
    }

    data->seek(seekOffset);

    auto bitwidth = _panel_sprite._bitwidth;

    size_t buffersize = ((w * bpp + 31) >> 5) << 2;  // readline 4Byte align.
    auto lineBuffer = (uint8_t*)alloca(buffersize);
    if (bpp <= 8) {
      do {
        if (bmpdata.biCompression == 1) {
          bmpdata.load_bmp_rle8(data, lineBuffer, w);
        } else
        if (bmpdata.biCompression == 2) {
          bmpdata.load_bmp_rle4(data, lineBuffer, w);
        } else {
          data->read(lineBuffer, buffersize);
        }
        memcpy(&_img8[y * bitwidth * bpp >> 3], lineBuffer, (w * bpp + 7) >> 3);
        y += flow;
      } while (--h);
    } else if (bpp == 16) {
      do {
        data->read(lineBuffer, buffersize);
        auto img = (uint16_t*)(&_img8[y * bitwidth * bpp >> 3]);
        y += flow;
        for (size_t i = 0; i < w; ++i)
        {
          img[i] = (lineBuffer[i << 1] << 8) + lineBuffer[(i << 1) + 1];
        }
      } while (--h);
    } else if (bpp == 24) {
      do {
        data->read(lineBuffer, buffersize);
        auto img = &_img8[y * bitwidth * bpp >> 3];
        y += flow;
        for (size_t i = 0; i < w; ++i) {
          img[i * 3    ] = lineBuffer[i * 3 + 2];
          img[i * 3 + 1] = lineBuffer[i * 3 + 1];
          img[i * 3 + 2] = lineBuffer[i * 3    ];
        }
      } while (--h);
    } else if (bpp == 32) {
      do {
        data->read(lineBuffer, buffersize);
        auto img = &_img8[y * bitwidth * 3];
        y += flow;
        for (size_t i = 0; i < w; ++i) {
          img[i * 3    ] = lineBuffer[(i << 2) + 2];
          img[i * 3 + 1] = lineBuffer[(i << 2) + 1];
          img[i * 3 + 2] = lineBuffer[(i << 2) + 0];
        }
      } while (--h);
    }
    return true;
  }

//----------------------------------------------------------------------------
 }
}

