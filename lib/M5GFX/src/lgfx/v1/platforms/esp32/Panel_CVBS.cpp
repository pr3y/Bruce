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

Inspiration Sources:
 [Roger Cheng](https://github.com/Roger-random/ESP_8_BIT_composite)
 [rossum](https://github.com/rossumur/esp_8_bit)
/----------------------------------------------------------------------------*/

#if defined (ESP_PLATFORM)
#include <sdkconfig.h>
#if !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_types.h>
#include <esp_log.h>
#include <driver/dac.h>
#include <soc/rtc.h>
#include <soc/periph_defs.h>
#include <soc/i2s_struct.h>

#if __has_include(<esp32/rom/lldesc.h>)
 #include <esp32/rom/lldesc.h>
#else
 #include <rom/lldesc.h>
#endif

#if __has_include(<esp_chip_info.h>)
 #include <esp_chip_info.h>
#endif

#if __has_include(<driver/i2s_std.h>)
 #include <driver/i2s_std.h>
 #define LGFX_I2S_STD_ENABLED
#else
 #include <driver/i2s.h>
#endif
#if __has_include(<esp_private/periph_ctrl.h>)
 // ESP-IDF v5
 #include <esp_private/periph_ctrl.h>
#endif

#include <math.h>

#include "Panel_CVBS.hpp"
#include "common.hpp"
#include "../../misc/pixelcopy.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr const char *TAG = "Panel_CVBS";
//debug
  #define ISR_BEGIN()
  #define ISR_END()
  #define MEMCPY_BEGIN()
  #define MEMCPY_END()

  // PSRAM使用時、表示内容を先行してSRAMにmemcpyするためのクラス;
  class scanline_cache_t
  {
  public:
    static constexpr size_t cache_num = 8;  // 先読み保持可能なデータ数;

    int prev_index;

    typedef void(*tasktype)(void*);

    bool begin(size_t line_width, UBaseType_t task_priority, BaseType_t task_pinned_core)
    {
      _datasize = line_width;
      _buffer = (uint8_t*)heap_alloc_dma((line_width * cache_num + 3) & ~3u);
// printf("scanline_cache: %08x alloc\n", _buffer);
      if (_buffer == nullptr) { return false; }
      memset(_src, 0, sizeof(_src));
      _push_idx = 0;
      _using_idx = cache_num - 1;
      prev_index = 0;
      if (task_pinned_core >= portNUM_PROCESSORS)
      {
        task_pinned_core = (xPortGetCoreID() + 1) % portNUM_PROCESSORS;
      }
      xTaskCreatePinnedToCore(task_memcpy, "task_memcpy", 2048, this, task_priority, &_task_handle, task_pinned_core);
      return true;
    }

    void end(void)
    {
      if (_buffer == nullptr) { return; }

      if (_task_handle)
      {
        _datasize = 0;
        xTaskNotify(_task_handle, false, eNotifyAction::eSetValueWithOverwrite);
        _task_handle = nullptr;
      }

// printf("scanline_cache: %08x free\n", _buffer);
      heap_free(_buffer);
      _buffer = nullptr;
    }

    inline bool IRAM_ATTR prepare(const uint8_t* ptr)
    {
      if (_using_idx == _push_idx) { return false; }

      for (size_t i = 0; i < cache_num; ++i)
      {
        if (_src[i] == ptr) { return true; }
      }

      _src[_push_idx] = ptr;
      _push_idx = (_push_idx + 1) & (cache_num - 1);

      BaseType_t flg = pdFALSE;
      xTaskNotifyFromISR(_task_handle, true, eNotifyAction::eSetValueWithOverwrite, &flg);

      return (_using_idx != _push_idx);
    }

    inline const uint8_t* IRAM_ATTR get(const uint8_t* ptr)
    {
      size_t idx = _using_idx;
      size_t idx_e = idx;
      while (_src[idx] != ptr)
      {
        idx = (idx + 1) & (cache_num - 1);
        if (idx == idx_e) { break; }
      }
      _using_idx = idx;
      return &_buffer[idx * _datasize];
    }

  private:
    uint8_t* _buffer = nullptr;    // 先読みバッファ(memcpy先アドレス)
    const uint8_t* _src[cache_num] = { nullptr }; // キューアドレス(memcpy元アドレス)
    TaskHandle_t _task_handle = nullptr;
    size_t _datasize;     // データサイズ(memcpyする量)
    uint8_t _push_idx;    // 新規予約代入先インデクス;
    uint8_t _using_idx;   // 使用中インデクス;

    static void task_memcpy(void* args)
    { /// memcpy from PSRAM to SRAM task;
      auto me = (scanline_cache_t*)args;

      size_t pop_idx = 0;
      while (ulTaskNotifyTake( true, portMAX_DELAY ))
      {
        MEMCPY_BEGIN()
        while (pop_idx != me->_push_idx)
        {
          auto src = me->_src[pop_idx];
          if (src)
          {
            memcpy(&(me->_buffer[me->_datasize * pop_idx]), src, me->_datasize);
            pop_idx = (pop_idx + 1) & (cache_num - 1);
          }
        }
        MEMCPY_END()
      }
      vTaskDelete(nullptr);
    }
  };

  struct internal_t
  {
    static constexpr const uint8_t dma_desc_count = 2;
    uint8_t** lines = nullptr;        // フレームバッファ配列ポインタ;
    uint16_t* allocated_list = nullptr;  // フレームバッファのalloc割当対象のインデクス番号(free時に使用);
    uint32_t* palette = nullptr;   // RGB332から波形に変換するためのテーブル;
    void (*fp_blit)(uint32_t*, const uint8_t*, const uint8_t*, const uint32_t*, int, int);
    uint32_t burst_wave[2];       // カラーバースト信号の波形データ(EVENとODDで２通り)
    intr_handle_t isr_handle = nullptr;
    lldesc_t dma_desc[dma_desc_count];
    int32_t mul_ratio = 0;
    int16_t offset_y;
    uint16_t memory_height;
    uint16_t panel_height;
    uint16_t panel_width;
    uint16_t leftside_index;
    volatile uint16_t current_scanline = 0;
    volatile uint16_t converted_scanline = 0;
    uint16_t BLANKING_LEVEL;
    uint16_t BLACK_LEVEL;
    uint16_t WHITE_LEVEL;
    uint8_t burst_shift = 0;        // カラーバースト信号の反転・位相ずらし処理状態保持用;
    uint8_t use_psram = 0;          // フレームバッファ PSRAM使用モード 0=不使用 / 1=半分PSRAM / 2=全部PSRAM
    uint8_t pixel_per_bytes = 1;
    static constexpr uint8_t SYNC_LEVEL = 0;
  };

  static scanline_cache_t _scanline_cache;
  static internal_t internal;


  static uint32_t setup_palette_ntsc_inner(uint32_t rgb, uint32_t diff_level, uint32_t base_level, float satuation_base, float chroma_scale)
  {
// NTSCの I・Q信号は基準位相から-147度ずれている。;
// 加えて、このライブラリのburst_waveの位相基準は-45度となっている。;
// この両者を合わせて 147+45=192 を引いた値が基準位相となる。;
// つまり 360-192 = 168度を基準とする。;
    static constexpr float BASE_RAD = (M_PI * 168) / 180; // 2.932153;

    uint32_t r = rgb >> 16;
    uint32_t g = (rgb >> 8) & 0xFF;
    uint32_t b = rgb & 0xFF;

    float y = r * 0.299f + g * 0.587f + b * 0.114f;
    float i = (b - y) * -0.2680f + (r - y) * 0.7358f;
    float q = (b - y) *  0.4127f + (r - y) * 0.4778f;
    y = y * diff_level / 256 + base_level;

    float phase_offset = atan2f(i, q) + BASE_RAD;
    float saturation = sqrtf(i * i + q * q) * chroma_scale;
    saturation = saturation * satuation_base;

    uint8_t buf[4];
    uint8_t frac[4];
    int frac_total = 0;
    for (int j = 0; j < 4; j++)
    {
      int tmp = ((int)(y + sinf(phase_offset + (float)M_PI / 2 * j) * saturation));
      frac[j] = tmp & 0xFF;
      frac_total += frac[j];
      tmp >>= 8;
      buf[j] = tmp < 0 ? 0 : tmp > 255 ? 255 : tmp;
    }
    // 切り捨てた端数分を補正する
    while (frac_total > 128)
    {
      frac_total -= 256;
      int target_idx = 0;
      const uint8_t idxtbl[] = { 0, 2, 1, 3 };
      for (int j = 1; j < 4; j++)
      {
        if (frac[idxtbl[j]] > frac[target_idx] || frac[target_idx] == 255) {
          target_idx = idxtbl[j];
        }
      }
      if (buf[target_idx] == 255) { break; }
      buf[target_idx]++;
      frac[target_idx] = 0;
    }
    // I2Sに渡す際に処理負荷を軽減できるよう、予めバイトスワップ等を行ったテーブルを作成しておく;
    return buf[0] << 24
          | buf[1] <<  8
          | buf[2] << 16
          | buf[3] <<  0
          ;
  }

  static void setup_palette_ntsc_565(uint32_t* palette, uint_fast16_t white_level, uint_fast16_t black_level, uint_fast8_t chroma_level)
  {
    float chroma_scale = chroma_level / 7168.0f;
    float satuation_base = black_level / 2;
    uint32_t diff_level = white_level - black_level;

    uint32_t base_level = black_level / 2;
    for (int idx = 0; idx < 256; ++idx)
    {
      { // RGB565の上位1Byteに対するテーブル
        int r = (idx >> 3);
        int g = (idx & 7) << 3;
        r = (r * 0x21) >> 2;
        g = (g * 0x41) >> 4;
        palette[idx << 1] = setup_palette_ntsc_inner(r<<16|g<<8, diff_level, base_level, satuation_base, chroma_scale);
      }
      { // RGB565の下位1Byteに対するテーブル
        int g = idx >> 5;
        int b = idx & 0x1F;
        b = (b * 0x21) >> 2;
        g = (g * 0x41) >> 4;
        palette[(idx << 1) + 1] = setup_palette_ntsc_inner(g<<8|b, diff_level, base_level, satuation_base, chroma_scale);
      }
    }
  }

  static void setup_palette_ntsc_332(uint32_t* palette, uint_fast16_t white_level, uint_fast16_t black_level, uint_fast8_t chroma_level)
  {
    float chroma_scale = chroma_level / 7168.0f;
    float satuation_base = black_level / 2;
    uint32_t diff_level = white_level - black_level;

    for (int rgb332 = 0; rgb332 < 256; ++rgb332)
    {
      int r = (( rgb332 >> 5)         * 0x49) >> 1;
      int g = (((rgb332 >> 2) & 0x07) * 0x49) >> 1;
      int b = (( rgb332       & 0x03) * 0x55);

      palette[rgb332] = setup_palette_ntsc_inner(r<<16|g<<8|b, diff_level, black_level, satuation_base, chroma_scale);
    }
  }

  static void setup_palette_ntsc_gray(uint32_t* palette, uint_fast16_t white_level, uint_fast16_t black_level, uint_fast8_t chroma_level)
  {
    float chroma_scale = chroma_level / 7168.0f;
    float satuation_base = black_level / 2;
    uint32_t diff_level = white_level - black_level;

    for (int idx = 0; idx < 256; ++idx)
    {
      palette[idx] = setup_palette_ntsc_inner(idx<<16|idx<<8|idx, diff_level, black_level, satuation_base, chroma_scale);
    }
  }

  static void setup_palette_pal_inner(uint8_t *result, uint32_t rgb, int diff_level, float base_level, float chroma_scale)
  {
    static constexpr const int8_t sin_tbl[5] = { 0, -1, 0, 1, 0 };

    // I2Sに渡す際に処理負荷を軽減できるよう、予めバイトスワップされたテーブルを作成するため、インデクス順を入れ替える
    static constexpr const int8_t idx_tbl[4] = { 3, 1, 2, 0 };
    uint32_t r = rgb >> 16;
    uint32_t g = (rgb >> 8) & 0xFF;
    uint32_t b = rgb & 0xFF;

    float y = r * 0.299f + g * 0.587f + b * 0.114f;
    float u = -0.147407 * r - 0.289391 * g + 0.436798 * b;
    float v =  0.614777 * r - 0.514799 * g - 0.099978 * b;
    y = y * diff_level / 256 + base_level;
    u *= chroma_scale;
    v *= chroma_scale;

    uint8_t frac[8];
    int frac_total[2] = {0,0};

    for (int j = 0; j < 4; j++)
    {
      float s = u * sin_tbl[j    ];
      float c = v * sin_tbl[j + 1]; // cos
      int i = idx_tbl[j];
      int tmp = ((int)(y + s + c));
      frac[i  ] = tmp & 0xFF;
      frac_total[0] += frac[i  ];
      tmp >>= 8;
      result[i  ] = tmp < 0 ? 0 : tmp > 255 ? 255 : tmp;
      tmp = ((int)(y + s - c));
      i += 4;
      frac[i] = tmp & 0xFF;
      frac_total[1] += frac[i];
      tmp >>= 8;
      result[i] = tmp < 0 ? 0 : tmp > 255 ? 255 : tmp;
    }

    // 切り捨てた端数分を補正する
    for (int i = 0; i < 2; ++i) {
      while (frac_total[i] > 128)
      {
        frac_total[i] -= 256;
        int target_idx = i*4;
        for (int j = 1; j < 4; j++)
        {
          if (frac[j+i*4] > frac[target_idx] || frac[target_idx] == 255) {
            target_idx = j+i*4;
          }
        }
        if (result[target_idx] == 255) { break; }
        result[target_idx]++;
        frac[target_idx] = 0;
      }
    }
  }

  static void setup_palette_pal_565(uint32_t* palette, uint_fast16_t white_level, uint_fast16_t black_level, uint_fast8_t chroma_level)
  {
    auto e = palette;
    auto o = &palette[512];

    uint32_t result_buf[2];
    float chroma_scale = black_level * chroma_level / 14336.0f;

    int32_t diff_level = white_level - black_level;
    float base_level = (float)black_level / 2;
    for (int idx = 0; idx < 256; ++idx)
    {
      { // RGB565の上位1Byteに対するテーブル
        int r = (idx >> 3);
        int g = (idx & 7) << 3;
        r = (r * 0x21) >> 2;
        g = (g * 0x41) >> 4;

        setup_palette_pal_inner((uint8_t*)result_buf, r<<16|g<<8, diff_level, base_level, chroma_scale);
        e[idx << 1] = result_buf[0];
        o[idx << 1] = result_buf[1];
      }
      { // RGB565の下位1Byteに対するテーブル
        int g = idx >> 5;
        int b = idx & 0x1F;
        b = (b * 0x21) >> 2;
        g = (g * 0x41) >> 4;

        setup_palette_pal_inner((uint8_t*)result_buf, g<<8|b, diff_level, base_level, chroma_scale);
        e[(idx << 1) + 1] = result_buf[0];
        o[(idx << 1) + 1] = result_buf[1];
      }
    }
  }

  static void setup_palette_pal_332(uint32_t* palette, uint_fast16_t white_level, uint_fast16_t black_level, uint_fast8_t chroma_level)
  {
    auto e = palette;
    auto o = &palette[256];

    uint32_t result_buf[2];
    float chroma_scale = black_level * chroma_level / 14336.0f;

    int32_t diff_level = white_level - black_level;
    float base_level = (float)black_level;

    for (int rgb332 = 0; rgb332 < 256; ++rgb332)
    {
      int r = (( rgb332 >> 5)         * 0x49) >> 1;
      int g = (((rgb332 >> 2) & 0x07) * 0x49) >> 1;
      int b = (( rgb332       & 0x03) * 0x55);

      setup_palette_pal_inner((uint8_t*)result_buf, r<<16|g<<8|b, diff_level, base_level, chroma_scale);

      e[rgb332] = result_buf[0];
      o[rgb332] = result_buf[1];
    }
  }

  static void setup_palette_pal_gray(uint32_t* palette, uint_fast16_t white_level, uint_fast16_t black_level, uint_fast8_t chroma_level)
  {
    auto e = palette;
    auto o = &palette[256];

    uint32_t result_buf[2];
    float chroma_scale = black_level * chroma_level / 14336.0f;

    int32_t diff_level = white_level - black_level;
    float base_level = (float)black_level;

    for (int idx = 0; idx < 256; ++idx)
    {
      setup_palette_pal_inner((uint8_t*)result_buf, idx<<16|idx<<8|idx, diff_level, base_level, chroma_scale);

      e[idx] = result_buf[0];
      o[idx] = result_buf[1];
    }
  }

  struct signal_spec_info_t
  {
    static constexpr const size_t sync_proc_count = 12;
    uint16_t total_scanlines;     // 走査線数(２フィールド、１フレーム);
    uint16_t scanline_width;      // 走査線内のサンプル数 (カラークロック数 x4);
    uint8_t hsync_equalizing;     // 等化パルス幅;
    uint8_t hsync_short;          // 水平同期期間のSYNC幅;
    uint16_t hsync_long;          // 垂直同期期間のSYNC幅;
    uint8_t burst_start;
    uint8_t burst_cycle;          // バースト信号の数;
    uint8_t active_start;
    uint8_t burst_shift_mask;
    uint16_t display_width;       // X方向 表示可能ピクセル数;
    uint16_t display_height;      // Y方向 表示可能ピクセル数;
    uint8_t sync_proc[2][sync_proc_count];     // 垂直同期期間の処理内容テーブル 偶数行・奇数行で2要素,各要素12ライン分;
    uint8_t vsync_lines;          // 垂直同期期間(表示期間外)の走査線数(単フィールド分)
  };

  static signal_spec_info_t _signal_spec_info;

  static constexpr const signal_spec_info_t signal_spec_info_list[]
  { // NTSC
    { 525         // 走査線525本;
    , 910         // 1走査線あたり 227.5 x4 sample
    , 32          // equalizing = 32 sample (2.3us)
    , 66          // hsync_short = 66 sample (4.7us)
    , 380         // hsync_long = 380 sample
    , 76          // burst start = 76 sample
    , 9           // burst cycle = 9 cycle
    , 148         // active_start = 148 sample (10.8us)
    , 2           // burst_shift_mask バースト信号反転動作;
    , 720         // width max 720
    , 480         // height max 480
    , { { 0x55, 0x55, 0x00, 0x22, 0x22, 0x00, 0x55, 0x55, 0x00, 0xB0, 0xB0, 0x00 } // NTSC EVEN
      , { 0x05, 0x55, 0x50, 0x02, 0x22, 0x20, 0x05, 0x55, 0x50, 0x04, 0xB0, 0xB0 } // NTSC ODD
      }
    , 22
    }
  , // NTSC_J
    { 525         // 走査線525本;
    , 910         // 1走査線あたり 227.5 x4 sample
    , 32          // equalizing = 32 sample (2.3us)
    , 66          // hsync_short = 66 sample (4.7us)
    , 380         // hsync_long = 380 sample
    , 76          // burst start = 76 sample
    , 9           // burst cycle = 9 cycle
    , 148         // active_start = 148 sample (10.8us)
    , 2           // burst_shift_mask バースト信号反転動作;
    , 720         // width max 720
    , 480         // height max 480
    , { { 0x55, 0x55, 0x00, 0x22, 0x22, 0x00, 0x55, 0x55, 0x00, 0xB0, 0xB0, 0x00 } // NTSC EVEN
      , { 0x05, 0x55, 0x50, 0x02, 0x22, 0x20, 0x05, 0x55, 0x50, 0x04, 0xB0, 0xB0 } // NTSC ODD

      }
    , 22
    }
  , // PAL
    { 625         // 走査線625本;
    , 1136        // 1走査線あたり 284 x4 sample (正確には283.75x4 = 1135だが、2の倍数でないとI2S出力できないため1136とする)
    , 40          // equalizing = 40 sample (2.3us)
    , 84          // hsync_shor = 84 sample (4.7us)
    , 484         // hsync_long 484 sample
    , 98          // burst start = 98 sample (5.6us)
    , 10          // burst cycle = 10 cycle
    , 216         // active_start = 216 sample (12.0us)
    , 1           // burst_shift_mask パレットインデクス変更動作;
    , 864         // max width 864
    , 576         // max height 576
    , { { 0x05, 0x55, 0x50, 0x22, 0x22, 0x05, 0x55, 0x50, 0x34, 0xB0, 0xB0, 0x00 } // PAL EVEN
      , { 0x00, 0x55, 0x55, 0x02, 0x22, 0x20, 0x55, 0x55, 0x04, 0xB0, 0xB0, 0x00 } // PAL ODD
      }
    , 25
    }
  , // PAL_M  (PAL_M方式は周波数等がNTSCと共通、カラー情報の仕様がPALと共通)
    { 525         // 走査線525本;
    , 908         // 1走査線あたり 227.5 x4 sample
    , 32          // equalizing = 32 sample (2.3us)
    , 66          // hsync_short = 66 sample (4.7us)
    , 380         // hsync_long = 380 sample
    , 80          // burst start = 84 sample
    , 9           // burst cycle = 9 cycle
    , 148         // active_start = 148 sample (10.8us)
    , 1           // burst_shift_mask パレットインデクス変更動作;
    , 720         // width max 720
    , 480         // height max 480
    , { { 0x55, 0x55, 0x00, 0x22, 0x22, 0x00, 0x55, 0x55, 0x00, 0xB0, 0xB0, 0x00 } // NTSC EVEN
      , { 0x05, 0x55, 0x50, 0x02, 0x22, 0x20, 0x05, 0x55, 0x50, 0x04, 0xB0, 0xB0 } // NTSC ODD
      }
    , 22
    }
  , // PAL_N
    { 625         // 走査線625本;
    , 916
    , 32
    , 66
    , 380
    , 80
    , 9           // burst cycle = 9 cycle
    , 156
    , 1           // burst_shift_mask パレットインデクス変更動作;
    , 720         // max width 720
    , 576         // max height 576
    , { { 0x05, 0x55, 0x50, 0x22, 0x22, 0x05, 0x55, 0x50, 0x34, 0xB0, 0xB0, 0x00 } // PAL EVEN
      , { 0x00, 0x55, 0x55, 0x02, 0x22, 0x20, 0x55, 0x55, 0x04, 0xB0, 0xB0, 0x00 } // PAL ODD
      }
    , 25
    }
  };

  struct signal_setup_info_t
  {
    void (*setup_palette_332)(uint32_t*, uint_fast16_t, uint_fast16_t, uint_fast8_t); // RGB332用パレット生成関数のポインタ;
    void (*setup_palette_565)(uint32_t*, uint_fast16_t, uint_fast16_t, uint_fast8_t); // RGB565用パレット生成関数のポインタ;
    void (*setup_palette_gray)(uint32_t*, uint_fast16_t, uint_fast16_t, uint_fast8_t); // グレースケール用パレット生成関数のポインタ;
    uint16_t blanking_mv;         // SYNCレベルとBLANKINGレベルの電圧差 mV
    uint16_t black_mv;            // SYNCレベルと黒レベルの電圧差 mV
    uint16_t white_mv;            // SYNCレベルと白レベルの電圧差 mV
    uint8_t palette_num_256;      // パレット面数 (palはODD_EVENで2倍使用する);
    uint8_t sdm0;
    uint8_t sdm1;
    uint8_t sdm2;
    uint8_t div_n;
    uint8_t div_b;
    uint8_t div_a;
  };

/*
  PAL   = 4.43361875
  NTSC  = 3.579545
  SECAM = 4.406250
  PAL_M = 3.57561149
  PAL_N = 3.58205625
*/

  static constexpr const signal_setup_info_t signal_setup_info_list[]
  { // NTSC
    { setup_palette_ntsc_332
    , setup_palette_ntsc_565
    , setup_palette_ntsc_gray
    , 286         // 286mV = 0IRE
    , 340         // 340mV = 7.5IRE  米国仕様では黒レベルは 7.5IRE
    , 960         // 960mV  黄色の振幅の最大値が100IRE付近になるよう、白レベルは100IREよりも低く調整しておく;
    , 1           // パレット数は256
    // APLL設定 14.318237 映像に縞模様ノイズが出にくい;
    //  意図的に要求仕様を外している。 ( 0x049746 = 14.318181 = 3.579545 x4 // 要求仕様に近い )
    , 0x48, 0x97, 0x04
    // CLKDIV設定 (ESP32 rev0用)
    , 5, 10, 17
    }
  , // NTSC_J
    { setup_palette_ntsc_332
    , setup_palette_ntsc_565
    , setup_palette_ntsc_gray
    , 286         // 286mV = 0IRE
    , 286         // 286mV = 0IRE  日本仕様では黒レベルは 0IRE
    , 960
    , 1           // パレット数は256
    // APLL設定 14.318237 映像に縞模様ノイズが出にくい;
    //  意図的に要求仕様を外している。 ( 0x049746 = 14.318181 = 3.579545 x4 // 要求仕様に近い )
    , 0x48, 0x97, 0x04
    // CLKDIV設定 (ESP32 rev0用)
    , 5, 10, 17
    }
  , // PAL
    { setup_palette_pal_332
    , setup_palette_pal_565
    , setup_palette_pal_gray
    , 300
    , 300
    , 960
    , 2           // パレット数は512
    // APLL設定 17.734476mhz ~4x   4.43361875 x4
    , 0x04, 0xA4, 0x06
    // CLKDIV設定 (ESP32 rev0用)
    , 4, 24, 47
    }
  , // PAL_M
    { setup_palette_pal_332
    , setup_palette_pal_565
    , setup_palette_pal_gray
    , 300
    , 300
    , 960
    , 2           // パレット数は512
    // APLL設定
    , 0xDA, 0x94, 0x04
    // CLKDIV設定 (ESP32 rev0用)
    , 5, 19, 32
    }
  , // PAL_N
    { setup_palette_pal_332
    , setup_palette_pal_565
    , setup_palette_pal_gray
    , 300
    , 300
    , 960
    , 2           // パレット数は512
    // APLL設定 // 3.58205625 x4
    , 0xD1, 0x98, 0x04
    // CLKDIV設定 (ESP32 rev0用)
    , 5, 7, 12
    }
  };

#if 1  // 1:asm / 0:cpp   switch

// a6 = シフト量反転 SARレジスタと入替、シフト量を 8 or 0 で変化させる
// a9 = ratio diff
#define ASM_INIT_BLIT \
    "ssl        a6                      \n" \
    "addi       a6, a6, 24              \n" \
    "srai       a9, a7, 1               \n" \
    "addmi      a9, a9, -16384          \n"

#define ASM_READ_RGB332_2PIXEL \
    "l8ui       a10,a3, 0               \n" \
    "l8ui       a11,a3, 1               \n" \
    "addi       a3, a3, 2               \n" \
    "addx4      a10,a10,a5              \n" \
    "l32i       a10,a10,0               \n" \
    "addx4      a11,a11,a5              \n" \
    "l32i       a11,a11,0               \n"

#define ASM_READ_RGB565_2PIXEL \
    "l8ui       a10,a3, 0               \n" \
    "l8ui       a12,a3, 1               \n" \
    "l8ui       a11,a3, 2               \n" \
    "l8ui       a13,a3, 3               \n" \
    "addx8      a10,a10,a5              \n" \
    "l32i       a10,a10,0               \n" \
    "addx8      a12,a12,a5              \n" \
    "l32i       a12,a12,4               \n" \
    "addx8      a11,a11,a5              \n" \
    "l32i       a11,a11,0               \n" \
    "addx8      a13,a13,a5              \n" \
    "l32i       a13,a13,4               \n" \
    "addi       a3, a3, 4               \n" \
    "add        a10,a10,a12             \n" \
    "add        a11,a11,a13             \n"

#define ASM_READ_RGB332_4PIXEL \
    "l8ui       a10,a3, 0               \n" \
    "l8ui       a11,a3, 1               \n" \
    "l8ui       a12,a3, 2               \n" \
    "l8ui       a13,a3, 3               \n" \
    "addx4      a10,a10,a5              \n" \
    "l32i       a10,a10,0               \n" \
    "addx4      a11,a11,a5              \n" \
    "l32i       a11,a11,0               \n" \
    "addx4      a12,a12,a5              \n" \
    "l32i       a12,a12,0               \n" \
    "addx4      a13,a13,a5              \n" \
    "l32i       a13,a13,0               \n" \
    "addi       a3, a3, 4               \n"

#define ASM_READ_RGB565_4PIXEL \
    "l8ui       a12,a3, 1               \n" \
    "l8ui       a10,a3, 0               \n" \
    "l8ui       a13,a3, 3               \n" \
    "l8ui       a11,a3, 2               \n" \
    "addx8      a12,a12,a5              \n" \
    "l32i       a12,a12,4               \n" \
    "addx8      a10,a10,a5              \n" \
    "l32i       a10,a10,0               \n" \
    "addx8      a13,a13,a5              \n" \
    "l32i       a13,a13,4               \n" \
    "addx8      a11,a11,a5              \n" \
    "l32i       a11,a11,0               \n" \
    "l8ui       a14,a3, 5               \n" \
    "add        a10,a10,a12             \n" \
    "l8ui       a12,a3, 4               \n" \
    "add        a11,a11,a13             \n" \
    "l8ui       a15,a3, 7               \n" \
    "l8ui       a13,a3, 6               \n" \
    "addx8      a14,a14,a5              \n" \
    "l32i       a14,a14,4               \n" \
    "addx8      a12,a12,a5              \n" \
    "l32i       a12,a12,0               \n" \
    "addx8      a15,a15,a5              \n" \
    "l32i       a15,a15,4               \n" \
    "addx8      a13,a13,a5              \n" \
    "l32i       a13,a13,0               \n" \
    "addi       a3, a3, 8               \n" \
    "add        a12,a12,a14             \n" \
    "add        a13,a13,a15             \n"



/* blit_関数が呼び出された直後のレジスタの値
    a0 : リターンアドレス     (使用しない)
    a1 : スタックポインタ     (変更不可)
    a2 : uint32_t d           (ループ中で加算しながら利用する)
    a3 : const uint8_t* s     (ループ中で加算しながら利用する)
    a4 : size_t src_length    (ループ回数として設定後、別用途に利用)
    a5 : const uint32_t* p    (変更せずそのまま利用する)
    a6 : int32_t odd          (そのまま利用する)
    a7 : int32_t ratio        (変更せずそのまま利用する)
//
    a8 : - ratio - 32768
    a9 : diff                 比率判定用に利用
*/

  // x5 ~ x6
  void IRAM_ATTR blit_x50_x60_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x50_x60_565:                  \n"
    ASM_READ_RGB565_2PIXEL

    "sll        a12,a10                 \n"
    "s32i       a12,a2, 0               \n" // 0,1 保存
    "s32i       a12,a2, 8               \n" // 4,5 保存
    "sll        a13,a11                 \n"
    "s32i       a13,a2, 16              \n" // 8,9 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a15,a11                 \n"
    "s32i       a15,a2, 12              \n" // 6,7 保存
    "bgez       a9, BGEZ_x50_x60_565    \n"
// diffがマイナスの時の処理 x5.0
    "s16i       a13,a2, 8               \n" //   5 保存
    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 5*4             \n" // 出力先 += 5 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x50_x60_565\n"
    "retw                               \n"

"BGEZ_x50_x60_565:                  \n"
// diffがプラスの時の処理 x6.0
    "s32i       a15,a2, 20              \n" // 10,11 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 6*4             \n" // 出力先 += 6 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x50_x60_565\n"
    );
  }

  // x5 ~ x6
  void IRAM_ATTR blit_x50_x60_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x50_x60_332:                  \n"
    ASM_READ_RGB332_2PIXEL

    "sll        a12,a10                 \n"
    "s32i       a12,a2, 0               \n" // 0,1 保存
    "s32i       a12,a2, 8               \n" // 4,5 保存
    "sll        a13,a11                 \n"
    "s32i       a13,a2, 16              \n" // 8,9 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a15,a11                 \n"
    "s32i       a15,a2, 12              \n" // 6,7 保存
    "bgez       a9, BGEZ_x50_x60_332    \n"
