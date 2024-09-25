#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_Speaker_Class_H__
#define __M5_Speaker_Class_H__

#include "m5unified_common.h"

#if defined ( ESP_PLATFORM )

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2s.h>

#endif

#include <atomic>

#ifndef I2S_PIN_NO_CHANGE
#define I2S_PIN_NO_CHANGE (-1)
#endif

namespace m5
{
  class M5Unified;

  struct speaker_config_t
  {
    /// i2s_data_out (for spk)
    int pin_data_out = I2S_PIN_NO_CHANGE;

    /// i2s_bck
    int pin_bck = I2S_PIN_NO_CHANGE;

    /// i2s_ws (lrck)
    int pin_ws = I2S_PIN_NO_CHANGE;

    /// output sampling rate (Hz)
    uint32_t sample_rate = 48000;

    /// use stereo output
    bool stereo = false;

    /// use single gpio buzzer, ( need only pin_data_out )
    bool buzzer = false;

    /// use DAC speaker, ( need only pin_data_out ) ( for ESP32, only GPIO_NUM_25 or GPIO_NUM_26 )
    /// ※ for ESP32, need `i2s_port = I2S_NUM_0`. ( DAC+I2S_NUM_1 is not available )
    bool use_dac = false;

    /// Zero level reference value when using DAC ( 0=Dynamic change )
    uint8_t dac_zero_level = 0;

    /// multiplier for output value
    uint8_t magnification = 16;

    /// for I2S dma_buf_len (max 1024)
    size_t dma_buf_len = 256;

    /// for I2S dma_buf_count
    size_t dma_buf_count = 8;

    /// background task priority
    uint8_t task_priority = 2;

    /// background task pinned core
    uint8_t task_pinned_core = ~0;

    /// I2S port
    i2s_port_t i2s_port = i2s_port_t::I2S_NUM_0;
  };

  class Speaker_Class
  {
  friend M5Unified;
  public:
    virtual ~Speaker_Class(void) {}

    speaker_config_t config(void) const { return _cfg; }
    void config(const speaker_config_t& cfg) { _cfg = cfg; }

    bool begin(void);

    void end(void);

    bool isRunning(void) const { return _task_running; }

    bool isEnabled(void) const
    {
#if defined (ESP_PLATFORM)
      return _cfg.pin_data_out >= 0;
#else
      return true;
#endif
    }

    /// now in playing or not.
    /// @return false=not playing / true=playing
    bool isPlaying(void) const volatile { return _play_channel_bits.load(); }

    /// now in playing or not.
    /// @param channel virtual channel number. (0~7), (default = automatically selected)
    /// @return 0=not playing / 1=playing (There's room in the queue) / 2=playing (There's no room in the queue.)
    size_t isPlaying(uint8_t channel) const volatile { return (channel < sound_channel_max) ? ((bool)_ch_info[channel].wavinfo[0].repeat) + ((bool)_ch_info[channel].wavinfo[1].repeat) : 0; }

    /// Get the number of channels that are playing.
    /// @return number of channels that are playing.
    size_t getPlayingChannels(void) const volatile { return __builtin_popcount(_play_channel_bits.load()); }

    /// sets the output master volume of the sound.
    /// @param master_volume master volume (0~255)
    void setVolume(uint8_t master_volume) { _master_volume = master_volume; }

    /// gets the output master volume of the sound.
    /// @return master volume.
    uint8_t getVolume(void) const { return _master_volume; }

    /// sets the output volume of the sound for the all virtual channel.
    /// @param volume channel volume (0~255)
    void setAllChannelVolume(uint8_t volume) { for (size_t ch = 0; ch < sound_channel_max; ++ch) { _ch_info[ch].volume = volume; } }

    /// sets the output volume of the sound for the specified virtual channel.
    /// @param channel virtual channel number. (0~7)
    /// @param volume channel volume (0~255)
    void setChannelVolume(uint8_t channel, uint8_t volume) { if (channel < sound_channel_max) { _ch_info[channel].volume = volume; } }

    /// gets the output volume of the sound for the specified virtual channel.
    /// @param channel virtual channel number. (0~7)
    /// @return channel volume.
    uint8_t getChannelVolume(uint8_t channel) const { return (channel < sound_channel_max) ? _ch_info[channel].volume : 0; }

