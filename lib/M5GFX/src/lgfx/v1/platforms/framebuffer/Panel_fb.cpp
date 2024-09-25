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

Porting for Linux FrameBuffer:
 [imliubo](https://github.com/imliubo)
/----------------------------------------------------------------------------*/
#if defined ( LGFX_LINUX_FB )

#include "Panel_fb.hpp"

#include "../common.hpp"
#include "../../Bus.hpp"

#include <list>
#include <dirent.h>
#include <string>
#include <fstream>
#include <sstream>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------
  void Panel_fb::fb_draw_rgb_pixel(int x, int y, uint32_t rawcolor)
  {
    unsigned int pix_offset = 0;
    uint8_t r = 0, g = 0, b = 0;

    // GGGBBBBB RRRRRGGG
    if (_write_depth = color_depth_t::rgb565_2Byte)
    {
      b = (uint8_t)((rawcolor >> 8) & 0b11111);
      g = (uint8_t)((((rawcolor >> 10) & 0b111000)) | (rawcolor & 0b111));
      r = (uint8_t)((rawcolor >> 3) & 0b11111);

      pix_offset = x * 2 + y * _fix_info.line_length;
      unsigned short c = (r << 11) | (g << 5) | b;
      // write 'two bytes at once'
      *((unsigned short*)(_fbp + pix_offset)) = c;
    }
    // BBBBBBBB GGGGGGGG RRRRRRRR
    else if (_write_depth = color_depth_t::rgb888_3Byte)
    {
      b = (uint8_t)((rawcolor >> 16) & 0xff);
      g = (uint8_t)((rawcolor >> 8) & 0xff);
      r = (uint8_t)((rawcolor >> 0) & 0xff);

      pix_offset = x * 3 + y * _fix_info.line_length;
      *((char*)(_fbp + pix_offset)) = b;
      *((char*)(_fbp + pix_offset + 1)) = g;
      *((char*)(_fbp + pix_offset + 2)) = r;
    }
  }

  void Panel_fb::fb_draw_argb_pixel(int x, int y, uint32_t rawcolor)
  {
    unsigned int pix_offset = x * 4 + y * _fix_info.line_length;

    uint8_t r = 0, g = 0, b = 0, a = 0;

    // BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
    b = (uint8_t)((rawcolor >> 24) & 0xff);
    g = (uint8_t)((rawcolor >> 16) & 0xff);
    r = (uint8_t)((rawcolor >> 8) & 0xff);
    a = (uint8_t)((rawcolor >> 0) & 0xff);
    
    *((char*)(_fbp + pix_offset)) = b;
    *((char*)(_fbp + pix_offset + 1)) = g;
    *((char*)(_fbp + pix_offset + 2)) = r;
    *((char*)(_fbp + pix_offset + 3)) = a;
  }

  Panel_fb::~Panel_fb(void)
  {
    // unmap fb file from memory
    munmap(_fbp, _screensize);
    // reset the display mode
    if (ioctl(_fbfd, FBIOPUT_VSCREENINFO, &_fix_info)) {
        printf("Error re-setting variable information.\n");
    }
    // close fb file    
    close(_fbfd);

    memset(&_fix_info, 0, sizeof(_fix_info));
    memset(&_var_info, 0, sizeof(_fix_info));
  }

  Panel_fb::Panel_fb(void) : Panel_Device(), _fbp(nullptr)
  {
    _fbp = 0;
  }

  bool Panel_fb::init(bool use_reset)
  {
    _fbfd = open(_config_detail.device_name, O_RDWR);
    if (_fbfd == -1) {
      // detect target framebuffer.
      DIR* sysfs_graphics = opendir("/sys/class/graphics");
      struct dirent* entry;
      std::string path;
      std::string target = "/dev/fb0";
      while((entry = readdir(sysfs_graphics)) != NULL) {
        if( entry->d_type == DT_LNK ) {
          path = "/sys/class/graphics/";
          path.append(entry->d_name);
          path.append("/name");
          std::ifstream fs(path.c_str());
          if( !fs.is_open() ) continue;
          std::stringstream buffer;
          buffer << fs.rdbuf();
          if( buffer.str().find_first_of(_config_detail.device_name) != std::string::npos ) {
            target = "/dev/";
            target.append(entry->d_name);
            break;
          }
        }
      }
      closedir(sysfs_graphics);

      _fbfd = open(target.c_str(), O_RDWR);
      if (_fbfd == -1) {
        printf("Error: cannot open framebuffer device.\n");
        return false;
      }
    }
    // printf("The framebuffer device was opened successfully.\n");

    // Get variable screen information
    if (ioctl(_fbfd, FBIOGET_VSCREENINFO, &_var_info)) {
        printf("Error reading variable information.\n");
        return false;
    }
    // printf("%dx%d, %dbpp\n", _var_info.xres, _var_info.yres, _var_info.bits_per_pixel);

    // 16/24/32
    setColorDepth((color_depth_t)_var_info.bits_per_pixel);

    // Get fixed screen information
    if (ioctl(_fbfd, FBIOGET_FSCREENINFO, &_fix_info)) {
        printf("Error reading fixed information.\n");
        return false;
    }

    // Figure out the size of the screen in bytes
    _screensize = _fix_info.smem_len;  //finfo.line_length * vinfo.yres;    

    // Map the device to memory
    _fbp = (char *)mmap(0, _screensize, PROT_READ | PROT_WRITE, MAP_SHARED, _fbfd, 0);
    if((intptr_t)_fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return false;
    }
    memset(_fbp, 0, _screensize);

    return Panel_Device::init(use_reset);
  }

  void Panel_fb::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    // Nothings to do
  }

  color_depth_t Panel_fb::setColorDepth(color_depth_t depth)
  {
    // TODO
    _write_bits = depth;
    _read_bits = depth;
    _write_depth = depth;
    _read_depth = depth;
    return depth;
  }

  void Panel_fb::beginTransaction(void) {}

  void Panel_fb::endTransaction(void) {}

  void Panel_fb::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;
    if (_internal_rotation & 1) std::swap(_width, _height);
  }

  void Panel_fb::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    xs = std::max<unsigned long>(0u, std::min<uint_fast16_t>(_width  - 1, xs));
    xe = std::max<unsigned long>(0u, std::min<uint_fast16_t>(_width  - 1, xe));
    ys = std::max<unsigned long>(0u, std::min<uint_fast16_t>(_height - 1, ys));
    ye = std::max<unsigned long>(0u, std::min<uint_fast16_t>(_height - 1, ye));
    _xpos = xs;
    _xs = xs;
    _xe = xe;
    _ypos = ys;
    _ys = ys;
    _ye = ye;
  }

  void Panel_fb::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    uint_fast8_t rotation = _internal_rotation;
    if (rotation)
    {
      if ((1u << rotation) & 0b10010110) { y = _height - (y + 1); }
      if (rotation & 2)                  { x = _width  - (x + 1); }
      if (rotation & 1) { std::swap(x, y); }
    }

    switch (_write_depth)
    {
      case color_depth_t::rgb565_2Byte:
      case color_depth_t::rgb888_3Byte:
        fb_draw_rgb_pixel(x, y, rawcolor);
        break;
      case color_depth_t::argb8888_4Byte:
        fb_draw_argb_pixel(x, y, rawcolor);
        break;
      default:
        break;
    }

    if (!getStartCount())
    {
      display(x, y, 1, 1);
    }
  }

  void Panel_fb::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    uint_fast8_t rotation = _internal_rotation;
    if (rotation)
    {
      if ((1u << rotation) & 0b10010110) { y = _height - (y + 1); }
      if (rotation & 2)                  { x = _width  - (x + 1); }
      if (rotation & 1) { std::swap(x, y); }
    }

    for (size_t width = 0; width < w; width++)
    {
      for (size_t height = 0; height < h; height++)
      {
        uint_fast16_t x_pos = x + width;
        uint_fast16_t y_pos = y + height;

        switch (_write_depth)
        {
          case color_depth_t::rgb565_2Byte:
          case color_depth_t::rgb888_3Byte:
            fb_draw_rgb_pixel(x_pos, y_pos, rawcolor);
            break;
          case color_depth_t::argb8888_4Byte:
            fb_draw_argb_pixel(x_pos, y_pos, rawcolor);
            break;
          default:
            break;
        }
      }
    }
  }

  void Panel_fb::writeBlock(uint32_t rawcolor, uint32_t length)
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

  void Panel_fb::_rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty)
  {
    // NOT TEST
    uint32_t addx = param->src_x32_add;
    uint32_t addy = param->src_y32_add;
    uint_fast8_t r = _internal_rotation;
    uint_fast8_t bitr = 1u << r;
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

  void Panel_fb::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    // NOT TEST
    uint_fast16_t xs = _xs;
    uint_fast16_t xe = _xe;
    uint_fast16_t ys = _ys;
    uint_fast16_t ye = _ye;
    uint_fast16_t x = _xpos;
    uint_fast16_t y = _ypos;
    const size_t bits = _write_bits;
    auto k = _cfg.panel_width * bits >> 3;

    uint_fast8_t r = _internal_rotation;
    if (!r)
    {
      uint_fast16_t linelength;
      do {
        linelength = std::min<uint_fast16_t>(xe - x + 1, length);
        param->fp_copy(&_fbp[y * k], x, x + linelength, param);
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
      size_t yw = _cfg.panel_width;
      if (r & 1) std::swap(xw, yw);
      size_t idx = y * yw + x * xw;
      auto data = (uint8_t*)param->src_data;
      do
      {
        auto dst = &_fbp[idx * bytes];
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
          param->fp_copy(&_fbp[x * k], y, y + 1, param);
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
          param->fp_copy(&_fbp[y * k], x, x + 1, param);
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

  void Panel_fb::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool)
  {
    // NOT TEST
    uint_fast8_t r = _internal_rotation;
    if (r == 0 && param->transp == pixelcopy_t::NON_TRANSP && param->no_convert)
    {
      auto sx = param->src_x;
      auto bits = param->src_bits;

      auto bw = _cfg.panel_width * bits >> 3;
      auto dst = &_fbp[bw * y];
      auto sw = param->src_bitwidth * bits >> 3;
      auto src = &((uint8_t*)param->src_data)[param->src_y * sw];
      if (sw == bw && this->_cfg.panel_width == w && sx == 0 && x == 0)
      {
        memcpy(dst, src, bw * h);
        return;
      }
      y = 0;
      dst +=  x * bits >> 3;
      src += sx * bits >> 3;
      w    =  w * bits >> 3;
      do
      {
        memcpy(&dst[y * bw], &src[y * sw], w);
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

    y *= _cfg.panel_width;
    do
    {
      int32_t pos = x + y;
      int32_t end = pos + w;
      while (end != (pos = param->fp_copy(_fbp, pos, end, param))
         &&  end != (pos = param->fp_skip(      pos, end, param)));
      param->src_x32 = (sx32 += nextx);
      param->src_y32 = (sy32 += nexty);
      y += _cfg.panel_width;
    } while (--h);
  }

  void Panel_fb::writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param)
  {
    // NOT TEST
    uint32_t nextx = 0;
    uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
    if (_internal_rotation)
    {
      _rotate_pixelcopy(x, y, w, h, param, nextx, nexty);
    }
    uint32_t sx32 = param->src_x32;
    uint32_t sy32 = param->src_y32;

    uint32_t pos = x + y * _cfg.panel_width;
    uint32_t end = pos + w;
    param->fp_copy(_fbp, pos, end, param);
    while (--h)
    {
      pos += _cfg.panel_width;
      end = pos + w;
      param->src_x32 = (sx32 += nextx);
      param->src_y32 = (sy32 += nexty);
      param->fp_copy(_fbp, pos, end, param);
    }
  }

  void Panel_fb::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    // TODO
  }

  void Panel_fb::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {

    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { src_y = _height - (src_y + h); dst_y = _height - (dst_y + h); }
      if (r & 2)                  { src_x = _width  - (src_x + w); dst_x = _width  - (dst_x + w); }
      if (r & 1) { std::swap(src_x, src_y);  std::swap(dst_x, dst_y);  std::swap(w, h); }
    }

    if ((dst_x + w) > _var_info.xres) w = _var_info.xres - dst_x;
    if ((dst_y + h) > _var_info.yres) h = _var_info.yres - dst_y;
    size_t bytes = _write_bits >> 3;
    size_t len = w * bytes;
    int32_t add = _var_info.xres * bytes;  // _cfg.panel_width may not was the screen width, use _var_info.xres instead.
    char *src = (_fbp + (src_x * bytes + src_y * _fix_info.line_length));
    char *dst = (_fbp + (dst_x * bytes + dst_y * _fix_info.line_length));

    do
    {
      memmove(dst, src, len);
      src += add;
      dst += add;
    } while (--h);
  }

  uint_fast8_t Panel_fb::getTouchRaw(touch_point_t* tp, uint_fast8_t count)
  {
    memcpy(tp, &_touch_point, sizeof(touch_point_t));
    return _touch_point.size ? 1 : 0;
  }

//----------------------------------------------------------------------------
 }
}

#endif