// diffがマイナスの時の処理 x5.0
    "s16i       a13,a2, 8               \n" //   5 保存
    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 5*4             \n" // 出力先 += 5 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x50_x60_332\n"
    "retw                               \n"

"BGEZ_x50_x60_332:                  \n"
// diffがプラスの時の処理 x6.0
    "s32i       a15,a2, 20              \n" // 10,11 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 6*4             \n" // 出力先 += 6 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x50_x60_332\n"
    );
  }

  // x4 ~ x5
  void IRAM_ATTR blit_x40_x50_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x40_x50_565:                  \n"
    ASM_READ_RGB565_2PIXEL

    "sll        a12,a10                 \n"
    "s32i       a12,a2, 0               \n" // 0,1 保存
    "sll        a13,a11                 \n"
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a15,a11                 \n"
    "s32i       a15,a2, 12              \n" // 6,7 保存

    "bgez       a9, BGEZ_x40_x50_565    \n"
// diffがマイナスの時の処理 x4.0
    "s32i       a13,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 4*4             \n" // 出力先 += 4 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x40_x50_565\n"
    "retw                               \n"

"BGEZ_x40_x50_565:                  \n"
// diffがプラスの時の処理 x5.0
    "s32i       a12,a2, 8               \n" // 4,5 保存
    "s32i       a13,a2, 16              \n" // 8,9 保存
    "s16i       a13,a2, 8               \n" //   5 保存
    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 5*4             \n" // 出力先 += 5 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x40_x50_565\n"
    );
  }

  // x4 ~ x5
  void IRAM_ATTR blit_x40_x50_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x40_x50_332:                  \n"
    ASM_READ_RGB332_2PIXEL

    "sll        a12,a10                 \n"
    "s32i       a12,a2, 0               \n" // 0,1 保存
    "sll        a13,a11                 \n"
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a15,a11                 \n"
    "s32i       a15,a2, 12              \n" // 6,7 保存

    "bgez       a9, BGEZ_x40_x50_332    \n"
