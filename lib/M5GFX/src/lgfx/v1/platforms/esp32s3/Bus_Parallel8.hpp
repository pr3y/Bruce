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

#if __has_include (<esp_lcd_panel_io.h>)
#include <esp_lcd_panel_io.h>
#include <esp_private/gdma.h>
#include <hal/dma_types.h>

#include "../../Bus.hpp"
#include "../common.hpp"

struct lcd_cam_dev_t;

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
      // LCD_CAM peripheral number. No need to change (only 0 for ESP32-S3.)
      int port = 0;

      // max 80MHz.
      uint32_t freq_write = 16000000;
      uint32_t freq_read  =  8000000;
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
    void release(void) override;

    void beginTransaction(void) override;
    void endTransaction(void) override;
    void wait(void) override;
    bool busy(void) const override;

    void flush(void) override {};
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

    static constexpr size_t CACHE_SIZE = 256;

    config_t _cfg;
    FlipBuffer _flip_buffer;
    uint32_t _clock_reg_value;
    uint32_t _cache[2][CACHE_SIZE / sizeof(uint32_t)];
    uint32_t* _cache_flip;

    void _init_pin(bool read = false);
    void _read_bytes(uint8_t* dst, uint32_t length);

    void _alloc_dmadesc(size_t len);
    void _setup_dma_desc_links(const uint8_t *data, int32_t len);

    volatile lcd_cam_dev_t* _dev;

    uint32_t _dmadesc_size = 0;
    dma_descriptor_t* _dmadesc = nullptr;
    esp_lcd_i80_bus_handle_t _i80_bus = nullptr;
    int _dma_ch;
    volatile uint32_t* _dma_out_link_reg;
    volatile uint32_t* _dma_outstatus_reg;
  };

//----------------------------------------------------------------------------
 }
}
#endif