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
#include "Panel_IT8951.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/colortype.hpp"

#if __has_include (<esp_log.h>)
 #include <esp_log.h>
#endif

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

  static constexpr int8_t Bayer[16] = {-30, 2, -22, 10, 18, -14, 26, -6, -18, 14, -26, 6, 30, -2, 22, -10};

//Built in I80 Command Code
  static constexpr uint32_t IT8951_TCON_SYS_RUN         = 0x0001;
  static constexpr uint32_t IT8951_TCON_STANDBY         = 0x0002;
  static constexpr uint32_t IT8951_TCON_SLEEP           = 0x0003;
  static constexpr uint32_t IT8951_TCON_REG_RD          = 0x0010;
  static constexpr uint32_t IT8951_TCON_REG_WR          = 0x0011;
  static constexpr uint32_t IT8951_TCON_MEM_BST_RD_T    = 0x0012;
  static constexpr uint32_t IT8951_TCON_MEM_BST_RD_S    = 0x0013;
  static constexpr uint32_t IT8951_TCON_MEM_BST_WR      = 0x0014;
  static constexpr uint32_t IT8951_TCON_MEM_BST_END     = 0x0015;
  static constexpr uint32_t IT8951_TCON_LD_IMG          = 0x0020;
  static constexpr uint32_t IT8951_TCON_LD_IMG_AREA     = 0x0021;
  static constexpr uint32_t IT8951_TCON_LD_IMG_END      = 0x0022;

//I80 User defined command code
  static constexpr uint32_t IT8951_I80_CMD_DPY_AREA     = 0x0034;
  static constexpr uint32_t IT8951_I80_CMD_GET_DEV_INFO = 0x0302;
  static constexpr uint32_t IT8951_I80_CMD_DPY_BUF_AREA = 0x0037;
  static constexpr uint32_t IT8951_I80_CMD_VCOM         = 0x0039;
  static constexpr uint32_t IT8951_I80_CMD_FILLRECT     = 0x003A;

  static constexpr uint32_t IT8951_ROTATE_0    = 0;
  static constexpr uint32_t IT8951_ROTATE_90   = 1;
  static constexpr uint32_t IT8951_ROTATE_180  = 2;
  static constexpr uint32_t IT8951_ROTATE_270  = 3;

  static constexpr uint32_t IT8951_2BPP           = 0;
  static constexpr uint32_t IT8951_3BPP           = 1;
  static constexpr uint32_t IT8951_4BPP           = 2;
  static constexpr uint32_t IT8951_8BPP           = 3;
  static constexpr uint32_t IT8951_LDIMG_B_ENDIAN = 1;

/*-----------------------------------------------------------------------
IT8951 Registers defines
------------------------------------------------------------------------*/
//Register Base Address
  static constexpr uint32_t IT8951_DISPLAY_REG_BASE    = 0x1000; //Register RW access

//Base Address of Basic LUT Registers
  static constexpr uint32_t IT8951_LUT0EWHR   = (IT8951_DISPLAY_REG_BASE + 0x0000); //LUT0 Engine Width Height Reg
  static constexpr uint32_t IT8951_LUT0XYR    = (IT8951_DISPLAY_REG_BASE + 0x0040); //LUT0 XY Reg
  static constexpr uint32_t IT8951_LUT0BADDR  = (IT8951_DISPLAY_REG_BASE + 0x0080); //LUT0 Base Address Reg
  static constexpr uint32_t IT8951_LUT0MFN    = (IT8951_DISPLAY_REG_BASE + 0x00C0); //LUT0 Mode and Frame number Reg
  static constexpr uint32_t IT8951_LUT01AF    = (IT8951_DISPLAY_REG_BASE + 0x0114); //LUT0 and LUT1 Active Flag Reg

