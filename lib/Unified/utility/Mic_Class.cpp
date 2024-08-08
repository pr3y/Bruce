#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Mic_Class.hpp"

#include "../M5Unified.hpp"

#if __has_include (<esp_idf_version.h>)
 #include <esp_idf_version.h>
 #if ESP_IDF_VERSION_MAJOR >= 4
  #define NON_BREAK ;[[fallthrough]];
 #endif
#endif

#ifndef NON_BREAK
#define NON_BREAK ;
#endif

#if __has_include(<sdkconfig.h>)
#include <sdkconfig.h>
#include <esp_log.h>
#include <math.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace m5
{
#if defined ( ESP_PLATFORM )
#if defined (ESP_IDF_VERSION_VAL)
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
  #define COMM_FORMAT_I2S (I2S_COMM_FORMAT_STAND_I2S)
  #define COMM_FORMAT_MSB (I2S_COMM_FORMAT_STAND_MSB)
 #endif
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 3)
  #define SAMPLE_RATE_TYPE uint32_t
 #endif
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  #define MIC_CLASS_ADC_WIDTH_BITS ADC_WIDTH_BIT_12
  #define MIC_CLASS_ADC_ATTEN_DB ADC_ATTEN_DB_11
 #endif
#endif

#ifndef COMM_FORMAT_I2S
 #define COMM_FORMAT_I2S (I2S_COMM_FORMAT_I2S)
 #define COMM_FORMAT_MSB (I2S_COMM_FORMAT_I2S_MSB)
#endif

#ifndef SAMPLE_RATE_TYPE
 #define SAMPLE_RATE_TYPE int
#endif

#ifndef MIC_CLASS_ADC_WIDTH_BITS
 #define MIC_CLASS_ADC_WIDTH_BITS ADC_WIDTH_12Bit
 #define MIC_CLASS_ADC_ATTEN_DB ADC_ATTEN_11db
