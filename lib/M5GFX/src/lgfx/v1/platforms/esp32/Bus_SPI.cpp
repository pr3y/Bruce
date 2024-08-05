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

#include "Bus_SPI.hpp"

/// ESP32-S3をターゲットにした際にREG_SPI_BASEが定義されていなかったので応急処置 ;
#if defined ( CONFIG_IDF_TARGET_ESP32S3 )
 #if ( ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 0) )
  #define REG_SPI_BASE(i)   (DR_REG_SPI1_BASE + (((i)>1) ? (((i)* 0x1000) + 0x20000) : (((~(i)) & 1)* 0x1000 )))
 #endif
#elif defined ( CONFIG_IDF_TARGET_ESP32 ) || !defined ( CONFIG_IDF_TARGET )
 #define LGFX_SPIDMA_WORKAROUND
#endif

#include "../../misc/pixelcopy.hpp"

#if __has_include (<soc/dport_reg.h>)
  #include <soc/dport_reg.h>
#endif

#include <driver/rtc_io.h>
#include <esp_heap_caps.h>
#include <esp_log.h>

#if __has_include (<esp_private/periph_ctrl.h>)
 #include <esp_private/periph_ctrl.h>
#else
 #include <driver/periph_ctrl.h>
#endif

#if defined (ARDUINO) // Arduino ESP32
 #include <soc/periph_defs.h>
 #include <esp32-hal-cpu.h>
#endif
#include <driver/spi_master.h>

#if defined ESP_IDF_VERSION_MAJOR && ESP_IDF_VERSION_MAJOR >= 5
    #include <rom/gpio.h> // dispatched by core
#elif defined ( CONFIG_IDF_TARGET_ESP32S3 ) && __has_include (<esp32s3/rom/gpio.h>)
   #include <esp32s3/rom/gpio.h>  // dispatched by config
#elif defined ( CONFIG_IDF_TARGET_ESP32S2 ) && __has_include (<esp32s2/rom/gpio.h>)
   #include <esp32s2/rom/gpio.h>  // dispatched by config
#elif defined ( CONFIG_IDF_TARGET_ESP32 ) && __has_include (<esp32/rom/gpio.h>)
   #include <esp32/rom/gpio.h>
#else
   #include <rom/gpio.h> // dispatched by core
#endif   

#ifndef SPI_PIN_REG
 #define SPI_PIN_REG SPI_MISC_REG
#endif

#if defined (SOC_GDMA_SUPPORTED)  // for C3/C6/S3
 #include <soc/gdma_channel.h>
 #include <soc/gdma_reg.h>
 #include <soc/gdma_struct.h>
 #if !defined DMA_OUT_LINK_CH0_REG
  #define DMA_OUT_LINK_CH0_REG       GDMA_OUT_LINK_CH0_REG
  #define DMA_OUTFIFO_STATUS_CH0_REG GDMA_OUTFIFO_STATUS_CH0_REG
  #define DMA_OUTLINK_START_CH0      GDMA_OUTLINK_START_CH0
  #if defined (GDMA_OUTFIFO_EMPTY_L3_CH0)
   #define DMA_OUTFIFO_EMPTY_CH0      GDMA_OUTFIFO_EMPTY_L3_CH0
  #else
   #define DMA_OUTFIFO_EMPTY_CH0      GDMA_OUTFIFO_EMPTY_CH0
  #endif
 #endif
#endif

#include "common.hpp"

