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

#if __has_include(<esp32/rom/lldesc.h>)
 #include <esp32/rom/lldesc.h>
#else
 #include <rom/lldesc.h>
#endif

#if __has_include(<freertos/FreeRTOS.h>)
 #include <freertos/FreeRTOS.h>
#endif

#if __has_include(<driver/i2s_std.h>)
 #include <driver/i2s_std.h>
#else
 #include <driver/i2s.h>
#endif

#include <soc/i2s_struct.h>

#include "../../Bus.hpp"
#include "../common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Bus_Parallel16 : public IBus
  {
  public:
    struct config_t
    {
      union {
        i2s_port_t i2s_port = I2S_NUM_0;
        int port;
      };

      // max 40MHz , 27MHz , 20MHz , 16MHz , 13.3MHz , 11.43MHz , 10MHz , 8.9MHz  and more ...
      uint32_t freq_write = 16000000;
      union
      {
        int8_t pin_ctrl[3] = { -1, -1, -1 };
        struct
        {
          int8_t pin_rd;
          int8_t pin_wr;
          int8_t pin_rs;  // D/C
        };
      };
      union
      {
        int8_t pin_data[16];
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
          int8_t pin_d8;
          int8_t pin_d9;
          int8_t pin_d10;
          int8_t pin_d11;
          int8_t pin_d12;
          int8_t pin_d13;
          int8_t pin_d14;
          int8_t pin_d15;
        };
      };
    };


    const config_t& config(void) const { return _cfg; }
    void config(const config_t& config);

    bus_type_t busType(void) const override { return bus_type_t::bus_parallel16; }

    bool init(void) override;
    void release(void) override;

    void beginTransaction(void) override;
    void endTransaction(void) override;
    void wait(void) override;
    bool busy(void) const override;
    uint32_t getClock(void) const override { return _cfg.freq_write; }
    void setClock(uint32_t freq) override { if (_cfg.freq_write != freq) { _cfg.freq_write = freq; config(_cfg); } }

    void flush(void) override;
    bool writeCommand(uint32_t data, uint_fast8_t bit_length) override;
    void writeData(uint32_t data, uint_fast8_t bit_length) override;
    void writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count) override;
    void writePixels(pixelcopy_t* param, uint32_t length) override;
    void writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma) override;

    void initDMA(void) override {}
    void addDMAQueue(const uint8_t* data, uint32_t length) override { writeBytes(data, length, true, true); }
    void execDMAQueue(void) override { flush(); };
    uint8_t* getDMABuffer(uint32_t length) override { return _flip_buffer.getBuffer(length); }

    void beginRead(void) override;
    void endRead(void) override;
    uint32_t readData(uint_fast8_t bit_length) override;
    bool readBytes(uint8_t* dst, uint32_t length, bool use_dma) override;
    void readPixels(void* dst, pixelcopy_t* param, uint32_t length) override;

  private:

    void dc_control(bool flg)
    {
      auto reg = _gpio_reg_dc[flg];
      auto mask = _mask_reg_dc;
      auto i2s_dev = (i2s_dev_t*)_dev;
      if (i2s_dev->out_link.val)
      {
#if defined (CONFIG_IDF_TARGET_ESP32S2)
        while (!(i2s_dev->lc_state0.out_empty)) {}
#else
        while (!(i2s_dev->lc_state0 & 0x80000000)) {} // I2S_OUT_EMPTY
#endif
        i2s_dev->out_link.val = 0;
      }
      while (!i2s_dev->state.tx_idle) {}
      *reg = mask;
    }

    static constexpr size_t CACHE_SIZE = 132;

    config_t _cfg;
    FlipBuffer _flip_buffer;
    size_t _div_num;
    size_t _cache_index;
    uint32_t _cache[2][CACHE_SIZE];
    uint32_t* _cache_flip;

    bool _has_align_data;
    uint8_t _align_data;

    uint32_t _pin_mask_h, _pin_mask_l, _pin_index_h, _pin_index_l;

    void _wait(void);
    void _init_pin(bool read = false);
    size_t _flush(size_t idx, bool dc = true);
    void _read_bytes(uint8_t* dst, uint32_t length);

    void _alloc_dmadesc(size_t len);
    void _setup_dma_desc_links(const uint8_t *data, int32_t len);

    volatile uint32_t* _gpio_reg_dc[2] = { nullptr, nullptr };
    uint32_t _mask_reg_dc = 0;
    bool _direct_dc;

    uint32_t _clkdiv_write;
    volatile void *_dev;

    lldesc_t* _dmadesc = nullptr;
    uint32_t _dmadesc_size = 0;
  };

//----------------------------------------------------------------------------
 }
}
