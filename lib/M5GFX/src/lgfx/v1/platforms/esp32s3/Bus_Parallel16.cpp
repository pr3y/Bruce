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

#include "Bus_Parallel16.hpp"
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

  void Bus_Parallel16::config(const config_t& cfg)
  {
    _cfg = cfg;
    auto port = cfg.port;
    _dev = getDev(port);
  }

  bool Bus_Parallel16::init(void)
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
      bus_config.data_gpio_nums[i  ] = _cfg.pin_data[i+8];
      bus_config.data_gpio_nums[i+8] = _cfg.pin_data[i  ];
    }
    bus_config.bus_width = 16;
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
      ESP_LOGE("Bus_Parallel16", "DMA channel not found...");
    }

    for (size_t i = 0; i < 16; ++i)
    {
      gpio_ll_input_enable(&GPIO, (gpio_num_t)_cfg.pin_data[i]);
    }

    uint32_t div_a, div_b, div_n, clkcnt;
    calcClockDiv(&div_a, &div_b, &div_n, &clkcnt, 240*1000*1000, std::min<uint32_t>(_cfg.freq_write, 50000000u));
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

  void Bus_Parallel16::_init_pin(bool read)
  {
    int8_t* pins = _cfg.pin_data;
    if (read)
    {
      for (size_t i = 0; i < 16; ++i)
      {
        gpio_ll_output_disable(&GPIO, (gpio_num_t)pins[i]);
      }
    }
    else
    {
      auto idx_base = LCD_DATA_OUT0_IDX;
      for (size_t i = 0; i < 8; ++i)
      {
        gpio_matrix_out(pins[i]  , idx_base + i+8, 0, 0);
        gpio_matrix_out(pins[i+8], idx_base + i  , 0, 0);
      }
    }
  }

  void Bus_Parallel16::release(void)
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

  void Bus_Parallel16::beginTransaction(void)
  {
    auto dev = _dev;
    dev->lcd_clock.val = _clock_reg_value;

    dev->lcd_misc.val = LCD_CAM_LCD_CD_IDLE_EDGE;
    // dev->lcd_misc.lcd_cd_idle_edge = 1;
    // dev->lcd_misc.lcd_cd_cmd_set = 0;
    // dev->lcd_misc.lcd_cd_dummy_set = 0;
    // dev->lcd_misc.lcd_cd_data_set = 0;

    dev->lcd_user.val = 0;
    // dev->lcd_user.lcd_byte_order = false;
    // dev->lcd_user.lcd_bit_order = false;
    // dev->lcd_user.lcd_8bits_order = false;

    dev->lcd_user.val = LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE;

    _cache_flip = _cache[0];
  }

  void Bus_Parallel16::endTransaction(void)
  {
    if (_has_align_data) { _send_align_data(); }
    auto dev = _dev;
    while (dev->lcd_user.val & LCD_CAM_LCD_START) {}
  }

  void Bus_Parallel16::wait(void)
  {
    if (_has_align_data) { _send_align_data(); }
    auto dev = _dev;
    while (dev->lcd_user.val & LCD_CAM_LCD_START) {}
  }

  bool Bus_Parallel16::busy(void) const
  {
    auto dev = _dev;
    return (dev->lcd_user.val & LCD_CAM_LCD_START);
  }

  void Bus_Parallel16::_send_align_data(void)
  {
    _has_align_data = false;
    auto dev = _dev;
    dev->lcd_cmd_val.lcd_cmd_value = _align_data;
    auto reg_lcd_user = &(dev->lcd_user.val);
    while (*reg_lcd_user & LCD_CAM_LCD_START) {}
    *reg_lcd_user = LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
  }

  bool Bus_Parallel16::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
    if (_has_align_data) { _send_align_data(); }

    auto dev = _dev;
    auto reg_lcd_user = &(dev->lcd_user.val);

    dev->lcd_misc.val = LCD_CAM_LCD_CD_IDLE_EDGE | LCD_CAM_LCD_CD_CMD_SET;

    dev->lcd_cmd_val.val = data;

    if (bit_length <= 16)
    {
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
      return true;
    }

    while (*reg_lcd_user & LCD_CAM_LCD_START) {}
    *reg_lcd_user = LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
    return true;
  }

  void Bus_Parallel16::writeData(uint32_t data, uint_fast8_t bit_length)
  {
    auto bytes = bit_length >> 3;
    auto dev = _dev;
    auto reg_lcd_user = &(dev->lcd_user.val);
    dev->lcd_misc.val = LCD_CAM_LCD_CD_IDLE_EDGE;
    if (_has_align_data)
    {
      _has_align_data = false;
      dev->lcd_cmd_val.val = _align_data | (data << 8);
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
      if (--bytes == 0) { return; }
      data >>= 8;
    }

    if (bytes > 1)
    {
      dev->lcd_cmd_val.val = data;
      if (bytes == 4)
      {
        while (*reg_lcd_user & LCD_CAM_LCD_START) {}
        *reg_lcd_user = LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
        return;
      }
      while (*reg_lcd_user & LCD_CAM_LCD_START) {}
      *reg_lcd_user = LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
      if (bytes == 2) { return; }
      data >>= 16;
    }
    _has_align_data = true;
    _align_data = data;
  }

  void Bus_Parallel16::writeDataRepeat(uint32_t color_raw, uint_fast8_t bit_length, uint32_t count)
  {
// do
// {
//   writeData(color_raw, bit_length);
// } while (--count);
// return;

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
    uint32_t len;
    do
    {
      len = length;
      if (length > 252u)
      {
        len = ((((length - 1) % 252) + 12) / 12) * 12;
      }
      _cache_flip = _cache[(dst == _cache[0])];
      writeBytes((const uint8_t*)dst, len, true, true);
    } while (0 != (length -= len));
  }

  void Bus_Parallel16::writePixels(pixelcopy_t* param, uint32_t length)
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

  void Bus_Parallel16::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    auto dev = _dev;
    auto reg_lcd_user = &(dev->lcd_user.val);
    if ((length + _has_align_data) > 1)
    {
      do
      {
        if (length <= 4)
        {
          if (dc)
          {
            writeData(*(const uint32_t*)data, length << 3);
          }
          else
          {
            writeCommand(*(const uint32_t*)data, length << 3);
          }
          return;
        }

        if (use_dma && !_has_align_data)
        {
          size_t len = length & ~1u;
          while (*reg_lcd_user & LCD_CAM_LCD_START) {}
          _setup_dma_desc_links(&data[4], len - 4);
          *_dma_out_link_reg = DMA_OUTLINK_START_CH0 | ((uintptr_t)(&_dmadesc[0]) & 0xFFFFF);
          dev->lcd_cmd_val.val = data[0] | data[1] << 8 | data[2] <<16 | data[3] << 24;
          length -= len;
          data += len;
        }
        else
        {
          size_t len = (length > CACHE_SIZE)
                     ? (((length - 1) % (CACHE_SIZE-_has_align_data)) + 4) & ~3u
                     : length;
          if (_has_align_data != (bool)(len & 1))
          {
            if (++len > length) { len -= 2; }
          }
          length -= len;
          auto c = (uint8_t*)_cache_flip;
          while (*reg_lcd_user & LCD_CAM_LCD_START) {}
          memcpy(&c[_has_align_data], data, (len + 3) & ~3u);
          data += len;
          if (_has_align_data)
          {
            _has_align_data = false;
            c[0] = _align_data;
            ++len;
          }
          _setup_dma_desc_links(&c[4], len - 4);
          *_dma_out_link_reg = DMA_OUTLINK_START_CH0 | ((uintptr_t)(&_dmadesc[0]) & 0xFFFFF);
          _cache_flip = _cache[(_cache_flip == _cache[0])];
          dev->lcd_cmd_val.val = c[0] | c[1] << 8 | c[2] <<16 | c[3] << 24;
        }
        dev->lcd_misc.val = dc
                          ?  LCD_CAM_LCD_CD_IDLE_EDGE
                          : (LCD_CAM_LCD_CD_IDLE_EDGE | LCD_CAM_LCD_CD_CMD_SET | LCD_CAM_LCD_CD_DATA_SET);

        while (*_dma_outstatus_reg & DMA_OUTFIFO_EMPTY_CH0 ) {}

        *reg_lcd_user = LCD_CAM_LCD_ALWAYS_OUT_EN | LCD_CAM_LCD_2BYTE_EN | LCD_CAM_LCD_CMD_2_CYCLE_EN | LCD_CAM_LCD_DOUT | LCD_CAM_LCD_CMD | LCD_CAM_LCD_UPDATE | LCD_CAM_LCD_START;
      } while (length & ~1u);
    }
    if (length)
    {
      _has_align_data = true;
      _align_data = data[0];
    }
  }

  void Bus_Parallel16::beginRead(void)
  {
    if (_has_align_data) { _send_align_data(); }
    wait();
    _init_pin(true);
    gpio_matrix_out(_cfg.pin_rs, 0x100, 0, 0);
    gpio_lo(_cfg.pin_rd);
  }

  void Bus_Parallel16::endRead(void)
  {
    gpio_hi(_cfg.pin_rd);
    _init_pin();
    gpio_matrix_out(_cfg.pin_rs, LCD_DC_IDX, 0, 0);
  }

  void Bus_Parallel16::_read_bytes(uint8_t* dst, uint32_t length)
  {
    union
    {
      uint32_t in32[2];
      uint8_t in[8];
    };

    uint32_t mh = (((((((((((((((
                  (_cfg.pin_d8  & 7)) << 3)
                + (_cfg.pin_d9  & 7)) << 3)
                + (_cfg.pin_d10 & 7)) << 3)
                + (_cfg.pin_d11 & 7)) << 3)
                + (_cfg.pin_d12 & 7)) << 3)
                + (_cfg.pin_d13 & 7)) << 3)
                + (_cfg.pin_d14 & 7)) << 3)
                + (_cfg.pin_d15 & 7))
                ;

    uint32_t ih = (((((((((((((((
                  (_cfg.pin_d8  >> 3)) << 3)
                + (_cfg.pin_d9  >> 3)) << 3)
                + (_cfg.pin_d10 >> 3)) << 3)
                + (_cfg.pin_d11 >> 3)) << 3)
                + (_cfg.pin_d12 >> 3)) << 3)
                + (_cfg.pin_d13 >> 3)) << 3)
                + (_cfg.pin_d14 >> 3)) << 3)
                + (_cfg.pin_d15 >> 3))
                ;

    uint32_t ml = (((((((((((((((
                  (_cfg.pin_d0 & 7)) << 3)
                + (_cfg.pin_d1 & 7)) << 3)
                + (_cfg.pin_d2 & 7)) << 3)
                + (_cfg.pin_d3 & 7)) << 3)
                + (_cfg.pin_d4 & 7)) << 3)
                + (_cfg.pin_d5 & 7)) << 3)
                + (_cfg.pin_d6 & 7)) << 3)
                + (_cfg.pin_d7 & 7))
                ;

    uint32_t il = (((((((((((((((
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
    uint32_t val;
    do
    {
      in32[1] = GPIO.in1.val;
      in32[0] = GPIO.in;
      *reg_rd_h = mask_rd;
      val =              (1 & (in[(ih >>  0) & 7] >> ((mh >>  0) & 7)));
      val = (val << 1) + (1 & (in[(ih >>  3) & 7] >> ((mh >>  3) & 7)));
      val = (val << 1) + (1 & (in[(ih >>  6) & 7] >> ((mh >>  6) & 7)));
      val = (val << 1) + (1 & (in[(ih >>  9) & 7] >> ((mh >>  9) & 7)));
      val = (val << 1) + (1 & (in[(ih >> 12) & 7] >> ((mh >> 12) & 7)));
      val = (val << 1) + (1 & (in[(ih >> 15) & 7] >> ((mh >> 15) & 7)));
      val = (val << 1) + (1 & (in[(ih >> 18) & 7] >> ((mh >> 18) & 7)));
      val = (val << 1) + (1 & (in[(ih >> 21) & 7] >> ((mh >> 21) & 7)));
      *dst++ = val;
      *reg_rd_l = mask_rd;

      if (0 == --length) { *reg_rd_l = mask_rd; break; }

      val =              (1 & (in[(il >>  0) & 7] >> ((ml >>  0) & 7)));
      val = (val << 1) + (1 & (in[(il >>  3) & 7] >> ((ml >>  3) & 7)));
      val = (val << 1) + (1 & (in[(il >>  6) & 7] >> ((ml >>  6) & 7)));
      val = (val << 1) + (1 & (in[(il >>  9) & 7] >> ((ml >>  9) & 7)));
      val = (val << 1) + (1 & (in[(il >> 12) & 7] >> ((ml >> 12) & 7)));
      val = (val << 1) + (1 & (in[(il >> 15) & 7] >> ((ml >> 15) & 7)));
      val = (val << 1) + (1 & (in[(il >> 18) & 7] >> ((ml >> 18) & 7)));
      val = (val << 1) + (1 & (in[(il >> 21) & 7] >> ((ml >> 21) & 7)));
      *dst++ = val;
    } while (--length);
  }

  uint32_t Bus_Parallel16::readData(uint_fast8_t bit_length)
  {
    union {
      uint32_t res;
      uint8_t raw[4];
    };
    _read_bytes(raw, (bit_length + 7) >> 3);
    return res;
  }

  bool Bus_Parallel16::readBytes(uint8_t* dst, uint32_t length, bool use_dma)
  {
    _read_bytes(dst, length);
    return true;
  }

  void Bus_Parallel16::readPixels(void* dst, pixelcopy_t* param, uint32_t length)
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

  void Bus_Parallel16::_alloc_dmadesc(size_t len)
  {
    if (_dmadesc) heap_caps_free(_dmadesc);
    _dmadesc_size = len;
    _dmadesc = (dma_descriptor_t*)heap_caps_malloc(sizeof(dma_descriptor_t) * len, MALLOC_CAP_DMA);
  }

  void Bus_Parallel16::_setup_dma_desc_links(const uint8_t *data, int32_t len)
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
