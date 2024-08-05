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
#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)

#include "Bus_Parallel8.hpp"
#include "../../misc/pixelcopy.hpp"

#include <soc/dport_reg.h>
#include <soc/i2s_struct.h>
#include <rom/gpio.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

#ifndef I2S_CLKA_ENA
#define I2S_CLKA_ENA  (BIT(21))
#endif

  // #define SAFE_I2S_FIFO_WR_REG(i) (0x6000F000 + ((i)*0x1E000))
  // #define SAFE_I2S_FIFO_RD_REG(i) (0x6000F004 + ((i)*0x1E000))
  #define SAFE_I2S_FIFO_WR_REG(i) (0x3FF4F000 + ((i)*0x1E000))
  #define SAFE_I2S_FIFO_RD_REG(i) (0x3FF4F004 + ((i)*0x1E000))

  static constexpr size_t CACHE_THRESH = 128;

  static constexpr uint32_t _conf_reg_default = I2S_TX_MSB_RIGHT | I2S_TX_RIGHT_FIRST | I2S_RX_RIGHT_FIRST;
  static constexpr uint32_t _conf_reg_start   = _conf_reg_default | I2S_TX_START;
  static constexpr uint32_t _conf_reg_reset   = _conf_reg_default | I2S_TX_RESET;
  static constexpr uint32_t _sample_rate_conf_reg_32bit = 32 << I2S_TX_BITS_MOD_S | 32 << I2S_RX_BITS_MOD_S | 1 << I2S_TX_BCK_DIV_NUM_S | 1 << I2S_RX_BCK_DIV_NUM_S;
  static constexpr uint32_t _sample_rate_conf_reg_16bit = 16 << I2S_TX_BITS_MOD_S | 16 << I2S_RX_BITS_MOD_S | 1 << I2S_TX_BCK_DIV_NUM_S | 1 << I2S_RX_BCK_DIV_NUM_S;
  static constexpr uint32_t _fifo_conf_default = 1 << I2S_TX_FIFO_MOD | 1 << I2S_RX_FIFO_MOD | 32 << I2S_TX_DATA_NUM_S | 32 << I2S_RX_DATA_NUM_S;
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

  void Bus_Parallel8::config(const config_t& cfg)
  {
    _cfg = cfg;
    auto port = cfg.i2s_port;
    _dev = getDev(port);

    _i2s_fifo_wr_reg = reg(SAFE_I2S_FIFO_WR_REG(port));
    
    // clock = 80MHz(PLL_D2_CLK)
    static constexpr uint32_t pll_d2_clock = 80 * 1000 * 1000;

    // I2S_CLKM_DIV_NUM 4=20MHz  /  5=16MHz  /  8=10MHz  /  10=8MHz
    _div_num = std::min<uint32_t>(255u, std::max<uint32_t>(3u, 1 + (pll_d2_clock / (1 + _cfg.freq_write))));

    _clkdiv_write =             I2S_CLK_EN
                  |        1 << I2S_CLKM_DIV_A_S
                  |        0 << I2S_CLKM_DIV_B_S
                  | _div_num << I2S_CLKM_DIV_NUM_S
                  ;
  }

  bool Bus_Parallel8::init(void)
  {
    _init_pin();

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

    memset(&_dmadesc, 0, sizeof(lldesc_t));

    return true;
  }

  void Bus_Parallel8::_init_pin(void)
  {
#if defined (CONFIG_IDF_TARGET_ESP32S2)
    auto idx_base = I2S0O_DATA_OUT8_IDX;
#else
    auto idx_base = (_cfg.i2s_port == I2S_NUM_0) ? I2S0O_DATA_OUT8_IDX : I2S1O_DATA_OUT8_IDX;
#endif
    for (size_t i = 0; i < 8; ++i)
    {
      int32_t pin = _cfg.pin_data[i];
      if (pin < 0) { continue; }
      gpio_pad_select_gpio(pin);
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT_OUTPUT);
      gpio_matrix_out(pin, idx_base + i, 0, 0);
    }

    for (size_t i = 0; i < 3; ++i)
    {
      int32_t pin = _cfg.pin_ctrl[i];
      if (pin < 0) { continue; }
      gpio_pad_select_gpio(pin);
      gpio_hi(pin);
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
    }

    gpio_matrix_out(_cfg.pin_rs, idx_base + 8, 0, 0);

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
  }

  void Bus_Parallel8::release(void)
  {  }

  void Bus_Parallel8::beginTransaction(void)
  {
    auto i2s_dev = (i2s_dev_t*)_dev;
    i2s_dev->clkm_conf.val = _clkdiv_write;
    i2s_dev->out_link.val = 0;
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_16bit;
    i2s_dev->fifo_conf.val = _fifo_conf_dma;

    _cache_index = 0;
    _cache_flip = _cache[0];
  }

  void Bus_Parallel8::endTransaction(void)
  {
    if (_cache_index)
    {
      _cache_index = _flush(_cache_index, true);
    }
    _wait();
  }

  void Bus_Parallel8::_wait(void)
  {
    //i2s_dev->int_clr.val = I2S_TX_REMPTY_INT_CLR;
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

  void Bus_Parallel8::wait(void)
  {
    if (_cache_index)
    {
      _cache_index = _flush(_cache_index, true);
    }
    _wait();
  }

  bool Bus_Parallel8::busy(void) const
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

  void Bus_Parallel8::flush(void)
  {
    if (_cache_index)
    {
      _cache_index = _flush(_cache_index, true);
    }
  }

  // /// WiFi,BT使用状況確認
  // static bool checkWireless(void)
  // {
  //   return *reg(DPORT_WIFI_CLK_EN_REG) & 0x7FF;
  // }
//*
// use DMA
  size_t Bus_Parallel8::_flush(size_t count, bool force)
  {
    bool slow = _div_num > 8;

    size_t idx_e = count & ~1;
    auto i2s_dev = (i2s_dev_t*)_dev;

    if (idx_e)
    {
      if (i2s_dev->out_link.val)
      {
#if defined (CONFIG_IDF_TARGET_ESP32S2)
        while (!(i2s_dev->lc_state0.out_empty)) {}
#else
        while (!(i2s_dev->lc_state0 & 0x80000000)) {} // I2S_OUT_EMPTY
#endif
        i2s_dev->out_link.val = 0;
      }
      _dmadesc.buf = (uint8_t*)_cache_flip;
      *(uint32_t*)&_dmadesc = idx_e << 1 | idx_e << 13 | 0xC0000000;
      while (!i2s_dev->state.tx_idle) {}
      i2s_dev->conf.val = _conf_reg_reset | I2S_TX_FIFO_RESET;
      i2s_dev->out_link.val = I2S_OUTLINK_START | ((uint32_t)&_dmadesc & I2S_OUTLINK_ADDR);
      while (!i2s_dev->state.tx_fifo_reset_back) {}
      auto cache_old = _cache_flip;
      _cache_flip = (cache_old == _cache[0]) ? _cache[1] : _cache[0];
      i2s_dev->int_clr.val = ~0u;

// DMAの準備待ちウェイト …無線使用中はウェイトを増やす
//    size_t wait = (16 << checkWireless()) + (_div_num >> 2);
      size_t wait = 16 + (_div_num >> 2);
      do { __asm__ __volatile__ ("nop"); } while (--wait);
      i2s_dev->conf.val = _conf_reg_start;
      if (slow)
      {
        wait = _div_num >> 1;
        do { __asm__ __volatile__ ("nop"); } while (--wait);
      }
      // if (slow) while (i2s_dev->state.tx_idle) {}
      count -= idx_e;
      if (!count) return 0;
      
      // 送り残しがあれば次回分のキャッシュに移しておく;
      *(uint32_t*)_cache_flip = *(uint32_t*)(&cache_old[idx_e]);
    }

    if (!force)
    {
      return count;
    }

    // ここから DMAで送信しきれなかった端数ぶんの送信処理
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
    i2s_dev->conf.val = _conf_reg_reset;
    i2s_dev->fifo_conf.val = _fifo_conf_default;
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_32bit;
    size_t idx = 0;
    do
    {
      *_i2s_fifo_wr_reg = _cache_flip[idx ^ 1] << 16;
    } while (++idx != count);

    size_t wait = 16 + _div_num;
    do { __asm__ __volatile__ ("nop"); } while (--wait);
    i2s_dev->conf.val = _conf_reg_start;
    if (slow)
    {
      wait = _div_num >> 1;
      do { __asm__ __volatile__ ("nop"); } while (--wait);
    }
    while (!i2s_dev->state.tx_idle) {}
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_16bit;
    i2s_dev->fifo_conf.val = _fifo_conf_dma;

    return 0;
  }
/*/
// use FIFO
  size_t Bus_Parallel8::_flush(size_t count, bool force)
  {
    bool slow = _div_num > 8;
    while (!i2s_dev->int_raw.tx_rempty || (slow && !i2s_dev->state.tx_idle))
    {}
    i2s_dev->conf.val = _conf_reg_reset;

    size_t idx_e = std::min(CACHE_THRESH, count & ~3);

    auto cache = _cache_flip;

    if (idx_e)
    {
      size_t idx = 0;
      auto c = (uint32_t*)cache;
      do
      {
        *_i2s_fifo_wr_reg = c[idx>>1];
        *_i2s_fifo_wr_reg = c[(idx>>1)+1];
      } while ((idx += 4) != idx_e);
      if (slow) ets_delay_us(_div_num >> 6);
      i2s_dev->conf.val = _conf_reg_start;
      i2s_dev->int_clr.val = I2S_TX_REMPTY_INT_CLR;

      count -= idx_e;

      if (!count) return 0;

      memmove(cache, &cache[idx_e], (count + 1) << 1);
      // idx = 0;
      // {
      //   cache[idx] = cache[idx_e + idx];
      // } while (idx++ != count);
    }
    if (!force)
    {
      return count;
    }

    size_t idx = 0;
    while (!i2s_dev->int_raw.tx_rempty || (slow && !i2s_dev->state.tx_idle))
    {}
    i2s_dev->conf.val = _conf_reg_reset;

    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_32bit;
    do
    {
      *_i2s_fifo_wr_reg = cache[idx^1] << 16;
    } while (++idx != count);

    if (slow) ets_delay_us(_div_num >> 6);
    i2s_dev->conf.val = _conf_reg_start;
    i2s_dev->int_clr.val = I2S_TX_REMPTY_INT_CLR;
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_16bit;

    return 0;
  }
//*/
  bool Bus_Parallel8::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
    auto idx = _cache_index;
    auto bytes = bit_length >> 3;
    auto c = _cache_flip;
    do
    {
      c[idx++ ^ 1] = data & 0xFF;
      data >>= 8;
    } while (--bytes);
    if (idx >= CACHE_THRESH)
    {
      idx = _flush(idx);
    }
    _cache_index = idx;
    return true;
  }

  void Bus_Parallel8::writeData(uint32_t data, uint_fast8_t bit_length)
  {
    auto idx = _cache_index;
    auto bytes = bit_length >> 3;
    auto c = _cache_flip;
    do
    {
      c[idx++ ^ 1] = data | 0x100;
      data >>= 8;
    } while (--bytes);
    if (idx >= CACHE_THRESH)
    {
      idx = _flush(idx);
    }
    _cache_index = idx;
  }

  void Bus_Parallel8::writeDataRepeat(uint32_t color_raw, uint_fast8_t bit_length, uint32_t length)
  {
    size_t bytes = bit_length >> 3;
    auto idx = _cache_index;
    auto c = _cache_flip;

    if (bytes == 2)
    {
      if (idx & 1)
      {
        color_raw = color_raw << 8 | color_raw | 0x01000100;
        c[idx ^ 1] = color_raw;
      }
      else
      {
        color_raw = color_raw << 16 | color_raw >> 8 | 0x01000100;
      }
      auto cache = (uint32_t*)&c[(idx + 1) & ~1];
      for (;;)
      {
        *cache++ = color_raw;
        idx += 2;
        if (idx >= CACHE_THRESH)
        {
          idx = _flush(idx);
          cache = (uint32_t*)&_cache_flip[(idx + 1) & ~1];
        }
        if (!--length) break;
      }
    }
    else
//*/
    {
      size_t b = 0;
      uint16_t raw[bytes];
      do
      {
        raw[b] = color_raw | 0x100;
        color_raw >>= 8;
      } while (++b != bytes);
      b = 0;
      for (;;)
      {
        c[idx ^ 1] = raw[b];
        ++idx;
        if (++b == bytes)
        {
          b = 0;
          if (idx >= CACHE_THRESH)
          {
            idx = _flush(idx);
            c = _cache_flip;
          }
          if (!--length) break;
        }
      }
    }
    _cache_index = idx;
  }

  void Bus_Parallel8::writePixels(pixelcopy_t* param, uint32_t length)
  {
    uint8_t buf[CACHE_THRESH];
    const uint32_t bytes = param->dst_bits >> 3;
    auto fp_copy = param->fp_copy;
    const uint32_t limit = CACHE_THRESH / bytes;
    uint8_t len = length % limit;
    if (len) {
      fp_copy(buf, 0, len, param);
      writeBytes(buf, len * bytes, true, false);
      if (0 == (length -= len)) return;
    }
    do {
      fp_copy(buf, 0, limit, param);
      writeBytes(buf, limit * bytes, true, false);
    } while (length -= limit);
  }
