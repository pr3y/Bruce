#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_I2C_CLASS_H__
#define __M5_I2C_CLASS_H__

#include "m5unified_common.h"
#if __has_include ( <driver/i2c.h> )

#include <driver/i2c.h>

#endif

#include <cstdint>
#include <cstddef>

namespace m5
{
  class I2C_Class
  {
  public:
    /// setup I2C port parameters. (No begin)
    /// @param port_num I2C number. (I2C_NUM_0 or I2C_NUM_1).
    /// @param pin_sda SDA pin number.
    /// @param pin_scl SCL pin number.
    void setPort(i2c_port_t port_num, int pin_sda, int pin_scl);

    /// setup and begin I2C peripheral. (No communication is performed.)
    /// @param port_num I2C number. (I2C_NUM_0 or I2C_NUM_1).
    /// @param pin_sda SDA pin number.
    /// @param pin_scl SCL pin number.
    /// @return success(true) or failed(false).
    bool begin(i2c_port_t port_num, int pin_sda, int pin_scl);

    /// begin I2C peripheral. (No communication is performed.)
    /// @return success(true) or failed(false).
    bool begin(void);

    /// release I2C peripheral.
    /// @return success(true) or failed(false).
    bool release(void) const;

    /// Sends the I2C start condition and the address of the slave.
    /// @param address slave addr.
    /// @param read bit of read flag. true=read / false=write.
    /// @return success(true) or failed(false).
    bool start(std::uint8_t address, bool read, std::uint32_t freq) const;

    /// Sends the I2C repeated start condition and the address of the slave.
    /// @param address slave addr.
    /// @param read bit of read flag. true=read / false=write.
    /// @return success(true) or failed(false).
    bool restart(std::uint8_t address, bool read, std::uint32_t freq) const;

    /// Sends the I2C stop condition.
    /// If an ACK error occurs, return false.
    /// @return success(true) or failed(false).
    bool stop(void) const;

    /// Send 1 byte of data.
    /// @param data write data.
    /// @return success(true) or failed(false).
    bool write(std::uint8_t data) const;

    /// Send multiple bytes of data.
    /// @param[in] data write data array.
    /// @param     length data array length.
    /// @return success(true) or failed(false).
    bool write(const std::uint8_t* data, std::size_t length) const;

    /// Receive multiple bytes of data.
    /// @param[out] result read data array.
    /// @param      length data array length.
    /// @return success(true) or failed(false).
    bool read(std::uint8_t* result, std::size_t length, bool last_nack = false) const;

    //----------

    /// Write multiple bytes value to the register. Performs a series of communications from START to STOP.
    /// @param address slave addr.
    /// @param reg     register number.
    /// @param[in] data write data array.
    /// @param     length data array length.
    /// @return success(true) or failed(false).
    bool writeRegister(std::uint8_t address, std::uint8_t reg, const std::uint8_t* data, std::size_t length, std::uint32_t freq) const;

    /// Read multiple bytes value from the register. Performs a series of communications from START to STOP.
    /// @param address slave addr.
    /// @param reg     register number.
    /// @param[out] result read data array.
    /// @param      length data array length.
    /// @return success(true) or failed(false).
    bool readRegister(std::uint8_t address, std::uint8_t reg, std::uint8_t* result, std::size_t length, std::uint32_t freq) const;

    /// Write a 1-byte value to the register. Performs a series of communications from START to STOP.
    /// @param address slave addr.
    /// @param reg     register number.
    /// @param data    write data.
    /// @return success(true) or failed(false).
    bool writeRegister8(std::uint8_t address, std::uint8_t reg, std::uint8_t data, std::uint32_t freq) const;

    /// Read a 1-byte value from the register. Performs a series of communications from START to STOP.
    /// @param address slave addr.
    /// @param reg     register number.
    /// @return read value.
    std::uint8_t readRegister8(std::uint8_t address, std::uint8_t reg, std::uint32_t freq) const;

    /// Write a 1-byte value to the register by bit add operation. Performs a series of communications from START to STOP.
    /// @param address slave addr.
    /// @param reg     register number.
    /// @param data    add bit data.
    /// @return success(true) or failed(false).
    bool bitOn(std::uint8_t address, std::uint8_t reg, std::uint8_t data, std::uint32_t freq) const;

    /// Write a 1-byte value to the register by bit erase operation. Performs a series of communications from START to STOP.
    /// @param address slave addr.
    /// @param reg     register number.
    /// @param data    erase bit data.
    /// @return success(true) or failed(false).
    bool bitOff(std::uint8_t address, std::uint8_t reg, std::uint8_t data, std::uint32_t freq) const;

    /// execute I2C scan. (for 7bit address)
    /// @param[out] result data array needs 120 Bytes.
    void scanID(bool* result) const;

    i2c_port_t getPort(void) const { return _port_num; }
    int8_t getSDA(void) const { return _pin_sda; }
    int8_t getSCL(void) const { return _pin_scl; }

    bool isEnabled(void) const { return _port_num >= 0; }

  private:
    i2c_port_t _port_num = (i2c_port_t)-1;
    int8_t _pin_sda = -1;
    int8_t _pin_scl = -1;
  };



  /// for internal I2C device
  extern I2C_Class In_I2C;

  /// for external I2C device
  extern I2C_Class Ex_I2C;



  class I2C_Device
  {
  public:
    I2C_Device(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c = &In_I2C)
    : _i2c   { i2c      }
    , _freq  { freq     }
    , _addr  { i2c_addr }
    , _init  { false    }
    {}

    void setPort(I2C_Class* i2c) { _i2c = i2c; }

    void setClock(std::uint32_t freq) { _freq = freq; }

    bool writeRegister8(std::uint8_t reg, std::uint8_t data) const
    {
      return _i2c->writeRegister8(_addr, reg, data, _freq);
    }

    std::uint8_t readRegister8(std::uint8_t reg) const
    {
      return _i2c->readRegister8(_addr, reg, _freq);
    }

    bool writeRegister8Array(const std::uint8_t* reg_data_array, std::size_t length) const;

    bool writeRegister(std::uint8_t reg, const std::uint8_t* data, std::size_t length) const
    {
      return _i2c->writeRegister(_addr, reg, data, length, _freq);
    }

    bool readRegister(std::uint8_t reg, std::uint8_t* result, std::size_t length) const
    {
      return _i2c->readRegister(_addr, reg, result, length, _freq);
    }

    bool bitOn(std::uint8_t reg, std::uint8_t bit) const
    {
      return _i2c->bitOn(_addr, reg, bit, _freq);
    }

    bool bitOff(std::uint8_t reg, std::uint8_t bit) const
    {
      return _i2c->bitOff(_addr, reg, bit, _freq);
    }

    bool isEnabled(void) const { return _init; }

  protected:
    I2C_Class *_i2c;
    std::uint32_t _freq;
    std::uint8_t _addr;
    bool _init;
  };

}

#endif
#endif