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

#include <string.h>

#if defined (CONFIG_IDF_TARGET_ESP32S3) && __has_include(<esp32s3/rom/lldesc.h>)
 #include <esp32s3/rom/lldesc.h>
#elif defined (CONFIG_IDF_TARGET_ESP32S2) && __has_include(<esp32s2/rom/lldesc.h>)
 #include <esp32s2/rom/lldesc.h>
#elif defined (CONFIG_IDF_TARGET_ESP32C3) && __has_include(<esp32c3/rom/lldesc.h>)
 #include <esp32c3/rom/lldesc.h>
#elif __has_include(<esp32/rom/lldesc.h>)
 #include <esp32/rom/lldesc.h>
#else
 #include <rom/lldesc.h>
#endif

#if __has_include(<esp_private/spi_common_internal.h>)
 // ESP-IDF v5
 #include <esp_private/spi_common_internal.h>
#elif __has_include(<driver/spi_common_internal.h>)
 // ESP-IDF v4
 #include <driver/spi_common_internal.h>
#endif

#include <driver/spi_common.h>
#include <soc/spi_reg.h>

#if __has_include(<esp_idf_version.h>)
 #include <esp_idf_version.h>
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
  #define LGFX_ESP32_SPI_DMA_CH SPI_DMA_CH_AUTO
 #endif
#endif

#ifndef LGFX_ESP32_SPI_DMA_CH
#define LGFX_ESP32_SPI_DMA_CH 0
#endif

