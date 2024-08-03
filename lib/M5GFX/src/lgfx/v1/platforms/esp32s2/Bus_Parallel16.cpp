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
#if defined (CONFIG_IDF_TARGET_ESP32S2)

#include "Bus_Parallel16.hpp"
#include "../../misc/pixelcopy.hpp"

#include <soc/dport_reg.h>
#include <rom/gpio.h>
#include <hal/gpio_ll.h>
#include <esp_log.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr size_t CACHE_THRESH = 128;

  // static constexpr uint32_t _conf_reg_default = I2S_TX_MSB_RIGHT | I2S_TX_RIGHT_FIRST | I2S_RX_RIGHT_FIRST | I2S_TX_DMA_EQUAL;
  static constexpr uint32_t _conf_reg_default = I2S_TX_RIGHT_FIRST | I2S_RX_RIGHT_FIRST | I2S_TX_DMA_EQUAL;
  static constexpr uint32_t _conf_reg_start   = _conf_reg_default | I2S_TX_START;
  static constexpr uint32_t _conf_reg_reset   = _conf_reg_default | I2S_TX_RESET;
  static constexpr uint32_t _sample_rate_conf_reg_bufferd = 32 << I2S_TX_BITS_MOD_S | 32 << I2S_RX_BITS_MOD_S | 1 << I2S_TX_BCK_DIV_NUM_S | 1 << I2S_RX_BCK_DIV_NUM_S;
  static constexpr uint32_t _sample_rate_conf_reg_direct = 16 << I2S_TX_BITS_MOD_S | 16 << I2S_RX_BITS_MOD_S | 1 << I2S_TX_BCK_DIV_NUM_S | 1 << I2S_RX_BCK_DIV_NUM_S;
  static constexpr uint32_t _fifo_conf_default = 1 << I2S_TX_FIFO_MOD | 1 << I2S_RX_FIFO_MOD | 32 << I2S_TX_DATA_NUM_S | 32 << I2S_RX_DATA_NUM_S | I2S_TX_FIFO_MOD_FORCE_EN;
  static constexpr uint32_t _fifo_conf_dma     = _fifo_conf_default | I2S_DSCR_EN;

  static __attribute__ ((always_inline)) inline volatile uint32_t* reg(uint32_t addr) { return (volatile uint32_t *)ETS_UNCACHED_ADDR(addr); }

  static i2s_dev_t* getDev(i2s_port_t port)
  {
#if defined (CONFIG_IDF_TARGET_ESP32S2)
    return &I2S0;
#else
    return (port == 0) ? &I2S0 : &I2S1;
#endif
  }

  void Bus_Parallel16::config(const config_t& cfg)
  {
    _cfg = cfg;
    auto port = cfg.i2s_port;
    _dev = getDev(port);
    
    _mask_reg_dc = (cfg.pin_rs < 0) ? 0 : (1ul << (cfg.pin_rs & 31));
    _gpio_reg_dc[0] = get_gpio_lo_reg(cfg.pin_rs);
    _gpio_reg_dc[1] = get_gpio_hi_reg(cfg.pin_rs);

    // clock = 80MHz(PLL160 / 2)
    static constexpr uint32_t pll_160M_clock_d2 = 160 * 1000 * 1000 >> 1;

    // I2S_CLKM_DIV_NUM 2=40MHz  /  3=27MHz  /  4=20MHz  /  5=16MHz  /  8=10MHz  /  10=8MHz
    _div_num = std::min<uint32_t>(255u, 1 + ((pll_160M_clock_d2) / (1 + _cfg.freq_write)));

    _clkdiv_write = I2S_CLK_160M_PLL << I2S_CLK_SEL_S
                  |             I2S_CLK_EN
                  |        1 << I2S_CLKM_DIV_A_S
                  |        0 << I2S_CLKM_DIV_B_S
                  | _div_num << I2S_CLKM_DIV_NUM_S
                  ;
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

    auto idx_base = I2S0O_DATA_OUT8_IDX;

    gpio_matrix_out(_cfg.pin_rs , I2S0O_DATA_OUT0_IDX, 0, 0);

    _direct_dc = false;

    uint32_t dport_clk_en;
    uint32_t dport_rst;

    if (_cfg.i2s_port == I2S_NUM_0) {
      idx_base = I2S0O_WS_OUT_IDX;
      dport_clk_en = DPORT_I2S0_CLK_EN;
      dport_rst = DPORT_I2S0_RST;
    }
#if !defined (CONFIG_IDF_TARGET_ESP32S2)
    else
    {
      idx_base = I2S1O_WS_OUT_IDX;
      dport_clk_en = DPORT_I2S1_CLK_EN;
      dport_rst = DPORT_I2S1_RST;
    }
#endif
    gpio_matrix_out(_cfg.pin_wr, idx_base, 1, 0); // WR (Write-strobe in 8080 mode, Active-low)

    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, dport_clk_en);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, dport_rst);

    auto i2s_dev = (i2s_dev_t*)_dev;
    //Reset I2S subsystem
    i2s_dev->conf.val = I2S_TX_RESET | I2S_RX_RESET | I2S_TX_FIFO_RESET | I2S_RX_FIFO_RESET;
    i2s_dev->conf.val = _conf_reg_default;

    i2s_dev->int_ena.val = 0;
    i2s_dev->timing.val = 0;

    //Reset DMA
    i2s_dev->lc_conf.val = I2S_IN_RST | I2S_OUT_RST | I2S_AHBM_RST | I2S_AHBM_FIFO_RST;
    i2s_dev->lc_conf.val = I2S_OUT_EOF_MODE | I2S_OUTDSCR_BURST_EN | I2S_OUT_DATA_BURST_EN;

    i2s_dev->in_link.val = 0;
    i2s_dev->out_link.val = 0;

    i2s_dev->conf1.val = I2S_TX_PCM_BYPASS | I2S_TX_STOP_EN;
    i2s_dev->conf2.val = I2S_LCD_EN;
    i2s_dev->conf_chan.val = 1 << I2S_TX_CHAN_MOD_S | 1 << I2S_RX_CHAN_MOD_S;

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
        // gpio_pad_select_gpio(pins[i]);
        gpio_set_direction((gpio_num_t)pins[i], GPIO_MODE_INPUT);
      }
    }
    else
    {
      for (size_t i = 0; i < 8; ++i)
      {
        // gpio_set_direction((gpio_num_t)pins[i], GPIO_MODE_INPUT_OUTPUT);
        gpio_pad_select_gpio(pins[i  ]);
        gpio_pad_select_gpio(pins[i+8]);
        gpio_matrix_out(pins[i  ], I2S0O_DATA_OUT8_IDX + i+8, 0, 0);
        gpio_matrix_out(pins[i+8], I2S0O_DATA_OUT8_IDX + i  , 0, 0);
      }
    }
  }

  void Bus_Parallel16::release(void)
  {
    if (_dmadesc)
    {
      heap_caps_free(_dmadesc);
      _dmadesc = nullptr;
      _dmadesc_size = 0;
    }
  }

  void Bus_Parallel16::beginTransaction(void)
  {
    auto i2s_dev = (i2s_dev_t*)_dev;
    i2s_dev->clkm_conf.val = _clkdiv_write;
    i2s_dev->out_link.val = 0;
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_bufferd;
    i2s_dev->fifo_conf.val = _fifo_conf_dma;

    _cache_index = 0;
    _cache_flip = _cache[0];

    _has_align_data = false;
  }

  void Bus_Parallel16::endTransaction(void)
  {
    if (_cache_index)
    {
      _flush(_cache_index, true);
      _cache_index = 0;
      delayMicroseconds(1);
    }
    _wait();
  }

  void Bus_Parallel16::_wait(void)
  {
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
  }

  void Bus_Parallel16::wait(void)
  {
    if (_cache_index)
    {
      _flush(_cache_index, true);
      _cache_index = 0;
      delayMicroseconds(1);
    }
    _wait();
  }

  bool Bus_Parallel16::busy(void) const
  {
    auto i2s_dev = (i2s_dev_t*)_dev;
#if defined (CONFIG_IDF_TARGET_ESP32S2)
    return !i2s_dev->state.tx_idle
         || (i2s_dev->out_link.val && !(i2s_dev->lc_state0.out_empty)); // I2S_OUT_EMPTY
#else
    return !i2s_dev->state.tx_idle
         || (i2s_dev->out_link.val && !(i2s_dev->lc_state0 & 0x80000000)); // I2S_OUT_EMPTY
#endif
  }

  void Bus_Parallel16::flush(void)
  {
    if (_cache_index)
    {
      _flush(_cache_index, true);
      _cache_index = 0;
    }
  }

  size_t Bus_Parallel16::_flush(size_t count, bool dc)
  {
    bool slow = _div_num > 8;

    auto i2s_dev = (i2s_dev_t*)_dev;
    if (i2s_dev->out_link.val)
    {
#if defined (CONFIG_IDF_TARGET_ESP32S2)
      while (!(i2s_dev->lc_state0.out_empty)) {}
#else
      while (!(i2s_dev->lc_state0 & 0x80000000)) {} // I2S_OUT_EMPTY
#endif
    }
    _dmadesc[0].buf = (uint8_t *)_cache_flip;
    *(uint32_t*)&_dmadesc[0] = count << 2 | count << 14 | 0xC0000000;
    _dmadesc[0].empty = 0;

    while (!i2s_dev->state.tx_idle) {}

    i2s_dev->conf.val = _conf_reg_reset;
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_bufferd;
    i2s_dev->out_link.val = I2S_OUTLINK_START | ((uint32_t)&_dmadesc[0] & I2S_OUTLINK_ADDR);

    int wait = 40 - (_div_num << 3);
    if (_direct_dc)
    {
      _direct_dc = false;
      gpio_matrix_out(_cfg.pin_rs, I2S0O_DATA_OUT0_IDX, 0, 0);
      wait -= 16;
    }
    if (wait > 0)
    { /// OUTLINK_START～TX_STARTの時間が短すぎるとデータの先頭を送り損じる事があるのでnopウェイトを入れる;
      do { __asm__ __volatile__ ("nop"); } while (--wait);
    }
    i2s_dev->conf.val = _conf_reg_start;
    if (slow)
    {
      wait = _div_num >> 1;
      do { __asm__ __volatile__ ("nop"); } while (--wait);
    }

    _cache_flip = (_cache_flip == _cache[0]) ? _cache[1] : _cache[0];
    return 0;
  }

  bool Bus_Parallel16::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
    auto idx = _cache_index;
    int bytes = bit_length >> 4;
    auto c = _cache_flip;

    if (_has_align_data)
    {
      _has_align_data = false;
      c[idx++] = (_align_data << 16) | 0x100;
    }

    do
    {
      c[idx++] = data << 16;
      data >>= 16;
    } while (0 < --bytes);
    if (idx >= CACHE_THRESH)
    {
      _flush(idx);
      idx = 0;
    }
    _cache_index = idx;
    return true;
  }

  void Bus_Parallel16::writeData(uint32_t data, uint_fast8_t bit_length)
  {
    auto idx = _cache_index;
    auto c = _cache_flip;
    auto bytes = bit_length >> 3;

    if (_has_align_data)
    {
      _has_align_data = false;
      c[idx++] = ((data << 8 | _align_data) << 16) | 0x100;
      --bytes;
      data >>= 8;
    }

    while (1 < bytes)
    {
      bytes -= 2;
      c[idx++] = (data << 16) | 0x100;
      data >>= 16;
    }

    if (idx >= CACHE_THRESH)
    {
      _flush(idx);
      idx = 0;
    }
    _cache_index = idx;
    if (bytes == 1)
    {
      _has_align_data = true;
      _align_data = data;
    }
  }

  void Bus_Parallel16::writeDataRepeat(uint32_t color_raw, uint_fast8_t bit_length, uint32_t length)
  {
    size_t bytes = bit_length >> 3;
    auto idx = _cache_index;
    auto c = _cache_flip;

    if (bytes == 2)
    {
      int step = (idx == 0) ? 1 : 0;
      color_raw = color_raw << 16 | 0x100;
      while (length)
      {
        --length;
        *(uint32_t*)(&c[idx]) = color_raw;
        if (++idx >= CACHE_THRESH)
        {
          _flush(idx);
          if (++step == 2 && length >= CACHE_THRESH)
          {
            memcpy(_cache_flip, c, CACHE_THRESH * sizeof(uint32_t));
            do
            {
              _flush(CACHE_THRESH);
              length -= CACHE_THRESH;
            } while (length >= CACHE_THRESH);
            idx = length;
            break;
          }
          c = _cache_flip;
          idx = 0;
        }
      }
    }
    else // if (bytes == 3)
    {
      // size_t step = 0;
      uint32_t raw[3] = 
      { color_raw             << 16 | 0x100
      , color_raw | color_raw << 24 | 0x100
      , color_raw              << 8 | 0x100
      };
      if (_has_align_data)
      {
        _has_align_data = false;
        --length;
        c[idx++] = ((color_raw << 8 | _align_data) << 16) | 0x100;
        c[idx++] = raw[2];
        if (idx >= CACHE_THRESH)
        {
          _flush(idx);
          idx = 0;
          c = _cache_flip;
          // step = 1;
        }
      }
      while (1 < length)
      {
        length -= 2;
        c[idx  ] = raw[0];
        c[idx+1] = raw[1];
        c[idx+2] = raw[2];
        idx += 3;
        if (idx >= CACHE_THRESH)
        {
          _flush(idx);
          idx = 0;
/*
          if (++step == 2)
          {
            if (length > ((CACHE_THRESH / 3) * 2))
            {
              memcpy(_cache_flip, c, CACHE_THRESH * sizeof(uint32_t));
              do
              {
                _flush(((CACHE_THRESH / 3) * 3));
                length -= ((CACHE_THRESH / 3) * 2);
              } while (length >= ((CACHE_THRESH / 3) * 2));
              idx = (length >> 1) * 3;
              length = length & 1;
              break;
            }
          }
//*/
          c = _cache_flip;
        }
      }
      if (length == 1)
      {
        _has_align_data = true;
        c[idx++] = raw[0];
        _align_data = raw[1] >> 16;
      }
    }
    _cache_index = idx;
  }

  void Bus_Parallel16::writePixels(pixelcopy_t* param, uint32_t length)
  {
    if (_cache_index)
    {
      _flush(_cache_index);
      _cache_index = 0;
    }
    const uint32_t bytes = param->dst_bits >> 3;
    auto fp_copy = param->fp_copy;

    const uint32_t limit = (CACHE_THRESH * sizeof(_cache[0][0])) / bytes;
    uint8_t len = length % limit;

    auto c = (uint8_t*)_cache_flip;
    if (len)
    {
      fp_copy(c, 0, len, param);
      _cache_flip = (_cache_flip == _cache[0]) ? _cache[1] : _cache[0];
      writeBytes(c, len * bytes, true, true);
      if (0 == (length -= len)) return;
    }
    do
    {
      c = (uint8_t*)_cache_flip;
      fp_copy(c, 0, limit, param);
      _cache_flip = (_cache_flip == _cache[0]) ? _cache[1] : _cache[0];
      writeBytes(c, limit * bytes, true, true);

    } while (length -= limit);
  }

  void Bus_Parallel16::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    if (_cache_index)
    {
      _flush(_cache_index);
      _cache_index = 0;
    }

    auto c = (uint8_t*)_cache_flip;
    auto i2s_dev = (i2s_dev_t*)_dev;

    if ((length + _has_align_data) > 1)
    {
      do
      {
        dc_control(dc);
        i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_direct;
        i2s_dev->conf.val = _conf_reg_reset;
        if (use_dma && !_has_align_data)
        {
          auto len = length & ~1u;
          _setup_dma_desc_links(data, len);
          length -= len;
          data += len;
        }
        else
        {
          size_t len = ((length - 1) % (CACHE_THRESH * sizeof(_cache[0][0]))) + 1;
          if (_has_align_data != (bool)(len & 1))
          {
            if (++len > length) { len -= 2; }
          }
          memcpy(&c[_has_align_data], data, (len + 3) & ~3u);
          length -= len;
          data += len;
          if (_has_align_data)
          {
            _has_align_data = false;
            c[0] = _align_data;
            ++len;
          }
          _setup_dma_desc_links((const uint8_t*)_cache_flip, len);

          _cache_flip = (_cache_flip == _cache[0]) ? _cache[1] : _cache[0];
          c = (uint8_t*)_cache_flip;
        }
        i2s_dev->out_link.val = I2S_OUTLINK_START | ((uint32_t)_dmadesc & I2S_OUTLINK_ADDR);
        int wait = 40 - (_div_num << 3);
        if (!_direct_dc)
        {
          _direct_dc = true;
          gpio_matrix_out(_cfg.pin_rs, 0x100, 0, 0);
          wait -= 16;
        }
        if (wait > 0)
        { /// OUTLINK_START～TX_STARTの時間が短すぎるとデータの先頭を送り損じる事があるのでnopウェイトを入れる;
          do { __asm__ __volatile__ ("nop"); } while (--wait);
        }
        i2s_dev->conf.val = _conf_reg_start;
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
    if (_cache_index) { _cache_index = _flush(_cache_index, true); }

    dc_control(true);
    if (!_direct_dc)
    {
      _direct_dc = true;
      gpio_matrix_out(_cfg.pin_rs, 0x100, 0, 0);
    }
    gpio_lo(_cfg.pin_rd);

    _init_pin(true);
  }

  void Bus_Parallel16::endRead(void)
  {
    gpio_hi(_cfg.pin_rd);

    _init_pin();
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

      if (0 == --length) { *reg_rd_l = mask_rd; break; }

      val =              (1 & (in[(il >>  0) & 7] >> ((ml >>  0) & 7)));
      val = (val << 1) + (1 & (in[(il >>  3) & 7] >> ((ml >>  3) & 7)));
      val = (val << 1) + (1 & (in[(il >>  6) & 7] >> ((ml >>  6) & 7)));
      val = (val << 1) + (1 & (in[(il >>  9) & 7] >> ((ml >>  9) & 7)));
      val = (val << 1) + (1 & (in[(il >> 12) & 7] >> ((ml >> 12) & 7)));
      val = (val << 1) + (1 & (in[(il >> 15) & 7] >> ((ml >> 15) & 7)));
      val = (val << 1) + (1 & (in[(il >> 18) & 7] >> ((ml >> 18) & 7)));
      val = (val << 1) + (1 & (in[(il >> 21) & 7] >> ((ml >> 21) & 7)));
      *reg_rd_l = mask_rd;
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
    _dmadesc = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t) * len, MALLOC_CAP_DMA);
  }

  void Bus_Parallel16::_setup_dma_desc_links(const uint8_t *data, int32_t len)
  {
    static constexpr size_t MAX_DMA_LEN = (4096-4);

    if (_dmadesc_size * MAX_DMA_LEN < len)
    {
      _alloc_dmadesc(len / MAX_DMA_LEN + 1);
    }
    lldesc_t *dmadesc = _dmadesc;

    while (len > MAX_DMA_LEN)
    {
      len -= MAX_DMA_LEN;
      dmadesc->buf = (uint8_t *)data;
      data += MAX_DMA_LEN;
      *(uint32_t*)dmadesc = MAX_DMA_LEN | MAX_DMA_LEN << 12 | 0x80000000;
      dmadesc->qe.stqe_next = dmadesc + 1;
      dmadesc++;
    }
    *(uint32_t*)dmadesc = ((len + 3) & ( ~3 )) | len << 12 | 0xC0000000;
    dmadesc->buf = (uint8_t *)data;
    dmadesc->qe.stqe_next = nullptr;
  }

//----------------------------------------------------------------------------
 }
}

#endif
#endif
