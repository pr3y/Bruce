#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "I2C_Class.hpp"

#include <M5GFX.h>

namespace m5
{
  I2C_Class In_I2C;
  I2C_Class Ex_I2C;

  void I2C_Class::setPort(i2c_port_t port_num, int sda, int scl)
  {
    _port_num = port_num;
    _pin_sda = sda;
    _pin_scl = scl;
    m5gfx::i2c::setPins(port_num, sda, scl).has_value();
  }

  bool I2C_Class::begin(i2c_port_t port_num, int sda, int scl)
  {
    setPort(port_num, sda, scl);
    return begin();
  }

  bool I2C_Class::begin(void)
  {
    return m5gfx::i2c::init(_port_num).has_value();
  }

  bool I2C_Class::release(void) const
  {
    return m5gfx::i2c::release(_port_num).has_value();
  }

  bool I2C_Class::start(std::uint8_t address, bool read, std::uint32_t freq) const
  {
    return m5gfx::i2c::beginTransaction(_port_num, address, freq, read).has_value();
  }

  bool I2C_Class::restart(std::uint8_t address, bool read, std::uint32_t freq) const
  {
    return m5gfx::i2c::restart(_port_num, address, freq, read).has_value();
  }

  bool I2C_Class::stop(void) const
  {
    return m5gfx::i2c::endTransaction(_port_num).has_value();
  }

  bool I2C_Class::write(std::uint8_t data) const
  {
    return m5gfx::i2c::writeBytes(_port_num, &data, 1).has_value();
  }

  bool I2C_Class::write(const std::uint8_t* __restrict__ data, std::size_t length) const
  {
    return m5gfx::i2c::writeBytes(_port_num, data, length).has_value();
  }

  bool I2C_Class::read(std::uint8_t* __restrict__ result, std::size_t length, bool last_nack) const
  {
    return m5gfx::i2c::readBytes(_port_num, result, length, last_nack).has_value();
  }

  bool I2C_Class::writeRegister(std::uint8_t address, std::uint8_t reg, const std::uint8_t* __restrict__ data, std::size_t length, std::uint32_t freq) const
  {
    return m5gfx::i2c::beginTransaction(_port_num, address, freq, false).has_value()
        && m5gfx::i2c::writeBytes(_port_num, &reg, 1).has_value()
        && m5gfx::i2c::writeBytes(_port_num, data, length).has_value()
        && m5gfx::i2c::endTransaction(_port_num).has_value();
  }

  bool I2C_Class::readRegister(std::uint8_t address, std::uint8_t reg, std::uint8_t* __restrict__ result, std::size_t length, std::uint32_t freq) const
  {
    return m5gfx::i2c::readRegister(_port_num, address, reg, result, length, freq).has_value();
  }

  bool I2C_Class::writeRegister8(std::uint8_t address, std::uint8_t reg, std::uint8_t data, std::uint32_t freq) const
  {
    return m5gfx::i2c::writeRegister8(_port_num, address, reg, data, 0, freq).has_value();
  }

  std::uint8_t I2C_Class::readRegister8(std::uint8_t address, std::uint8_t reg, std::uint32_t freq) const
  {
    return m5gfx::i2c::readRegister8(_port_num, address, reg, freq).value_or(0);
  }

  bool I2C_Class::bitOn(std::uint8_t address, std::uint8_t reg, std::uint8_t data, std::uint32_t freq) const
  {
    return m5gfx::i2c::bitOn(_port_num, address, reg, data, freq).has_value();
  }

  bool I2C_Class::bitOff(std::uint8_t address, std::uint8_t reg, std::uint8_t data, std::uint32_t freq) const
  {
    return m5gfx::i2c::bitOff(_port_num, address, reg, data, freq).has_value();
  }

  void I2C_Class::scanID(bool* __restrict__ result) const
  {
    // ESP32S3ではアドレス0~7をスキャン対象に含めると動作が停止する
    for (int i = 8; i < 0x78; i++)
    {
      result[i] = start(i, false, 400000) && stop();
    }
  }

  bool I2C_Device::writeRegister8Array(const std::uint8_t* reg_data_array, std::size_t length) const
  {
    for (size_t i = 0; i < length; i+=2)
    {
      if (!_i2c->writeRegister8(_addr, reg_data_array[i], reg_data_array[i+1], _freq))
      {
        return false;
      }
    }
    return true;
  }
}
#endif