    /// stop sound output.
    void stop(void);

    /// stop sound output for the specified virtual channel.
    /// @param channel virtual channel number. (0~7)
    void stop(uint8_t channel);

    /// play simple tone sound.
    /// @param frequency tone frequency (Hz)
    /// @param duration tone duration (msec)
    /// @param channel virtual channel number. (0~7), (default = automatically selected)
    /// @param stop_current_sound true=start a new output without waiting for the current one to finish.
    /// @param raw_data Single amplitude audio data. 8bit unsigned wav.
    /// @param array_len size of raw_data.
    /// @param stereo true=data is stereo / false=data is mono.
    bool tone(float frequency, uint32_t duration, int channel, bool stop_current_sound, const uint8_t* raw_data, size_t array_len, bool stereo = false)
    {
      return _play_raw(raw_data, array_len, false, false, frequency * (array_len >> stereo), stereo, (duration != UINT32_MAX) ? (uint32_t)(duration * frequency / 1000) : UINT32_MAX, channel, stop_current_sound, true);
    }

    /// play simple tone sound.
    /// @param frequency tone frequency (Hz)
    /// @param duration tone duration (msec)
    /// @param channel virtual channel number. (0~7), (default = automatically selected)
    bool tone(float frequency, uint32_t duration = UINT32_MAX, int channel = -1, bool stop_current_sound = true) { return tone(frequency, duration, channel, stop_current_sound, _default_tone_wav, sizeof(_default_tone_wav), false); }

    /// play raw sound wave data. (for signed 8bit wav data)
    /// @param raw_data wave data.
    /// @param array_len Number of data array elements.
    /// @param sample_rate the sampling rate (Hz) (default = 44100)
    /// @param stereo true=data is stereo / false=data is monaural.
    /// @param repeat number of times played repeatedly. (default = 1)
    /// @param channel virtual channel number (If omitted, use an available channel.)
    /// @param stop_current_sound true=start a new output without waiting for the current one to finish.
    /// @attention If you want to use the data generated at runtime, you can either have three buffers and use them in sequence, or have two buffers and use them alternately, then split them in half and call playRaw twice.
    /// @attention If noise is present in the output sounds, consider increasing the priority of the task that generates the data.
    bool playRaw(const int8_t* raw_data, size_t array_len, uint32_t sample_rate = 44100, bool stereo = false, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false)
    {
      return _play_raw(static_cast<const void* >(raw_data), array_len, false, true, sample_rate, stereo, repeat, channel, stop_current_sound, false);
    }
    [[deprecated("The playRAW function has been renamed to playRaw")]]
    bool playRAW(const int8_t* raw_data, size_t array_len, uint32_t sample_rate = 44100, bool stereo = false, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false)
    {
      return _play_raw(static_cast<const void* >(raw_data), array_len, false, true, sample_rate, stereo, repeat, channel, stop_current_sound, false);
    }

    /// play raw sound wave data. (for unsigned 8bit wav data)
    /// @param raw_data wave data.
    /// @param array_len Number of data array elements.
    /// @param sample_rate the sampling rate (Hz) (default = 44100)
    /// @param stereo true=data is stereo / false=data is monaural.
    /// @param repeat number of times played repeatedly. (default = 1)
    /// @param channel virtual channel number (If omitted, use an available channel.)
    /// @param stop_current_sound true=start a new output without waiting for the current one to finish.
    /// @attention If you want to use the data generated at runtime, you can either have three buffers and use them in sequence, or have two buffers and use them alternately, then split them in half and call playRaw twice.
    /// @attention If noise is present in the output sounds, consider increasing the priority of the task that generates the data.
    bool playRaw(const uint8_t* raw_data, size_t array_len, uint32_t sample_rate = 44100, bool stereo = false, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false)
    {
      return _play_raw(static_cast<const void* >(raw_data), array_len, false, false, sample_rate, stereo, repeat, channel, stop_current_sound, false);
    }
    [[deprecated("The playRAW function has been renamed to playRaw")]]
    bool playRAW(const uint8_t* raw_data, size_t array_len, uint32_t sample_rate = 44100, bool stereo = false, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false)
    {
      return _play_raw(static_cast<const void* >(raw_data), array_len, false, false, sample_rate, stereo, repeat, channel, stop_current_sound, false);
    }

