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
#if defined (CONFIG_IDF_TARGET_ESP32S3)
#if __has_include (<esp_lcd_panel_io.h>)
#include <esp_lcd_panel_io.h>

#include "Bus_Parallel8.hpp"
#include "../../misc/pixelcopy.hpp"

#include <esp_log.h>
#include <rom/gpio.h>
#include <hal/gpio_ll.h>
#include <hal/lcd_hal.h>
#include <soc/lcd_cam_reg.h>
#include <soc/lcd_cam_struct.h>

#include <soc/gdma_channel.h>
#include <soc/gdma_reg.h>
#if !defined (DMA_OUT_LINK_CH0_REG)
  #define DMA_OUT_LINK_CH0_REG       GDMA_OUT_LINK_CH0_REG
  #define DMA_OUTFIFO_STATUS_CH0_REG GDMA_OUTFIFO_STATUS_CH0_REG
  #define DMA_OUTLINK_START_CH0      GDMA_OUTLINK_START_CH0
  #define DMA_OUTFIFO_EMPTY_CH0      GDMA_OUTFIFO_EMPTY_L3_CH0
#endif

#if ( ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 0) )
 #if !defined (LCD_CAM_LCD_UPDATE)
  #define LCD_CAM_LCD_UPDATE LCD_CAM_LCD_UPDATE_REG
 #endif
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static __attribute__ ((always_inline)) inline volatile uint32_t* reg(uint32_t addr) { return (volatile uint32_t *)ETS_UNCACHED_ADDR(addr); }

  static lcd_cam_dev_t* getDev(int port)
  {
    return &LCD_CAM;
  }

  void Bus_Parallel8::config(const config_t& cfg)
  {
    _cfg = cfg;
    auto port = cfg.port;
    _dev = getDev(port);
  }

  bool Bus_Parallel8::init(void)
  {
    _init_pin();

    for (size_t i = 0; i < 3; ++i)
    {
      int32_t pin = _cfg.pin_ctrl[i];
      if (pin < 0) { continue; }
      gpio_pad_select_gpio(pin);
      gpio_hi(pin);
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
    }

    gpio_matrix_out(_cfg.pin_rs, LCD_DC_IDX, 0, 0);
    gpio_matrix_out(_cfg.pin_wr, LCD_PCLK_IDX, 0, 0);

    esp_lcd_i80_bus_config_t bus_config;
    memset(&bus_config, 0, sizeof(esp_lcd_i80_bus_config_t));
    bus_config.clk_src = lcd_clock_source_t::LCD_CLK_SRC_PLL160M; // IDFのバージョンによってenumの値が異なるので注意
    bus_config.dc_gpio_num = _cfg.pin_rs;
    bus_config.wr_gpio_num = _cfg.pin_wr;
    for (int i = 0; i < 8; ++i)
    {
      bus_config.data_gpio_nums[i] = _cfg.pin_data[i];
      bus_config.data_gpio_nums[i+8] = -1;
    }
    bus_config.bus_width = 8;
    bus_config.max_transfer_bytes = 4092;

    esp_lcd_new_i80_bus(&bus_config, &_i80_bus);
    _dma_out_link_reg  = nullptr;
    _dma_outstatus_reg = nullptr;
    _dma_ch = search_dma_out_ch(SOC_GDMA_TRIG_PERIPH_LCD0);
    if (_dma_ch >= 0)
    {
      _dma_out_link_reg  = reg(DMA_OUT_LINK_CH0_REG       + _dma_ch * 0xC0);
      _dma_outstatus_reg = reg(DMA_OUTFIFO_STATUS_CH0_REG + _dma_ch * 0xC0);
    }
    else
    {
      ESP_LOGE("Bus_Parallel8", "DMA channel not found...");
    }

    for (size_t i = 0; i < 8; ++i)
    {
      gpio_ll_input_enable(&GPIO, (gpio_num_t)_cfg.pin_data[i]);
    }

    uint32_t div_a, div_b, div_n, clkcnt;
    calcClockDiv(&div_a, &div_b, &div_n, &clkcnt, 240*1000*1000, _cfg.freq_write);
    lcd_cam_lcd_clock_reg_t lcd_clock;
    lcd_clock.lcd_clkcnt_n = std::max<uint32_t>(1u, clkcnt - 1);
    lcd_clock.lcd_clk_equ_sysclk = (clkcnt == 1);
    lcd_clock.lcd_ck_idle_edge = true;
    lcd_clock.lcd_ck_out_edge = false;
    lcd_clock.lcd_clkm_div_num = div_n;
    lcd_clock.lcd_clkm_div_b = div_b;
    lcd_clock.lcd_clkm_div_a = div_a;
    lcd_clock.lcd_clk_sel = 2; // clock_select: 1=XTAL CLOCK / 2=240MHz / 3=160MHz
    lcd_clock.clk_en = true;

    _clock_reg_value = lcd_clock.val;

    _alloc_dmadesc(1);
    return true;
  }

  void Bus_Parallel8::_init_pin(bool read)
  {
    int8_t* pins = _cfg.pin_data;
    if (read)
    {
      for (size_t i = 0; i < 8; ++i)
      {
        gpio_ll_output_disable(&GPIO, (gpio_num_t)pins[i]);
      }
    }
    else
    {
      auto idx_base = LCD_DATA_OUT0_IDX;
      for (size_t i = 0; i < 8; ++i)
      {
        gpio_matrix_out(pins[i], idx_base + i, 0, 0);
      }
    }
  }

  void Bus_Parallel8::release(void)
  {
    if (_i80_bus)
    {
      esp_lcd_del_i80_bus(_i80_bus);
    }
    if (_dmadesc)
    {
      heap_caps_free(_dmadesc);
      _dmadesc = nullptr;
      _dmadesc_size = 0;
    }
  }

  void Bus_Parallel8::beginTransaction(void)
  {
    auto dev = _dev;
    dev->lcd_clock.val = _clock_reg_value;
    // int clk_div = std::min(63u, std::max(1u, 120*1000*1000 / (_cfg.freq_write+1)));
    // dev->lcd_clock.lcd_clk_sel = 2; // clock_select: 1=XTAL CLOCK / 2=240MHz / 3=160MHz
    // dev->lcd_clock.lcd_clkcnt_n = clk_div;
    // dev->lcd_clock.lcd_clk_equ_sysclk = 0;
    // dev->lcd_clock.lcd_ck_idle_edge = true;
    // dev->lcd_clock.lcd_ck_out_edge = false;

    dev->lcd_misc.val = LCD_CAM_LCD_CD_IDLE_EDGE;
    // dev->lcd_misc.lcd_cd_idle_edge = 1;
    // dev->lcd_misc.lcd_cd_cmd_set = 0;
    // dev->lcd_misc.lcd_cd_dummy_set = 0;
    // dev->lcd_misc.lcd_cd_data_set = 0;

    dev->lcd_user.val = 0;
    // dev->lcd_user.lcd_byte_order = false;
    // dev->lcd_user.lcd_bit_order = false;
    // dev->lcd_user.lcd_8bits_order = false;

    dev->lcd_user.val = LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE;

    _cache_flip = _cache[0];
  }

  void Bus_Parallel8::endTransaction(void)
  {
    auto dev = _dev;
    while (dev->lcd_user.val & LCD_CAM_LCD_START) {}
  }

  void Bus_Parallel8::wait(void)
  {
    auto dev = _dev;
    while (dev->lcd_user.val & LCD_CAM_LCD_START) {}
  }

  bool Bus_Parallel8::busy(void) const
  {
    auto dev = _dev;
    return (dev->lcd_user.val & LCD_CAM_LCD_START);
  }

  bool Bus_Parallel8::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
    auto bytes = bit_length >> 3;
    auto dev = _dev;
    auto reg_lcd_user = &(dev->lcd_user.val);
    dev->lcd_misc.val = LCD_CAM_LCD_CD_IDLE_EDGE | LCD_CAM_LCD_CD_CMD_SET;
    do
    {
      dev->lcd_cmd_val.lcd_cmd_value = data;
      data >>= 8;
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
    } while (--bytes);
    return true;
  }

  void Bus_Parallel8::writeData(uint32_t data, uint_fast8_t bit_length)
  {
    auto bytes = bit_length >> 3;
    auto dev = _dev;
    auto reg_lcd_user = &(dev->lcd_user.val);
    dev->lcd_misc.val = LCD_CAM_LCD_CD_IDLE_EDGE;

    if (bytes & 1)
    {
      dev->lcd_cmd_val.lcd_cmd_value = data;
      data >>= 8;
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
      if (0 == --bytes) { return; }
    }

    dev->lcd_cmd_val.lcd_cmd_value = (data & 0xFF) | (data << 8);
    while (*reg_lcd_user & LCD_CAM_LCD_START) {}
    *reg_lcd_user = LCD_CAM_LCD_CMD | LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
    bytes >>= 1;
    if (--bytes)
    {
      data >>= 16;
      dev->lcd_cmd_val.lcd_cmd_value = (data & 0xFF) | (data << 8);
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_CMD | LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
    }
  }

  void Bus_Parallel8::writeDataRepeat(uint32_t color_raw, uint_fast8_t bit_length, uint32_t count)
  {
    uint32_t regbuf0 = color_raw | color_raw << bit_length;
    uint32_t regbuf1;
    uint32_t regbuf2;
    // make 12Bytes data.
    bool bits24 = (bit_length == 24);
    if (bits24) {
      regbuf1 = regbuf0 >> 8 | regbuf0 << 16;
      regbuf2 = regbuf0 >>16 | regbuf0 <<  8;
    } else {
      if (bit_length == 8) { regbuf0 |= regbuf0 << 16; }
      regbuf1 = regbuf0;
      regbuf2 = regbuf0;
    }
    auto dst = _cache_flip;
    _cache_flip = _cache[(dst == _cache[0])];
    dst[0] = regbuf0;
    dst[1] = regbuf1;
    dst[2] = regbuf2;
    dst[3] = regbuf0;
    dst[4] = regbuf1;
    dst[5] = regbuf2;
    dst[6] = regbuf0;
    dst[7] = regbuf1;
    dst[8] = regbuf2;

    uint32_t length = count * (bit_length >> 3);
    if (length > 36)
    {
      memcpy((void*)&dst[ 9], dst, 36);
      if (length > 72)
      {
        memcpy((void*)&dst[18], dst, 72);
        if (length > 144)
        {
          memcpy((void*)&dst[36], dst, 108);
        }
      }
    }
    uint32_t len = 0;
    do
    {
      len = length;
      if (length > 252u)
      {
        len = ((((length - 1) % 252) + 12) / 12) * 12;
      }
      writeBytes((const uint8_t*)dst, len, true, true);
    } while (0 != (length -= len));
  }

  void Bus_Parallel8::writePixels(pixelcopy_t* param, uint32_t length)
  {
    const uint32_t bytes = param->dst_bits >> 3;
    auto fp_copy = param->fp_copy;
    const uint32_t limit = CACHE_SIZE / bytes;
    uint8_t len = length % limit;
    if (len)
    {
      auto c = _cache_flip;
      fp_copy(c, 0, len, param);
      _cache_flip = _cache[(c == _cache[0])];
      writeBytes((uint8_t*)c, len * bytes, true, true);
      if (0 == (length -= len)) return;
    }
    do
    {
      auto c = _cache_flip;
      fp_copy(_cache_flip, 0, limit, param);
      _cache_flip = _cache[(c == _cache[0])];
      writeBytes((uint8_t*)c, limit * bytes, true, true);
    } while (length -= limit);
  }

  void Bus_Parallel8::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    if (length <= 4)
    {
      if (dc)
      {
        writeData(*(const uint32_t*)data, length << 3);
        return;
      }
      else
      {
        writeCommand(*(const uint32_t*)data, length << 3);
        return;
      }
    }
    uint32_t slow_wait = 8000000 / _cfg.freq_write;

    auto dev = _dev;
    do
    {
      auto reg_lcd_user = &(dev->lcd_user.val);
      dev->lcd_misc.lcd_cd_cmd_set  = !dc;
      dev->lcd_cmd_val.lcd_cmd_value = data[0] | data[1] << 16;
      uint32_t cmd_val = data[2] | data[3] << 16;
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_CMD | LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;

      if (use_dma)
      {
        if (slow_wait) { delayMicroseconds(slow_wait); }
        _setup_dma_desc_links(&data[4], length - 4);
        *_dma_out_link_reg = DMA_OUTLINK_START_CH0 | (((uintptr_t)_dmadesc) & 0xFFFFF);
        length = 0;
      }
      else
      {
        size_t len = length;
        if (len > CACHE_SIZE)
        {
          len = (((len - 1) % CACHE_SIZE) + 4) & ~3u;
        }
        memcpy(_cache_flip, &data[4], (len-4+3)&~3);
        _setup_dma_desc_links((const uint8_t*)_cache_flip, len-4);
        *_dma_out_link_reg = DMA_OUTLINK_START_CH0 | (((uintptr_t)_dmadesc) & 0xFFFFF);
        length -= len;
        data += len;
        _cache_flip = _cache[(_cache_flip == _cache[0])];
      }
      dev->lcd_cmd_val.lcd_cmd_value = cmd_val;
      dev->lcd_misc.lcd_cd_data_set = !dc;
      *reg_lcd_user = LCD_CAM_LCD_ALWAYS_OUT_EN | LCD_CAM_LCD_DOUT | LCD_CAM_LCD_CMD | LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_UPDATE;
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_ALWAYS_OUT_EN | LCD_CAM_LCD_DOUT | LCD_CAM_LCD_CMD | LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_START;
    } while (length);
  }

  void Bus_Parallel8::beginRead(void)
  {
    wait();
    _init_pin(true);
    gpio_lo(_cfg.pin_rd);
    gpio_matrix_out(_cfg.pin_rs, 0x100, 0, 0);
  }

  void Bus_Parallel8::endRead(void)
  {
    gpio_hi(_cfg.pin_rd);
    _init_pin();
    gpio_matrix_out(_cfg.pin_rs, LCD_DC_IDX, 0, 0);
  }

  void Bus_Parallel8::_read_bytes(uint8_t* dst, uint32_t length)
  {
    union
    {
      uint32_t in32[2];
      uint8_t in[8];
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
      in32[1] = GPIO.in1.val;
      in32[0] = GPIO.in;
      *reg_rd_h = mask_rd;
      val =              (1 & (in[(idx >>  0) & 7] >> ((mask >>  0) & 7)));
      val = (val << 1) + (1 & (in[(idx >>  3) & 7] >> ((mask >>  3) & 7)));
      val = (val << 1) + (1 & (in[(idx >>  6) & 7] >> ((mask >>  6) & 7)));
      val = (val << 1) + (1 & (in[(idx >>  9) & 7] >> ((mask >>  9) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 12) & 7] >> ((mask >> 12) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 15) & 7] >> ((mask >> 15) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 18) & 7] >> ((mask >> 18) & 7)));
      val = (val << 1) + (1 & (in[(idx >> 21) & 7] >> ((mask >> 21) & 7)));
      *reg_rd_l = mask_rd;
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

  void Bus_Parallel8::_alloc_dmadesc(size_t len)
  {
    if (_dmadesc) heap_caps_free(_dmadesc);
    _dmadesc_size = len;
    _dmadesc = (dma_descriptor_t*)heap_caps_malloc(sizeof(dma_descriptor_t) * len, MALLOC_CAP_DMA);
  }

  void Bus_Parallel8::_setup_dma_desc_links(const uint8_t *data, int32_t len)
  {
    static constexpr size_t MAX_DMA_LEN = (4096-4);

    if (_dmadesc_size * MAX_DMA_LEN < len)
    {
      _alloc_dmadesc(len / MAX_DMA_LEN + 1);
    }
    dma_descriptor_t *dmadesc = _dmadesc;

    while (len > MAX_DMA_LEN)
    {
      len -= MAX_DMA_LEN;
      dmadesc->buffer = (uint8_t *)data;
      data += MAX_DMA_LEN;
      *(uint32_t*)dmadesc = MAX_DMA_LEN | MAX_DMA_LEN << 12 | 0x80000000;
      dmadesc->next = dmadesc + 1;
      dmadesc++;
    }
    *(uint32_t*)dmadesc = ((len + 3) & ( ~3 )) | len << 12 | 0xC0000000;
    dmadesc->buffer = (uint8_t *)data;
    dmadesc->next = nullptr;
  }

//----------------------------------------------------------------------------
 }
}

#endif
#endif
#endif
