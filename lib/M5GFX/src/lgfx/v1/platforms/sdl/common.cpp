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
#if !defined (ARDUINO)

#include "common.hpp"

#if defined ( SDL_h_ )

#include <chrono>
#include <thread>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static uint8_t _gpio_dummy_values[EMULATED_GPIO_MAX];

  void pinMode(int_fast16_t pin, pin_mode_t mode) {}
  void lgfxPinMode(int_fast16_t pin, pin_mode_t mode) {}

  void gpio_hi(uint32_t pin) { _gpio_dummy_values[pin & (EMULATED_GPIO_MAX - 1)] = 1; }
  void gpio_lo(uint32_t pin) { _gpio_dummy_values[pin & (EMULATED_GPIO_MAX - 1)] = 0; }
  bool gpio_in(uint32_t pin) { return _gpio_dummy_values[pin & (EMULATED_GPIO_MAX - 1)]; }

  unsigned long millis(void)
  {
    return SDL_GetTicks();
  }

  unsigned long micros(void)
  {
    return SDL_GetPerformanceCounter() / (SDL_GetPerformanceFrequency() / (1000 * 1000));
  }

  void delay(unsigned long milliseconds)
  {
    if (milliseconds < 1024)
    {
      delayMicroseconds(milliseconds * 1000);
    }
    else
    {
      SDL_Delay(milliseconds);
    }
  }

  void delayMicroseconds(unsigned int us)
  {
    auto start = micros();
    if (us >= 2000)
    {
      SDL_Delay((us / 1000) - 1);
    }
    do
    {
      std::this_thread::yield();
    } while (micros() - start < us);
  }

//----------------------------------------------------------------------------

  namespace spi
  {
    cpp::result<void, error_t> init(int spi_host, int spi_sclk, int spi_miso, int spi_mosi)  { return cpp::fail(error_t::unknown_err); }
    void release(int spi_host) {}
    void beginTransaction(int spi_host, uint32_t freq, int spi_mode) {}
    void endTransaction(int spi_host) {}
    void writeBytes(int spi_host, const uint8_t* data, size_t length) {}
    void readBytes(int spi_host, uint8_t* data, size_t length) {}
  }

//----------------------------------------------------------------------------

  namespace i2c
  {
    cpp::result<void, error_t> init(int i2c_port, int pin_sda, int pin_scl) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> release(int i2c_port) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> restart(int i2c_port, int i2c_addr, uint32_t freq, bool read) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> beginTransaction(int i2c_port, int i2c_addr, uint32_t freq, bool read) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> endTransaction(int i2c_port) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> writeBytes(int i2c_port, const uint8_t *data, size_t length) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> readBytes(int i2c_port, uint8_t *data, size_t length, bool last_nack = false) { return cpp::fail(error_t::unknown_err); }

//--------

    cpp::result<void, error_t> transactionWrite(int i2c_port, int addr, const uint8_t *writedata, uint8_t writelen, uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> transactionRead(int i2c_port, int addr, uint8_t *readdata, uint8_t readlen, uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> transactionWriteRead(int i2c_port, int addr, const uint8_t *writedata, uint8_t writelen, uint8_t *readdata, size_t readlen, uint32_t freq)  { return cpp::fail(error_t::unknown_err); }

    cpp::result<uint8_t, error_t> readRegister8(int i2c_port, int addr, uint8_t reg, uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> writeRegister8(int i2c_port, int addr, uint8_t reg, uint8_t data, uint8_t mask, uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
  }

//----------------------------------------------------------------------------
 }
}

#endif
#endif