    /// play raw sound wave data. (for signed 16bit wav data)
    /// @param raw_data wave data.
    /// @param array_len Number of data array elements.
    /// @param sample_rate the sampling rate (Hz) (default = 44100)
    /// @param stereo true=data is stereo / false=data is monaural.
    /// @param repeat number of times played repeatedly. (default = 1)
    /// @param channel virtual channel number (If omitted, use an available channel.)
    /// @param stop_current_sound true=start a new output without waiting for the current one to finish.
    /// @attention If you want to use the data generated at runtime, you can either have three buffers and use them in sequence, or have two buffers and use them alternately, then split them in half and call playRaw twice.
    /// @attention If noise is present in the output sounds, consider increasing the priority of the task that generates the data.
    bool playRaw(const int16_t* raw_data, size_t array_len, uint32_t sample_rate = 44100, bool stereo = false, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false)
    {
      return _play_raw(static_cast<const void* >(raw_data), array_len, true, true, sample_rate, stereo, repeat, channel, stop_current_sound, false);
    }

    /// @deprecated "playRAW" function has been renamed to "playRaw"
    [[deprecated("The playRAW function has been renamed to playRaw")]]
    bool playRAW(const int16_t* raw_data, size_t array_len, uint32_t sample_rate = 44100, bool stereo = false, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false)
    {
      return _play_raw(static_cast<const void* >(raw_data), array_len, true, true, sample_rate, stereo, repeat, channel, stop_current_sound, false);
    }

    /// play WAV format sound data.
    /// @param wav_data wave data. (WAV header included)
    /// @param repeat number of times played repeatedly. (default = 1)
    /// @param channel virtual channel number (If omitted, use an available channel.)
    /// @param stop_current_sound true=start a new output without waiting for the current one to finish.
    bool playWav(const uint8_t* wav_data, size_t data_len = ~0u, uint32_t repeat = 1, int channel = -1, bool stop_current_sound = false);

  protected:

    static constexpr const size_t sound_channel_max = 8;

    static const uint8_t _default_tone_wav[16];

    void setCallback(void* args, bool(*func)(void*, bool)) { _cb_set_enabled = func; _cb_set_enabled_args = args; }

    struct wav_info_t
    {
      volatile uint32_t repeat = 0;   /// -1 mean infinity repeat
      uint32_t sample_rate_x256 = 0;
      const void* data = nullptr;
      size_t length = 0;
      union
      {
        volatile uint8_t flg = 0;
        struct
        {
          uint8_t is_stereo      : 1;
          uint8_t is_16bit       : 1;
          uint8_t is_signed      : 1;
          uint8_t stop_current   : 1;
          uint8_t no_clear_index : 1;
        };
      };
      void clear(void);
    };

    struct channel_info_t
    {
      wav_info_t wavinfo[2]; // current/next flip info.
      size_t index = 0;
      int diff = 0;
      volatile uint8_t volume = 255; // channel volume (not master volume)
      volatile bool flip = false;

      float liner_buf[2][2] = { { 0, 0 }, { 0, 0 } };
    };

    channel_info_t _ch_info[sound_channel_max];

    static void spk_task(void* args);

    esp_err_t _setup_i2s(void);
    bool _play_raw(const void* wav, size_t array_len, bool flg_16bit, bool flg_signed, float sample_rate, bool flg_stereo, uint32_t repeat_count, int channel, bool stop_current_sound, bool no_clear_index);
    bool _set_next_wav(size_t ch, const wav_info_t& wav);

    speaker_config_t _cfg;
    volatile uint8_t _master_volume = 64;

    bool (*_cb_set_enabled)(void* args, bool enabled) = nullptr;
    void* _cb_set_enabled_args = nullptr;

    volatile bool _task_running = false;
    std::atomic<uint16_t> _play_channel_bits = { 0 };
#if defined (SDL_h_)
    SDL_Thread* _task_handle = nullptr;
#else
    TaskHandle_t _task_handle = nullptr;
    volatile SemaphoreHandle_t _task_semaphore = nullptr;
#endif
  };
}

#endif
#endif