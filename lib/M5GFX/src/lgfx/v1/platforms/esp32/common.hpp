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

#include "../../misc/DataWrapper.hpp"
#include "../../misc/enum.hpp"
#include "../../../utility/result.hpp"

#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <soc/soc.h>
#include <soc/spi_reg.h>
#include <soc/i2s_reg.h>
#include <soc/gpio_struct.h>
#include <soc/gpio_sig_map.h>
#include <esp_timer.h>

#if !defined ( REG_SPI_BASE )
 /// ESP32-S3をターゲットにした際にREG_SPI_BASEが定義されていなかったので応急処置 5.3まで;
 #if defined ( CONFIG_IDF_TARGET_ESP32S3 )
  #define REG_SPI_BASE(i)   (DR_REG_SPI1_BASE + (((i)>1) ? (((i)* 0x1000) + 0x20000) : (((~(i)) & 1)* 0x1000 )))
 #else
  //#define REG_SPI_BASE(i) (DR_REG_SPI0_BASE - (i) * 0x1000)
  #define REG_SPI_BASE(i)     (DR_REG_SPI2_BASE)
 #endif
#endif

#if defined ( ESP_IDF_VERSION_VAL )
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  #define LGFX_IDF_V5
 #endif
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  __attribute__ ((unused)) static inline unsigned long millis(void) { return (unsigned long) (esp_timer_get_time() / 1000ULL); }
  __attribute__ ((unused)) static inline unsigned long micros(void) { return (unsigned long) (esp_timer_get_time()); }
  __attribute__ ((unused)) static inline void delayMicroseconds(uint32_t us)
  {
#if defined ( LGFX_IDF_V5 )
    esp_rom_delay_us(us);
#else
    ets_delay_us(us);
#endif
  }
  __attribute__ ((unused)) static inline void delay(uint32_t ms)
  {
    uint32_t time = micros();
    vTaskDelay( std::max<uint32_t>(2u, ms / portTICK_PERIOD_MS) - 1 );
    if (ms != 0 && ms < portTICK_PERIOD_MS*8)
    {
      ms *= 1000;
      time = micros() - time;
      if (time < ms)
      {
        delayMicroseconds(ms - time);
      }
    }
  }

  static inline void* heap_alloc(      size_t length) { return heap_caps_malloc(length, MALLOC_CAP_8BIT);  }
  static inline void* heap_alloc_dma(  size_t length) { return heap_caps_malloc((length + 3) & ~3, MALLOC_CAP_DMA);  }
  static inline void* heap_alloc_psram(size_t length) { return heap_caps_malloc((length + 3) & ~3, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);  }
  static inline void heap_free(void* buf) { heap_caps_free(buf); }

  /// 引数のポインタが組込RAMか判定する  true=内部RAM / false=外部RAMやROM等;
#if defined ( CONFIG_IDF_TARGET_ESP32S3 )
  static inline bool isEmbeddedMemory(const void* ptr) { return (((uintptr_t)ptr & 0x3FF80000u) == 0x3FC80000u); }
#else
  static inline bool isEmbeddedMemory(const void* ptr) { return (((uintptr_t)ptr & 0x3FF80000u) == 0x3FF00000u); }
#endif

  enum pin_mode_t
  { output
  , input
  , input_pullup
  , input_pulldown
  };

  void pinMode(int_fast16_t pin, pin_mode_t mode);
  inline void lgfxPinMode(int_fast16_t pin, pin_mode_t mode)
  {
    pinMode(pin, mode);
  }

#if defined ( CONFIG_IDF_TARGET_ESP32C3 ) || defined ( CONFIG_IDF_TARGET_ESP32C6 )
  static inline volatile uint32_t* get_gpio_hi_reg(int_fast8_t pin) { return &GPIO.out_w1ts.val; }
  static inline volatile uint32_t* get_gpio_lo_reg(int_fast8_t pin) { return &GPIO.out_w1tc.val; }
  static inline bool gpio_in(int_fast8_t pin) { return GPIO.in.val & (1 << (pin & 31)); }
#else
  static inline volatile uint32_t* get_gpio_hi_reg(int_fast8_t pin) { return (pin & 32) ? &GPIO.out1_w1ts.val : &GPIO.out_w1ts; }
//static inline volatile uint32_t* get_gpio_hi_reg(int_fast8_t pin) { return (volatile uint32_t*)((pin & 32) ? 0x60004014 : 0x60004008) ; } // workaround Eratta
  static inline volatile uint32_t* get_gpio_lo_reg(int_fast8_t pin) { return (pin & 32) ? &GPIO.out1_w1tc.val : &GPIO.out_w1tc; }
//static inline volatile uint32_t* get_gpio_lo_reg(int_fast8_t pin) { return (volatile uint32_t*)((pin & 32) ? 0x60004018 : 0x6000400C) ; }
  static inline bool gpio_in(int_fast8_t pin) { return ((pin & 32) ? GPIO.in1.data : GPIO.in) & (1 << (pin & 31)); }
#endif
  static inline void gpio_hi(int_fast8_t pin) { if (pin >= 0) *get_gpio_hi_reg(pin) = 1 << (pin & 31); } // ESP_LOGI("LGFX", "gpio_hi: %d", pin); }
  static inline void gpio_lo(int_fast8_t pin) { if (pin >= 0) *get_gpio_lo_reg(pin) = 1 << (pin & 31); } // ESP_LOGI("LGFX", "gpio_lo: %d", pin); }

  uint32_t getApbFrequency(void);
  uint32_t FreqToClockDiv(uint32_t fapb, uint32_t hz);

  /// for I2S and LCD_CAM peripheral clock
  void calcClockDiv(uint32_t* div_a, uint32_t* div_b, uint32_t* div_n, uint32_t* clkcnt, uint32_t baseClock, uint32_t targetFreq);

  // esp_efuse_get_pkg_ver
  uint32_t get_pkg_ver(void);

  // Find GDMA assigned to a peripheral;
  int32_t search_dma_out_ch(int peripheral_select);
  int32_t search_dma_in_ch(int peripheral_select);

  void debug_memory_dump(const void* src, size_t len);