#include "../../Bus.hpp"
#include "../common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Bus_SPI : public IBus
  {
#if defined ( SPI_UPDATE )
    static constexpr uint32_t SPI_EXECUTE = SPI_USR | SPI_UPDATE;
    #define SPI_MOSI_DLEN_REG(i) (REG_SPI_BASE(i) + 0x1C)
    #define SPI_MISO_DLEN_REG(i) (REG_SPI_BASE(i) + 0x1C)
#else
    static constexpr uint32_t SPI_EXECUTE = SPI_USR;
#endif
  public:
    struct config_t
    {
      // max80MHz , 40MHz , 26.67MHz , 20MHz , 16MHz , and more ...
      uint32_t freq_write = 16000000;
      uint32_t freq_read  =  8000000;
      int16_t pin_sclk = -1;
      int16_t pin_miso = -1;
      int16_t pin_mosi = -1;
      int16_t pin_dc   = -1;
      uint8_t spi_mode = 0;
      bool spi_3wire = true;
      bool use_lock = true;
      uint8_t dma_channel = LGFX_ESP32_SPI_DMA_CH;
#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
      spi_host_device_t spi_host = VSPI_HOST;
#else
      spi_host_device_t spi_host = SPI2_HOST;
#endif
    };

    constexpr Bus_SPI(void) = default;

    const config_t& config(void) const { return _cfg; }

    void config(const config_t& config);

    bus_type_t busType(void) const override { return bus_type_t::bus_spi; }

    bool init(void) override;
    void release(void) override;

    void beginTransaction(void) override;
    void endTransaction(void) override;
    void wait(void) override;
    bool busy(void) const override;
    uint32_t getClock(void) const override { return _cfg.freq_write; }
    void setClock(uint32_t freq) override { if (_cfg.freq_write != freq) { _cfg.freq_write = freq; _last_freq_apb = 0; } }
    uint32_t getReadClock(void) const override { return _cfg.freq_read; }
    void setReadClock(uint32_t freq) override { if (_cfg.freq_read != freq) { _cfg.freq_read = freq; _last_freq_apb = 0; } }

    void flush(void) override {}
    bool writeCommand(uint32_t data, uint_fast8_t bit_length) override;
    void writeData(uint32_t data, uint_fast8_t bit_length) override;
    void writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count) override;
    void writePixels(pixelcopy_t* pc, uint32_t length) override;
    void writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma) override;

    void initDMA(void) override {}
    void addDMAQueue(const uint8_t* data, uint32_t length) override;
    void execDMAQueue(void) override;
    uint8_t* getDMABuffer(uint32_t length) override { return _flip_buffer.getBuffer(length); }

    void beginRead(uint_fast8_t dummy_bits) override;
    void beginRead(void) override;
    void endRead(void) override;
    uint32_t readData(uint_fast8_t bit_length) override;
    bool readBytes(uint8_t* dst, uint32_t length, bool use_dma) override;
    void readPixels(void* dst, pixelcopy_t* pc, uint32_t length) override;

  private:

    static __attribute__ ((always_inline)) inline volatile uint32_t* reg(uint32_t addr) { return (volatile uint32_t *)ETS_UNCACHED_ADDR(addr); }
    __attribute__ ((always_inline)) inline void exec_spi(void) {        *_spi_cmd_reg = SPI_EXECUTE; }
    __attribute__ ((always_inline)) inline void wait_spi(void) { while (*_spi_cmd_reg & SPI_USR); }
    __attribute__ ((always_inline)) inline void set_write_len(uint32_t bitlen) { *_spi_mosi_dlen_reg = bitlen - 1; }
    __attribute__ ((always_inline)) inline void set_read_len( uint32_t bitlen) { *reg(SPI_MISO_DLEN_REG(_spi_port)) = bitlen - 1; }

    void dc_control(bool flg)
    {
      auto reg = _gpio_reg_dc[flg];
      auto mask = _mask_reg_dc;
      auto spi_cmd_reg = _spi_cmd_reg;
#if !defined ( CONFIG_IDF_TARGET ) || defined ( CONFIG_IDF_TARGET_ESP32 )
      while (*spi_cmd_reg & SPI_USR) {}    // wait SPI
#else
 #if defined ( SOC_GDMA_SUPPORTED )
      auto dma = _clear_dma_reg;
      if (dma)
      {
        _clear_dma_reg = nullptr;
        while (*spi_cmd_reg & SPI_USR) {}    // wait SPI
        *dma = 0;
      }
      else
      {
        while (*spi_cmd_reg & SPI_USR) {}    // wait SPI
      }
 #else
      auto dma = _spi_dma_out_link_reg;
      while (*spi_cmd_reg & SPI_USR) {}    // wait SPI
      *dma = 0;
 #endif
#endif
      *reg = mask;
    }

    void _alloc_dmadesc(size_t len);
    void _spi_dma_reset(void);
    void _setup_dma_desc_links(const uint8_t *data, int32_t len);

    config_t _cfg;
    FlipBuffer _flip_buffer;
    volatile uint32_t* _gpio_reg_dc[2] = { nullptr, nullptr };
    volatile uint32_t* _spi_mosi_dlen_reg = nullptr;
    volatile uint32_t* _spi_w0_reg = nullptr;
    volatile uint32_t* _spi_cmd_reg = nullptr;
    volatile uint32_t* _spi_user_reg = nullptr;
    volatile uint32_t* _spi_dma_out_link_reg = nullptr;
    volatile uint32_t* _spi_dma_outstatus_reg = nullptr;
    volatile uint32_t* _clear_dma_reg = nullptr;
    uint32_t _last_freq_apb = 0;
    uint32_t _clkdiv_write = 0;
    uint32_t _clkdiv_read = 0;
    uint32_t _user_reg = 0;
    uint32_t _mask_reg_dc = 0;
    uint32_t _dma_queue_bytes = 0;
    lldesc_t* _dmadesc = nullptr;
    uint32_t _dmadesc_size = 0;
    lldesc_t* _dma_queue = nullptr;
    uint32_t _dma_queue_size = 0;
    uint32_t _dma_queue_capacity = 0;
    uint8_t _spi_port = 0;
    uint8_t _dma_ch = 0;
    bool _inited = false;
  };

//----------------------------------------------------------------------------
 }
}
