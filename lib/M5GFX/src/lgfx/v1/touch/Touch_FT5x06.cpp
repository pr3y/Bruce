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
#include "Touch_FT5x06.hpp"

#include "../platforms/common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr uint8_t FT5x06_CIPHER_REG  = 0xA3;
  static constexpr uint8_t FT5x06_INTMODE_REG = 0xA4;
  static constexpr uint8_t FT5x06_POWER_REG   = 0xA5;
  static constexpr uint8_t FT5x06_VENDID_REG  = 0xA8;
  static constexpr uint8_t FT5x06_PERIODACTIVE = 0x88;

  static constexpr uint8_t FT5x06_MONITOR  = 0x01;
  static constexpr uint8_t FT5x06_SLEEP_IN = 0x03;

  bool Touch_FT5x06::_write_reg(uint8_t reg, uint8_t val)
  {
    return i2c::writeRegister8(_cfg.i2c_port, _cfg.i2c_addr, reg, val, 0, _cfg.freq).has_value();

    // uint8_t data[] = { reg, val };
    // return lgfx::i2c::transactionWrite(_cfg.i2c_port, _cfg.i2c_addr, data, 2, _cfg.freq).has_value();
  }

  bool Touch_FT5x06::_read_reg(uint8_t reg, uint8_t *data, size_t length)
  {
    return lgfx::i2c::transactionWriteRead(_cfg.i2c_port, _cfg.i2c_addr, &reg, 1, data, length, _cfg.freq).has_value();
  }

  bool Touch_FT5x06::_check_init(void)
  {
    if (_inited) return true;

    uint8_t tmp[6] = { 0 };
    _inited = _write_reg(0x00, 0x00)
          && _read_reg(FT5x06_CIPHER_REG, tmp, 6)
          && _write_reg(FT5x06_INTMODE_REG, 0x00) // INT Polling mode
          && tmp[5]
          ;
#if defined ( ESP_LOGV )
if (_inited)
{
  ESP_LOGV("FT5x06", "CIPHER:0x%02x / FIRMID:0x%02x / VENDID:0x%02x", tmp[0], tmp[3], tmp[5]);
}
#endif
    return _inited;
  }

  bool Touch_FT5x06::init(void)
  {
    _inited = false;

    if (_cfg.pin_rst >= 0)
    {
      lgfx::pinMode(_cfg.pin_rst, pin_mode_t::output);
      lgfx::gpio_lo(_cfg.pin_rst);
      lgfx::delay(1);
      lgfx::gpio_hi(_cfg.pin_rst);
    }

    if (_cfg.pin_int >= 0)
    {
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pullup);
    }
    return (lgfx::i2c::init(_cfg.i2c_port, _cfg.pin_sda, _cfg.pin_scl).has_value()) && _check_init();
  }

  void Touch_FT5x06::wakeup(void)
  {
    if (!_check_init()) return;
    if (_cfg.pin_int >= 0)
    {
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pulldown);
      delayMicroseconds(512);
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pullup);
    }
    _write_reg(FT5x06_POWER_REG, FT5x06_MONITOR);
  }

  void Touch_FT5x06::sleep(void)
  {
    if (!_check_init()) return;
    _write_reg(FT5x06_POWER_REG, FT5x06_SLEEP_IN);
  }

  size_t Touch_FT5x06::_read_data(uint8_t* readdata)
  {
    /// 戻り値res: 通信に失敗した場合は0、通信に成功した場合はByte数
    size_t res = 0;
    if (lgfx::i2c::beginTransaction(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq, false))
    {
      readdata[0] = 0x02;
      if (lgfx::i2c::writeBytes(_cfg.i2c_port, readdata, 1)
      && lgfx::i2c::restart(_cfg.i2c_port, _cfg.i2c_addr, _cfg.freq, true)
      && lgfx::i2c::readBytes(_cfg.i2c_port, readdata, 1))
      {
        uint_fast8_t points = std::min<uint_fast8_t>(max_touch_points, readdata[0] & 0x0Fu);
        if (points)
        {
#ifdef PORTDUINO_LINUX_HARDWARE
          readdata[1] = 0x03;
          lgfx::i2c::writeBytes(_cfg.i2c_port, &readdata[1], 1);
#endif
          if (lgfx::i2c::readBytes(_cfg.i2c_port, &readdata[1], points * 6 - 2))
          {
            res = points * 6 - 1;
          }
        }
        else
        {
          res = 1;
        }
      }
      lgfx::i2c::endTransaction(_cfg.i2c_port).has_value();
    }
    return res;
  }

  uint_fast8_t Touch_FT5x06::getTouchRaw(touch_point_t *tp, uint_fast8_t count)
  {
    if (!_check_init() || count == 0) return 0;
    if (_cfg.pin_int >= 0)
    {
      if (_flg_released != (bool)gpio_in(_cfg.pin_int))
      {
        _flg_released = !_flg_released;
        _write_reg(FT5x06_INTMODE_REG, 0x00); // INT Polling mode
      }
      if (_flg_released)
      {
        return 0;
      }
    }
    if (count > max_touch_points) { count = max_touch_points; }

    uint8_t readdata[2][30];
    size_t readlen[2];
    if (1 == (readlen[0] = _read_data(readdata[0]))) { return 0; }
    size_t comparelen;
    int32_t retry = 5;
    do
    { // 読出し中に値が変わる事があるので、連続読出しして前回と同値でなければリトライする;
      readlen[retry & 1] = _read_data(readdata[retry & 1]);
      comparelen = std::min(readlen[0], readlen[1]);
    } while ((0 == comparelen || memcmp(readdata[0], readdata[1], readlen[0])) && --retry);

    if (count > readdata[0][0]) count = readdata[0][0];
    for (size_t idx = 0; idx < count; ++idx)
    {
      auto data = &readdata[0][idx * 6];
      tp[idx].size = 1;
      tp[idx].x = (data[1] & 0x0F) << 8 | data[2];
      tp[idx].y = (data[3] & 0x0F) << 8 | data[4];
      tp[idx].id = data[3] >> 4;
    }
    return count;
  }

//----------------------------------------------------------------------------
 }
}