//----------------------------------------------------------------------------

#if defined (ARDUINO)
 #if defined (_SD_H_)
   #define LGFX_FILESYSTEM_SD SD
 #endif
 #if defined (_LITTLEFS_H_) || defined (__LITTLEFS_H) || defined (_LiffleFS_H_)
   #define LGFX_FILESYSTEM_LITTLEFS LittleFS
 #endif
 #if defined (_SPIFFS_H_)
   #define LGFX_FILESYSTEM_SPIFFS SPIFFS
 #endif
 #if defined (_FFAT_H_)
   #define LGFX_FILESYSTEM_FFAT FFat
 #endif

 #if defined (FS_H) \
  || defined (LGFX_FILESYSTEM_SD) \
  || defined (LGFX_FILESYSTEM_LITTLEFS) \
  || defined (LGFX_FILESYSTEM_SPIFFS) \
  || defined (LGFX_FILESYSTEM_FFAT)

  template <>
  struct DataWrapperT<fs::File> : public DataWrapper {
    DataWrapperT(fs::File* fp = nullptr) : DataWrapper{}, _fp { fp } {
      need_transaction = true;
    }
    int read(uint8_t *buf, uint32_t len) override { return _fp->read(buf, len); }
    //void skip(int32_t offset) override { _fp->seek(offset, SeekCur); }
    //bool seek(uint32_t offset) override { return _fp->seek(offset, SeekSet); }
    //bool seek(uint32_t offset, SeekMode mode) { return _fp->seek(offset, mode); }
    void close(void) override { if (_fp) _fp->close(); }
    int32_t tell(void) override { return _fp->position(); }
protected:
    fs::File *_fp;
  };

  template <>
  struct DataWrapperT<fs::FS> : public DataWrapperT<fs::File> {
    DataWrapperT(fs::FS* fs, fs::File* fp = nullptr) : DataWrapperT<fs::File> { fp }, _fs { fs } {
#if defined (LGFX_FILESYSTEM_SD)
      need_transaction = (fs == &LGFX_FILESYSTEM_SD);
#endif
    }
    bool open(const char* path) override
    {
      _file = _fs->open(path, "r");
      DataWrapperT<fs::File>::_fp = &_file;
      return _file;
    }

protected:
    fs::FS* _fs;
    fs::File _file;
  };

  #if defined (LGFX_FILESYSTEM_SD)
  template <>
  struct DataWrapperT<fs::SDFS> : public DataWrapperT<fs::FS> {
    DataWrapperT(fs::FS* fs, fs::File* fp = nullptr) : DataWrapperT<fs::FS>(fs, fp) {}
  };
  #endif
  #if defined (LGFX_FILESYSTEM_SPIFFS)
  template <>
  struct DataWrapperT<fs::SPIFFSFS> : public DataWrapperT<fs::FS> {
    DataWrapperT(fs::FS* fs, fs::File* fp = nullptr) : DataWrapperT<fs::FS>(fs, fp) {}
  };
  #endif
  #if defined (LGFX_FILESYSTEM_LITTLEFS)
  template <>
  struct DataWrapperT<fs::LittleFSFS> : public DataWrapperT<fs::FS> {
    DataWrapperT(fs::FS* fs, fs::File* fp = nullptr) : DataWrapperT<fs::FS>(fs, fp) {}
  };
  #endif
  #if defined (LGFX_FILESYSTEM_FFAT)
  template <>
  struct DataWrapperT<fs::F_Fat> : public DataWrapperT<fs::FS> {
    DataWrapperT(fs::FS* fs, fs::File* fp = nullptr) : DataWrapperT<fs::FS>(fs, fp) {}
  };
  #endif
 #endif
#endif

//----------------------------------------------------------------------------

  namespace gpio
  {
    class pin_backup_t
    {
    public:
      pin_backup_t(int pin_num);
      void backup(void);
      void restore(void);

    private:
      uint32_t _io_mux_gpio_reg;
      uint32_t _gpio_pin_reg;
      uint32_t _gpio_func_out_reg;
      gpio_num_t _pin_num;
      bool _gpio_enable;
    };

    enum command_t : uint8_t
    {
      command_end = 0,              // コマンド終了
      command_read,                 // [1]=GPIO番号 1bit読みとる
      command_write_low,            // [1]=GPIO番号 LOW出力
      command_write_high,           // [1]=GPIO番号 HIGH出力
      command_mode_output,          // [1]=GPIO番号 outputモードに変更する
      command_mode_input,           // [1]=GPIO番号 inputモードに変更する
      command_mode_input_pulldown,  // [1]=GPIO番号 input pulldownモードに変更する
      command_mode_input_pullup,    // [1]=GPIO番号 input pullupモードに変更する
      command_delay,                // [1]=停止する時間[ミリ秒]
    };
    bool command(command_t cmd, uint8_t pin);
    uint32_t command(const uint8_t* cmd_list);
  }

//----------------------------------------------------------------------------

  namespace spi
  {
    cpp::result<void, error_t> init(int spi_host, int spi_sclk, int spi_miso, int spi_mosi, int dma_channel);
    void beginTransaction(int spi_host);
  }

//----------------------------------------------------------------------------

  namespace i2c
  {
    cpp::result<void, error_t> setPins(int i2c_port, int pin_sda, int pin_scl);
    cpp::result<void, error_t> init(int i2c_port);
  }

//----------------------------------------------------------------------------
 }
}
