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

#include "Bus_I2C.hpp"
#include "../../misc/pixelcopy.hpp"

#include <soc/i2c_struct.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void Bus_I2C::config(const config_t& config)
  {
    _cfg = config;
  }

  bool Bus_I2C::init(void)
  {
    _state = state_t::state_none;
    return lgfx::i2c::init(_cfg.i2c_port, _cfg.pin_sda, _cfg.pin_scl).has_value();
  }

  void Bus_I2C::release(void)
  {
    lgfx::i2c::release(_cfg.i2c_port);
    _state = state_t::state_none;
  }

  void Bus_I2C::beginTransaction(void)
  {
    // 既に開始直後の場合は終了;
    if (_state == state_t::state_write_none)
    {
      return;
    }

    if (_state != state_none)
    {
      lgfx::i2c::endTransaction(_cfg.i2c_port);
    }
    lgfx::i2c::beginTransaction(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq_write, false);
    _state = state_t::state_write_none;
  }

  void Bus_I2C::beginRead(void)
  {
    if (_state == state_t::state_read)
    {
      return;
    }
    
    if (_state != state_t::state_none)
    {
      lgfx::i2c::restart(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq_read, true);
    }
    else
    {
      lgfx::i2c::beginTransaction(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq_read, true);
    }
    _state = state_t::state_read;
  }

  void Bus_I2C::endTransaction(void)
  {
    if (_state == state_t::state_none)
    {
      return;
    }

    _state = state_t::state_none;
    lgfx::i2c::endTransaction(_cfg.i2c_port);
  }

  void Bus_I2C::endRead(void)
  {
    endTransaction();
  }

  void Bus_I2C::wait(void)
  {
#if I2C_NUM_MAX > 1
    auto dev = (_cfg.i2c_port == 0) ? &I2C0 : &I2C1;
#else
    auto dev = &I2C0;
#endif

#if defined (CONFIG_IDF_TARGET_ESP32C3) || defined (CONFIG_IDF_TARGET_ESP32C6) || defined (CONFIG_IDF_TARGET_ESP32S3)
    while (dev->sr.bus_busy) { taskYIELD(); }
#else
    while (dev->status_reg.bus_busy) { taskYIELD(); }
#endif
  }

  bool Bus_I2C::busy(void) const
  {
#if I2C_NUM_MAX > 1
    auto dev = (_cfg.i2c_port == 0) ? &I2C0 : &I2C1;
#else
    auto dev = &I2C0;
#endif

#if defined (CONFIG_IDF_TARGET_ESP32C3) || defined (CONFIG_IDF_TARGET_ESP32C6) || defined (CONFIG_IDF_TARGET_ESP32S3)
    return dev->sr.bus_busy;
#else
    return dev->status_reg.bus_busy;
#endif
  }

  void Bus_I2C::dc_control(bool dc)
  {
    // リード中の場合はトランザクションを終了しておく;
    if (_state == state_t::state_read)
    {
      _state = state_t::state_none;
      lgfx::i2c::endTransaction(_cfg.i2c_port);
    }

    // まだトランザクションが開始されていない場合は開始しておく;
    if (_state == state_t::state_none)
    {
      _state = state_t::state_write_none;
      lgfx::i2c::beginTransaction(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq_write, false);
    }

    // DCプリフィクスなしの場合は後の処理は不要;
    if (_cfg.prefix_len == 0) return;

    state_t st = dc ? state_t::state_write_data : state_t::state_write_cmd;
    // 既に送信済みのDCプリフィクスが要求と一致している場合は終了;
    if (_state == st) return;

    // DCプリフィクスが送信済みの場合、送信済みのDCプリフィクスと要求が不一致なのでトランザクションをやり直す。;
    int retry = 3;
    do
    {
      if (_state != state_t::state_write_none)
      {
        lgfx::i2c::endTransaction(_cfg.i2c_port);
        lgfx::i2c::beginTransaction(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq_write, false);
      }
      _state = st;
    } while (lgfx::i2c::writeBytes(_cfg.i2c_port, (uint8_t*)(dc ? &_cfg.prefix_data : &_cfg.prefix_cmd), _cfg.prefix_len).has_error() && --retry);
    if (!retry) { _state = state_none; }
  }

  bool Bus_I2C::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
    dc_control(false);
    return lgfx::i2c::writeBytes(_cfg.i2c_port, (uint8_t*)&data, (bit_length >> 3)).has_value();
  }

  void Bus_I2C::writeData(uint32_t data, uint_fast8_t bit_length)
  {
    dc_control(true);
    lgfx::i2c::writeBytes(_cfg.i2c_port, (uint8_t*)&data, (bit_length >> 3));
  }

  void Bus_I2C::writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t length)
  {
    dc_control(true);
    const uint8_t dst_bytes = bit_length >> 3;
    uint32_t buf0 = data | data << bit_length;
    uint32_t buf1;
    uint32_t buf2;
    // make 12Bytes data.
    if (dst_bytes != 3)
    {
      if (dst_bytes == 1)
      {
        buf0 |= buf0 << 16;
      }
      buf1 = buf0;
      buf2 = buf0;
    }
    else
    {
      buf1 = buf0 >>  8 | buf0 << 16;
      buf2 = buf0 >> 16 | buf0 <<  8;
    }
    uint32_t src[8] = { buf0, buf1, buf2, buf0, buf1, buf2, buf0, buf1 };
    auto buf = reinterpret_cast<uint8_t*>(src);
    uint32_t limit = 32 / dst_bytes;
    uint32_t len;
    do
    {
      len = ((length - 1) % limit) + 1;
      i2c::writeBytes(_cfg.i2c_port, buf, len * dst_bytes);
    } while (length -= len);
  }

  void Bus_I2C::writePixels(pixelcopy_t* param, uint32_t length)
  {
    dc_control(true);
    const uint8_t dst_bytes = param->dst_bits >> 3;
    uint32_t limit = 32 / dst_bytes;
    uint32_t len;
    uint8_t buf[32];
    do
    {
      len = ((length - 1) % limit) + 1;
      param->fp_copy(buf, 0, len, param);
      i2c::writeBytes(_cfg.i2c_port, buf, len * dst_bytes);
    } while (length -= len);
  }

  void Bus_I2C::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    dc_control(dc);
    i2c::writeBytes(_cfg.i2c_port, data, length);
  }

  uint32_t Bus_I2C::readData(uint_fast8_t bit_length)
  {
    beginRead();
    uint32_t res;
    i2c::readBytes(_cfg.i2c_port, reinterpret_cast<uint8_t*>(&res), bit_length >> 3);
    return res;
  }

  bool Bus_I2C::readBytes(uint8_t* dst, uint32_t length, bool use_dma, bool last_nack)
  {
    beginRead();
    return i2c::readBytes(_cfg.i2c_port, dst, length, last_nack).has_value();
  }

  void Bus_I2C::readPixels(void* dst, pixelcopy_t* param, uint32_t length)
  {
    beginRead();
    const auto bytes = param->src_bits >> 3;
    uint32_t regbuf[8];
    uint32_t limit = 32 / bytes;

    param->src_data = regbuf;
    int32_t dstindex = 0;
    do {
      uint32_t len = (limit > length) ? length : limit;
      length -= len;
      i2c::readBytes(_cfg.i2c_port, (uint8_t*)regbuf, len * bytes);
      param->src_x = 0;
      dstindex = param->fp_copy(dst, dstindex, dstindex + len, param);
    } while (length);
  }

//----------------------------------------------------------------------------
 }
}

#endif