// diffがマイナスの時の処理 x4.0
    "s32i       a13,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 4*4             \n" // 出力先 += 4 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x40_x50_332\n"
    "retw                               \n"

"BGEZ_x40_x50_332:                  \n"
// diffがプラスの時の処理 x5.0
    "s32i       a12,a2, 8               \n" // 4,5 保存
    "s32i       a13,a2, 16              \n" // 8,9 保存
    "s16i       a13,a2, 8               \n" //   5 保存
    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 5*4             \n" // 出力先 += 5 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x40_x50_332\n"
    );
  }

  // x3 ~ x4
  void IRAM_ATTR blit_x30_x40_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x30_x40_565:                  \n"
    ASM_READ_RGB565_2PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a11                 \n"
    "bgez       a9, BGEZ_x30_x40_565    \n"
// diffがマイナスの時の処理 x3.0
    "s16i       a14,a2, 4               \n" //   3 保存
    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x30_x40_565\n"
    "retw                               \n"

"BGEZ_x30_x40_565:                  \n"
// diffがプラスの時の処理 x4.0
    "s32i       a14,a2, 12              \n" // 6,7 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 4*4             \n" // 出力先 += 4 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x30_x40_565\n"
    );
  }

  // x3 ~ x4
  void IRAM_ATTR blit_x30_x40_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x30_x40_332:                  \n"
    ASM_READ_RGB332_2PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a11                 \n"
    "bgez       a9, BGEZ_x30_x40_332    \n"