//*
  void Bus_Parallel8::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    uint32_t dc_data = dc ? 0x01000100 : 0;
    auto idx = _cache_index;
    auto c = _cache_flip;

    for (;;)
    {
      while (length && ((idx & 3) || (length < 4)))
      {
        --length;
        c[idx^1] = *data++ | dc_data;
        ++idx;
      }
      if (CACHE_THRESH <= idx)
      {
        idx = _flush(idx);
        c = _cache_flip;
      }
      if (!length)
      {
        break;
      }
      size_t limit = std::min<uint32_t>(CACHE_THRESH, (length + idx) & ~3);
      length -= (limit - idx);
      do
      {
        *(uint32_t*)(&c[idx  ]) = (data[0] << 16) | data[1] | dc_data;
        *(uint32_t*)(&c[idx+2]) = (data[2] << 16) | data[3] | dc_data;
        data += 4;
      } while ((idx += 4) < limit);
    }
    _cache_index = idx;
  }

  void Bus_Parallel8::beginRead(void)
  {
    if (_cache_index) { _cache_index = _flush(_cache_index, true); }
    _wait();

    gpio_matrix_out(_cfg.pin_rs, 0x100, 0, 0);
    gpio_lo(_cfg.pin_rd);
  }

  void Bus_Parallel8::endRead(void)
  {
    gpio_hi(_cfg.pin_rd);

    auto idx_base = (_cfg.i2s_port == I2S_NUM_0) ? I2S0O_DATA_OUT16_IDX : I2S1O_DATA_OUT16_IDX;
    gpio_matrix_out(_cfg.pin_rs, idx_base, 0, 0);
  }

  void Bus_Parallel8::_read_bytes(uint8_t* __restrict dst, uint32_t length)
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

//----------------------------------------------------------------------------
 }
}

#endif
#endif
