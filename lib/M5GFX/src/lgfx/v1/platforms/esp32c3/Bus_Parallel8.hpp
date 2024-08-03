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

  class Bus_Parallel8 : public IBus
  {
  public:
    struct config_t
    {
      // max 40MHz , 27MHz , 20MHz , 16MHz , 13.3MHz , 11.43MHz , 10MHz , 8.9MHz  and more ...
      int8_t pin_wr = -1;
      int8_t pin_rd = -1;
      int8_t pin_rs = -1;  // D/C
      union
      {
        int8_t pin_data[8];
        struct
        {
          int8_t pin_d0;
          int8_t pin_d1;
          int8_t pin_d2;
          int8_t pin_d3;
          int8_t pin_d4;
          int8_t pin_d5;
          int8_t pin_d6;
          int8_t pin_d7;
        };
      };
    };


    const config_t& config(void) const { return _cfg; }
    void config(const config_t& config);

    bus_type_t busType(void) const override { return bus_type_t::bus_parallel8; }

    bool init(void) override;
    void release(void) override {};

    void beginTransaction(void) override {};
    void endTransaction(void) override {};
    void wait(void) override {};
    bool busy(void) const override { return false; }

    void flush(void) override {};
    bool writeCommand(uint32_t data, uint_fast8_t bit_length) override;
    void writeData(uint32_t data, uint_fast8_t bit_length) override;
    void writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count) override;
    void writePixels(pixelcopy_t* param, uint32_t length) override;
    void writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma) override;

    void initDMA(void) override {}
    void addDMAQueue(const uint8_t* data, uint32_t length) override { writeBytes(data, length, true, true); }
    void execDMAQueue(void) override {};
    uint8_t* getDMABuffer(uint32_t length) override { return _flip_buffer.getBuffer(length); }

    void beginRead(void) override;
    void endRead(void) override;
    uint32_t readData(uint_fast8_t bit_length) override;
    bool readBytes(uint8_t* dst, uint32_t length, bool use_dma) override;
    void readPixels(void* dst, pixelcopy_t* param, uint32_t length) override;

  private:

    uint32_t _gpio_tbl[256];
    uint32_t _gpio_low_mask = 0;
    uint32_t _gpio_wr_mask = 0;
    volatile uint32_t* _gpio_reg_dc[2] = { nullptr, nullptr };
    uint32_t _mask_reg_dc = 0;

    config_t _cfg;
    SimpleBuffer _flip_buffer;
    size_t _div_num;
    
    void _init_pin(bool read = false);
    void _read_bytes(uint8_t* dst, uint32_t length);
  };

//----------------------------------------------------------------------------
 }
}