// diffがマイナスの時の処理 x3.0
    "s16i       a14,a2, 4               \n" //   3 保存
    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x30_x40_332\n"
    "retw                               \n"

"BGEZ_x30_x40_332:                  \n"
// diffがプラスの時の処理 x4.0
    "s32i       a14,a2, 12              \n" // 6,7 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 4*4             \n" // 出力先 += 4 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x30_x40_332\n"
    );
  }

  // x2 ~ x3
  void IRAM_ATTR blit_x20_x30_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x20_x30_565:                  \n"
    ASM_READ_RGB565_2PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "bgez       a9, BGEZ_x20_x30_565    \n"
// diffがマイナスの時の処理 x2.0

    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 2*4             \n" // 出力先 += 2 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x20_x30_565\n"
    "retw                               \n"

"BGEZ_x20_x30_565:                  \n"
// diffがプラスの時の処理 x3.0
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a11                 \n" // a14 = !odd a10
    "s16i       a14,a2, 4               \n" //   3 保存

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x20_x30_565\n"
    );
  }

  // x2 ~ x3
  void IRAM_ATTR blit_x20_x30_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x20_x30_332:                  \n"
    ASM_READ_RGB332_2PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "bgez       a9, BGEZ_x20_x30_332    \n"
// diffがマイナスの時の処理 x2.0

    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 2*4             \n" // 出力先 += 2 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x20_x30_332\n"
    "retw                               \n"

"BGEZ_x20_x30_332:                  \n"
// diffがプラスの時の処理 x3.0
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a10                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a11                 \n" // a14 = !odd a10
    "s16i       a14,a2, 4               \n" //   3 保存

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x20_x30_332\n"
    );
  }

  // x1.5~x2.0
  void IRAM_ATTR blit_x15_x20_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x15_x20_565:                  \n"
    ASM_READ_RGB565_4PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "sll        a14,a12                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存

    "bgez       a9, BGEZ_x15_x20_565    \n"
// diffがマイナスの時の処理 x1.5
    "sll        a14,a13                 \n"
    "s16i       a14,a2, 8               \n" //   5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a12                 \n"
    "s16i       a14,a2, 4               \n" //   3 保存

    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x15_x20_565\n"
    "retw                               \n"

"BGEZ_x15_x20_565:                  \n"
// diffがプラスの時の処理 x2.0
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a13                 \n"
    "s32i       a14,a2, 12              \n" // 6,7 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 4*4             \n" // 出力先 += 4 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x15_x20_565\n"
    );
  }

  // x1.5~x2.0
  void IRAM_ATTR blit_x15_x20_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x15_x20_332:                  \n"
    ASM_READ_RGB332_4PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "sll        a14,a12                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存

    "bgez       a9, BGEZ_x15_x20_332    \n"
// diffがマイナスの時の処理 x1.5
    "sll        a14,a13                 \n"
    "s16i       a14,a2, 8               \n" //   5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a12                 \n"
    "s16i       a14,a2, 4               \n" //   3 保存

    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x15_x20_332\n"
    "retw                               \n"

"BGEZ_x15_x20_332:                  \n"
// diffがプラスの時の処理 x2.0
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2,3 保存
    "sll        a14,a13                 \n"
    "s32i       a14,a2, 12              \n" // 6,7 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 4*4             \n" // 出力先 += 4 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x15_x20_332\n"
    );
  }

  // x1.0~x1.5
  void IRAM_ATTR blit_x10_x15_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x10_x15_565:                  \n"
    ASM_READ_RGB565_4PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "bgez       a9, BGEZ_x10_x15_565    \n"
// diffがマイナスの時の処理 x1.0

    "sll        a14,a11                 \n"
    "s16i       a14,a2, 0               \n" //   1 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a12                 \n"
    "s32i       a14,a2, 4               \n" // 2   保存
    "sll        a14,a13                 \n"
    "s16i       a14,a2, 4               \n" //   3 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 2*4             \n" // 出力先 += 2 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x10_x15_565\n"
    "retw                               \n"

"BGEZ_x10_x15_565:                  \n"
// diffがプラスの時の処理 x1.5
    "sll        a14,a13                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2   保存
    "sll        a14,a12                 \n"
    "s16i       a14,a2, 4               \n" //   3 保存

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x10_x15_565\n"
    );
  }

  // x1.0~x1.5
  void IRAM_ATTR blit_x10_x15_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int odd, int ratio)
  {
    __asm__ (
    ASM_INIT_BLIT
"LOOP_x10_x15_332:                  \n"
    ASM_READ_RGB332_4PIXEL

    "sll        a14,a10                 \n"
    "s32i       a14,a2, 0               \n" // 0,1 保存
    "bgez       a9, BGEZ_x10_x15_332    \n"
// diffがマイナスの時の処理 x1.0

    "sll        a14,a11                 \n"
    "s16i       a14,a2, 0               \n" //   1 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a12                 \n"
    "s32i       a14,a2, 4               \n" // 2   保存
    "sll        a14,a13                 \n"
    "s16i       a14,a2, 4               \n" //   3 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ

    "add        a9, a9, a7              \n" // diff += ratio
    "addi       a2, a2, 2*4             \n" // 出力先 += 2 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x10_x15_332\n"
    "retw                               \n"

"BGEZ_x10_x15_332:                  \n"
// diffがプラスの時の処理 x1.5
    "sll        a14,a13                 \n"
    "s32i       a14,a2, 8               \n" // 4,5 保存
    "xsr        a6, SAR                 \n" // シフト量スイッチ
    "sll        a14,a11                 \n"
    "s32i       a14,a2, 4               \n" // 2   保存
    "sll        a14,a12                 \n"
    "s16i       a14,a2, 4               \n" //   3 保存

    "addmi      a9, a9, -32768          \n" // diff -= 32768
    "addi       a2, a2, 3*4             \n" // 出力先 += 3 * sizeof(uint32_t)
    "bltu       a3, a4, LOOP_x10_x15_332\n"
    );
  }

