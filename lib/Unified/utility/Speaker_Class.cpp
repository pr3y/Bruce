#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Speaker_Class.hpp"

#include "../M5Unified.hpp"

#if !defined (SDL_h_)

#if __has_include (<esp_idf_version.h>)
 #include <esp_idf_version.h>
 #if ESP_IDF_VERSION_MAJOR >= 4
  #define NON_BREAK ;[[fallthrough]];
 #endif
#endif

#if __has_include (<soc/pcr_struct.h>)
#include <soc/pcr_struct.h>
#endif

#ifndef NON_BREAK
#define NON_BREAK ;
#endif

#include <sdkconfig.h>
#include <esp_log.h>

#endif

#include <math.h>

namespace m5
{
#if defined (ESP_IDF_VERSION_VAL)
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
  #define COMM_FORMAT_I2S (I2S_COMM_FORMAT_STAND_I2S)
  #define COMM_FORMAT_MSB (I2S_COMM_FORMAT_STAND_MSB)
 #endif
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 3)
  #define SAMPLE_RATE_TYPE uint32_t
 #endif
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  #define I2S_DRIVER_VERSION 2
 #endif
#endif
#ifndef I2S_DRIVER_VERSION
#define I2S_DRIVER_VERSION 1
#endif

#ifndef COMM_FORMAT_I2S
#define COMM_FORMAT_I2S (I2S_COMM_FORMAT_I2S)
#define COMM_FORMAT_MSB (I2S_COMM_FORMAT_I2S_MSB)
#endif

#ifndef SAMPLE_RATE_TYPE
#define SAMPLE_RATE_TYPE int
#endif

  const uint8_t Speaker_Class::_default_tone_wav[16] = { 177, 219, 246, 255, 246, 219, 177, 128, 79, 37, 10, 1, 10, 37, 79, 128 }; // sin wave data

#if defined (SDL_h_)
  esp_err_t Speaker_Class::_setup_i2s(void)
  {
    return ESP_OK;
  }
#else
  esp_err_t Speaker_Class::_setup_i2s(void)
  {
    if (_cfg.pin_data_out < 0) { return ESP_FAIL; }

#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
    /// DACが使用できるのはI2Sポート0のみ。;
    if (_cfg.use_dac && _cfg.i2s_port != I2S_NUM_0) { return ESP_FAIL; }
#endif

    i2s_config_t i2s_config;
    memset(&i2s_config, 0, sizeof(i2s_config_t));
    i2s_config.mode                 = (i2s_mode_t)( I2S_MODE_MASTER | I2S_MODE_TX );
    i2s_config.sample_rate          = 48000; // dummy setting
    i2s_config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format       = _cfg.stereo || _cfg.buzzer
                                    ? I2S_CHANNEL_FMT_RIGHT_LEFT
                                    : I2S_CHANNEL_FMT_ONLY_RIGHT;
    i2s_config.communication_format = (i2s_comm_format_t)( COMM_FORMAT_I2S );
    i2s_config.tx_desc_auto_clear   = true;
#if I2S_DRIVER_VERSION > 1
    i2s_config.dma_desc_num         = _cfg.dma_buf_count;
    i2s_config.dma_frame_num        = _cfg.dma_buf_len;
#else
    i2s_config.dma_buf_count        = _cfg.dma_buf_count;
    i2s_config.dma_buf_len          = _cfg.dma_buf_len;
#endif
    i2s_pin_config_t pin_config;
    memset(&pin_config, ~0u, sizeof(i2s_pin_config_t)); /// all pin set to I2S_PIN_NO_CHANGE
    pin_config.bck_io_num     = _cfg.pin_bck;
    pin_config.ws_io_num      = _cfg.pin_ws;
    pin_config.data_out_num   = _cfg.pin_data_out;

    esp_err_t err;
    if (ESP_OK != (err = i2s_driver_install(_cfg.i2s_port, &i2s_config, 0, nullptr)))
    {
      i2s_driver_uninstall(_cfg.i2s_port);
      err = i2s_driver_install(_cfg.i2s_port, &i2s_config, 0, nullptr);
    }
    if (err != ESP_OK) { return err; }

#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
    if (_cfg.use_dac)
    {
      i2s_dac_mode_t dac_mode = i2s_dac_mode_t::I2S_DAC_CHANNEL_BOTH_EN;
      if (!_cfg.stereo)
      {
        dac_mode = (_cfg.pin_data_out == GPIO_NUM_25)
                ? i2s_dac_mode_t::I2S_DAC_CHANNEL_RIGHT_EN // for GPIO 25
                : i2s_dac_mode_t::I2S_DAC_CHANNEL_LEFT_EN; // for GPIO 26
      }
      err = i2s_set_dac_mode(dac_mode);
      if (_cfg.i2s_port == I2S_NUM_0)
      { /// レジスタを操作してDACモードの設定を有効にする(I2S0のみ。I2S1はDAC,ADC非対応) ;
        I2S0.conf2.lcd_en = true;
        I2S0.conf.tx_right_first = false;
        I2S0.conf.tx_msb_shift = 0;
        I2S0.conf.tx_short_sync = 0;
      }
    }
    else
#endif
    {
      err = i2s_set_pin(_cfg.i2s_port, &pin_config);
    }

    return err;
  }
