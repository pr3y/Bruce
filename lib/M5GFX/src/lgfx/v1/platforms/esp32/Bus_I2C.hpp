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
#pragma once

#include "../../Bus.hpp"
#include "../common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Bus_I2C : public IBus
  {
  public:
    struct config_t
    {
      uint32_t freq_write = 400000;
      uint32_t freq_read = 400000;
      int16_t pin_scl = 22;
      int16_t pin_sda = 21;
      uint8_t i2c_port = 0;      // e.g. ESP32 0=I2C_NUM_0 / 1=I2C_NUM_1
      uint8_t i2c_addr = 0x3C;
      uint32_t prefix_cmd = 0x00;
      uint32_t prefix_data = 0x40;
      uint32_t prefix_len = 1;
    };

    const config_t& config(void) const { return _cfg; }

    void config(const config_t& config);

    bus_type_t busType(void) const override { return bus_type_t::bus_i2c; }

    bool init(void) override;
    void release(void) override;

    void beginTransaction(void) override;
    void endTransaction(void) override;
    void wait(void) override;
    bool busy(void) const override;

    void flush(void) override {}
    bool writeCommand(uint32_t data, uint_fast8_t bit_length) override;
    void writeData(uint32_t data, uint_fast8_t bit_length) override;
    void writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count) override;
    void writePixels(pixelcopy_t* param, uint32_t length) override;
    void writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma) override;

    void initDMA(void) {}
    void addDMAQueue(const uint8_t* data, uint32_t length) override { writeBytes(data, length, true, true); }
    void execDMAQueue(void) {}
    uint8_t* getDMABuffer(uint32_t length) override { return _flip_buffer.getBuffer(length); }

    void beginRead(void) override;
    void endRead(void) override;
    uint32_t readData(uint_fast8_t bit_length) override;
    bool readBytes(uint8_t* dst, uint32_t length, bool use_dma) override { return readBytes(dst, length, use_dma, false); }
    bool readBytes(uint8_t* dst, uint32_t length, bool use_dma, bool last_nack) override;
    void readPixels(void* dst, pixelcopy_t* param, uint32_t length) override;

  protected:

    config_t _cfg;
    SimpleBuffer _flip_buffer;
    bool _need_wait;
    enum state_t
    {
      state_none,
      state_write_none,
      state_write_cmd,
      state_write_data,
      state_read,
    };
    state_t _state = state_none;

    void dc_control(bool dc);
  };

//----------------------------------------------------------------------------
 }
}