#else

  // x5 ~ x6
  void IRAM_ATTR blit_x50_x60_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0h = s[0];
      uint32_t s0l = s[1];
      uint32_t s1h = s[2];
      uint32_t s1l = s[3];
      s0h = p[(s0h << 1)  ];
      s0l = p[(s0l << 1)+1];
      s1h = p[(s1h << 1)  ];
      s1l = p[(s1l << 1)+1];
      s += 4;
      uint32_t s0 = s0h + s0l;
      uint32_t s1 = s1h + s1l;

      uint32_t s0even = s0 << shift;
      uint32_t s1even = s1 << shift;
      shift ^= 8;
      uint32_t s0odd = s0 << shift;
      uint32_t s1odd = s1 << shift;
      d[0] = s0even;
      d[1] = s0odd;
      d[2] = s0even;
      d[3] = s1odd;
      d[4] = s1even;
      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[2]) = s1even;
        d += 5;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[5] = s1odd;
        shift ^= 8;
        d += 6;
        if (s >= s_end) { return; }
      }
    }
  }

  // x5 ~ x6
  void IRAM_ATTR blit_x50_x60_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      auto s0 = s[0];
      auto s1 = s[1];
      s += 2;
      s0 = p[s0];
      s1 = p[s1];

      uint32_t s0even = s0 << shift;
      uint32_t s1even = s1 << shift;
      shift ^= 8;
      uint32_t s0odd = s0 << shift;
      uint32_t s1odd = s1 << shift;
      d[0] = s0even;
      d[1] = s0odd;
      d[2] = s0even;
      d[3] = s1odd;
      d[4] = s1even;
      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[2]) = s1even;
        d += 5;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[5] = s1odd;
        shift ^= 8;
        d += 6;
        if (s >= s_end) { return; }
      }
    }
  }

  // x4 ~ x5
  void IRAM_ATTR blit_x40_x50_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0h = s[0];
      uint32_t s0l = s[1];
      uint32_t s1h = s[2];
      uint32_t s1l = s[3];
      s0h = p[(s0h << 1)  ];
      s0l = p[(s0l << 1)+1];
      s1h = p[(s1h << 1)  ];
      s1l = p[(s1l << 1)+1];
      s += 4;
      uint32_t s0 = s0h + s0l;
      uint32_t s1 = s1h + s1l;

      uint32_t s0even = s0 << shift;
      uint32_t s1even = s1 << shift;
      shift ^= 8;
      uint32_t s0odd = s0 << shift;
      uint32_t s1odd = s1 << shift;
      d[0] = s0even;
      d[1] = s0odd;
      d[3] = s1odd;
      if (diff < 0)
      {
        diff += ratio;
        d[2] = s1even;
        shift ^= 8;
        d += 4;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[4] = s1even;
        d[2] = s0even;
        *((uint16_t*)&d[2]) = s1even;
        d += 5;
        if (s >= s_end) { return; }
      }
    }
  }

  // x4 ~ x5
  void IRAM_ATTR blit_x40_x50_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      auto s0 = s[0];
      auto s1 = s[1];
      s += 2;
      s0 = p[s0];
      s1 = p[s1];

      uint32_t s0even = s0 << shift;
      uint32_t s1even = s1 << shift;
      shift ^= 8;
      uint32_t s0odd = s0 << shift;
      uint32_t s1odd = s1 << shift;
      d[0] = s0even;
      d[1] = s0odd;
      d[3] = s1odd;
      if (diff < 0)
      {
        diff += ratio;
        d[2] = s1even;
        shift ^= 8;
        d += 4;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[4] = s1even;
        d[2] = s0even;
        *((uint16_t*)&d[2]) = s1even;
        d += 5;
        if (s >= s_end) { return; }
      }
    }
  }

  // x3 ~ x4
  void IRAM_ATTR blit_x30_x40_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0h = s[0];
      uint32_t s0l = s[1];
      uint32_t s1h = s[2];
      uint32_t s1l = s[3];
      s0h = p[(s0h << 1)  ];
      s0l = p[(s0l << 1)+1];
      s1h = p[(s1h << 1)  ];
      s1l = p[(s1l << 1)+1];
      s += 4;
      uint32_t s0 = s0h + s0l;
      uint32_t s1 = s1h + s1l;

      uint32_t s0even = s0 << shift;
      uint32_t s1even = s1 << shift;
      shift ^= 8;
      uint32_t s0odd = s0 << shift;
      uint32_t s1odd = s1 << shift;
      d[0] = s0even;
      d[1] = s0odd;
      d[2] = s1even;
      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[1]) = s1odd;
        d += 3;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[3] = s1odd;
        shift ^= 8;
        d += 4;
        if (s >= s_end) { return; }
      }
    }
  }

  // x3 ~ x4
  void IRAM_ATTR blit_x30_x40_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      auto s0 = s[0];
      auto s1 = s[1];
      s += 2;
      s0 = p[s0];
      s1 = p[s1];

      uint32_t s0even = s0 << shift;
      uint32_t s1even = s1 << shift;
      shift ^= 8;
      uint32_t s0odd = s0 << shift;
      uint32_t s1odd = s1 << shift;
      d[0] = s0even;
      d[1] = s0odd;
      d[2] = s1even;
      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[1]) = s1odd;
        d += 3;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[3] = s1odd;
        shift ^= 8;
        d += 4;
        if (s >= s_end) { return; }
      }
    }
  }

  // x2 ~ x3
  void IRAM_ATTR blit_x20_x30_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0h = s[0];
      uint32_t s0l = s[1];
      uint32_t s1h = s[2];
      uint32_t s1l = s[3];
      s0h = p[(s0h << 1)  ];
      s0l = p[(s0l << 1)+1];
      s1h = p[(s1h << 1)  ];
      s1l = p[(s1l << 1)+1];
      s += 4;
      uint32_t s0 = s0h + s0l;
      uint32_t s1 = s1h + s1l;

      uint32_t s0even = s0 << shift;
      d[0] = s0even;
      if (diff < 0)
      {
        diff += ratio;
        shift ^= 8;
        uint32_t s1odd = s1 << shift;
        d[1] = s1odd;
        shift ^= 8;
        d += 2;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        uint32_t s1even = s1 << shift;
        shift ^= 8;
        uint32_t s0odd = s0 << shift;
        uint32_t s1odd = s1 << shift;
        d[1] = s0odd;
        d[2] = s1even;
        *((uint16_t*)&d[1]) = s1odd;
        d += 3;
        if (s >= s_end) { return; }
      }
    }
  }

  // x2 ~ x3
  void IRAM_ATTR blit_x20_x30_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      auto s0 = s[0];
      auto s1 = s[1];
      s += 2;
      s0 = p[s0];
      s1 = p[s1];

      uint32_t s0even = s0 << shift;
      d[0] = s0even;
      if (diff < 0)
      {
        diff += ratio;
        shift ^= 8;
        uint32_t s1odd = s1 << shift;
        d[1] = s1odd;
        shift ^= 8;
        d += 2;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        uint32_t s1even = s1 << shift;
        shift ^= 8;
        uint32_t s0odd = s0 << shift;
        uint32_t s1odd = s1 << shift;
        d[1] = s0odd;
        d[2] = s1even;
        *((uint16_t*)&d[1]) = s1odd;
        d += 3;
        if (s >= s_end) { return; }
      }
    }
  }

  // x1.5~x2.0
  void IRAM_ATTR blit_x15_x20_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0h = s[0];
      uint32_t s0l = s[1];
      uint32_t s1h = s[2];
      uint32_t s1l = s[3];
      s0h = p[(s0h << 1)  ];
      s0l = p[(s0l << 1)+1];
      s1h = p[(s1h << 1)  ];
      s1l = p[(s1l << 1)+1];
      uint32_t s0 = s0h + s0l;
      uint32_t s1 = s1h + s1l;

      uint32_t s2h = s[4];
      uint32_t s2l = s[5];
      uint32_t s3h = s[6];
      uint32_t s3l = s[7];
      s2h = p[(s2h << 1)  ];
      s2l = p[(s2l << 1)+1];
      s3h = p[(s3h << 1)  ];
      s3l = p[(s3l << 1)+1];
      s += 8;
      uint32_t s2 = s2h + s2l;
      uint32_t s3 = s3h + s3l;

      d[0] = s0 << shift;
      d[2] = s2 << shift;

      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[2]) = s3 << shift;
        shift ^= 8;
        d[1] = s1 << shift;
        *((uint16_t*)&d[1]) = s2 << shift;
        d += 3;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        shift ^= 8;
        d[1] = s1 << shift;
        d[3] = s3 << shift;
        shift ^= 8;
        d += 4;
        if (s >= s_end) { return; }
      }
    }
  }

  // x1.5~x2.0
  void IRAM_ATTR blit_x15_x20_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0 = s[0];
      uint32_t s1 = s[1];
      uint32_t s2 = s[2];
      uint32_t s3 = s[3];
      s0 = p[s0];
      s1 = p[s1];
      s2 = p[s2];
      s3 = p[s3];
      s += 4;

      d[0] = s0 << shift;
      d[2] = s2 << shift;

      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[2]) = s3 << shift;
        shift ^= 8;
        d[1] = s1 << shift;
        *((uint16_t*)&d[1]) = s2 << shift;
        d += 3;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        shift ^= 8;
        d[1] = s1 << shift;
        d[3] = s3 << shift;
        shift ^= 8;
        d += 4;
        if (s >= s_end) { return; }
      }
    }
  }

  // x1.0~x1.5
  void IRAM_ATTR blit_x10_x15_565(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0h = s[0];
      uint32_t s0l = s[1];
      uint32_t s1h = s[2];
      uint32_t s1l = s[3];
      s0h = p[(s0h << 1)  ];
      s0l = p[(s0l << 1)+1];
      s1h = p[(s1h << 1)  ];
      s1l = p[(s1l << 1)+1];
      uint32_t s0 = s0h + s0l;
      uint32_t s1 = s1h + s1l;

      uint32_t s2h = s[4];
      uint32_t s2l = s[5];
      uint32_t s3h = s[6];
      uint32_t s3l = s[7];
      s2h = p[(s2h << 1)  ];
      s2l = p[(s2l << 1)+1];
      s3h = p[(s3h << 1)  ];
      s3l = p[(s3l << 1)+1];
      s += 8;
      uint32_t s2 = s2h + s2l;
      uint32_t s3 = s3h + s3l;

      d[0] = s0 << shift;
      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[0]) = s1 << shift;
        shift ^= 8;
        d[1] = s2 << shift;
        *((uint16_t*)&d[1]) = s3 << shift;
        shift ^= 8;
        d += 2;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[2] = s3 << shift;
        shift ^= 8;
        d[1] = s1 << shift;
        *((uint16_t*)&d[1]) = s2 << shift;
        d += 3;
        if (s >= s_end) { return; }
      }
    }
  }

  // x1.0~x1.5
  void IRAM_ATTR blit_x10_x15_332(uint32_t* __restrict d, const uint8_t* s, const uint8_t* s_end, const uint32_t* p, int shift, int ratio)
  {
    int diff = (ratio - 32768) >> 1;
    for (;;)
    {
      uint32_t s0 = s[0];
      uint32_t s1 = s[1];
      uint32_t s2 = s[2];
      uint32_t s3 = s[3];
      s0 = p[s0];
      s1 = p[s1];
      s2 = p[s2];
      s3 = p[s3];
      s += 4;

      d[0] = s0 << shift;
      if (diff < 0)
      {
        diff += ratio;
        *((uint16_t*)&d[0]) = s1 << shift;
        shift ^= 8;
        d[1] = s2 << shift;
        *((uint16_t*)&d[1]) = s3 << shift;
        shift ^= 8;
        d += 2;
        if (s >= s_end) { return; }
      }
      else
      {
        diff -= 32768;
        d[2] = s3 << shift;
        shift ^= 8;
        d[1] = s1 << shift;
        *((uint16_t*)&d[1]) = s2 << shift;
        d += 3;
        if (s >= s_end) { return; }
      }
    }
  }