#endif

  void calcClockDiv(uint32_t* div_a, uint32_t* div_b, uint32_t* div_n, uint32_t baseClock, uint32_t targetFreq)
  {
    if (baseClock <= targetFreq << 1)
    { /// Nは最小2のため、基準クロックが目標クロックの2倍より小さい場合は値を確定する;
      *div_n = 2;
      *div_a = 1;
      *div_b = 0;
      return;
    }
    uint32_t save_n = 255;
    uint32_t save_a = 63;
    uint32_t save_b = 62;
    if (targetFreq)
    {
      float fdiv = (float)baseClock / targetFreq;
      uint32_t n = (uint32_t)fdiv;
      if (n < 256)
      {
        fdiv -= n;

        float check_base = baseClock;
// 探索時の誤差を少なくするため、値を大きくしておく;
        while ((int32_t)targetFreq >= 0) { targetFreq <<= 1; check_base *= 2; }
        float check_target = targetFreq;

        uint32_t save_diff = UINT32_MAX;
        if (n < 255)
        { /// 初期値の設定はNがひとつ上のものを設定しておく;
          save_a = 1;
          save_b = 0;
          save_n = n + 1;
          save_diff = abs((int)(check_target - check_base / (float)save_n));
        }

        for (uint32_t a = 1; a < 64; ++a)
        {
          uint32_t b = roundf(a * fdiv);
          if (a <= b) { continue; }
          uint32_t diff = abs((int)(check_target - ((check_base * a) / (n * a + b))));
          if (save_diff <= diff) { continue; }
          save_diff = diff;
          save_a = a;
          save_b = b;
          save_n = n;
          if (!diff) { break; }
        }
      }
    }
    *div_n = save_n;
    *div_a = save_a;
    *div_b = save_b;
  }