#endif


  uint32_t Mic_Class::_calc_rec_rate(void) const
  {
    int rate = (_cfg.sample_rate * _cfg.over_sampling);
    return rate;
  }

  esp_err_t Mic_Class::_setup_i2s(void)
  {
    if (_cfg.pin_data_in  < 0) { return ESP_FAIL; }

    i2s_config_t i2s_config;
    memset(&i2s_config, 0, sizeof(i2s_config_t));
    i2s_config.mode                 = (i2s_mode_t)( I2S_MODE_MASTER | I2S_MODE_RX );
	  i2s_config.sample_rate          = 48000; // dummy setting.
    i2s_config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format       = _cfg.stereo ? I2S_CHANNEL_FMT_RIGHT_LEFT : _cfg.left_channel ? I2S_CHANNEL_FMT_ONLY_LEFT : I2S_CHANNEL_FMT_ONLY_RIGHT;
    i2s_config.communication_format = (i2s_comm_format_t)( COMM_FORMAT_I2S );
    i2s_config.dma_buf_count        = _cfg.dma_buf_count;
    i2s_config.dma_buf_len          = _cfg.dma_buf_len;

    i2s_pin_config_t pin_config;
    memset(&pin_config, ~0u, sizeof(i2s_pin_config_t)); /// all pin set to I2S_PIN_NO_CHANGE
#if defined (ESP_IDF_VERSION_VAL)
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
    pin_config.mck_io_num     = _cfg.pin_mck;
 #endif
#endif
    pin_config.bck_io_num     = _cfg.pin_bck;
    pin_config.ws_io_num      = _cfg.pin_ws;
    pin_config.data_in_num    = _cfg.pin_data_in;

    esp_err_t err;
    if (ESP_OK != (err = i2s_driver_install(_cfg.i2s_port, &i2s_config, 0, nullptr)))
    {
      i2s_driver_uninstall(_cfg.i2s_port);
      err = i2s_driver_install(_cfg.i2s_port, &i2s_config, 0, nullptr);
    }
    if (err != ESP_OK) { return err; }

#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
    if (_cfg.use_adc)
    {
      if (((size_t)_cfg.pin_data_in) > 39) { return ESP_FAIL; }
      static constexpr const uint8_t adc_table[] =
      {
        ADC2_CHANNEL_1 , // GPIO  0
        255            ,
        ADC2_CHANNEL_2 , // GPIO  2
        255            ,
        ADC2_CHANNEL_0 , // GPIO  4
        255, 255, 255, 255, 255, 255, 255,
        ADC2_CHANNEL_5 , // GPIO 12
        ADC2_CHANNEL_4 , // GPIO 13
        ADC2_CHANNEL_6 , // GPIO 14
        ADC2_CHANNEL_3 , // GPIO 15
        255, 255, 255, 255, 255, 255, 255, 255, 255,
        ADC2_CHANNEL_8 , // GPIO 25
        ADC2_CHANNEL_9 , // GPIO 26
        ADC2_CHANNEL_7 , // GPIO 27
        255, 255, 255, 255,
        ADC1_CHANNEL_4 , // GPIO 32
        ADC1_CHANNEL_5 , // GPIO 33
        ADC1_CHANNEL_6 , // GPIO 34
        ADC1_CHANNEL_7 , // GPIO 35
        ADC1_CHANNEL_0 , // GPIO 36
        ADC1_CHANNEL_1 , // GPIO 37
        ADC1_CHANNEL_2 , // GPIO 38
        ADC1_CHANNEL_3 , // GPIO 39
      };
      int adc_ch = adc_table[_cfg.pin_data_in];
      if (adc_ch == 255) { return ESP_FAIL; }

      adc_unit_t unit = _cfg.pin_data_in >= 32 ? ADC_UNIT_1 : ADC_UNIT_2;
      adc_set_data_width(unit, MIC_CLASS_ADC_WIDTH_BITS);
      err = i2s_set_adc_mode(unit, (adc1_channel_t)adc_ch);
      if (unit == ADC_UNIT_1)
      {
        adc1_config_channel_atten((adc1_channel_t)adc_ch, MIC_CLASS_ADC_ATTEN_DB);
      }
      else
      {
        adc2_config_channel_atten((adc2_channel_t)adc_ch, MIC_CLASS_ADC_ATTEN_DB);
      }
      if (_cfg.i2s_port == I2S_NUM_0)
      { /// レジスタを操作してADCモードの設定を有効にする ;
        I2S0.conf2.lcd_en = true;
        I2S0.conf.rx_right_first = 0;
        I2S0.conf.rx_msb_shift = 0;
        I2S0.conf.rx_mono = 0;
        I2S0.conf.rx_short_sync = 0;
        I2S0.fifo_conf.rx_fifo_mod = true;
        I2S0.conf_chan.rx_chan_mod = true;
      }
    }
    else
#endif
    {
      err = i2s_set_pin(_cfg.i2s_port, &pin_config);
    }

    return err;
  }

  // クロックディバイダー計算用関数 (実装は Speaker_Class.cpp内)
  void calcClockDiv(uint32_t* div_a, uint32_t* div_b, uint32_t* div_n, uint32_t baseClock, uint32_t targetFreq);

  void Mic_Class::mic_task(void* args)
  {
    auto self = (Mic_Class*)args;
    int oversampling = self->_cfg.over_sampling;
    if (     oversampling < 1) { oversampling = 1; }
    else if (oversampling > 8) { oversampling = 8; }

    bool use_pdm = (self->_cfg.pin_bck < 0 && !self->_cfg.use_adc);
    static constexpr uint32_t PLL_D2_CLK = 80*1000*1000; // 80 MHz
    uint32_t bits = (self->_cfg.use_adc) ? 1 : 16; /// 1サンプリング当たりの出力ビット数;
    uint32_t div_a, div_b, div_n;

    // CoreS3 のマイクはmclkの倍率(div_m)の値を8以上に設定しないと精度が落ちる。
    uint32_t div_m = 8;

    // PDM録音時、DSR(データサンプリングレート) 64に設定する
    if (use_pdm) { bits = 64; div_m = 2; }
    calcClockDiv(&div_a, &div_b, &div_n, PLL_D2_CLK / (bits * div_m), self->_cfg.sample_rate * oversampling);

#if defined ( I2S1I_BCK_OUT_IDX )
    auto dev = (self->_cfg.i2s_port == i2s_port_t::I2S_NUM_1) ? &I2S1 : &I2S0;
#else
    auto dev = &I2S0;
#endif

#if defined ( CONFIG_IDF_TARGET_ESP32C3 ) || defined ( CONFIG_IDF_TARGET_ESP32C6 ) || defined ( CONFIG_IDF_TARGET_ESP32S3 )

    dev->rx_conf.rx_pdm_en = use_pdm;
    dev->rx_conf.rx_tdm_en = !use_pdm;
#if defined (I2S_RX_PDM2PCM_EN)
    dev->rx_conf.rx_pdm2pcm_en = use_pdm;
    dev->rx_conf.rx_pdm_sinc_dsr_16_en = 0;
#endif
    if (!use_pdm) {
      dev->rx_conf.rx_mono = 0;
      dev->rx_conf.rx_mono_fst_vld = 0;
      dev->rx_tdm_ctrl.rx_tdm_tot_chan_num = self->_cfg.stereo ? 1 : 0;
      dev->rx_conf.rx_update = 1;
    }

    dev->rx_conf1.rx_bck_div_num = div_m - 1;
    dev->rx_clkm_conf.rx_clkm_div_num = div_n;

    dev->rx_clkm_div_conf.val = 0;
    if (div_b > (div_a >> 1)) {
      dev->rx_clkm_div_conf.rx_clkm_div_yn1 = 1;
      div_b = div_a - div_b;
    }
    int div_y = 1;
    int div_x = 0;
    if (div_b)
    {
      div_x = div_a / div_b - 1;
      div_y = div_a % div_b;

      if (div_y == 0)
      { // div_yが0になる場合、分数成分が無視される不具合があり、
        // 指定よりクロックが速くなってしまう。
        // 回避策として、誤差が少なくなる設定値を導入する。
        // これにより、誤差をクロック周期512回に1回程度のズレに抑える。;
        div_y = 1;
        div_b = 511;
      }
    }

    dev->rx_clkm_div_conf.rx_clkm_div_x = div_x;
    dev->rx_clkm_div_conf.rx_clkm_div_y = div_y;
    dev->rx_clkm_div_conf.rx_clkm_div_z = div_b;

    dev->rx_clkm_conf.rx_clk_sel = 2;   // PLL_160M_CLK
    dev->tx_clkm_conf.clk_en = 1;
    dev->rx_clkm_conf.rx_clk_active = 1;

#else

    if (use_pdm)
    {
      dev->pdm_conf.rx_sinc_dsr_16_en = 1; // 0=DSR64 / 1=DSR128
      dev->pdm_conf.pdm2pcm_conv_en = 1;
      dev->pdm_conf.rx_pdm_en = 1;
    }

    dev->sample_rate_conf.rx_bck_div_num = div_m;
    dev->clkm_conf.clkm_div_a = div_a;
    dev->clkm_conf.clkm_div_b = div_b;
    dev->clkm_conf.clkm_div_num = div_n;
    dev->clkm_conf.clka_en = 0; // APLL disable : PLL_160M

    // If RX is not reset here, BCK polarity may be inverted.
    dev->conf.rx_reset = 1;
    dev->conf.rx_fifo_reset = 1;
    dev->conf.rx_reset = 0;
    dev->conf.rx_fifo_reset = 0;

#endif

    i2s_start(self->_cfg.i2s_port);

    int32_t gain = self->_cfg.magnification;
    const float f_gain = (float)gain / (oversampling << 1);
    size_t src_idx = ~0u;
    size_t src_len = 0;
    int32_t sum_value[4] = { 0,0 };
    int32_t prev_value[2] = { 0, 0 };
    const bool in_stereo = self->_cfg.stereo;
    int32_t os_remain = oversampling;
    const size_t dma_buf_len = self->_cfg.dma_buf_len;
    int16_t* src_buf = (int16_t*)alloca(dma_buf_len * sizeof(int16_t));

    i2s_read(self->_cfg.i2s_port, src_buf, dma_buf_len, &src_len, portTICK_PERIOD_MS);
    i2s_read(self->_cfg.i2s_port, src_buf, dma_buf_len, &src_len, portTICK_PERIOD_MS);

    while (self->_task_running)
    {
      bool rec_flip = self->_rec_flip;
      recording_info_t* current_rec = &(self->_rec_info[!rec_flip]);
      recording_info_t* next_rec    = &(self->_rec_info[ rec_flip]);

      size_t dst_remain = current_rec->length;
      if (dst_remain == 0)
      {
        rec_flip = !rec_flip;
        self->_rec_flip = rec_flip;
        xSemaphoreGive(self->_task_semaphore);
        std::swap(current_rec, next_rec);
        dst_remain = current_rec->length;
        if (dst_remain == 0)
        {
          self->_is_recording = false;
          ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
          src_idx = ~0u;
          src_len = 0;
          sum_value[0] = 0;
          sum_value[1] = 0;
          continue;
        }
      }
      self->_is_recording = true;

      for (;;)
      {
        if (src_idx >= src_len)
        {
          i2s_read(self->_cfg.i2s_port, src_buf, dma_buf_len, &src_len, 100 / portTICK_PERIOD_MS);
          src_len >>= 1;
          src_idx = 0;
        }

        do
        {
          sum_value[0] += src_buf[src_idx  ];
          sum_value[1] += src_buf[src_idx+1];
          src_idx += 2;
        } while (--os_remain && (src_idx < src_len));

        if (os_remain) { continue; }
        os_remain = oversampling;

#if defined (CONFIG_IDF_TARGET_ESP32)
        auto sv0 = sum_value[1];
        auto sv1 = sum_value[0];
#else
        auto sv0 = sum_value[0];
        auto sv1 = sum_value[1];
#endif
        if (self->_cfg.use_adc) {
          sv0 -= 2048 * oversampling;
          sv1 -= 2048 * oversampling;
        }

        auto value_tmp = sv0 + sv1;
        int32_t offset = self->_offset;
        sum_value[0] = sv0 + offset;
        sum_value[1] = sv1 + offset;
        // Automatic zero level adjustment
        offset = (32 + offset * 62 - value_tmp) >> 6;
        self->_offset = offset;

        int32_t noise_filter = self->_cfg.noise_filter_level;
        if (noise_filter)
        {
          for (int i = 0; i < 2; ++i)
          {
            int32_t v = (sum_value[i] * (256 - noise_filter) + prev_value[i] * noise_filter + 128) >> 8;
            prev_value[i] = v;
            sum_value[i] = v * f_gain;
          }
        }
        else
        {
          for (int i = 0; i < 2; ++i)
          {
            sum_value[i] *= f_gain;
          }
        }

        int output_num = 2;

        if (in_stereo != current_rec->is_stereo)
        {
          if (in_stereo)
          { // stereo -> mono  convert.
            sum_value[0] = (sum_value[0] + sum_value[1] + 1) >> 1;
            output_num = 1;
          }
          else
          { // mono -> stereo  convert.
            auto tmp = sum_value[1];
            sum_value[3] = tmp;
            sum_value[2] = tmp;
            sum_value[1] = sum_value[0];
            output_num = 4;
          }
        }
        for (int i = 0; i < output_num; ++i)
        {
          auto value = sum_value[i];
          if (current_rec->is_16bit)
          {
            if (     value < INT16_MIN+16) { value = INT16_MIN+16; }
            else if (value > INT16_MAX-16) { value = INT16_MAX-16; }
            auto dst = (int16_t*)(current_rec->data);
            *dst++ = value;
            current_rec->data = dst;
          }
          else
          {
            value = ((value + 128) >> 8) + 128;
            if (     value < 0) { value = 0; }
            else if (value > 255) { value = 255; }
            auto dst = (uint8_t*)(current_rec->data);
            *dst++ = value;
            current_rec->data = dst;
          }
        }
        sum_value[0] = 0;
        sum_value[1] = 0;
        dst_remain -= output_num;
        if ((int32_t)dst_remain <= 0)
        {
          current_rec->length = 0;
          break;
        }
      }
    }
    self->_is_recording = false;
    i2s_stop(self->_cfg.i2s_port);

    self->_task_handle = nullptr;
    vTaskDelete(nullptr);
  }

  bool Mic_Class::begin(void)
  {
    if (_task_running)
    {
      auto rate = _calc_rec_rate();
      if (_rec_sample_rate == rate)
      {
        return true;
      }
      do { vTaskDelay(1); } while (isRecording());
      end();
      _rec_sample_rate = rate;
    }

    if (_task_semaphore == nullptr) { _task_semaphore = xSemaphoreCreateBinary(); }

    bool res = true;
    if (_cb_set_enabled) { res = _cb_set_enabled(_cb_set_enabled_args, true); }

    res = (ESP_OK == _setup_i2s()) && res;
    if (res)
    {
      size_t stack_size = 1024 + (_cfg.dma_buf_len * sizeof(int16_t));
      _task_running = true;
#if portNUM_PROCESSORS > 1
      if (_cfg.task_pinned_core < portNUM_PROCESSORS)
      {
        xTaskCreatePinnedToCore(mic_task, "mic_task", stack_size, this, _cfg.task_priority, &_task_handle, _cfg.task_pinned_core);
      }
      else
#endif
      {
        xTaskCreate(mic_task, "mic_task", stack_size, this, _cfg.task_priority, &_task_handle);
      }
    }

    return res;
  }

  void Mic_Class::end(void)
  {
    if (!_task_running) { return; }
    _task_running = false;
    if (_task_handle)
    {
      if (_task_handle) { xTaskNotifyGive(_task_handle); }
      do { vTaskDelay(1); } while (_task_handle);
    }

    if (_cb_set_enabled) { _cb_set_enabled(_cb_set_enabled_args, false); }
    i2s_driver_uninstall(_cfg.i2s_port);
  }

  bool Mic_Class::_rec_raw(void* recdata, size_t array_len, bool flg_16bit, uint32_t sample_rate, bool flg_stereo)
  {
    recording_info_t info;
    info.data = recdata;
    info.length = array_len;
    info.is_16bit = flg_16bit;
    info.is_stereo = flg_stereo;

    _cfg.sample_rate = sample_rate;

    if (!begin()) { return false; }
    if (array_len == 0) { return true; }
    while (_rec_info[_rec_flip].length) { xSemaphoreTake(_task_semaphore, 1); }
    _rec_info[_rec_flip] = info;
    if (this->_task_handle)
    {
      xTaskNotifyGive(this->_task_handle);
    }
    return true;
  }
#endif
}
#endif
#endif