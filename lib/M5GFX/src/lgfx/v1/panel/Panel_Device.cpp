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
#include "Panel_Device.hpp"
#include "../Bus.hpp"
#include "../Light.hpp"
#include "../Touch.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  Panel_Device::Panel_Device(void)
  {
    setBus(nullptr);
  }

  void Panel_Device::setBus(IBus* bus)
  {
    static Bus_NULL nullobj;
    _bus = bus ? bus : &nullobj;
  }

  void Panel_Device::setBrightness(uint8_t brightness)
  {
    if (_light) _light->setBrightness(brightness);
  }

  void Panel_Device::initBus(void)
  {
    _bus->init();
  }

  void Panel_Device::releaseBus(void)
  {
    _bus->release();
  }

  bool Panel_Device::init(bool use_reset)
  {
    init_rst();
    init_cs();
    if (_light)
    {
      _light->init(0);
    }
    if (use_reset)
    {
      rst_control(false);
      delay(8);
    }
    _bus->init();
    rst_control(true);
    if (use_reset)
    {
      delay(64);
    }
    return true;
  }

  bool Panel_Device::initTouch(void)
  {
    if (_touch)
    {
      return _touch->init();
    }
    return false;
  }

  void Panel_Device::initDMA(void)
  {
    _bus->initDMA();
  }
  void Panel_Device::waitDMA(void)
  {
    _bus->wait();
  }
  bool Panel_Device::dmaBusy(void)
  {
    return _bus->busy();
  }

  void Panel_Device::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    _bus->flush();
  }

  void Panel_Device::writeCommand(uint32_t data, uint_fast8_t length)
  {
    if (_cfg.dlen_16bit)
    {
      if (_has_align_data)
      {
        _has_align_data = false;
        _bus->writeData(0, 8);
      }
      if (length == 1) { length = 2; data <<= 8; }
    }
    _bus->writeCommand(data, length << 3);
  }

  void Panel_Device::writeData(uint32_t data, uint_fast8_t length)
  {
    if (!_cfg.dlen_16bit)
    {
      _bus->writeData(data, length << 3);
    }
    else
    {
      if (length == 1)
      {
        _bus->writeData(data << 8, 16);
      }
      else
      {
        _bus->writeData(data, length << 3);
      }
    }
  }
/*
  void Panel_Device::writeBytes(const uint8_t* data, uint32_t len, bool use_dma)
  {
    _bus->writeBytes(data, len, use_dma);
    if (_cfg.dlen_16bit && (_write_bits & 15) && (len & 1))
    {
      _has_align_data = !_has_align_data;
    }
  }
*/

  void Panel_Device::command_list(const uint8_t *addr)
  {
    for (;;)
    {                // For each command...
      uint8_t cmd = *addr++;
      uint8_t num = *addr++;   // Number of args to follow
      if (cmd == 0xFF && num == 0xFF) break;
      writeCommand(cmd, 1);  // Read, issue command
      _bus->flush();
      uint_fast8_t ms = num & CMD_INIT_DELAY;       // If hibit set, delay follows args
      num &= ~CMD_INIT_DELAY;          // Mask out delay bit
      if (num)
      {
        do
        {                   // For each argument...
          writeData(*addr++, 1);  // Read, issue argument
          _bus->flush();
        } while (--num);
      }
      if (ms)
      {
        ms = *addr++;        // Read post-command delay time (ms)
        delay( (ms==255 ? 500 : ms) );
      }
    }
  }

