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

Porting for SDL:
 [imliubo](https://github.com/imliubo)
/----------------------------------------------------------------------------*/
#pragma once

#include "../../misc/DataWrapper.hpp"
#include "../../misc/enum.hpp"
#include "../../../utility/result.hpp"

#include <stdlib.h>
#include <stdio.h>

#define SDL_MAIN_HANDLED
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL_main.h>
#include <SDL.h>
#endif
#if defined (SDL_h_)

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------
  static constexpr size_t EMULATED_GPIO_MAX = 128;

  unsigned long millis(void);

  unsigned long micros(void);

  void delay(unsigned long milliseconds);

  void delayMicroseconds(unsigned int us);

  static inline void* heap_alloc(      size_t length) { return malloc(length); }
  static inline void* heap_alloc_psram(size_t length) { return malloc(length); }
  static inline void* heap_alloc_dma(  size_t length) { return malloc(length); } // aligned_alloc(16, length);
  static inline void heap_free(void* buf) { free(buf); }

  void gpio_hi(uint32_t pin);
  void gpio_lo(uint32_t pin);
  bool gpio_in(uint32_t pin);

  enum pin_mode_t
  { output
  , input
  , input_pullup
  , input_pulldown
  };

  void pinMode(int_fast16_t pin, pin_mode_t mode);
  void lgfxPinMode(int_fast16_t pin, pin_mode_t mode);

//----------------------------------------------------------------------------

  struct FileWrapper : public DataWrapper
  {
    FileWrapper() : DataWrapper()
    {
      need_transaction = false;
    }
    FILE* _fp;
#if defined (__STDC_WANT_SECURE_LIB__)
    bool open(const char* path) override { return (0 == fopen_s(&_fp, path, "rb")); }
#else
    bool open(const char* path) override { return (_fp = fopen(path, "rb")); }
#endif
    int read(uint8_t *buf, uint32_t len) override { return fread((char*)buf, 1, len, _fp); }
    void skip(int32_t offset) override { seek(offset, SEEK_CUR); }
    bool seek(uint32_t offset) override { return seek(offset, SEEK_SET); }
    bool seek(uint32_t offset, int origin) { return fseek(_fp, offset, origin); }
    void close() override { if (_fp) fclose(_fp); }
    int32_t tell(void) override { return ftell(_fp); }
  };

//----------------------------------------------------------------------------

  namespace spi
  {
    cpp::result<void, error_t> init(int spi_host, int spi_sclk, int spi_miso, int spi_mosi, int dma_channel);
    void beginTransaction(int spi_host);
  }

//----------------------------------------------------------------------------

  namespace i2c
  {
    static inline cpp::result<void, error_t> setPins(int i2c_port, int pin_sda, int pin_scl) { return {}; }
    static inline cpp::result<void, error_t> init(int i2c_port) { return {}; }
  }

//----------------------------------------------------------------------------
 }
}

#endif