//Update Parameter Setting Register
  static constexpr uint32_t IT8951_UP0SR      = (IT8951_DISPLAY_REG_BASE + 0x134); //Update Parameter0 Setting Reg
  static constexpr uint32_t IT8951_UP1SR      = (IT8951_DISPLAY_REG_BASE + 0x138); //Update Parameter1 Setting Reg
  static constexpr uint32_t IT8951_LUT0ABFRV  = (IT8951_DISPLAY_REG_BASE + 0x13C); //LUT0 Alpha blend and Fill rectangle Value
  static constexpr uint32_t IT8951_UPBBADDR   = (IT8951_DISPLAY_REG_BASE + 0x17C); //Update Buffer Base Address
  static constexpr uint32_t IT8951_LUT0IMXY   = (IT8951_DISPLAY_REG_BASE + 0x180); //LUT0 Image buffer X/Y offset Reg
  static constexpr uint32_t IT8951_LUTAFSR    = (IT8951_DISPLAY_REG_BASE + 0x224); //LUT Status Reg (status of All LUT Engines)
  static constexpr uint32_t IT8951_BGVR       = (IT8951_DISPLAY_REG_BASE + 0x250); //Bitmap (1bpp) image color table

//System Registers
  static constexpr uint32_t IT8951_SYS_REG_BASE        = 0x0000;

//Address of System Registers
  static constexpr uint32_t IT8951_I80CPCR             = (IT8951_SYS_REG_BASE + 0x04);