//----------------------------------------------------------------------------

  void Panel_Device::writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param)
  {
    auto src_x = param->src_x;
    auto buffer = reinterpret_cast<argb8888_t*>(const_cast<void*>(param->src_data));
    auto bytes = param->dst_bits >> 3;
// ESP_LOGI("LGFX","DEBUG: %d %d", param->dst_bits, bytes);
    // uint8_t* dmabuf = _bus->getFlipBuffer(w * bytes);
    // memset(dmabuf, 0, w * bytes);
    // param->fp_copy(dmabuf, 0, w, param);
    // setWindow(x, y, x + w - 1, y);
    // writeBytes(dmabuf, w * bytes, true);
    // return;
    pixelcopy_t pc_read(nullptr, _write_depth, _read_depth);
    pixelcopy_t pc_write(nullptr, _write_depth, _write_depth);
    for (;;)
    {
      uint8_t* dmabuf = _bus->getDMABuffer((w+1) * bytes);
      pc_write.src_data = dmabuf;
      uint32_t xstart = 0, drawed_x = 0;
      do
      {
        uint_fast8_t a = buffer[xstart].a;
        if (!a)
        {
          if (drawed_x < xstart)
          {
            param->src_x = drawed_x;
            param->fp_copy(dmabuf, drawed_x, xstart, param);

            pc_write.src_x = drawed_x;
            writeImage(x + drawed_x, y, xstart - drawed_x, 1, &pc_write, true);
          }
          drawed_x = xstart + 1;
        }
        else
        {
          while (255 == buffer[xstart].a && ++xstart != w);
          if (xstart == w) break;
          uint32_t j = xstart;
          while (++j != w && buffer[j].a && buffer[j].a != 255);
          readRect(x + xstart, y, j - xstart + 1, 1, &dmabuf[xstart * bytes], &pc_read);
          if (w == (xstart = j)) break;
        }
      } while (++xstart != w);
      if (drawed_x < xstart)
      {
        param->src_x = drawed_x;
        param->fp_copy(dmabuf, drawed_x, xstart, param);

        pc_write.src_x = drawed_x;
        writeImage(x + drawed_x, y, xstart - drawed_x, 1, &pc_write, true);
      }
      if (!--h) return;
      param->src_x = src_x;
      param->src_y++;
      ++y;
    }
  }

  void Panel_Device::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    pixelcopy_t pc_read( (void*)nullptr, _write_depth, _read_depth);
    pixelcopy_t pc_write((void*)nullptr, _write_depth, _write_depth);
    size_t write_bytes = (_write_depth + 7) >> 3;
    startWrite();

    auto dir = get_fastread_dir();
    if (dir == fastread_dir_t::fastread_vertical
    || (dir == fastread_dir_t::fastread_nothing && (w < h)))
    {
      const uint32_t buflen = h * write_bytes;
      auto buf = (uint8_t*)alloca(buflen);
      pc_write.src_data = buf;
      pc_write.src_width = 1;
      pc_write.src_bitwidth = 1;
      int32_t add = (src_x < dst_x) ?   - 1 : 1;
      int32_t pos = (src_x < dst_x) ? w - 1 : 0;
      do {
        readRect(src_x + pos, src_y, 1, h, buf, &pc_read);
        pc_write.src_x = 0;
        pc_write.src_y = 0;
        writeImage(dst_x + pos, dst_y, 1, h, &pc_write, true);
        //setWindow(dst_x + pos, dst_y, dst_x + pos, dst_y);
        //writePixels(&pc_write, h);
        pos += add;
      } while (--w);
      waitDMA();
    }
    else
    {
      const uint32_t buflen = w * write_bytes;
      auto buf = (uint8_t*)alloca(buflen);
      pc_write.src_data = buf;
      int32_t add = (src_y < dst_y) ?   - 1 : 1;
      int32_t pos = (src_y < dst_y) ? h - 1 : 0;
      do {
        readRect(src_x, src_y + pos, w, 1, buf, &pc_read);
        pc_write.src_x = 0;
        pc_write.src_y = 0;
        writeImage(dst_x, dst_y + pos, w, 1, &pc_write, true);
        pos += add;
      } while (--h);
      waitDMA();
    }
    endWrite();
  }

//----------------------------------------------------------------------------

  void Panel_Device::init_cs(void)
  {
    auto pin = _cfg.pin_cs;
    if (pin < 0) return;
    lgfx::gpio_hi(pin);
    lgfx::pinMode(pin , pin_mode_t::output);
  }

  void Panel_Device::init_rst(void)
  {
    auto pin = _cfg.pin_rst;
    if (pin < 0) return;
    lgfx::gpio_hi(pin);
    lgfx::pinMode(pin, pin_mode_t::output);
  }

  void Panel_Device::cs_control(bool level)
  {
    auto pin = _cfg.pin_cs;
    if (pin < 0) return;
    if (level)
    {
      gpio_hi(pin);
    }
    else
    {
      gpio_lo(pin);
    }
  }

  void Panel_Device::rst_control(bool level)
  {
    auto pin = _cfg.pin_rst;
    if (pin < 0) return;
    if (level)
    {
      gpio_hi(pin);
    }
    else
    {
      gpio_lo(pin);
    }
  }