#include <algorithm>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void Bus_SPI::config(const config_t& cfg)
  {
    _cfg = cfg;

    auto spi_port = (uint32_t)(cfg.spi_host) + 1;  // FSPI=1  HSPI=2  VSPI=3;
    _spi_port = spi_port;
    _spi_w0_reg           = reg(SPI_W0_REG(          spi_port));
    _spi_cmd_reg          = reg(SPI_CMD_REG(         spi_port));
    _spi_user_reg         = reg(SPI_USER_REG(        spi_port));
    _spi_mosi_dlen_reg    = reg(SPI_MOSI_DLEN_REG(   spi_port));
#if defined ( SOC_GDMA_SUPPORTED )
#elif defined ( SPI_DMA_STATUS_REG )
    _spi_dma_out_link_reg = reg(SPI_DMA_OUT_LINK_REG(spi_port));
    _spi_dma_outstatus_reg = reg(SPI_DMA_STATUS_REG(spi_port));
#else
    _spi_dma_out_link_reg = reg(SPI_DMA_OUT_LINK_REG(spi_port));
    _spi_dma_outstatus_reg = reg(SPI_DMA_OUTSTATUS_REG(spi_port));
#endif
    if (cfg.pin_dc < 0)
    { // D/Cピン不使用の場合はGPIOレジスタの代わりにダミーとしてmask_reg_dcのアドレスを設定しておく;
      _mask_reg_dc = 0;
      _gpio_reg_dc[0] = &_mask_reg_dc;
      _gpio_reg_dc[1] = &_mask_reg_dc;
    }
    else
    {
      _mask_reg_dc = (1ul << (cfg.pin_dc & 31));
      _gpio_reg_dc[0] = get_gpio_lo_reg(cfg.pin_dc);
      _gpio_reg_dc[1] = get_gpio_hi_reg(cfg.pin_dc);
    }
    _last_freq_apb = 0;

    auto spi_mode = cfg.spi_mode;
    _user_reg = (spi_mode == 1 || spi_mode == 2) ? SPI_CK_OUT_EDGE | SPI_USR_MOSI : SPI_USR_MOSI;
//ESP_LOGI("LGFX","Bus_SPI::config  spi_port:%d  dc:%0d %02x", spi_port, _cfg.pin_dc, _mask_reg_dc);
  }

  bool Bus_SPI::init(void)
  {
//ESP_LOGI("LGFX","Bus_SPI::init");
    dc_control(true);
    pinMode(_cfg.pin_dc, pin_mode_t::output);

    int dma_ch = _cfg.dma_channel;
#if defined (ESP_IDF_VERSION)
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
    dma_ch = dma_ch ? SPI_DMA_CH_AUTO : SPI_DMA_DISABLED;
 #endif
#endif
    _inited = spi::init(_cfg.spi_host, _cfg.pin_sclk, _cfg.pin_miso, _cfg.pin_mosi, dma_ch).has_value();

#if defined ( SOC_GDMA_SUPPORTED )
    // 割当られたDMAチャネル番号を取得する

#if defined ( SOC_GDMA_TRIG_PERIPH_SPI3 )
    int peri_sel = (_spi_port == 3) ? SOC_GDMA_TRIG_PERIPH_SPI3 : SOC_GDMA_TRIG_PERIPH_SPI2;
#else
    int peri_sel = SOC_GDMA_TRIG_PERIPH_SPI2;
#endif

    int assigned_dma_ch = search_dma_out_ch(peri_sel);

    if (assigned_dma_ch >= 0)
    { // DMAチャンネルが特定できたらそれを使用する;
      _spi_dma_out_link_reg  = reg(DMA_OUT_LINK_CH0_REG       + assigned_dma_ch * sizeof(GDMA.channel[0]));
      _spi_dma_outstatus_reg = reg(DMA_OUTFIFO_STATUS_CH0_REG + assigned_dma_ch * sizeof(GDMA.channel[0]));
    }
#elif defined ( CONFIG_IDF_TARGET_ESP32 ) || !defined ( CONFIG_IDF_TARGET )

    dma_ch = (*reg(DPORT_SPI_DMA_CHAN_SEL_REG) >> (_cfg.spi_host * 2)) & 3;
    // ESP_LOGE("LGFX", "SPI_HOST: %d / DMA_CH: %d", _cfg.spi_host, dma_ch);

#endif

    _dma_ch = dma_ch;

    return _inited;
  }

  static void gpio_reset(size_t pin)
  {
    if (pin >= GPIO_NUM_MAX) return;
    gpio_reset_pin( (gpio_num_t)pin);
    gpio_matrix_out((gpio_num_t)pin, SIG_GPIO_OUT_IDX, 0, 0);
    // gpio_matrix_in には、ArduinoESP32 v1.0.x系では重大なバグがある。(無関係なピンに対して設定変更が行われることがある)
    // gpio_matrix_in( (gpio_num_t)pin, 0x100, 0   );
  }

  void Bus_SPI::release(void)
  {
//ESP_LOGI("LGFX","Bus_SPI::release");
    if (!_inited) return;
    _inited = false;
    spi::release(_cfg.spi_host);
    gpio_reset(_cfg.pin_dc  );
    gpio_reset(_cfg.pin_mosi);
    gpio_reset(_cfg.pin_miso);
    gpio_reset(_cfg.pin_sclk);
  }

  void Bus_SPI::beginTransaction(void)
  {
//ESP_LOGI("LGFX","Bus_SPI::beginTransaction");
    uint32_t freq_apb = getApbFrequency();
    uint32_t clkdiv_write = _clkdiv_write;
    if (_last_freq_apb != freq_apb)
    {
      _last_freq_apb = freq_apb;
      _clkdiv_read = FreqToClockDiv(freq_apb, _cfg.freq_read);
      clkdiv_write = FreqToClockDiv(freq_apb, _cfg.freq_write);
      _clkdiv_write = clkdiv_write;
      dc_control(true);
      pinMode(_cfg.pin_dc, pin_mode_t::output);
    }

    auto spi_mode = _cfg.spi_mode;
    uint32_t pin  = (spi_mode & 2) ? SPI_CK_IDLE_EDGE : 0;

    if (_cfg.use_lock) spi::beginTransaction(_cfg.spi_host);

    *_spi_user_reg = _user_reg;
    auto spi_port = _spi_port;
    (void)spi_port;
    *reg(SPI_PIN_REG(spi_port)) = pin;
    *reg(SPI_CLOCK_REG(spi_port)) = clkdiv_write;
#if defined ( SPI_UPDATE )
    *_spi_cmd_reg = SPI_UPDATE;
#endif
  }

  void Bus_SPI::endTransaction(void)
  {
    dc_control(true);
#if defined ( LGFX_SPIDMA_WORKAROUND )
    if (_dma_ch) { spicommon_dmaworkaround_idle(_dma_ch); }
#endif
    if (_cfg.use_lock) spi::endTransaction(_cfg.spi_host);
#if defined (ARDUINO) // Arduino ESP32
    *_spi_user_reg = SPI_USR_MOSI | SPI_USR_MISO | SPI_DOUTDIN; // for other SPI device (e.g. SD card)
#endif
  }

  void Bus_SPI::wait(void)
  {
    auto spi_cmd_reg = _spi_cmd_reg;
    while (*spi_cmd_reg & SPI_USR);
  }

  bool Bus_SPI::busy(void) const
  {
    return (*_spi_cmd_reg & SPI_USR);
  }

  bool Bus_SPI::writeCommand(uint32_t data, uint_fast8_t bit_length)
  {
//ESP_LOGI("LGFX","writeCmd: %02x  len:%d   dc:%02x", data, bit_length, _mask_reg_dc);
    --bit_length;
    auto spi_mosi_dlen_reg = _spi_mosi_dlen_reg;
    auto spi_w0_reg = _spi_w0_reg;
    auto spi_cmd_reg = _spi_cmd_reg;
    auto gpio_reg_dc = _gpio_reg_dc[0];
    auto mask_reg_dc = _mask_reg_dc;
#if !defined ( CONFIG_IDF_TARGET ) || defined ( CONFIG_IDF_TARGET_ESP32 )
    while (*spi_cmd_reg & SPI_USR) {}    // wait SPI
#else
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
#endif
    *spi_mosi_dlen_reg = bit_length;   // set bitlength
    *spi_w0_reg = data;                // set data
    *gpio_reg_dc = mask_reg_dc;        // D/C
    *spi_cmd_reg = SPI_EXECUTE;        // exec SPI
    return true;
  }

  void Bus_SPI::writeData(uint32_t data, uint_fast8_t bit_length)
  {
//ESP_LOGI("LGFX","writeData: %02x  len:%d", data, bit_length);
    --bit_length;
    auto spi_mosi_dlen_reg = _spi_mosi_dlen_reg;
    auto spi_w0_reg = _spi_w0_reg;
    auto spi_cmd_reg = _spi_cmd_reg;
    auto gpio_reg_dc = _gpio_reg_dc[1];
    auto mask_reg_dc = _mask_reg_dc;
#if !defined ( CONFIG_IDF_TARGET ) || defined ( CONFIG_IDF_TARGET_ESP32 )
    while (*spi_cmd_reg & SPI_USR) {}    // wait SPI
#else
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
#endif
    *spi_mosi_dlen_reg = bit_length;   // set bitlength
    *spi_w0_reg = data;                // set data
    *gpio_reg_dc = mask_reg_dc;        // D/C
    *spi_cmd_reg = SPI_EXECUTE;        // exec SPI
  }

  void Bus_SPI::writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count)
  {
    auto spi_mosi_dlen_reg = _spi_mosi_dlen_reg;
    auto spi_w0_reg = _spi_w0_reg;
    auto spi_cmd_reg = _spi_cmd_reg;
    auto gpio_reg_dc = _gpio_reg_dc[1];
    auto mask_reg_dc = _mask_reg_dc;
#if defined ( CONFIG_IDF_TARGET ) && !defined ( CONFIG_IDF_TARGET_ESP32 )
    auto dma = _clear_dma_reg;
    if (dma) { _clear_dma_reg = nullptr; }
#endif
    if (1 == count)
    {
      --bit_length;
      while (*spi_cmd_reg & SPI_USR);    // wait SPI
#if defined ( CONFIG_IDF_TARGET ) && !defined ( CONFIG_IDF_TARGET_ESP32 )
      if (dma) { *dma = 0; }
#endif
      *gpio_reg_dc = mask_reg_dc;        // D/C high (data)
      *spi_mosi_dlen_reg = bit_length;   // set bitlength
      *spi_w0_reg = data;                // set data
      *spi_cmd_reg = SPI_EXECUTE;        // exec SPI
      return;
    }

    uint32_t regbuf0 = data | data << bit_length;
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

    uint32_t length = bit_length * count;          // convert to bitlength.
    uint32_t len = (length <= 96u) ? length : (length <= 144u) ? 48u : 96u; // 1st send length = max 12Byte (96bit).

    length -= len;
    --len;

    while (*spi_cmd_reg & SPI_USR) {}  // wait SPI
#if defined ( CONFIG_IDF_TARGET ) && !defined ( CONFIG_IDF_TARGET_ESP32 )
    if (dma) { *dma = 0; }
#endif
    *gpio_reg_dc = mask_reg_dc;      // D/C high (data)
    *spi_mosi_dlen_reg = len;
    // copy to SPI buffer register
    spi_w0_reg[0] = regbuf0;
    spi_w0_reg[1] = regbuf1;
    spi_w0_reg[2] = regbuf2;
    *spi_cmd_reg = SPI_EXECUTE;      // exec SPI
    if (0 == length) return;

    uint32_t regbuf[7] = { regbuf0, regbuf1, regbuf2, regbuf0, regbuf1, regbuf2, regbuf0 } ;

    // copy to SPI buffer register
    memcpy((void*)&spi_w0_reg[3], regbuf, 24);
    memcpy((void*)&spi_w0_reg[9], regbuf, 28);

    // limit = 64Byte / depth_bytes;
    // When 24bit color, 504 bits out of 512bit buffer are used.
    // When 16bit color, it uses exactly 512 bytes. but, it behaves like a ring buffer, can specify a larger size.
    uint32_t limit;
    if (bits24)
    {
      limit = 504;
      len = length % limit;
    }
    else
    {
#if defined ( CONFIG_IDF_TARGET_ESP32 )
      limit = (1 << 11);
#else
      limit = (1 << 9);
#endif
      len = length & (limit - 1);
    }

    if (len)
    {
      length -= len;
      --len;
      while (*spi_cmd_reg & SPI_USR);
      *spi_mosi_dlen_reg = len;
      *spi_cmd_reg = SPI_EXECUTE;
      if (0 == length) return;
    }

    while (*spi_cmd_reg & SPI_USR);
    *spi_mosi_dlen_reg = limit - 1;
    *spi_cmd_reg = SPI_EXECUTE;
    while (length -= limit)
    {
      while (*spi_cmd_reg & SPI_USR);
      *spi_cmd_reg = SPI_EXECUTE;
    }
  }

  void Bus_SPI::writePixels(pixelcopy_t* param, uint32_t length)
  {
    const uint8_t bytes = param->dst_bits >> 3;
    if (_cfg.dma_channel)
    {
      uint32_t limit = (bytes == 2) ? 32 : 24;
      uint32_t len;
      do
      {
        len = (limit << 1) <= length ? limit : length;
        if (limit <= 256) limit <<= 1;
        auto dmabuf = _flip_buffer.getBuffer(len * bytes);
        if (dmabuf == nullptr) {
          break;
        }
        param->fp_copy(dmabuf, 0, len, param);
        writeBytes(dmabuf, len * bytes, true, true);
      } while (length -= len);
      if (length == 0) return;
    }

/// ESP32-C3 で HIGHPART を使用すると異常動作するため分岐する;
#if defined ( SPI_UPDATE )  // for C3/S3

    const uint32_t limit = (bytes == 2) ? 32 : 21;
    uint32_t l = (length - 1) / limit;
    uint32_t len = length - (l * limit);
    length = l;
    uint32_t regbuf[16];
    param->fp_copy(regbuf, 0, len, param);

    auto spi_w0_reg = _spi_w0_reg;

    dc_control(true);
    set_write_len(len * bytes << 3);

    memcpy((void*)spi_w0_reg, regbuf, (len * bytes + 3) & (~3));

    exec_spi();
    if (0 == length) return;


    param->fp_copy(regbuf, 0, limit, param);
    wait_spi();
    set_write_len(limit * bytes << 3);
    memcpy((void*)spi_w0_reg, regbuf, limit * bytes);
    exec_spi();


    while (--length)
    {
      param->fp_copy(regbuf, 0, limit, param);
      wait_spi();
      memcpy((void*)spi_w0_reg, regbuf, limit * bytes);
      exec_spi();
    }

#else

    const uint32_t limit = (bytes == 2) ? 16 : 10; //  limit = 32/bytes (bytes==2 is 16   bytes==3 is 10)
    uint32_t len = (length - 1) / limit;
    uint32_t highpart = (len & 1) << 3;
    len = length - (len * limit);
    uint32_t regbuf[8];
    param->fp_copy(regbuf, 0, len, param);

    auto spi_w0_reg = _spi_w0_reg;

    uint32_t user_reg = *_spi_user_reg;

    dc_control(true);
    set_write_len(len * bytes << 3);

    memcpy((void*)&spi_w0_reg[highpart], regbuf, (len * bytes + 3) & (~3));
    if (highpart) *_spi_user_reg = user_reg | SPI_USR_MOSI_HIGHPART;
    exec_spi();
    if (0 == (length -= len)) return;

    for (; length; length -= limit)
    {
      param->fp_copy(regbuf, 0, limit, param);
      memcpy((void*)&spi_w0_reg[highpart ^= 0x08], regbuf, limit * bytes);
      uint32_t user = user_reg;
      if (highpart) user |= SPI_USR_MOSI_HIGHPART;
      if (len != limit)
      {
        len = limit;
        wait_spi();
        set_write_len(limit * bytes << 3);
        *_spi_user_reg = user;
        exec_spi();
      }
      else
      {
        wait_spi();
        *_spi_user_reg = user;
        exec_spi();
      }
    }

#endif

  }

  void Bus_SPI::writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma)
  {
    if (length <= 64)
    {
      auto spi_w0_reg = _spi_w0_reg;
      auto aligned_len = (length + 3) & (~3);
      length <<= 3;
      dc_control(dc);
      set_write_len(length);
      memcpy((void*)spi_w0_reg, data, aligned_len);
      exec_spi();
      return;
    }

    if (_cfg.dma_channel)
    {
      if (false == use_dma && length < 1024)
      {
        auto buf = _flip_buffer.getBuffer(length);
        if (buf) {
          memcpy(buf, data, length);
          data = buf;
          use_dma = true;
        }
      }
      if (use_dma)
      {
        auto spi_dma_out_link_reg = _spi_dma_out_link_reg;
        auto cmd = _spi_cmd_reg;
        while (*cmd & SPI_USR) {}
        *spi_dma_out_link_reg = 0;
        _setup_dma_desc_links(data, length);
#if defined ( SOC_GDMA_SUPPORTED )
        auto dma = reg(SPI_DMA_CONF_REG(_spi_port));
        *dma = 0; /// Clear previous transfer
        uint32_t len = ((length - 1) & ((SPI_MS_DATA_BITLEN)>>3)) + 1;
        *spi_dma_out_link_reg = DMA_OUTLINK_START_CH0 | ((int)(&_dmadesc[0]) & 0xFFFFF);
        *dma = SPI_DMA_TX_ENA;
        _clear_dma_reg = dma;
#else
        auto dma_conf_reg = reg(SPI_DMA_CONF_REG(_spi_port));
        auto dma_conf = *dma_conf_reg & ~(SPI_OUT_DATA_BURST_EN | SPI_AHBM_RST | SPI_AHBM_FIFO_RST | SPI_OUT_RST);
        *dma_conf_reg = dma_conf | SPI_AHBM_RST | SPI_AHBM_FIFO_RST | SPI_OUT_RST;

        // 送信長が4の倍数の場合のみバーストモードを使用する
        // ※ 以下の3つの条件が揃うと、DMA転送の末尾付近でデータが化ける現象が起きる。
        //    1.送信クロック80MHz (APBクロックと1:1)
        //    2.DMAバースト読出し有効
        //    3.送信データ長が4の倍数ではない (1Byte~3Byteの端数がある場合)
        dma_conf |= (length & 3) ? (SPI_OUTDSCR_BURST_EN) : (SPI_OUTDSCR_BURST_EN | SPI_OUT_DATA_BURST_EN);

        *dma_conf_reg = dma_conf;
        uint32_t len = length;
        *spi_dma_out_link_reg = SPI_OUTLINK_START | ((int)(&_dmadesc[0]) & 0xFFFFF);
        _clear_dma_reg = spi_dma_out_link_reg;
#endif
        set_write_len(len << 3);
        *_gpio_reg_dc[dc] = _mask_reg_dc;

        // DMA準備完了待ち;
#if defined ( SOC_GDMA_SUPPORTED )
        while (*_spi_dma_outstatus_reg & DMA_OUTFIFO_EMPTY_CH0 ) {}
#elif defined (SPI_DMA_OUTFIFO_EMPTY)
        while (*_spi_dma_outstatus_reg & SPI_DMA_OUTFIFO_EMPTY ) {}
#else
 #if defined ( LGFX_SPIDMA_WORKAROUND )
        if (_dma_ch) { spicommon_dmaworkaround_transfer_active(_dma_ch); }
 #endif
#endif
        exec_spi();

#if defined ( SOC_GDMA_SUPPORTED )
        if (length -= len)
        {
          while (*cmd & SPI_USR) {}
          set_write_len(SPI_MS_DATA_BITLEN + 1);
          goto label_start;
          do
          {
            while (*cmd & SPI_USR) {}
label_start:
            exec_spi();
          } while (length -= ((SPI_MS_DATA_BITLEN + 1) >> 3));
        }
#endif
        return;
      }
    }

    auto spi_w0_reg = _spi_w0_reg;

/// ESP32-C3 で HIGHPART を使用すると異常動作するため分岐する;
#if defined ( SPI_UPDATE )  // for C3/S3

    uint32_t regbuf[16];
    constexpr uint32_t limit = 64;
    uint32_t len = ((length - 1) & 0x3F) + 1;

    memcpy(regbuf, data, len);
    dc_control(dc);
    set_write_len(len << 3);

    memcpy((void*)spi_w0_reg, regbuf, (len + 3) & (~3));
    exec_spi();
    if (0 == (length -= len)) return;

    data += len;
    memcpy(regbuf, data, limit);
    wait_spi();
    set_write_len(limit << 3);
    memcpy((void*)spi_w0_reg, regbuf, limit);
    exec_spi();
    if (0 == (length -= limit)) return;

    do
    {
      data += limit;
      memcpy(regbuf, data, limit);
      wait_spi();
      memcpy((void*)spi_w0_reg, regbuf, limit);
      exec_spi();
    } while (0 != (length -= limit));

#else

    constexpr uint32_t limit = 32;
    uint32_t len = ((length - 1) & 0x1F) + 1;
    uint32_t highpart = ((length - 1) & limit) >> 2; // 8 or 0

    uint32_t user_reg = _user_reg;
    dc_control(dc);
    set_write_len(len << 3);

    memcpy((void*)&spi_w0_reg[highpart], data, (len + 3) & (~3));
    if (highpart) *_spi_user_reg = user_reg | SPI_USR_MOSI_HIGHPART;
    exec_spi();
    if (0 == (length -= len)) return;

    for (; length; length -= limit)
    {
      data += len;
      memcpy((void*)&spi_w0_reg[highpart ^= 0x08], data, limit);
      uint32_t user = user_reg;
      if (highpart) user |= SPI_USR_MOSI_HIGHPART;
      if (len != limit)
      {
        len = limit;
        wait_spi();
        set_write_len(limit << 3);
        *_spi_user_reg = user;
        exec_spi();
      }
      else
      {
        wait_spi();
        *_spi_user_reg = user;
        exec_spi();
      }
    }

#endif

  }

  void Bus_SPI::addDMAQueue(const uint8_t* data, uint32_t length)
  {
    if (!_cfg.dma_channel)
    {
      writeBytes(data, length, true, true);
      return;
    }

    _dma_queue_bytes += length;
    size_t index = _dma_queue_size;
    size_t new_size = index + ((length-1) / SPI_MAX_DMA_LEN) + 1;

    if (_dma_queue_capacity < new_size)
    {
      _dma_queue_capacity = new_size + 8;
      auto new_queue = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t) * _dma_queue_capacity, MALLOC_CAP_DMA);
      if (index)
      {
        memcpy(new_queue, _dma_queue, sizeof(lldesc_t) * index);
      }
      if (_dma_queue != nullptr) { heap_free(_dma_queue); }
      _dma_queue = new_queue;
    }
    _dma_queue_size = new_size;
    lldesc_t *dmadesc = &_dma_queue[index];

    while (length > SPI_MAX_DMA_LEN)
    {
      *(uint32_t*)dmadesc = SPI_MAX_DMA_LEN | SPI_MAX_DMA_LEN << 12 | 0x80000000;
      dmadesc->buf = const_cast<uint8_t*>(data);
      dmadesc++;
      data += SPI_MAX_DMA_LEN;
      length -= SPI_MAX_DMA_LEN;
    }
    *(uint32_t*)dmadesc = ((length + 3) & ( ~3 )) | length << 12 | 0x80000000;
    dmadesc->buf = const_cast<uint8_t*>(data);
  }

  void Bus_SPI::execDMAQueue(void)
  {
    if (0 == _dma_queue_size) return;

    int index = _dma_queue_size - 1;
    _dma_queue_size = 0;
    _dma_queue[index].eof = 1;
    _dma_queue[index].qe.stqe_next = nullptr;
    while (--index >= 0)
    {
      _dma_queue[index].qe.stqe_next = &_dma_queue[index + 1];
    }

    std::swap(_dmadesc, _dma_queue);
    std::swap(_dmadesc_size, _dma_queue_capacity);

    dc_control(true);
    *_spi_dma_out_link_reg = 0;

#if defined ( SOC_GDMA_SUPPORTED )
    *_spi_dma_out_link_reg = DMA_OUTLINK_START_CH0 | ((int)(&_dmadesc[0]) & 0xFFFFF);
    auto dma = reg(SPI_DMA_CONF_REG(_spi_port));
    *dma = SPI_DMA_TX_ENA;
    _clear_dma_reg = dma;
    uint32_t len = ((_dma_queue_bytes - 1) & ((SPI_MS_DATA_BITLEN)>>3)) + 1;
#else
    auto dma_conf_reg = reg(SPI_DMA_CONF_REG(_spi_port));
    auto dma_conf = *dma_conf_reg & ~(SPI_OUT_DATA_BURST_EN | SPI_AHBM_RST | SPI_AHBM_FIFO_RST | SPI_OUT_RST);
    dma_conf |= SPI_OUTDSCR_BURST_EN;
    *dma_conf_reg = dma_conf | SPI_AHBM_RST | SPI_AHBM_FIFO_RST | SPI_OUT_RST;
    *dma_conf_reg = dma_conf;

    *_spi_dma_out_link_reg = SPI_OUTLINK_START | ((int)(&_dmadesc[0]) & 0xFFFFF);
    _clear_dma_reg = _spi_dma_out_link_reg;
    uint32_t len = _dma_queue_bytes;
    _dma_queue_bytes = 0;
#endif

    set_write_len(len << 3);
    // DMA準備完了待ち;
#if defined ( SOC_GDMA_SUPPORTED )
    while (*_spi_dma_outstatus_reg & DMA_OUTFIFO_EMPTY_CH0 ) {}
#elif defined (SPI_DMA_OUTFIFO_EMPTY)
    while (*_spi_dma_outstatus_reg & SPI_DMA_OUTFIFO_EMPTY ) {}
#else
 #if defined ( LGFX_SPIDMA_WORKAROUND )
    if (_dma_ch) { spicommon_dmaworkaround_transfer_active(_dma_ch); }
 #endif
#endif
    exec_spi();

#if defined ( SOC_GDMA_SUPPORTED )
    uint32_t length = _dma_queue_bytes - len;
    _dma_queue_bytes = 0;
    if (length)
    {
      wait_spi();
      set_write_len(SPI_MS_DATA_BITLEN + 1);
      goto label_start;
      do
      {
        wait_spi();
label_start:
        exec_spi();
      } while (length -= ((SPI_MS_DATA_BITLEN + 1) >> 3));
    }
#endif
  }

  void Bus_SPI::beginRead(uint_fast8_t dummy_bits)
  {
    beginRead();
    if (!dummy_bits) { return; }

#if defined ( SPI_UPDATE )  // for C3/S3

    /// ESP32-C3とS3は、1bitの送受信ができないため、CPOLの極性を反転させてダミークロックを生成する。;
    if (dummy_bits == 1)
    {
      auto pin_reg = reg(SPI_PIN_REG(_spi_port));
      auto cmd_reg = _spi_cmd_reg;
      auto value = *pin_reg;
      *pin_reg = value ^ SPI_CK_IDLE_EDGE;
      *cmd_reg = SPI_UPDATE;
      *pin_reg = value;
      *cmd_reg = SPI_UPDATE;
      return;
    }

#endif

    readData(dummy_bits);
  }

  void Bus_SPI::beginRead(void)
  {
    uint32_t pin = (_cfg.spi_mode & 2) ? SPI_CK_IDLE_EDGE : 0;
    uint32_t user = ((_cfg.spi_mode == 1 || _cfg.spi_mode == 2) ? SPI_CK_OUT_EDGE | SPI_USR_MISO : SPI_USR_MISO)
                        | (_cfg.spi_3wire ? SPI_SIO : 0);
    dc_control(true);
    *_spi_user_reg = user;
    *reg(SPI_PIN_REG(_spi_port)) = pin;
    *reg(SPI_CLOCK_REG(_spi_port)) = _clkdiv_read;
#if defined ( SPI_UPDATE )
    *_spi_cmd_reg = SPI_UPDATE;
#endif
  }

  void Bus_SPI::endRead(void)
  {
    uint32_t pin = (_cfg.spi_mode & 2) ? SPI_CK_IDLE_EDGE : 0;
    *_spi_user_reg = _user_reg;
    *reg(SPI_PIN_REG(_spi_port)) = pin;
    *reg(SPI_CLOCK_REG(_spi_port)) = _clkdiv_write;
#if defined ( SPI_UPDATE )
    *_spi_cmd_reg = SPI_UPDATE;
#endif
  }

  uint32_t Bus_SPI::readData(uint_fast8_t bit_length)
  {
    set_read_len(bit_length);
    auto spi_cmd_reg = _spi_cmd_reg;
    *spi_cmd_reg = SPI_EXECUTE;
    auto spi_w0_reg = _spi_w0_reg;
    uint32_t mask = (32 > bit_length) ? ~getSwap32((1 << (32 - bit_length))-1) : ~0;
    while (*spi_cmd_reg & SPI_USR);
    return *spi_w0_reg & mask;
  }

  bool Bus_SPI::readBytes(uint8_t* dst, uint32_t length, bool use_dma)
  {
#if defined ( SPI_DMA_IN_LINK_REG )
    if (_cfg.dma_channel && use_dma) {
      wait_spi();
      set_read_len(length << 3);

      auto dma_conf_reg = reg(SPI_DMA_CONF_REG(_spi_port));
      auto dma_conf = *dma_conf_reg & ~(SPI_AHBM_RST | SPI_AHBM_FIFO_RST | SPI_OUT_RST);
      *dma_conf_reg = dma_conf | SPI_AHBM_RST | SPI_AHBM_FIFO_RST | SPI_IN_RST;
      *dma_conf_reg = dma_conf | SPI_INDSCR_BURST_EN;

      _setup_dma_desc_links(dst, length);
      *reg(SPI_DMA_IN_LINK_REG(_spi_port)) = SPI_INLINK_START | ((int)(&_dmadesc[0]) & 0xFFFFF);
 #if defined ( LGFX_SPIDMA_WORKAROUND )
      if (_dma_ch) { spicommon_dmaworkaround_transfer_active(_dma_ch); }
 #endif
      exec_spi();
    }
    else
#endif
    {
      auto len1 = std::min<uint32_t>(length, 32u);  // 32 Byte read.
      auto len2 = len1;
      wait_spi();
      set_read_len(len1 << 3);
      exec_spi();

/// ESP32-C3 で HIGHPART を使用すると異常動作するため分岐する;
#if defined ( SPI_UPDATE )  // for C3/S3

      auto spi_w0_reg = _spi_w0_reg;
      do {
        if (0 == (length -= len1)) {
          len2 = len1;
          wait_spi();
          uint8_t tmp[32];
          memcpy(tmp, (void*)spi_w0_reg, (len2 + 3) & ~3u);
          memcpy(dst, tmp, len2);
        } else {
          if (length < len1) {
            len1 = length;
            wait_spi();
            set_read_len(len1 << 3);
          } else {
            wait_spi();
          }
          memcpy(dst, (void*)spi_w0_reg, (len2 + 3) & ~3u);
          exec_spi();
        }
        dst += len2;
      } while (length);

#else

      uint32_t userreg = *_spi_user_reg;
      uint32_t highpart = 8;
      auto spi_w0_reg = _spi_w0_reg;
      do {
        if (0 == (length -= len1)) {
          len2 = len1;
          wait_spi();
          *_spi_user_reg = userreg;
        } else {
          uint32_t user = userreg;
          if (highpart) user = userreg | SPI_USR_MISO_HIGHPART;
          if (length < len1) {
            len1 = length;
            wait_spi();
            set_read_len(len1 << 3);
          } else {
            wait_spi();
          }
          *_spi_user_reg = user;
          exec_spi();
        }
        memcpy(dst, (void*)&spi_w0_reg[highpart ^= 8], (len2+3)&~3u);
        dst += len2;
      } while (length);

#endif

    }
    return true;
  }

  void Bus_SPI::readPixels(void* dst, pixelcopy_t* param, uint32_t length)
  {
    auto len1 = std::min<uint32_t>(length, 10u);  // 10 pixel read
    auto len2 = len1;
    auto len_read_pixel = param->src_bits;
    uint32_t regbuf[8];
    wait_spi();
    set_read_len(len_read_pixel * len1);
    exec_spi();
    param->src_data = regbuf;
    int32_t dstindex = 0;
    auto spi_w0_reg = _spi_w0_reg;

/// ESP32-C3 で HIGHPART を使用すると異常動作するため分岐する;
#if defined ( SPI_UPDATE )  // for C3/S3

    do {
      if (0 == (length -= len1)) {
        len2 = len1;
        wait_spi();
        memcpy(regbuf, (void*)spi_w0_reg, ((len2 * len_read_pixel >> 3) + 3) & ~3);
      } else {
        if (length < len1) {
          len1 = length;
          wait_spi();
          set_read_len(len_read_pixel * len1);
        } else {
          wait_spi();
        }
        memcpy(regbuf, (void*)spi_w0_reg, ((len2 * len_read_pixel >> 3) + 3) & ~3);
        exec_spi();
      }
      param->src_x = 0;
      dstindex = param->fp_copy(dst, dstindex, dstindex + len2, param);
    } while (length);

#else

    uint32_t userreg = *_spi_user_reg;
    uint32_t highpart = 8;
    do {
      if (0 == (length -= len1)) {
        len2 = len1;
        wait_spi();
        *_spi_user_reg = userreg;
      } else {
        uint32_t user = userreg;
        if (highpart) user = userreg | SPI_USR_MISO_HIGHPART;
        if (length < len1) {
          len1 = length;
          wait_spi();
          set_read_len(len_read_pixel * len1);
        } else {
          wait_spi();
        }
        *_spi_user_reg = user;
        exec_spi();
      }
      memcpy(regbuf, (void*)&spi_w0_reg[highpart ^= 8], ((len2 * len_read_pixel >> 3)+3)&~3u);
      param->src_x = 0;
      dstindex = param->fp_copy(dst, dstindex, dstindex + len2, param);
    } while (length);

#endif

  }

  void Bus_SPI::_alloc_dmadesc(size_t len)
  {
    if (_dmadesc) heap_caps_free(_dmadesc);
    _dmadesc_size = len;
    _dmadesc = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t) * len, MALLOC_CAP_DMA);
  }

  void Bus_SPI::_spi_dma_reset(void)
  {
#if defined( SOC_GDMA_SUPPORTED )  // for C3/S3

#elif defined( CONFIG_IDF_TARGET_ESP32S2 )
    if (_cfg.spi_host == SPI2_HOST)
    {
      periph_module_reset( PERIPH_SPI2_DMA_MODULE );
    }
    else if (_cfg.spi_host == SPI3_HOST)
    {
      periph_module_reset( PERIPH_SPI3_DMA_MODULE );
    }
#else
    periph_module_reset( PERIPH_SPI_DMA_MODULE );
#endif
  }

  void Bus_SPI::_setup_dma_desc_links(const uint8_t *data, int32_t len)
  {          //spicommon_setup_dma_desc_links
    if (!_cfg.dma_channel) return;

    if (_dmadesc_size * SPI_MAX_DMA_LEN < len)
    {
      _alloc_dmadesc(len / SPI_MAX_DMA_LEN + 1);
    }
    lldesc_t *dmadesc = _dmadesc;

    while (len > SPI_MAX_DMA_LEN)
    {
      len -= SPI_MAX_DMA_LEN;
      dmadesc->buf = (uint8_t *)data;
      data += SPI_MAX_DMA_LEN;
      *(uint32_t*)dmadesc = SPI_MAX_DMA_LEN | SPI_MAX_DMA_LEN << 12 | 0x80000000;
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