//Memory Converter Registers
  static constexpr uint32_t IT8951_MCSR_BASE_ADDR      = 0x0200;
  static constexpr uint32_t IT8951_MCSR                = (IT8951_MCSR_BASE_ADDR + 0x0000);
  static constexpr uint32_t IT8951_LISAR               = (IT8951_MCSR_BASE_ADDR + 0x0008);


  Panel_IT8951::Panel_IT8951(void)
  {
    _cfg.panel_width  = _cfg.memory_width  = 2048;
    _cfg.panel_height = _cfg.memory_height = 2048;
    _epd_mode = epd_mode_t::epd_quality;
    _auto_display = true;
  }

  Panel_IT8951::~Panel_IT8951(void)
  {
  }

  color_depth_t Panel_IT8951::setColorDepth(color_depth_t depth)
  {
    _write_depth = color_depth_t::rgb888_3Byte;
    _read_depth = color_depth_t::rgb888_3Byte;
    return color_depth_t::rgb888_3Byte;
  }

  bool Panel_IT8951::init(bool use_reset)
  {
    _range_new.top = INT16_MAX;
    _range_new.left = INT16_MAX;
    _range_new.right = 0;
    _range_new.bottom = 0;

    if (!Panel_Device::init(use_reset))
    {
      return false;
    }
    pinMode(_cfg.pin_busy, pin_mode_t::input);

    _wait_busy();

    {
      uint32_t writeclock = _bus->getClock();
      if (writeclock > 12000000) { _bus->setClock(12000000); }
      uint32_t readclock = _bus->getReadClock();
      if (readclock > 2000000) { _bus->setReadClock(2000000); }

      startWrite();

      _write_command(IT8951_I80_CMD_GET_DEV_INFO);
      delay(1);
      uint16_t buf[20];
      _read_words(buf, 20);
      uint32_t addr = (buf[3] << 16) | buf[2];
      if (addr != 0 && addr != ~0u)
      {
        _tar_memaddr = addr;
// #if defined ( ESP_LOGV )
//         for (int i = 0; i < 20; ++i)
//         {
//           ESP_LOGV("Panel_IT8951", "buf[%d] = %04x : %d", i, buf[i], buf[i]);
//         }
// #endif
        if (_cfg.panel_width <= 0) { _cfg.panel_width = buf[0]; }
        if (_cfg.panel_height <= 0) { _cfg.panel_height = buf[1]; }
      }
      else
      {
        _tar_memaddr = 0x001236E0; /// default value for M5Paper.
#if defined ( ESP_LOGW )
        ESP_LOGW("Panel_IT8951", "can't read data from IT8951");
#endif
      }

      setInvert(_invert);
      setRotation(_rotation);

      _write_command(IT8951_TCON_SYS_RUN);
      _write_reg(IT8951_I80CPCR, 0x0001); //enable pack write

      _set_target_memory_addr(_tar_memaddr);

      if (_vcom > 0) { setVCOM(_vcom); }

      endWrite();

      _bus->setClock(writeclock);
      _bus->setReadClock(readclock);
    }

    return true;
  }

  uint16_t Panel_IT8951::getVCOM(void)
  {
    startWrite();
    _wait_busy();
    _write_command(IT8951_I80_CMD_VCOM);
    _write_word(0x0000);
    delay(1);
    uint16_t tmp[2] = { 0 }; // tmpの要素数を1にすると正しく読み取れない?;
    _read_words(tmp, 1);
#if defined ( ESP_LOGI )
    ESP_LOGI("DEBUG","getVCOM:%d", tmp[0]);
#endif
    endWrite();
    return tmp[0];
  }

  void Panel_IT8951::setVCOM(uint16_t vcom)
  {
    _vcom = vcom;
    if (_bus == nullptr || _cfg.pin_busy < 0) { return; }
    uint16_t current_vcom = getVCOM();
    if (current_vcom == vcom) { return; }
#if defined ( ESP_LOGI )
    ESP_LOGI("DEBUG","setVCOM:%d", vcom);
#endif

    startWrite();
    _write_command(IT8951_I80_CMD_VCOM);
    _write_word(0x0001);
    _write_word(vcom);
    endWrite();
  }

  void Panel_IT8951::beginTransaction(void)
  {
    if (_in_transaction) return;
    _in_transaction = true;
    _bus->beginTransaction();
  }

  void Panel_IT8951::endTransaction(void)
  {
    if (!_in_transaction) return;
    _in_transaction = false;
    _bus->endTransaction();
    cs_control(true);
  }

  bool Panel_IT8951::_wait_busy(uint32_t timeout)
  {
    _bus->wait();
    cs_control(true);
    if (_cfg.pin_busy >= 0 && !lgfx::gpio_in(_cfg.pin_busy))
    {
      auto start_ms = millis();
      do
      {
        uint32_t ms = millis() - start_ms;
        if (ms >= 16)
        {
          if (ms > timeout)
          {
#if defined ( ESP_LOGW )
            ESP_LOGW("Panel_IT8951", "wait_busy: timeout");
#endif
            return false;
          }
          delay(ms >> 4);
        }
      } while (!lgfx::gpio_in(_cfg.pin_busy));
    }
    cs_control(false);
    return true;
  }

  void Panel_IT8951::waitDisplay(void)
  {
    startWrite();
    _check_afsr();
    endWrite();
  }

  bool Panel_IT8951::displayBusy(void)
  {
    uint16_t infobuf[2] = { 1 };
    bool res = true;
    startWrite();
    if (_write_command(IT8951_TCON_REG_RD)
      && _write_word(IT8951_LUTAFSR)
      && _read_words(infobuf, 1))
    {
      res = (0 != infobuf[0]);
    }
    cs_control(true);
    endWrite();
    return res;
  }

  bool Panel_IT8951::_check_afsr(void)
  {
    uint32_t start_time = millis();
    uint16_t infobuf[2] = { 1 };
    do
    {
      if (_write_command(IT8951_TCON_REG_RD)
       && _write_word(IT8951_LUTAFSR)
       && _read_words(infobuf, 1)
       && infobuf[0] == 0)
      {
        break;
      }
      if (infobuf[0] == 65535) break; // IT8951 crashed
      delay(1);
    } while (millis() - start_time < 3000);

    cs_control(true);
    return infobuf[0] != 0;
  }

  bool Panel_IT8951::_write_command(uint16_t cmd)
  {
    uint32_t buf = getSwap16(0x6000) | getSwap16(cmd) << 16;
    if (!_wait_busy()) return false;
    _bus->writeData(buf, 32);
    return true;
  }

  bool Panel_IT8951::_write_word(uint16_t data)
  {
    uint32_t buf = getSwap16(data) << 16;
    if (!_wait_busy()) return false;
    _bus->writeData(buf, 32);
    return true;
  }

  bool Panel_IT8951::_write_args(uint16_t cmd, uint16_t *args, int32_t length)
  {
    if (_write_command(cmd)
     && _wait_busy())
    {
      _bus->writeData(0, 16);
      int32_t i = 0;
      do
      {
        uint32_t buf = getSwap16(args[i]);
        _bus->wait();
        while (!lgfx::gpio_in(_cfg.pin_busy));
        _bus->writeData(buf, 16);
      } while ( ++i < length );
      return true;
    }
    return false;
  }

  bool Panel_IT8951::_read_words(uint16_t *buf, uint32_t length)
  {
    if (!_wait_busy()) return false;
    _bus->writeData(getSwap16(0x1000), 16);
    _bus->beginRead(16);  // 16 bit dummy read
    _bus->readBytes(reinterpret_cast<uint8_t*>(buf), length << 1);
    _bus->endRead();
    cs_control(true);
    for (size_t i = 0; i < length; i++)
    {
      buf[i] = getSwap16(buf[i]);
    }
    return true;
  }

  bool Panel_IT8951::_write_reg(uint16_t addr, uint16_t data)
  {
    return _write_command(0x0011)
        && _write_word(addr)
        && _write_word(data);
  }

  bool Panel_IT8951::_set_target_memory_addr(uint32_t tar_addr)
  {
    return _write_reg(IT8951_LISAR + 2, tar_addr >> 16)
        && _write_reg(IT8951_LISAR    , tar_addr      );
  }

  bool Panel_IT8951::_set_area( uint32_t x, uint32_t y, uint32_t w, uint32_t h)
  {
    uint32_t rx, ry, rw, rh;
    rx = ((_it8951_rotation+1) & 2) ? _width  - w - x : x;
    ry = ( _it8951_rotation    & 2) ? _height - h - y : y;
    rw = w;
    rh = h;
    if (_it8951_rotation & 1) 
    {
      std::swap(rx, ry);
      std::swap(rw, rh);
    }

    _range_new.left   = std::min<uint32_t>(_range_new.left  , rx         );
    _range_new.right  = std::max<uint32_t>(_range_new.right , rx + rw - 1);
    _range_new.top    = std::min<uint32_t>(_range_new.top   , ry         );
    _range_new.bottom = std::max<uint32_t>(_range_new.bottom, ry + rh - 1);

    if (_epd_mode != epd_mode_t::epd_fastest
     && _range_old.horizon.intersectsWith(rx, rx + rw - 1)
     && _range_old.vertical.intersectsWith(ry, ry + rh - 1))
    {
      _check_afsr();
      _range_old.left = INT16_MAX;
      _range_old.top = INT16_MAX;
      _range_old.right = 0;
      _range_old.bottom = 0;
    }

    uint16_t params[5];
    params[0] = IT8951_LDIMG_B_ENDIAN << 8 | IT8951_4BPP << 4 | _it8951_rotation;
    params[1] = x;
    params[2] = y;
    params[3] = w;
    params[4] = h;
    return _write_args(IT8951_TCON_LD_IMG_AREA, params, 5);
  }

  bool Panel_IT8951::_update_raw_area(epd_update_mode_t mode)
  {
    if (_range_new.empty()) return false;
    uint32_t l = _range_new.left;
    uint32_t r = _range_new.right;

    // 更新範囲の幅が小さすぎる場合、IT8951がフリーズすることがある。;
    // 厳密には、範囲の左右端の座標値の下2ビット捨てた場合に同値になる場合、;
    // かつ、以前の表示更新がまだ動作中で範囲が重なる場合にフリーズする事例がある。;
    // この分岐でそれを防止する。;
    if ((l & ~3) == (r & ~3))
    {
      if (( l & 3 ) < (3-(r & 3)))
      {
        l = (l & ~3) - 1;
      }
      else
      {
        r = (r + 4) & ~3;
      }
    }
    uint32_t w = r - l + 1;
    uint16_t params[7];
    params[0] = l;
    params[1] = _range_new.top;
    params[2] = w;
    params[3] = _range_new.bottom - _range_new.top + 1;
    params[4] = mode;
    params[5] = (uint16_t)_tar_memaddr;
    params[6] = (uint16_t)(_tar_memaddr >> 16);
    return _write_args(IT8951_I80_CMD_DPY_BUF_AREA, params, 7);
  }

  void Panel_IT8951::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      if (_it8951_rotation & 4)
      {
        y = height() - y - h;
      }
      _set_area(x, y, w, h);
    }
    if (_range_new.empty()) return;

    _range_old = _range_new;
    epd_update_mode_t mode;
    switch (_epd_mode)
    {
    case epd_mode_t::epd_fastest:  mode = UPDATE_MODE_DU4;  break;
    case epd_mode_t::epd_fast:     mode = UPDATE_MODE_DU;   break;
    case epd_mode_t::epd_text:     mode = UPDATE_MODE_GL16; break;
    default:                       mode = UPDATE_MODE_GC16; break;
    }

    _update_raw_area(mode);

    _range_new.top = INT16_MAX;
    _range_new.left = INT16_MAX;
    _range_new.right = 0;
    _range_new.bottom = 0;
  }

  void Panel_IT8951::setInvert(bool invert)
  {
    // unimplemented
    _invert = invert;
  }

  void Panel_IT8951::setSleep(bool flg)
  {
    if (flg)
    {
      startWrite();
      _write_command(IT8951_TCON_SLEEP);
      endWrite();
    }
    else
    {
      init(true);
    }
  }

  void Panel_IT8951::setPowerSave(bool flg)
  {
    startWrite();
    _write_command(flg ? IT8951_TCON_STANDBY : IT8951_TCON_SYS_RUN);
    endWrite();
  }

  void Panel_IT8951::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