#endif

  /// 引数のポインタアドレスがSRAMかどうか判定する  true=SRAM / false=not SRAM (e.g. PSRAM FlashROM) ;
  static inline bool IRAM_ATTR isSRAM(const void* ptr)
  {
    return (((uintptr_t)ptr & 0x3FF00000u) == 0x3FF00000u);
  }

  static inline size_t IRAM_ATTR ScanLineToY(size_t scanline, bool odd)
  {
    return ((scanline << 1) + (!odd)) * internal.memory_height / (_signal_spec_info.display_height) - internal.offset_y;
  }

  void IRAM_ATTR i2s_intr_handler_video(void *arg)
  {
    bool flg_eof = I2S0.int_st.out_eof;
    I2S0.int_clr.val = I2S0.int_st.val;
    if (!flg_eof) { return; }

    uint16_t* buf = (uint16_t*)((lldesc_t*)I2S0.out_eof_des_addr)->buf;

    ISR_BEGIN();

    internal.current_scanline = internal.current_scanline + 1;
    if (internal.current_scanline >= _signal_spec_info.total_scanlines) {
      internal.current_scanline = 0;
    }

    // インターレース込みでの走査線位置を取得;
    int i = internal.current_scanline;
    // インターレースを外した走査線位置に変換する (奇数フィールドの場合に走査線位置が0基準になるように変換する)
    bool odd_field = i >= (_signal_spec_info.total_scanlines >> 1);
    if (odd_field) { i -= (_signal_spec_info.total_scanlines >> 1); }

    // getScanLine用の走査線位置を設定しておく;
    internal.converted_scanline = i;

    internal.burst_shift = internal.burst_shift ^ _signal_spec_info.burst_shift_mask;

    if (i >= _signal_spec_info.vsync_lines)
    {
      i -= _signal_spec_info.vsync_lines;
      size_t idx = ScanLineToY(i, odd_field);
      if (idx >= internal.panel_height)
      {
        if (idx - internal.panel_height < (internal.dma_desc_count << 1))
        {
          memset(&buf[_signal_spec_info.active_start], internal.BLACK_LEVEL >> 8, (_signal_spec_info.scanline_width - 22 - _signal_spec_info.active_start) << 1);
          // memset(&buf[_signal_spec_info.scanline_width - 22], internal.BLANKING_LEVEL >> 8, 22 << 1);
        }
      }
      else
      {
        const uint8_t* src = internal.lines[idx];
        if (!isSRAM(src))
        {
          src = _scanline_cache.get(src);
        }
        int pidx = 0;
        if (internal.burst_shift & 1)
        {
          pidx = internal.pixel_per_bytes << 8;
        }
        if (src) {
          internal.fp_blit( (uint32_t*)(&buf[internal.leftside_index]),
                            src,
                            &src[internal.panel_width * internal.pixel_per_bytes],
                            &internal.palette[pidx],
                            (internal.burst_shift & 2) << 2,  // burst_shift ? 8 : 0
                            internal.mul_ratio );
        }
      }
    }
    else
    {
      if (i < _signal_spec_info.sync_proc_count)
      {
        auto sync_proc = _signal_spec_info.sync_proc[odd_field][i];
        size_t half_index = (_signal_spec_info.scanline_width >> 1);
        if (sync_proc & 0x40)  // 水平期間前半のブランキングレベル化;
        {
          memset(buf, internal.BLANKING_LEVEL >> 8, half_index << 1);
          buf[(half_index - 1) ^ 1] = internal.BLANKING_LEVEL;
        }
        if (sync_proc & 0x04)  // 水平期間後半のブランキングレベル化;
        {
          int blank_idx = (half_index + 1) & ~1u;
          memset(&buf[blank_idx], internal.BLANKING_LEVEL >> 8, (_signal_spec_info.scanline_width - blank_idx) << 1);
          buf[half_index ^ 1] = internal.BLANKING_LEVEL;
        }
        if (sync_proc & 0x03) // 水平期間後半のパルス付与;
        {
          // 0x01=等化パルス幅  /  0x02=垂直同期パルス幅
          int syncwidth = ((sync_proc & 0x01) ? _signal_spec_info.hsync_equalizing : _signal_spec_info.hsync_long);
          memset(&buf[((_signal_spec_info.scanline_width >> 1) + 1) & ~1u], internal.SYNC_LEVEL >> 8, syncwidth << 1);
          buf[(_signal_spec_info.scanline_width >> 1) ^ 1] = internal.SYNC_LEVEL;
        }
        if (sync_proc & 0x30) // 水平期間前半のパルス付与;
        {
          int syncwidth = _signal_spec_info.hsync_equalizing;  // 等化パルス幅;
          switch ((sync_proc >> 4) & 3)
          {
          case 2: syncwidth = _signal_spec_info.hsync_long;  break;   // 垂直同期パルス幅;
          case 3: syncwidth = _signal_spec_info.hsync_short; break;   // 水平同期パルス幅;
          default: break;
          }
          memset(buf, internal.SYNC_LEVEL >> 8, syncwidth << 1);
        }
        if (sync_proc & 0x80) // バースト信号付与;
        {
          uint32_t b0 = internal.burst_wave[internal.burst_shift & 1];
          uint32_t b1 = b0 << 8;
          uint32_t* l = (uint32_t*)(&buf[_signal_spec_info.burst_start]);
          bool flg_swap = (bool)(_signal_spec_info.burst_start & 3) ^ (bool)(internal.burst_shift & 2);
          if (flg_swap) { std::swap(b0, b1); }
          int burst_len = _signal_spec_info.burst_cycle;
          do
          {
            *l++ = b0;
            *l++ = b1;
          } while (--burst_len);

          memset(&buf[_signal_spec_info.active_start], internal.BLACK_LEVEL >> 8, (_signal_spec_info.scanline_width - 22 - _signal_spec_info.active_start) << 1);
        }
      }
      i -= _signal_spec_info.vsync_lines;
    }

    if (internal.use_psram)
    {
      int32_t j = (i == - _scanline_cache.cache_num) ? 0 : _scanline_cache.prev_index;
      i += _scanline_cache.cache_num << 1;
      for (;j < i; ++j)
      {
        int idx = ScanLineToY(j, odd_field);
        if (idx >= internal.panel_height) { break; }
        auto ptr = internal.lines[idx];
        if (idx < 0 || isSRAM(ptr)) { continue; }
        if (!_scanline_cache.prepare(ptr)) { break; }
      }
      _scanline_cache.prev_index = j;
    }

    ISR_END();
  }



  Panel_CVBS::Panel_CVBS()
  {
    _cfg.memory_width  = _cfg.panel_width  = 180;
    _cfg.memory_height = _cfg.panel_height = 120;
    _write_depth = color_depth_t::rgb332_1Byte;
    _read_depth = color_depth_t::rgb332_1Byte;
  }

  Panel_CVBS::~Panel_CVBS()
  {
    deinit();
  }

  void Panel_CVBS::deinit(void)
  {
    if (_started)
    {
      _started = false;
      auto prevcurrent_scanline = internal.current_scanline;
      for (int i = 0; i < 20; ++i)
      {
        delay(1);
        auto tmp = internal.current_scanline;
        if (prevcurrent_scanline > tmp) { break; }
        prevcurrent_scanline = tmp;
      }
      esp_intr_disable(internal.isr_handle);
      for (int i = 0; i < internal.dma_desc_count; ++i)
      {
        internal.dma_desc[i].empty = 0;
      }
      esp_intr_free(internal.isr_handle);
      internal.isr_handle = nullptr;

      I2S0.out_link.stop = 1;
      I2S0.out_link.start = 0;
      I2S0.conf.tx_start = 0;

      dac_i2s_disable();
      switch (_config_detail.pin_dac)
      {
      default:
        break;
      case 25:
        dac_output_disable(DAC_CHANNEL_1); // for GPIO 25
        break;
      case 26:
        dac_output_disable(DAC_CHANNEL_2); // for GPIO 26
        break;
      }

      periph_module_disable(PERIPH_I2S0_MODULE);

#if defined ( LGFX_I2S_STD_ENABLED )
    rtc_clk_apll_enable(false);
#else
    rtc_clk_apll_enable(false,0,0,0,1);
#endif

// printf("dmabuf: %08x free\n", internal.dma_desc[0].buf);
      heap_free((void*)(internal.dma_desc[0].buf));

      deinitFrameBuffer();

// printf("internal.palette: %08x free\n", internal.palette);
      if (internal.palette != nullptr) { heap_free(internal.palette); }

      _scanline_cache.end();
    }

    for (int i = 0; i < internal.dma_desc_count; i++) {
      internal.dma_desc[i].buf = nullptr;
    }
    internal.palette = nullptr;
    internal.lines = nullptr;
    internal.burst_shift = 0;
    internal.current_scanline = 0;
  }

  bool Panel_CVBS::init(bool use_reset)
  {
    if (_started)
    {
      return true;
    }
    _started = true;

    dac_i2s_enable();
    switch (_config_detail.pin_dac)
    {
    default:
      ESP_LOGE(TAG, "DAC output gpio error: G%d  ... Select G25 or G26.", _config_detail.pin_dac);
      return false;
    case 25:
      dac_output_enable(DAC_CHANNEL_1); // for GPIO 25
      break;
    case 26:
      dac_output_enable(DAC_CHANNEL_2); // for GPIO 26
      break;
    }

    if (_config_detail.signal_type >= config_detail_t::signal_type_t::signal_type_max)
    {
      _config_detail.signal_type = (config_detail_t::signal_type_t)0;
    }

    const signal_spec_info_t& spec_info = signal_spec_info_list[_config_detail.signal_type];
    _signal_spec_info = spec_info;

    uint32_t pixelPerBytes = (getWriteDepth() & color_depth_t::bit_mask) >> 3;
    internal.pixel_per_bytes = pixelPerBytes;

// 幅方向の解像度に関する準備 ;
    {
      uint16_t output_width = std::min(_cfg.memory_width, spec_info.display_width);
      uint16_t panel_width = std::min(_cfg.panel_width , output_width);
      internal.panel_width = panel_width;

      uint_fast16_t offset_x = std::min<uint32_t>(_cfg.offset_x , output_width - panel_width);

      uint32_t scale_index = (spec_info.display_width << 1) / output_width;
      scale_index = (scale_index < 2 ? 2 : scale_index > 10 ? 10 : scale_index) - 2;

      /// 表示倍率に応じて出力データ生成関数を変更する;
      static constexpr void (*fp_tbl_332[])(uint32_t*, const uint8_t*, const uint8_t*, const uint32_t*, int, int) =
      {
        blit_x10_x15_332,
        blit_x15_x20_332,
        blit_x20_x30_332,
        blit_x20_x30_332,
        blit_x30_x40_332,
        blit_x30_x40_332,
        blit_x40_x50_332,
        blit_x40_x50_332,
        blit_x50_x60_332
      };
      static constexpr void (*fp_tbl_565[])(uint32_t*, const uint8_t*, const uint8_t*, const uint32_t*, int, int) =
      {
        blit_x10_x15_565,
        blit_x15_x20_565,
        blit_x20_x30_565,
        blit_x20_x30_565,
        blit_x30_x40_565,
        blit_x30_x40_565,
        blit_x40_x50_565,
        blit_x40_x50_565,
        blit_x50_x60_565
      };

      /// 描画時の引き延ばし倍率テーブル (例:2=等倍  3=1.5倍  4=2倍)  上位4bitと下位4bitで２種類の倍率を指定する;
      /// この２種類の倍率をデータ生成時に切り替えて任意サイズの出力倍率を実現する;
      static constexpr const uint8_t scale_tbl[] = { 0x23, 0x34, 0x46, 0x46, 0x68, 0x68, 0x8A, 0x8A, 0xAC };
      uint8_t scale_h = scale_tbl[scale_index];
      uint8_t scale_l = scale_h >> 4;
      scale_h &= 0x0F;

      internal.fp_blit = (pixelPerBytes == 1 ? fp_tbl_332 : fp_tbl_565)[scale_index];

      /// 表示倍率の比率を求める;
      int32_t mul_ratio_h = spec_info.display_width - (output_width * scale_h / 2);
      int32_t mul_ratio_l = spec_info.display_width - (output_width * scale_l / 2);
      int32_t mul_ratio = INT32_MAX;
      if (mul_ratio_h < 0) {
        mul_ratio_h = -mul_ratio_h;
        mul_ratio = ((mul_ratio_l << 15) + (mul_ratio_h >> 1)) / mul_ratio_h;
      }
      internal.mul_ratio = mul_ratio;

      // Xオフセットに表示倍率を掛けたものを描画開始位置情報に加える
      int scale_offset = (offset_x * spec_info.display_width + output_width-1) / output_width;

      internal.leftside_index = (spec_info.active_start + scale_offset) & ~3u;

// printf("scale_l:%d scale_h:%d swl:%d swh:%d  ratio:%d  a:%d b:%d left:%d  \n", scale_l, scale_h, output_width * scale_l, output_width * scale_h, mul_ratio, mul_ratio_h, mul_ratio_l, internal.leftside_index);
    }

    {
      uint16_t output_height = std::min(_cfg.memory_height, spec_info.display_height);
      uint16_t panel_height = std::min(_cfg.panel_height, output_height);
      internal.memory_height = output_height;
      internal.panel_height = panel_height;

      internal.offset_y = std::min<uint32_t>(_cfg.offset_y , output_height - panel_height);
    }

    setRotation(getRotation());

    const signal_setup_info_t& setup_info = signal_setup_info_list[_config_detail.signal_type];
    internal.palette = (uint32_t*)heap_alloc(setup_info.palette_num_256 * pixelPerBytes * 256 * sizeof(uint32_t));
// printf("internal.palette: %08x alloc\n", internal.palette);
    if (!internal.palette) { return false; }

    uint_fast8_t use_psram = _config_detail.use_psram;
    if (!initFrameBuffer(internal.panel_width * pixelPerBytes, internal.panel_height, use_psram)) { return false; }

    use_psram = isSRAM(_lines_buffer[0]) ? 0 : use_psram;
    internal.use_psram = use_psram;
    if (use_psram)
    {
      _scanline_cache.begin(( internal.panel_width * pixelPerBytes + 4 ) & ~3, _config_detail.task_priority, _config_detail.task_pinned_core);
    }

    size_t n = spec_info.scanline_width << 1;  // n=DMA 1回分のデータ量  最大値は4092;
    size_t len = (n + 3) & ~3u;

    uint8_t* dmabuf = (uint8_t*)heap_alloc_dma(len * internal.dma_desc_count);    // dma_descの個数分を纏めて確保しておく;
// printf("dmabuf: %08x alloc\n", dmabuf);
    if (dmabuf == nullptr)
    {
      return false;
    }
    memset(dmabuf, 0, len * internal.dma_desc_count);
    for (int i = 0; i < internal.dma_desc_count; ++i)
    {
      internal.dma_desc[i].buf = &dmabuf[i * len];
      internal.dma_desc[i].owner = 1;
      internal.dma_desc[i].eof = 1;
      internal.dma_desc[i].length = len;
      internal.dma_desc[i].size = n;
      internal.dma_desc[i].empty = (uint32_t)(&internal.dma_desc[(i + 1) & (internal.dma_desc_count - 1)]);
    }

    internal.lines = _lines_buffer;

    updateSignalLevel();

    //  Setup up the apll: See ref 3.2.7 Audio PLL
    //  f_xtal = (int)rtc_clk_xtal_freq_get() * 1000000;
    //  f_out = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536); // 250 < f_out < 500
    //  apll_freq = f_out/((o_div + 2) * 2)
    //  operating range of the f_out is 250 MHz ~ 500 MHz
    //  operating range of the apll_freq is 16 ~ 128 MHz.
    //  select sdm0,sdm1,sdm2 to produce nice multiples of colorburst frequencies

    //  see calc_freq() for math: (4+a)*10/((2 + b)*2) mhz
    //  up to 20mhz seems to work ok:
    //  rtc_clk_apll_enable(1,0x00,0x00,0x4,0);   // 20mhz for fancy DDS
    periph_module_enable(PERIPH_I2S0_MODULE);

    // setup interrupt
    if (esp_intr_alloc(ETS_I2S0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM,
        i2s_intr_handler_video, this, &internal.isr_handle) != ESP_OK)
    {
      return false;
    }


    bool use_apll = true;
    #if defined ( CONFIG_IDF_TARGET_ESP32 ) || !defined ( CONFIG_IDF_TARGET )
    {
      // ESP32 rev0 には APLLの設定値が正しく反映されないハードウェア不具合があるため、
      // APLLを使用せずにI2Sのクロック分周設定で代用する。
      // I2Sのクロック分周設定では要求仕様との誤差が大きくなる。
      // そのため波打ったり色が乱れる事があるが、これを完全に解消することは不可能である。
      esp_chip_info_t chip_info;
      esp_chip_info(&chip_info);
      if (chip_info.revision == 0) { use_apll = false; }
    }
    #endif
    I2S0.clkm_conf.clka_en = use_apll;
    I2S0.clkm_conf.clkm_div_num = 1;
    I2S0.clkm_conf.clkm_div_b = 0;
    I2S0.clkm_conf.clkm_div_a = 1;
    if (use_apll) {
#if defined ( LGFX_I2S_STD_ENABLED )
      rtc_clk_apll_enable( true );
      // 設定する前にapllをenableにしておく必要がある
      rtc_clk_apll_coeff_set( 1
                            , setup_info.sdm0
                            , setup_info.sdm1
                            , setup_info.sdm2
                            );
#else
      rtc_clk_apll_enable( true
                          , setup_info.sdm0
                          , setup_info.sdm1
                          , setup_info.sdm2
                          , 1
                          );
#endif
    }
    else
    {
      I2S0.clkm_conf.clkm_div_num = setup_info.div_n;
      I2S0.clkm_conf.clkm_div_b = setup_info.div_b;
      I2S0.clkm_conf.clkm_div_a = setup_info.div_a;
    }

    // reset conf
    I2S0.conf.tx_reset = 1;
    I2S0.conf.tx_reset = 0;

    /// 出力先のGPIOが25か26かでLEFT/RIGHTの出力順を変える。25==right / 26==left
    /// first側の出力は綺麗に出るが、もう一方の出力は値が乱れる (ランダムに前の出力値のビットが半端に混ざった外れ値が出る事がある);
    /// ゆえに出力先をfirstに設定しないと信号出力に外れ値ノイズが頻出することに注意;
    I2S0.conf.tx_right_first = (_config_detail.pin_dac == 25);
    I2S0.conf.tx_mono = 1;

    I2S0.conf2.lcd_en = 1;
    I2S0.conf_chan.tx_chan_mod = 1;
    I2S0.sample_rate_conf.tx_bits_mod = 16;
    I2S0.sample_rate_conf.tx_bck_div_num = 1;

    I2S0.out_link.addr = (uint32_t)internal.dma_desc;
    I2S0.out_link.start = 1;

    I2S0.fifo_conf.tx_fifo_mod = 1;  // 16-bit single channel data
    I2S0.fifo_conf.tx_fifo_mod_force_en = 1;

    I2S0.int_clr.val = 0xFFFFFFFF;
    I2S0.int_ena.out_eof = 1;
    if (ESP_OK != esp_intr_enable(internal.isr_handle)) { return false; }        // start interruprs!
    I2S0.conf.tx_start = 1;

    return Panel_FrameBufferBase::init(use_reset);
  }

  void Panel_CVBS::config_detail(const config_detail_t& config_detail)
  {
    bool flg_started = _started;
    if (flg_started)
    {
      deinit();
    }

    _config_detail = config_detail;

    if (flg_started)
    {
      init(false);
    }
  }

  color_depth_t Panel_CVBS::setColorDepth(color_depth_t depth)
  {
    if (depth != color_depth_t::grayscale_8bit) {
      depth = ((depth & color_depth_t::bit_mask) > 8) ? rgb565_2Byte : rgb332_1Byte;
    }
    if (depth != _write_depth)
    {
      bool flg_started = _started;
      if (flg_started)
      {
        deinit();
      }
      _write_depth = depth;
      _read_depth = depth;
      if (flg_started)
      {
        init(false);
      }
    }
    return depth;
  }

  void Panel_CVBS::setResolution(uint16_t width, uint16_t height, config_detail_t::signal_type_t type, int output_width, int output_height, int offset_x, int offset_y)
  {
    bool flg_started = _started;
    if (flg_started)
    {
      deinit();
    }

    if ((uint32_t)type < config_detail_t::signal_type_max)
    {
      _config_detail.signal_type = type;
    }
    const signal_spec_info_t& spec_info = signal_spec_info_list[_config_detail.signal_type];

    if (output_width  < 0) { output_width  = width; }
    if ((uint32_t)output_width  > spec_info.display_width ) { output_width  = spec_info.display_width; }
    if (width  > output_width ) { width  = output_width; }
    _cfg.panel_width = width;
    _cfg.memory_width = output_width;
    if (offset_x < 0) { offset_x = (output_width - width) >> 1; }
    else if (offset_x > output_width - width) { offset_x = output_width - width; }
    _cfg.offset_x = offset_x;

    if (output_height < 0) { output_height = height; }
    if ((uint32_t)output_height > spec_info.display_height) { output_height = spec_info.display_height; }
    if (height > output_height) { height = output_height; }
    _cfg.panel_height = height;
    _cfg.memory_height = output_height;
    if (offset_y < 0) { offset_y = (output_height - height) >> 1; }
    else if (offset_y > output_height - height) { offset_y = output_height - height; }
    _cfg.offset_y = offset_y;

    if (flg_started)
    {
      init(false);
    }
  }


  int32_t Panel_CVBS::getScanLine(void)
  {
    return internal.converted_scanline;
  }

  void Panel_CVBS::updateSignalLevel(void)
  {
    uint32_t level = 48 * _config_detail.output_level;

    const auto &setup_info = signal_setup_info_list[_config_detail.signal_type];

    internal.WHITE_LEVEL    = (setup_info.white_mv    * level) >> 8;
    internal.BLACK_LEVEL    = (setup_info.black_mv    * level) >> 8;
    internal.BLANKING_LEVEL = (setup_info.blanking_mv * level) >> 8;

    uint8_t blank_n = (internal.BLANKING_LEVEL - (internal.BLANKING_LEVEL >> 1)) >> 8;
    uint8_t blank_p = (internal.BLANKING_LEVEL + (internal.BLANKING_LEVEL >> 1)) >> 8;
    internal.burst_wave[0] = blank_n << 24
                           | blank_p << 8
                           | blank_p << 16
                           | blank_n << 0
                           ;
    internal.burst_wave[1] = blank_p << 24
                           | blank_p << 8
                           | blank_n << 16
                           | blank_n << 0
                           ;

    if (internal.palette)
    {
      const signal_setup_info_t& setup_info_ = signal_setup_info_list[_config_detail.signal_type];
      auto fp_setup_palette = internal.pixel_per_bytes == 1
                            ? setup_info_.setup_palette_332
                            : setup_info_.setup_palette_565
                            ;
      if (getWriteDepth() == grayscale_8bit) {
        fp_setup_palette = setup_info_.setup_palette_gray;
      }
      fp_setup_palette(internal.palette, internal.WHITE_LEVEL, internal.BLACK_LEVEL, _config_detail.chroma_level);
    }
  }

  void Panel_CVBS::setOutputLevel(uint8_t output_level)
  {
    _config_detail.output_level = output_level;
    updateSignalLevel();
  }

  void Panel_CVBS::setChromaLevel(uint8_t chroma)
  {
    _config_detail.chroma_level = chroma;
    updateSignalLevel();
  }

  static constexpr uint8_t linesPerChunk = 8;
  static inline int getIndexInterleave(int index)
  { // メモリを2ライン単位で交互に使用するように、インデクスを変換する。;
  // アルゴリズムを変更する場合はlinesPerChunkの値やdeinitと整合性を確認すること;
    int bit1_2 = index & 0x06u;
    int bit3 = index & 0x08u;
    return (index & ~0x0Eu) + (bit1_2 << 1) + (bit3 >> 2);
  }

  static inline uint8_t* sub_heap_alloc(bool flg_psram, size_t size)
  {
    uint8_t* res = nullptr;
    if (flg_psram) { res = (uint8_t*)heap_alloc_psram(size); }
    if (res == nullptr)
    {
      res = (uint8_t*)heap_alloc_dma(size);
    }
    if (res) { memset(res, 0, size); }
    return (uint8_t*)res;
  }

  bool Panel_CVBS::initFrameBuffer(size_t width, size_t height, uint8_t use_psram)
  {
// printf("initFrameBuffer w:%d h:%d \n", width, height);
    uint8_t** lineArray = (uint8_t**)heap_alloc_dma(height * sizeof(uint8_t*));

    size_t alloc_idx_len = (height / linesPerChunk + 2) * sizeof(uint16_t);
    uint16_t* allocated_list = (uint16_t*)sub_heap_alloc(use_psram, alloc_idx_len);
    memset(allocated_list, 0xFF, alloc_idx_len);

    internal.allocated_list = allocated_list;

// printf ("lineArray %08x alloc \n", lineArray);
    if ( nullptr == lineArray ) { return false; }

    /// 4byte alignment;
    width = (width + 3) & ~3u;

    _lines_buffer = lineArray;
    memset(lineArray, 0, height * sizeof(uint8_t*));

    size_t alloc_idx = 0;

    if (use_psram != 1)
    {
      for (int y = 0; y < height; y += linesPerChunk)
      {
        {
          uint32_t lines_remain = height - y;
          if (lines_remain > linesPerChunk) { lines_remain = linesPerChunk; }
          size_t chunkSize = width * lines_remain;
  //  ESP_LOGE(TAG, "y:%d i:%d interleave:%d lines:%d chunksize: %d", y, i, getIndexInterleave(idx), lines_remain, chunkSize);

          uint8_t* lineChunk = sub_heap_alloc(use_psram, chunkSize);
// printf ("line %08x alloc \n", lineChunk);
          if (lineChunk == nullptr)
          {
            ESP_LOGE(TAG, "framebuffer memory alloc fail.");

            deinitFrameBuffer();
            return false;
          }
          allocated_list[alloc_idx++] = y;
          size_t j = 0;
          do
          {
            lineArray[y + j] = lineChunk;
            lineChunk += width;
          } while (++j < lines_remain);
        }
      }
    }
    else
    {
      bool flg_psram = false;
      for (int y = 0; y < height; y += linesPerChunk * 2)
      {
        for (int i = 0; i <= 2; i += 2)
        {
          size_t idx = y + i;
          if (idx >= height) { break; }

          uint32_t lines_remain = 0;
          while (++lines_remain < linesPerChunk && (idx + getIndexInterleave(lines_remain) < height)) {}
          size_t chunkSize = width * lines_remain;
  //  ESP_LOGE(TAG, "y:%d i:%d interleave:%d lines:%d chunksize: %d", y, i, getIndexInterleave(idx), lines_remain, chunkSize);

          flg_psram = use_psram && (!flg_psram || use_psram > 1);
          uint8_t*  lineChunk = sub_heap_alloc(flg_psram, chunkSize);
// printf ("line %08x alloc \n", lineChunk);
          if (lineChunk == nullptr)
          {
            ESP_LOGE(TAG, "framebuffer memory alloc fail.");

            deinitFrameBuffer();
            return false;
          }
          allocated_list[alloc_idx++] = idx;
          size_t j = 0;
          do
          {
            lineArray[idx + getIndexInterleave(j)] = lineChunk;
            lineChunk += width;
          } while (++j < lines_remain);
        }
      }
    }

    return true;
  }

  void Panel_CVBS::deinitFrameBuffer(void)
  {
    auto lines = internal.lines;
    auto lst = internal.allocated_list;
    internal.lines = nullptr;
    internal.allocated_list = nullptr;
    _lines_buffer = nullptr;
    if (lst != nullptr)
    {
      if (lines != nullptr)
      {
        for (size_t i = 0; lst[i] != 0xFFFF; ++i)
        {
          heap_free(lines[lst[i]]);
          lines[lst[i]] = nullptr;
        }
        heap_free(lines);
      }
      heap_free(lst);
    }
  }

//----------------------------------------------------------------------------
 }
}
#endif
#endif
