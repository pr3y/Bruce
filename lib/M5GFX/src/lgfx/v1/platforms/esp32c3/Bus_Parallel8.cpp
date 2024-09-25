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
#if defined (ESP_PLATFORM)
#include <sdkconfig.h>
#if defined (CONFIG_IDF_TARGET_ESP32C3)

#include "Bus_Parallel8.hpp"
#include "../../misc/pixelcopy.hpp"

#include <rom/gpio.h>
#include <hal/gpio_ll.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void Bus_Parallel8::config(const config_t& cfg)
  {
    _cfg = cfg;

    for (int c = 0; c<256; c++)
    {
      uint32_t val = 0;
      for (int i = 0; i < 8; ++i)
      {
        if (c & (1 << i))
        {
          val |= 1ul << _cfg.pin_data[i];
        }
      }
      _gpio_tbl[c] = val;
    }
    _gpio_wr_mask = (1ul << (cfg.pin_wr & 31));
    _gpio_low_mask = _gpio_tbl[255] | _gpio_wr_mask;
    _mask_reg_dc = (cfg.pin_rs < 0) ? 0 : (1ul << (cfg.pin_rs & 31));
    _gpio_reg_dc[0] = get_gpio_lo_reg(cfg.pin_rs);
    _gpio_reg_dc[1] = get_gpio_hi_reg(cfg.pin_rs);
  }

  bool Bus_Parallel8::init(void)
  {
    int8_t* pins = _cfg.pin_data;
    for (size_t i = 0; i < 8; ++i)
    {
      pinMode(pins[i], pin_mode_t::output);
    }

    _init_pin();

    gpio_pad_select_gpio(_cfg.pin_rd);
    gpio_pad_select_gpio(_cfg.pin_wr);
    gpio_pad_select_gpio(_cfg.pin_rs);

    gpio_hi(_cfg.pin_rd);
    gpio_hi(_cfg.pin_wr);
    gpio_hi(_cfg.pin_rs);

    gpio_set_direction((gpio_num_t)_cfg.pin_rd, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_cfg.pin_wr, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_cfg.pin_rs, GPIO_MODE_OUTPUT);

    return true;
  }

  void Bus_Parallel8::_init_pin(bool read)
  {
    int8_t* pins = _cfg.pin_data;
    gpio_mode_t gpio_mode = read ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;
    for (size_t i = 0; i < 8; ++i)
    {
      gpio_pad_select_gpio(pins[i]);
      gpio_set_direction((gpio_num_t)pins[i], gpio_mode);
    }
  }

  bool Bus_Parallel8::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
    auto low  = _gpio_reg_dc[0];
    auto high = _gpio_reg_dc[1];
    uint32_t lmask = _mask_reg_dc | _gpio_low_mask;
    size_t bytes = bit_length >> 3;
    do
    {
      *low  = lmask;
      *high = _gpio_tbl[data & 0xFF];
      data >>= 8;
      *high = _gpio_wr_mask;
    } while (--bytes);
    return true;
  }

  void Bus_Parallel8::writeData(uint32_t data, uint_fast8_t bit_length)
  {
    auto low  = _gpio_reg_dc[0];
    auto high = _gpio_reg_dc[1];
    *high = _mask_reg_dc;
    uint32_t lmask = _gpio_low_mask;
    size_t bytes = bit_length >> 3;
    do
    {
      *low  = lmask;
      *high = _gpio_tbl[data & 0xFF];
      data >>= 8;
      *high = _gpio_wr_mask;
    } while (--bytes);
  }

  void Bus_Parallel8::writeDataRepeat(uint32_t color_raw, uint_fast8_t bit_length, uint32_t length)
  {
    auto low  = _gpio_reg_dc[0];
    auto high = _gpio_reg_dc[1];
    *high = _mask_reg_dc;
    size_t bytes = bit_length >> 3;
    uint32_t dat[4];
    for (size_t i = 0; i < bytes; ++i)
    {
      dat[i] = _gpio_tbl[color_raw & 0xFF];
      color_raw >>= 8;
    }
    auto clear = _gpio_low_mask;
    auto wr = _gpio_wr_mask;
    do
    {
      size_t i = 0;
      do
      {
        *low  = clear;
        *high = dat[i];
        *high = wr;
      } while (++i < bytes);
    } while (--length);
  }

  void Bus_Parallel8::writePixels(pixelcopy_t* param, uint32_t length)
  {
    static constexpr int bufsize = 128;
    uint8_t buf[bufsize];
    const uint32_t bytes = param->dst_bits >> 3;
    auto fp_copy = param->fp_copy;
    const uint32_t limit = bufsize / bytes;
    uint8_t len = length % limit;
    if (len)
    {
      fp_copy(buf, 0, len, param);
      writeBytes(buf, len * bytes, true, true);
      if (0 == (length -= len)) return;
    }
    do
    {
      fp_copy(buf, 0, limit, param);
      writeBytes(buf, limit * bytes, true, true);
    } while (length -= limit);
  }

  void Bus_Parallel8::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    *_gpio_reg_dc[dc] = _mask_reg_dc;
    auto low  = _gpio_reg_dc[0];
    auto high = _gpio_reg_dc[1];
    auto clear = _gpio_low_mask;
    auto wr = _gpio_wr_mask;
    auto tbl = _gpio_tbl;
    size_t i = 0;
    do
    {
      *low  = clear;
      *high = tbl[data[i]];
      *high = wr;
    } while (++i < length);
  }

  void Bus_Parallel8::beginRead(void)
  {
    *_gpio_reg_dc[1] = _mask_reg_dc;
    gpio_lo(_cfg.pin_rd);

    _init_pin(true);
  }

  void Bus_Parallel8::endRead(void)
  {
    gpio_hi(_cfg.pin_rd);

    _init_pin();
  }

  void Bus_Parallel8::_read_bytes(uint8_t* dst, uint32_t length)
  {
    union
    {
      uint32_t in32[1];
      uint8_t in[4];
    };

    uint32_t mask = (((((((((((((((
                    (_cfg.pin_d0 & 7)) << 3)
                  + (_cfg.pin_d1 & 7)) << 3)
                  + (_cfg.pin_d2 & 7)) << 3)
                  + (_cfg.pin_d3 & 7)) << 3)
                  + (_cfg.pin_d4 & 7)) << 3)
                  + (_cfg.pin_d5 & 7)) << 3)
                  + (_cfg.pin_d6 & 7)) << 3)
                  + (_cfg.pin_d7 & 7))
                  ;

    uint32_t idx = (((((((((((((((
                    (_cfg.pin_d0 >> 3)) << 3)
                  + (_cfg.pin_d1 >> 3)) << 3)
                  + (_cfg.pin_d2 >> 3)) << 3)
                  + (_cfg.pin_d3 >> 3)) << 3)
                  + (_cfg.pin_d4 >> 3)) << 3)
                  + (_cfg.pin_d5 >> 3)) << 3)
                  + (_cfg.pin_d6 >> 3)) << 3)
                  + (_cfg.pin_d7 >> 3))
                  ;

    auto reg_rd_h = get_gpio_hi_reg(_cfg.pin_rd);
    auto reg_rd_l = get_gpio_lo_reg(_cfg.pin_rd);
    uint32_t mask_rd = 1ul << (_cfg.pin_rd & 31);
    uint_fast8_t val;

    do
    {
      in32[0] = GPIO.in.val;
      *reg_rd_h = mask_rd;
      *reg_rd_l = mask_rd;
      val =              (1 & (in[(idx >>  0) & 7] >> ((mask >>  0) & 7)));
      val = (val << 1) + (1 & (in[(idx >>  3) & 7] >> ((mask >>  3) & 7)));
      val = (val << 1) + (1 & (in[(idx >>  6) & 7] >> ((mask >>  6) & 7)));
      val = (val << 1) + (1 & (in[(idx >>  9) & 7] >> ((mask >>  9) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 12) & 7] >> ((mask >> 12) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 15) & 7] >> ((mask >> 15) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 18) & 7] >> ((mask >> 18) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 21) & 7] >> ((mask >> 21) & 7)));
      *dst++ = val;
    } while (--length);
  }

  uint32_t Bus_Parallel8::readData(uint_fast8_t bit_length)
  {
    union {
      uint32_t res;
      uint8_t raw[4];
    };
    _read_bytes(raw, (bit_length + 7) >> 3);
    return res;
  }

  bool Bus_Parallel8::readBytes(uint8_t* dst, uint32_t length, bool use_dma)
  {
    _read_bytes(dst, length);
    return true;
  }

  void Bus_Parallel8::readPixels(void* dst, pixelcopy_t* param, uint32_t length)
  {
    uint32_t _regbuf[8];
    const auto bytes = param->src_bits >> 3;
    uint32_t limit = (bytes == 2) ? 16 : 10;
    param->src_data = _regbuf;
    int32_t dstindex = 0;
    do {
      uint32_t len2 = (limit > length) ? length : limit;
      length -= len2;

      _read_bytes((uint8_t*)_regbuf, len2 * bytes);
      param->src_x = 0;
      dstindex = param->fp_copy(dst, dstindex, dstindex + len2, param);
    } while (length);
  }

//----------------------------------------------------------------------------
 }
}

#endif
#endif