//  _it8951_rotation = ((-(r + _cfg.offset_rotation)) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));
    // IT8951の回転方向は左回りなので右回りになるよう変更する。;
    _it8951_rotation = ((-_internal_rotation) & 3) | (_internal_rotation & 4);

    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;
    if (_it8951_rotation & 1) { std::swap(_width, _height); }
  }

  void Panel_IT8951::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    _xpos = xs;
    _ypos = ys;
    _xs = xs;
    _ys = ys;
    _xe = xe;
    _ye = ye;
  }

  void Panel_IT8951::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    startWrite();
    writeFillRectPreclipped(x, y, 1, 1, rawcolor);
    endWrite();
  }

  void Panel_IT8951::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    if (_it8951_rotation & 4)
    {
      y = height() - y - h;
    }
    _set_area(x, y, w, h);
    bgr888_t color = rawcolor;
    int32_t sum = color.R8() + (color.G8() << 1) + color.B8();
    _wait_busy();
    _bus->writeData(0, 16);
    bool fast = _epd_mode == epd_mode_t::epd_fast || _epd_mode == epd_mode_t::epd_fastest;
    uint32_t wid = (((x + w + 3) >> 2) - (x >> 2));
    do
    {
      auto btbl = &Bayer[(y & 3) << 2];
      ++y;
      uint32_t value;
      if (fast)
      {
        value = (sum + btbl[2]*16 < 512 ? 0 : 0xF000)
              | (sum + btbl[3]*16 < 512 ? 0 : 0x0F00)
              | (sum + btbl[0]*16 < 512 ? 0 : 0x00F0)
              | (sum + btbl[1]*16 < 512 ? 0 : 0x000F);
      }
      else
      {
        value = std::min<int32_t>(15, (std::max<int32_t>(0, sum + btbl[2])) >> 6) << 12
              | std::min<int32_t>(15, (std::max<int32_t>(0, sum + btbl[3])) >> 6) <<  8
              | std::min<int32_t>(15, (std::max<int32_t>(0, sum + btbl[0])) >> 6) <<  4
              | std::min<int32_t>(15, (std::max<int32_t>(0, sum + btbl[1])) >> 6) ;
      }
      if (_invert) value = ~value;
      _bus->writeDataRepeat(value, 16, wid);
    } while (--h);
    _write_command(IT8951_TCON_LD_IMG_END);
  }

  void Panel_IT8951::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    uint16_t* writebuf = static_cast<uint16_t*>(heap_alloc(w * sizeof(bgr888_t) + 4));
    bgr888_t* readbuf = reinterpret_cast<lgfx::bgr888_t*>(&writebuf[2]);

    if (writebuf == nullptr) return;
    writebuf[0] = 0;

    int32_t add_y = 1;
    bool flg_setarea = false;
    if (_it8951_rotation & 4)
    {
      y = height() - (y + h);
      param->src_y += h - 1;
      add_y = -1;
    }
    bool fast = _epd_mode == epd_mode_t::epd_fast || _epd_mode == epd_mode_t::epd_fastest;
    auto sx = param->src_x32;

    bool fastdraw = (param->transp == pixelcopy_t::NON_TRANSP);
    if (fastdraw)
    {
      _set_area(x, y, w, h);
      flg_setarea = true;
      _wait_busy();
      _bus->writeData(0, 16);
    }

    do
    {
      uint32_t prev_pos = 0, new_pos = 0;
      do
      {
        new_pos = param->fp_copy(readbuf, prev_pos, w, param);
        if (new_pos != prev_pos)
        {
          if (!fastdraw)
          {
            if (flg_setarea)
            {
              _write_command(IT8951_TCON_LD_IMG_END);
            }
            flg_setarea = true;
            _set_area(x + prev_pos, y, new_pos - prev_pos, 1);
            //_wait_busy();
            //_bus->writeData(0, 16);
          }
          uint32_t writepos = 1;
          int32_t shift = (3 - ((x + prev_pos) & 3)) << 2;
          auto btbl = &Bayer[(y & 3) << 2];
          do
          {
            uint_fast16_t buf = 0;
            do
            {
              auto color = readbuf[prev_pos];
              int32_t pixel = color.R8() + (color.G8() << 1) + color.B8();
              if (fast)
              {
                pixel = (pixel + btbl[(x + prev_pos) & 3] * 16 < 512) ? 0 : 15;
              }
              else
              {
                pixel = std::min<int32_t>(15, std::max<int32_t>(0, pixel + btbl[(x + prev_pos) & 3]) >> 6);
              }
              buf |= pixel << shift;
              shift -= 4;
            } while (new_pos != ++ prev_pos && shift >= 0);
            if (_invert) buf = ~buf;
            writebuf[writepos] = getSwap16(buf);
            writepos++;
            shift = 12;
            //_bus->writeData(getSwap16(buf), 16);
          } while (new_pos != prev_pos);
          _wait_busy();
          _bus->writeBytes((uint8_t*)writebuf, writepos << 1, true, false);
        }
      } while (w != new_pos && w != (prev_pos = param->fp_skip(new_pos, w, param)));
      param->src_x32 = sx;
      param->src_y += add_y;
      ++y;
    } while (--h);
    heap_free(writebuf);
    if (flg_setarea)
    {
      _write_command(IT8951_TCON_LD_IMG_END);
    }
  }

  void Panel_IT8951::writeBlock(uint32_t rawcolor, uint32_t length)
  {
    auto xpos = _xpos;
    auto ypos = _ypos;
    int32_t len;
    do {
      len = std::min<int32_t>(length, _xe + 1 - _xpos);
      writeFillRectPreclipped(xpos, ypos, len, 1, rawcolor);
      xpos += len;
      if (xpos > _xe)
      {
        xpos = _xs;
        if (++ypos > _ye)
        {
          ypos = _ys;
        }
        _ypos = ypos;
      }
    } while (length -= len);
    _xpos = xpos;
  }

  void Panel_IT8951::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    uint32_t xs   = _xs  ;
    uint32_t ys   = _ys  ;
    uint32_t xe   = _xe  ;
    uint32_t ye   = _ye  ;
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    uint32_t w;

    uint32_t maxw = std::min(length, xe - xs + 1);
    bgr888_t* readbuf = static_cast<bgr888_t*>(heap_alloc(maxw * sizeof(bgr888_t)));
    if (readbuf == nullptr) return;
    do
    {
      w = std::min(length, xe - xs + 1);
      auto y = _it8951_rotation & 4 ? height() - ypos - 1 : ypos;
      int32_t prev_pos = 0, new_pos = 0;
      //do
      {
        new_pos = param->fp_copy(readbuf, prev_pos, w, param);
        //if (new_pos != prev_pos)
        {
          _set_area(xpos + prev_pos, y, new_pos - prev_pos, 1);
          _wait_busy();
          _bus->writeData(0, 16);
          uint32_t shift = (3 - ((xpos + prev_pos) & 3)) << 2;
          uint16_t buf = 0;
          auto btbl = &Bayer[(y & 3) << 2];
          do
          {
            auto color = readbuf[prev_pos];
            uint16_t pixel = std::min(15, std::max(0, (color.R8() + (color.G8() << 1) + color.B8() + btbl[(xpos + prev_pos) & 3])) >> 6);
            buf |= pixel << shift;
            if (shift)
            {
              shift -= 4;
            }
            else
            {
              _bus->writeData(getSwap16(buf), 16);
              buf = 0;
              shift = 12;
            }
          } while (new_pos != ++prev_pos);
          if (shift != 12)
          {
            if (_invert) buf = ~buf;
            _bus->writeData(getSwap16(buf), 16);
          }
          _write_command(IT8951_TCON_LD_IMG_END);
        }
      }// while (w != new_pos && w != (prev_pos = param->fp_skip(new_pos, w, param)));
      xpos += w;
      if (xpos > xe)
      {
        xpos = xs;
        if (++ypos > ye)
        {
          ypos = ys;
        }
      }
    } while (length -= w);
    _xpos = xpos;
    _ypos = ypos;

    heap_free(readbuf);
  }

  bool Panel_IT8951::_read_raw_line(int32_t raw_x, int32_t raw_y, int32_t len, uint16_t* __restrict buf)
  {
    uint16_t params[4];
    auto addr = _tar_memaddr + raw_x + raw_y * _cfg.panel_width;
    params[0] = (uint16_t)addr;
    params[1] = (uint16_t)(addr >> 16);
    params[2] = len; // (len + 15) & ~15;
    params[3] = 0;
    return _write_args(IT8951_TCON_MEM_BST_RD_T, params, 4)
        && _write_command(IT8951_TCON_MEM_BST_RD_S)
        && _read_words(buf, len);
  }

  void Panel_IT8951::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* __restrict dst, pixelcopy_t* param)
  {
/// IT8951には画素読出しコマンドが存在せず、画像メモリを直接読むコマンドが提供されている。;
/// 画像メモリを直接読み出す場合、ビットシフトや回転方向の解決などは自前で行う必要がある。;
    startWrite();

    uint32_t rx, ry, rw, rh;
    if (_it8951_rotation & 4)
    {
      y = height() - y - h;
    }
    switch(_it8951_rotation & 3)
    {
    default:
    case IT8951_ROTATE_0:
      rx = x;
      ry = y;
      rw = w;
      rh = h;
      break;
    case IT8951_ROTATE_90:
      rx = y;
      ry = _cfg.panel_height - w - x;
      rw = h;
      rh = w;
      break;
    case IT8951_ROTATE_180:
      rx = _cfg.panel_width - w - x;
      ry = _cfg.panel_height - h - y;
      rw = w;
      rh = h;
      break;
    case IT8951_ROTATE_270:
      rx = _cfg.panel_width - h - y;
      ry = x;
      rw = h;
      rh = w;
      break;
    }

    int32_t adjust_left = (rx & 3);
    uint32_t padding_len = (adjust_left + rw + 31) & ~31;
    auto readbuf = static_cast<uint8_t*>(heap_alloc(std::max(padding_len, rw * param->dst_bits >> 3)));
    auto colorbuf = static_cast<bgr888_t*>(heap_alloc(rw * sizeof(bgr888_t)));

    param->src_data = colorbuf;

    for (uint32_t j = 0; j < rh; ++j)
    {
      _read_raw_line(rx & ~3, ry, padding_len >> 1, reinterpret_cast<uint16_t*>(readbuf));
      for (uint32_t i = 0; i < rw; ++i)
      {
        uint_fast8_t l = 8 + readbuf[adjust_left + i];
        if (_invert) l = ~l;
        colorbuf[i].set(l,l,l);
      }
      param->src_x = 0;

      if (0 == (_it8951_rotation & 1))
      {
        if (_it8951_rotation & 2)
        {
          param->src_x32_add = (~0u << pixelcopy_t::FP_SCALE);
          param->src_x = rw - 1;
        }
        uint32_t dstpos = rw * ((_it8951_rotation & 4) ? (rh - j - 1) : j);
        param->fp_copy(dst, dstpos, dstpos + rw, param);
      }
      else
      {
        for (uint32_t i = 0; i < rw; ++i) {
          int32_t dstpos;
          switch (_it8951_rotation)
          {
          default:
        //case 0:   dstpos =       i           +       j      * rw; break;
          case 1:   dstpos =       i      * rh + (rh - j - 1)     ; break;
        //case 2:   dstpos = (rw - i - 1)      + (rh - j - 1) * rw; break;
          case 3:   dstpos = (rw - i - 1) * rh +       j          ; break;
        //case 4:   dstpos =       i           + (rh - j - 1) * rw; break;
          case 5:   dstpos = (rw - i - 1) * rh + (rh - j - 1)     ; break;
        //case 6:   dstpos = (rw - i - 1)      +       j      * rw; break;
          case 7:   dstpos =       i      * rh +       j          ; break;
          }
          param->fp_copy(dst, dstpos, dstpos + 1, param);
        }
      }
      ++ry;
    } while (--rh);
    cs_control(true);
    endWrite();

    heap_free(colorbuf);
    heap_free(readbuf);
  }

//----------------------------------------------------------------------------
 }
}