//----------------------------------------------------------------------------

  void Panel_Device::setTouch(ITouch* touch)
  {
    _touch = touch;
    touchCalibrate();
  }

  void Panel_Device::touchCalibrate(void)
  {
    if (_touch == nullptr) return;

    auto cfg = _touch->config();
    uint16_t parameters[8] =
      { cfg.x_min, cfg.y_min
      , cfg.x_min, cfg.y_max
      , cfg.x_max, cfg.y_min
      , cfg.x_max, cfg.y_max };
    setCalibrate(parameters);
  }

  void Panel_Device::setCalibrateAffine(float affine[6])
  {
    memcpy(_affine, affine, sizeof(float) * 6);
  }

  void Panel_Device::setCalibrate(uint16_t *parameters)
  {
    uint32_t vect[6] = {0,0,0,0,0,0};
    float mat[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
    float a;

    auto w = _cfg.panel_width;
    auto h = _cfg.panel_height;
    if (_touch->config().offset_rotation & 1)
    {
      std::swap(w, h);
    }

    for ( int i = 0; i < 4; ++i ) {
      int32_t tx = (i & 2) ? (w - 1) : 0;
      int32_t ty = (i & 1) ? (h - 1) : 0;
      int32_t px = parameters[i*2  ];
      int32_t py = parameters[i*2+1];
      a = px * px;
      mat[0][0] += a;
      a = px * py;
      mat[0][1] += a;
      mat[1][0] += a;
      a = px;
      mat[0][2] += a;
      mat[2][0] += a;
      a = py * py;
      mat[1][1] += a;
      a = py;
      mat[1][2] += a;
      mat[2][1] += a;
      mat[2][2] += 1;

      vect[0] += px * tx;
      vect[1] += py * tx;
      vect[2] +=      tx;
      vect[3] += px * ty;
      vect[4] += py * ty;
      vect[5] +=      ty;
    }

    {
      for ( int k = 0; k < 3; ++k )
      {
        float t = mat[k][k];
        for ( int i = 0; i < 3; ++i ) mat[k][i] /= t;

        mat[k][k] = 1 / t;
        for ( int j = 0; j < 3; ++j )
        {
          if ( j == k ) continue;

          float u = mat[j][k];

          for ( int i = 0; i < 3; ++i )
          {
            if ( i != k ) mat[j][i] -= mat[k][i] * u;
            else mat[j][i] = -u / t;
          }
        }
      }
    }

    float v0 = vect[0];
    float v1 = vect[1];
    float v2 = vect[2];
    _affine[0] = mat[0][0] * v0 + mat[0][1] * v1 + mat[0][2] * v2;
    _affine[1] = mat[1][0] * v0 + mat[1][1] * v1 + mat[1][2] * v2;
    _affine[2] = mat[2][0] * v0 + mat[2][1] * v1 + mat[2][2] * v2;
    float v3 = vect[3];
    float v4 = vect[4];
    float v5 = vect[5];
    _affine[3] = mat[0][0] * v3 + mat[0][1] * v4 + mat[0][2] * v5;
    _affine[4] = mat[1][0] * v3 + mat[1][1] * v4 + mat[1][2] * v5;
    _affine[5] = mat[2][0] * v3 + mat[2][1] * v4 + mat[2][2] * v5;
  }

  void Panel_Device::convertRawXY(touch_point_t *tp, uint_fast8_t count)
  {
    auto r = _internal_rotation;
    if (_touch) {
      auto offset = _touch->config().offset_rotation;
      r = ((r + offset) & 3) | ((r & 4) ^ (offset & 4));
    }
    bool vflip = (1 << r) & 0b10010110; // r 1,2,4,7

    for (size_t idx = 0; idx < count; ++idx)
    {
      int32_t tx = (_affine[0] * (float)tp[idx].x + _affine[1] * (float)tp[idx].y) + _affine[2];
      int32_t ty = (_affine[3] * (float)tp[idx].x + _affine[4] * (float)tp[idx].y) + _affine[5];
      if (r)
      {
        if (r & 1) { std::swap(tx, ty); }
        if (r & 2) { tx = (_width  - 1) - tx; }
        if (vflip) { ty = (_height - 1) - ty; }
      }
      tp[idx].x = tx;
      tp[idx].y = ty;
    }
  }

  uint_fast8_t Panel_Device::getTouchRaw(touch_point_t* tp, uint_fast8_t count)
  {
    if (_touch == nullptr) return 0;

    bool need_transaction = (getStartCount() && _touch->config().bus_shared);
    if (need_transaction) { endTransaction(); }
    count = _touch->getTouchRaw(tp, count);
    if (need_transaction) { beginTransaction(); }
    return count;
  }

  uint_fast8_t Panel_Device::getTouch(touch_point_t* tp, uint_fast8_t count)
  {
    auto res = getTouchRaw(tp, count);
    convertRawXY(tp, res);
    return res;
  }

//----------------------------------------------------------------------------

 }
}