/// レート変換係数 (実際に設定されるレートが浮動小数になる場合があるため、入力と出力の両方のサンプリングレートに係数を掛け、誤差を減らす);
  #define SAMPLERATE_MUL 256

  void Speaker_Class::spk_task(void* args)
  {
    auto self = (Speaker_Class*)args;
    const bool out_stereo = self->_cfg.stereo;
    const size_t dma_buf_len = self->_cfg.dma_buf_len & ~1;

#if defined (SDL_h_)

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec fmt;
    fmt.freq = self->_cfg.sample_rate;
    fmt.format = AUDIO_S16LSB;
    fmt.channels = self->_cfg.stereo ? 2 : 1;
    fmt.samples = dma_buf_len;
    fmt.callback = nullptr;
    auto res = SDL_OpenAudio(&fmt, nullptr);
    if (res == 0)
    {
      SDL_PauseAudio(0);
    }
    const int32_t spk_sample_rate_x256 = self->_cfg.sample_rate * SAMPLERATE_MUL;

#else
    const i2s_port_t i2s_port = self->_cfg.i2s_port;
    i2s_stop(i2s_port);

#if defined ( CONFIG_IDF_TARGET_ESP32C3 ) || defined (CONFIG_IDF_TARGET_ESP32C6) || defined ( CONFIG_IDF_TARGET_ESP32S3 )
    static constexpr uint32_t PLL_D2_CLK = 120*1000*1000; // 240 MHz/2
#else
    static constexpr uint32_t PLL_D2_CLK = 80*1000*1000; // 160 MHz/2
#endif
    uint32_t bits = (self->_cfg.use_dac) ? 1 : 16; /// 1サンプリング当たりの出力ビット数;
    uint32_t div_a, div_b, div_n;
    uint32_t div_m = 32 / bits; /// MCLKを使用しない場合、サンプリングレート誤差が少なくなるようにdiv_mを調整する;
    // MCLKを使用するデバイスに対応する場合には、div_mを使用してBCKとMCKの比率を調整する;

    calcClockDiv(&div_a, &div_b, &div_n, PLL_D2_CLK, div_m * bits * self->_cfg.sample_rate);

    /// 実際に設定されたサンプリングレートの算出を行う;
    const int32_t spk_sample_rate_x256 = (float)PLL_D2_CLK * SAMPLERATE_MUL / ((float)(div_b * div_m * bits) / (float)div_a + (div_n * div_m * bits));
//  ESP_EARLY_LOGW("Speaker_Class", "sample rate:%d Hz = %d MHz/(%d+(%d/%d))/%d/%d = %d Hz", self->_cfg.sample_rate, PLL_D2_CLK / 1000000, div_n, div_b, div_a, div_m, bits, spk_sample_rate_x256 / SAMPLERATE_MUL);

#if SOC_I2S_NUM == 1
    auto dev = &I2S0;
#else
    auto dev = (i2s_port == i2s_port_t::I2S_NUM_1) ? &I2S1 : &I2S0;
#endif

#if defined ( CONFIG_IDF_TARGET_ESP32C3 ) || defined (CONFIG_IDF_TARGET_ESP32C6) || defined ( CONFIG_IDF_TARGET_ESP32S3 )
    // モノラル設定時、同じデータを左右両方に送信する設定
    if (!self->_cfg.stereo && !self->_cfg.use_dac && !self->_cfg.buzzer)
    {
      dev->tx_conf.tx_mono = 1;
      dev->tx_conf.tx_chan_equal = 1;
    }

    dev->tx_conf1.tx_bck_div_num = div_m - 1;

    bool yn1 = (div_b > (div_a >> 1));
    if (yn1) {
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

#if __has_include (<soc/pcr_struct.h>) // for C6
    PCR.i2s_tx_clkm_div_conf.i2s_tx_clkm_div_x = div_x;
    PCR.i2s_tx_clkm_div_conf.i2s_tx_clkm_div_y = div_y;
    PCR.i2s_tx_clkm_div_conf.i2s_tx_clkm_div_z = div_b;
    PCR.i2s_tx_clkm_div_conf.i2s_tx_clkm_div_yn1 = yn1;
    PCR.i2s_tx_clkm_conf.i2s_tx_clkm_div_num = div_n;
    PCR.i2s_tx_clkm_conf.i2s_tx_clkm_sel = 1;   // PLL_240M_CLK
#else
    dev->tx_clkm_div_conf.tx_clkm_div_x = div_x;
    dev->tx_clkm_div_conf.tx_clkm_div_y = div_y;
    dev->tx_clkm_div_conf.tx_clkm_div_z = div_b;
    dev->tx_clkm_div_conf.tx_clkm_div_yn1 = yn1;
    dev->tx_clkm_conf.tx_clkm_div_num = div_n;
    dev->tx_clkm_conf.tx_clk_sel = 1;   // PLL_240M_CLK
#endif

    dev->tx_clkm_conf.clk_en = 1;
    dev->tx_clkm_conf.tx_clk_active = 1;

#else

    dev->sample_rate_conf.tx_bck_div_num = div_m;
    dev->clkm_conf.clkm_div_a = div_a;
    dev->clkm_conf.clkm_div_b = div_b;
    dev->clkm_conf.clkm_div_num = div_n;
    dev->clkm_conf.clka_en = 0; // APLL disable : PLL_160M

    // If TX is not reset here, BCK polarity may be inverted.
    dev->conf.tx_reset = 1;
    dev->conf.tx_fifo_reset = 1;
    dev->conf.tx_reset = 0;
    dev->conf.tx_fifo_reset = 0;

#endif
    i2s_zero_dma_buffer(i2s_port);

    enum spk_i2s_state
    {
      spk_i2s_stop,
      spk_i2s_mute,
      spk_i2s_run,
    };
    spk_i2s_state flg_i2s_started = spk_i2s_stop;
#endif
    // ステレオ出力の場合は倍率を2倍する
    const float magnification = (float)(self->_cfg.magnification << out_stereo) / spk_sample_rate_x256 / (1 << 28);

    int32_t dac_offset = std::min(INT16_MAX-255, self->_cfg.dac_zero_level << 8);

    uint8_t buf_cnt = 0;
    bool flg_nodata = false;

    union
    {
      int16_t surplus16 = 0;
      uint8_t surplus[2];
    };

    int32_t* sound_buf32 = (int32_t*)alloca(dma_buf_len * sizeof(int32_t));


    while (self->_task_running)
    {
      if (flg_nodata)
      {
#if defined (SDL_h_)
        SDL_Delay(1);
        if (0 == (self->_play_channel_bits.load())) { continue; }
#else
        if (buf_cnt)
        { // (no data... wait for new data)
          --buf_cnt;
          uint32_t wait_msec = 1 + (self->_cfg.dma_buf_len / (spk_sample_rate_x256 >> 17));
          flg_nodata = (0 == ulTaskNotifyTake( pdFALSE, wait_msec ));
        }

        if (flg_nodata && 0 == buf_cnt)
        {
          if (self->_cfg.use_dac && dac_offset)
          { // Gradual transition of DAC output to 0;
            flg_i2s_started = spk_i2s_mute;
            size_t idx = 0;
            do
            {
              auto tmp = (uint32_t)((1.0f + cosf(idx * M_PI / dma_buf_len)) * (dac_offset >> 1));
              sound_buf32[idx] = tmp | tmp << 16;
            } while (++idx < dma_buf_len);
            size_t write_bytes;
            i2s_write(i2s_port, sound_buf32, dma_buf_len * sizeof(int32_t), &write_bytes, portMAX_DELAY);
            if (self->_cfg.dac_zero_level == 0)
            {
              dac_offset = 0;
            }
          }

          // バッファ全てゼロになるまで出力を繰返す;
          memset(sound_buf32, 0, dma_buf_len * sizeof(uint32_t));
          // DAC使用時は長めに設定する
          size_t retry = (self->_cfg.dma_buf_count << self->_cfg.use_dac) + 1;
          while (!ulTaskNotifyTake( pdTRUE, 0 ) && --retry)
          {
            size_t write_bytes;
            i2s_write(i2s_port, sound_buf32, dma_buf_len * sizeof(int32_t), &write_bytes, portMAX_DELAY);
          }

          if (!retry)
          {
#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
            if (self->_cfg.use_dac)
            {
              flg_i2s_started = spk_i2s_stop;
              i2s_stop(i2s_port);
              i2s_set_dac_mode(i2s_dac_mode_t::I2S_DAC_CHANNEL_DISABLE);
            }
#endif
            // 新しいデータが届くまで待機;
            ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
          }
        }
#endif
      }

#if !defined (SDL_h_)
      ulTaskNotifyTake( pdTRUE, 0 );
#endif


    flg_nodata = true;

#if defined (SDL_h_)
      uint32_t border = std::max<int>(2048, self->_cfg.sample_rate >> 3);
      while (SDL_GetQueuedAudioSize(1) > border) { SDL_Delay(1); }
#else
      if (flg_i2s_started != spk_i2s_run)
      {
        if (flg_i2s_started == spk_i2s_stop)
        {
#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
          if (self->_cfg.use_dac)
          {
            i2s_dac_mode_t dac_mode = i2s_dac_mode_t::I2S_DAC_CHANNEL_BOTH_EN;
            if (!out_stereo)
            {
              dac_mode = (self->_cfg.pin_data_out == GPIO_NUM_25)
                      ? i2s_dac_mode_t::I2S_DAC_CHANNEL_RIGHT_EN // for GPIO 25
                      : i2s_dac_mode_t::I2S_DAC_CHANNEL_LEFT_EN; // for GPIO 26
            }
            i2s_set_dac_mode(dac_mode);
          }
#endif
          i2s_start(i2s_port);
        }

        if (self->_cfg.use_dac && self->_cfg.dac_zero_level != 0)
        {
          size_t idx = 0;
          do
          {
            auto tmp = (uint32_t)((1.0f - cosf(idx * M_PI / dma_buf_len)) * (dac_offset >> 1));
            sound_buf32[idx] = tmp | tmp << 16;
          } while (++idx < dma_buf_len);
          size_t write_bytes;
          i2s_write(i2s_port, sound_buf32, dma_buf_len * sizeof(int32_t), &write_bytes, portMAX_DELAY);
        }
        flg_i2s_started = spk_i2s_run;
      }
#endif
      memset(sound_buf32, 0, dma_buf_len * sizeof(int32_t));

      float volume = magnification * (self->_master_volume * self->_master_volume);

      size_t data_length = 0;

      for (size_t ch = 0; ch < sound_channel_max; ++ch)
      {
        if (0 == (self->_play_channel_bits.load() & (1 << ch))) { continue; }

        auto ch_info = &(self->_ch_info[ch]);
        int ch_diff = ch_info->diff;
        size_t ch_index = ch_info->index;

        wav_info_t* current_wav = &(ch_info->wavinfo[!ch_info->flip]);
        wav_info_t* next_wav    = &(ch_info->wavinfo[ ch_info->flip]);

        size_t idx = 0;

        if (current_wav->repeat == 0 || next_wav->stop_current)
        {
label_next_wav:
          bool clear_idx = (next_wav->repeat == 0
                        || !next_wav->no_clear_index
                        || (next_wav->data != current_wav->data));
          current_wav->clear();
          ch_info->flip = !ch_info->flip;
#if !defined (SDL_h_)
          xSemaphoreGive(self->_task_semaphore);
#endif
          std::swap(current_wav, next_wav);

          if (clear_idx)
          {
            ch_index = 0;
            if (current_wav->repeat == 0)
            {
              self->_play_channel_bits.fetch_and(~(1 << ch));
              if (current_wav->repeat == 0)
              {
                ch_info->diff = 0;
                ch_info->index = 0;
                continue;
              }
              self->_play_channel_bits.fetch_or(1 << ch);
            }
          }
        }
        auto data = (const uint8_t*)current_wav->data;
        const bool in_stereo = current_wav->is_stereo;
        const int32_t in_rate = current_wav->sample_rate_x256;
        int32_t tmp = ch_info->volume;
        tmp *= tmp;
        // 8bitのデータの場合は倍率を256倍する
        if (!current_wav->is_16bit) { tmp <<= 8; }
        const float ch_v = volume * tmp;

        auto liner_base = ch_info->liner_buf[0];
        auto liner_prev = ch_info->liner_buf[1];

        if (ch_diff < 0) { goto label_continue_sample; }

        if (ch_index >= current_wav->length)
        {
label_wav_end:
          ch_index -= current_wav->length;
          auto repeat = current_wav->repeat;
          if (repeat != ~0u)
          {
            current_wav->repeat = --repeat;
            if (repeat == 0)
            {
              goto label_next_wav;
            }
          }
        }

        do
        {
          do
          {
            if (ch_index >= current_wav->length)
            {
              goto label_wav_end;
            }

            int32_t l, r;
            if (current_wav->is_16bit)
            {
              auto wav = (const int16_t*)data;
              l = wav[ch_index];
              r = wav[ch_index += in_stereo];
              ch_index++;
              if (!current_wav->is_signed)
              {
                l = (l & 0xFFFF) + INT16_MIN;
                r = (r & 0xFFFF) + INT16_MIN;
              }
            }
            else
            {
              l = data[ch_index];
              r = data[ch_index += in_stereo];
              ch_index++;
              if (current_wav->is_signed)
              {
                l = (int8_t)l;
                r = (int8_t)r;
              }
              else
              {
                l += INT8_MIN;
                r += INT8_MIN;
              }
            }

            liner_prev[0] = liner_base[0];
            if (out_stereo)
            {
              liner_prev[1] = liner_base[1];
              liner_base[1] = r * ch_v;
            }
            else
            {
              l += r;
            }
            liner_base[0] = l * ch_v;

            ch_diff -= spk_sample_rate_x256;
          } while (ch_diff >= 0);

label_continue_sample:

/// liner_prevからliner_baseへの２サンプル間の線形補間;
          float base_l = liner_base[0];
          float step_l = base_l - liner_prev[0];
          base_l *= spk_sample_rate_x256;
          base_l += step_l * ch_diff;
          step_l *= in_rate;
          int32_t b_l = base_l;
          int32_t s_l = step_l;
          if (out_stereo)
          {
            float base_r = liner_base[1];
            float step_r = base_r - liner_prev[1];
            base_r *= spk_sample_rate_x256;
            base_r += step_r * ch_diff;
            step_r *= in_rate;
            int32_t b_r = base_r;
            int32_t s_r = step_r;
            do
            {
              sound_buf32[  idx] += b_l;
              sound_buf32[++idx] += b_r;
              b_l += s_l;
              b_r += s_r;
              ch_diff += in_rate;
            } while (++idx < dma_buf_len && ch_diff < 0);
          }
          else
          {
            do
            {
              sound_buf32[idx] += b_l;
              b_l += s_l;
              ch_diff += in_rate;
            } while (++idx < dma_buf_len && ch_diff < 0);
          }
          if (data_length < idx) { data_length = idx; }
        } while (idx < dma_buf_len);
        ch_info->diff = ch_diff;
        ch_info->index = ch_index;
      }

      flg_nodata = (data_length == 0);

      if (!flg_nodata)
      {
        if (++buf_cnt >= self->_cfg.dma_buf_count)
        {
          buf_cnt = self->_cfg.dma_buf_count;
        }

        if (self->_cfg.use_dac)
        {
  /// DAC出力は cfg.dac_zero_levelが0に設定されている場合、振幅のオフセットを動的に変更する。;
  /// DAC出力が低いほどノイズ音が減るため、なるべくDAC出力を下げてノイズを低減することを目的とする。;
          const bool zero_bias = (self->_cfg.dac_zero_level == 0);
          bool biasing = zero_bias;
          size_t idx = 0;
          do
          {
            int32_t v1 = sound_buf32[  idx] >> 8;
            int32_t v2 = sound_buf32[++idx] >> 8;
            int32_t vabs = std::max(abs(v1), abs(v2));
            if (dac_offset <= vabs)
            {
              if (zero_bias)
              {
                dac_offset = (INT16_MAX-255 < vabs) ? INT16_MAX-255 : vabs;
                biasing = false;
              }
              v1 += dac_offset;
              v2 += dac_offset;
              if (v1 < 0) { v1 = 0; }
              else if (v1 > UINT16_MAX) { v1 = UINT16_MAX; }
              if (v2 < 0) { v2 = 0; }
              else if (v2 > UINT16_MAX) { v2 = UINT16_MAX; }
            }
            else
            {
              v1 += dac_offset + surplus[0];
              surplus[0] = v1;
              v2 += dac_offset + surplus[out_stereo];
              surplus[out_stereo] = v2;
            }
            sound_buf32[idx >> 1] = v1 << 16 | v2;
          } while (++idx < data_length);
          if (biasing) { dac_offset -= (dac_offset * data_length) >> 15; }
        }
        else if (self->_cfg.buzzer)
        {
  /// ブザー出力は 1bit ΔΣ方式。 I2Sデータ出力をブザーの駆動信号として利用する;
  /// 出力はモノラル限定だが、I2Sへはステレオ扱いで出力する。;
  /// (I2Sをモノラル設定にした場合は同じデータが２チャンネル分送信されてしまうため、敢えてステレオ扱いとしている);
          int32_t tmp = (uint16_t)surplus16;
          size_t idx = 0;
          do
          {
            int32_t v = sound_buf32[idx] >> 8;
            v = INT16_MIN - v;
            uint32_t bitdata = 0;
            uint32_t bit = 0x80000000;
            do
            {
              if ((tmp += v) < 0)
              {
                tmp += 0x10000;
                bitdata |= bit;
              }
            } while (bit >>= 1);
            sound_buf32[idx] = bitdata;
          } while (++idx < data_length);
          surplus16 = flg_nodata ? 0x8000 : tmp;
        }
        else
        {
          size_t idx = 0;
          do
          {
            int32_t v1 = sound_buf32[idx] >> 8;
            if (v1 < INT16_MIN) { v1 = INT16_MIN; }
            else if (v1 > INT16_MAX) { v1 = INT16_MAX; }

            int32_t v2 = sound_buf32[++idx] >> 8;
            if (v2 < INT16_MIN) { v2 = INT16_MIN; }
            else if (v2 > INT16_MAX) { v2 = INT16_MAX; }

            sound_buf32[idx >> 1] = v1 << 16 | (uint16_t)v2;
          } while (++idx < data_length);
        }

#if defined (SDL_h_)
        SDL_QueueAudio(1, sound_buf32, data_length * sizeof(int16_t));
#else
        size_t write_bytes;
        size_t data_bytes = data_length * sizeof(int16_t) << self->_cfg.buzzer;
        i2s_write(i2s_port, sound_buf32, data_bytes, &write_bytes, 0);
        if (write_bytes < data_bytes) {
          auto sb8 = (uint8_t*)sound_buf32;
          i2s_write(i2s_port, &sb8[write_bytes], data_bytes - write_bytes, &write_bytes, portMAX_DELAY);
          buf_cnt = self->_cfg.dma_buf_count;
        }
#endif
      }
    }
#if defined ( SDL_h_ )
    SDL_CloseAudioDevice(1);
    SDL_CloseAudio();
#else
    i2s_stop(i2s_port);
#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)
    if (self->_cfg.use_dac)
    {
      i2s_set_dac_mode(i2s_dac_mode_t::I2S_DAC_CHANNEL_DISABLE);
      m5gfx::gpio_lo(self->_cfg.pin_data_out);
      m5gfx::pinMode(self->_cfg.pin_data_out, m5gfx::pin_mode_t::output);
    }
#endif
    self->_task_handle = nullptr;
    vTaskDelete(nullptr);
#endif
  }

  bool Speaker_Class::begin(void)
  {
    if (_task_running) { return true; }

#if !defined (SDL_h_)
    if (_task_semaphore == nullptr) { _task_semaphore = xSemaphoreCreateBinary(); }
#endif

    bool res = true;
    if (_cb_set_enabled) { res = _cb_set_enabled(_cb_set_enabled_args, true); }

    res = (ESP_OK == _setup_i2s()) && res;
    if (res)
    {
      _task_running = true;
#if defined (SDL_h_)
      _task_handle = SDL_CreateThread(reinterpret_cast<SDL_ThreadFunction>(spk_task), "spk_task", this);
#else
      size_t stack_size = 1280 + (_cfg.dma_buf_len * sizeof(uint32_t));

#if portNUM_PROCESSORS > 1
      if (_cfg.task_pinned_core < portNUM_PROCESSORS)
      {
        xTaskCreatePinnedToCore(spk_task, "spk_task", stack_size, this, _cfg.task_priority, &_task_handle, _cfg.task_pinned_core);
      }
      else
#endif
      {
        xTaskCreate(spk_task, "spk_task", stack_size, this, _cfg.task_priority, &_task_handle);
      }
#endif
    }

    return res;
  }

  void Speaker_Class::end(void)
  {
    if (_cb_set_enabled) { _cb_set_enabled(_cb_set_enabled_args, false); }
    if (_task_running)
    {
      _task_running = false;
      stop();
      if (_task_handle)
      {
#if defined (SDL_h_)
        SDL_WaitThread(_task_handle, nullptr);
        _task_handle = nullptr;
#else
        xTaskNotifyGive(_task_handle);
        do { vTaskDelay(1); } while (_task_handle);
#endif
      }
    }
    _play_channel_bits.store(0);
    for (size_t ch = 0; ch < sound_channel_max; ++ch)
    {
      auto chinfo = &_ch_info[ch];
      chinfo->wavinfo[0].clear();
      chinfo->wavinfo[1].clear();
    }
  }

  void Speaker_Class::stop(void)
  {
    wav_info_t tmp;
    tmp.stop_current = 1;
    for (size_t ch = 0; ch < sound_channel_max; ++ch)
    {
      auto chinfo = &_ch_info[ch];
      chinfo->wavinfo[chinfo->flip] = tmp;
    }
  }

  void Speaker_Class::stop(uint8_t ch)
  {
    if ((size_t)ch >= sound_channel_max)
    {
      stop();
    }
    else
    {
      wav_info_t tmp;
      tmp.stop_current = 1;
      auto chinfo = &_ch_info[ch];
      chinfo->wavinfo[chinfo->flip] = tmp;
    }
  }

  void Speaker_Class::wav_info_t::clear(void)
  {
    length = 0;
    data = nullptr;
    sample_rate_x256 = 0;
    flg = 0;
    repeat = 0;
  }

  bool Speaker_Class::_set_next_wav(size_t ch, const wav_info_t& wav)
  {
    auto chinfo = &_ch_info[ch];
    uint8_t chmask = 1 << ch;
    if (!wav.stop_current)
    {
      while ((_play_channel_bits.load() & chmask) && (chinfo->wavinfo[chinfo->flip].repeat))
      {
        if (chinfo->wavinfo[!chinfo->flip].repeat == ~0u) { return false; }
#if !defined (SDL_h_)
        xSemaphoreTake(_task_semaphore, 1);
#else
        SDL_Delay(1);
#endif
      }
    }
    chinfo->wavinfo[chinfo->flip] = wav;
    _play_channel_bits.fetch_or(chmask);

#if !defined (SDL_h_)
    xTaskNotifyGive(_task_handle);
#endif
    return true;
  }

  bool Speaker_Class::_play_raw(const void* data, size_t array_len, bool flg_16bit, bool flg_signed, float sample_rate, bool flg_stereo, uint32_t repeat_count, int channel, bool stop_current_sound, bool no_clear_index)
  {
    if (!begin() || (_task_handle == nullptr)) { return true; }
    if (array_len == 0 || data == nullptr) { return true; }
    size_t ch = (size_t)channel;
    if (ch >= sound_channel_max)
    {
      size_t bits = _play_channel_bits.load();
      for (ch = sound_channel_max - 1; ch < sound_channel_max; --ch)
      {
        if (0 == ((bits >> ch) & 1)) { break; }
      }
      if (ch >= sound_channel_max) { return false; }
    }
    wav_info_t info;
    info.data = data;
    info.length = array_len;
    info.repeat = repeat_count ? repeat_count : ~0u;
    info.sample_rate_x256 = sample_rate * SAMPLERATE_MUL;
    info.is_stereo = flg_stereo;
    info.is_16bit = flg_16bit;
    info.is_signed = flg_signed;
    info.stop_current = stop_current_sound;
    info.no_clear_index = no_clear_index;

    return _set_next_wav(ch, info);
  }

  bool Speaker_Class::playWav(const uint8_t* wav_data, size_t data_len, uint32_t repeat, int channel, bool stop_current_sound)
  {
    struct __attribute__((packed)) wav_header_t
    {
      char RIFF[4];
      uint32_t chunk_size;
      char WAVEfmt[8];
      uint32_t fmt_chunk_size;
      uint16_t audiofmt;
      uint16_t channel;
      uint32_t sample_rate;
      uint32_t byte_per_sec;
      uint16_t block_size;
      uint16_t bit_per_sample;
    };
    struct __attribute__((packed)) sub_chunk_t
    {
      char identifier[4];
      uint32_t chunk_size;
      uint8_t data[1];
    };

    auto wav = (wav_header_t*)wav_data;
    /*
    ESP_LOGD("wav", "RIFF           : %.4s" , wav->RIFF          );
    ESP_LOGD("wav", "chunk_size     : %d"   , wav->chunk_size    );
    ESP_LOGD("wav", "WAVEfmt        : %.8s" , wav->WAVEfmt       );
    ESP_LOGD("wav", "fmt_chunk_size : %d"   , wav->fmt_chunk_size);
    ESP_LOGD("wav", "audiofmt       : %d"   , wav->audiofmt      );
    ESP_LOGD("wav", "channel        : %d"   , wav->channel       );
    ESP_LOGD("wav", "sample_rate    : %d"   , wav->sample_rate   );
    ESP_LOGD("wav", "byte_per_sec   : %d"   , wav->byte_per_sec  );
    ESP_LOGD("wav", "block_size     : %d"   , wav->block_size    );
    ESP_LOGD("wav", "bit_per_sample : %d"   , wav->bit_per_sample);
    */
    if ( !wav_data
      || memcmp(wav->RIFF,    "RIFF",     4)
      || memcmp(wav->WAVEfmt, "WAVEfmt ", 8)
      || wav->audiofmt != 1
      || wav->bit_per_sample < 8
      || wav->bit_per_sample > 16
      || wav->channel == 0
      || wav->channel > 2
      )
    {
      return false;
    }

    sub_chunk_t* sub = (sub_chunk_t*)(wav_data + offsetof(wav_header_t, audiofmt) + wav->fmt_chunk_size);
    /*
    ESP_LOGD("wav", "sub id         : %.4s" , sub->identifier);
    ESP_LOGD("wav", "sub chunk_size : %d"   , sub->chunk_size);
    */
    while(memcmp(sub->identifier, "data", 4) && (uint8_t*)sub < wav_data + wav->chunk_size + 8)
    {
      sub = (sub_chunk_t*)((uint8_t*)sub + offsetof(sub_chunk_t, data) + sub->chunk_size);
      /*
      ESP_LOGD("wav", "sub id         : %.4s" , sub->identifier);
      ESP_LOGD("wav", "sub chunk_size : %d"   , sub->chunk_size);
      */
    }
    if (memcmp(sub->identifier, "data", 4))
    {
      return false;
    }

    data_len = data_len > sizeof(wav_header_t) ? data_len - sizeof(wav_header_t) : 0;
    if (data_len > sub->chunk_size) { data_len = sub->chunk_size; }
    bool flg_16bit = (wav->bit_per_sample >> 4);
    return _play_raw( sub->data
                    , data_len >> flg_16bit
                    , flg_16bit
                    , flg_16bit
                    , wav->sample_rate
                    , wav->channel > 1
                    , repeat
                    , channel
                    , stop_current_sound
                    , false
                    );
  }

}
#endif