#ifndef __M5GFX_M5UNITLCD__
#define __M5GFX_M5UNITLCD__

// If you want to use a set of functions to handle SD/SPIFFS/HTTP,
//  please include <SD.h>,<SPIFFS.h>,<HTTPClient.h> before <M5GFX.h>
// #include <SD.h>
// #include <SPIFFS.h>
// #include <HTTPClient.h>

#if defined (ESP_PLATFORM)
 #include <sdkconfig.h>
#else
 #include "lgfx/v1/platforms/sdl/Panel_sdl.hpp"
#endif

#include "lgfx/v1/panel/Panel_M5UnitLCD.hpp"
#include "M5GFX.h"

#ifndef M5UNITLCD_SDA
 #if defined ( ARDUINO )
  #define M5UNITLCD_SDA SDA
 #elif defined (CONFIG_IDF_TARGET_ESP32S3) || defined (CONFIG_IDF_TARGET_ESP32C6)
  #define M5UNITLCD_SDA 2
 #elif defined (CONFIG_IDF_TARGET_ESP32C3)
  #define M5UNITLCD_SDA 1
 #else
  #define M5UNITLCD_SDA 21
 #endif
#endif

#ifndef M5UNITLCD_SCL
 #if defined ( ARDUINO )
  #define M5UNITLCD_SCL SCL
 #elif defined (CONFIG_IDF_TARGET_ESP32S3) || defined (CONFIG_IDF_TARGET_ESP32C6)
  #define M5UNITLCD_SCL 1
 #elif defined (CONFIG_IDF_TARGET_ESP32C3)
  #define M5UNITLCD_SCL 0
 #else
  #define M5UNITLCD_SCL 22
 #endif
#endif

#ifndef M5UNITLCD_ADDR
#define M5UNITLCD_ADDR 0x3E
#endif

#ifndef M5UNITLCD_FREQ
#define M5UNITLCD_FREQ 400000
#endif

class M5UnitLCD : public M5GFX
{
  lgfx::Bus_I2C::config_t _bus_cfg;

public:

  struct config_t
  {
    uint8_t pin_sda = 255;
    uint8_t pin_scl = 255;
    uint8_t i2c_addr = M5UNITLCD_ADDR;
    int8_t i2c_port = -1;
    uint32_t i2c_freq = M5UNITLCD_FREQ;
  };

  config_t config(void) const { return config_t(); }

#if defined (SDL_h_)

  M5UnitLCD(const config_t &cfg)
  {
    setup(cfg.pin_sda, cfg.pin_scl, cfg.i2c_freq, cfg.i2c_port, cfg.i2c_addr);
  }
  M5UnitLCD(uint8_t pin_sda = M5UNITLCD_SDA, uint8_t pin_scl = M5UNITLCD_SCL, uint32_t i2c_freq = M5UNITLCD_FREQ, int8_t i2c_port = -1, uint8_t i2c_addr = M5UNITLCD_ADDR)
  {
    setup(pin_sda, pin_scl, i2c_freq, i2c_port, i2c_addr);
  }

  using lgfx::LGFX_Device::init;
  bool init(uint8_t pin_sda, uint8_t pin_scl, uint32_t i2c_freq = M5UNITLCD_FREQ, int8_t i2c_port = -1, uint8_t i2c_addr = M5UNITLCD_ADDR)
  {
    return init();
  }

  void setup(uint8_t pin_sda = M5UNITLCD_SDA, uint8_t pin_scl = M5UNITLCD_SCL, uint32_t i2c_freq = M5UNITLCD_FREQ, int8_t i2c_port = -1, uint8_t i2c_addr = M5UNITLCD_ADDR)
  {
    _board = lgfx::board_t::board_M5UnitLCD;
  }

  bool init_impl(bool use_reset, bool use_clear)
  {
    if (_panel_last.get() != nullptr) {
      return true;
    }
    auto p = new lgfx::Panel_sdl;
    {
      auto cfg = p->config();
      cfg.memory_width = 135;
      cfg.panel_width  = 135;
      cfg.memory_height = 240;
      cfg.panel_height  = 240;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      p->config(cfg);
      uint_fast8_t scale = 1;
#if defined (M5GFX_SCALE)
 #if M5GFX_SCALE > 1
      scale = M5GFX_SCALE;
 #endif
#endif
      p->setScaling(scale, scale);
      p->setWindowTitle("UnitLCD");
    }
    setPanel(p);
    if (lgfx::LGFX_Device::init_impl(use_reset, use_clear)) {
      _panel_last.reset(p);
      return true;
    }
    setPanel(nullptr);
    delete p;
    return false;
  }

#else

  M5UnitLCD(const config_t &cfg)
  {
    uint8_t pin_sda = cfg.pin_sda < GPIO_NUM_MAX ? cfg.pin_sda : M5UNITLCD_SDA;
    uint8_t pin_scl = cfg.pin_scl < GPIO_NUM_MAX ? cfg.pin_scl : M5UNITLCD_SCL;
    setup(pin_sda, pin_scl, cfg.i2c_freq, cfg.i2c_port, cfg.i2c_addr);
  }

  M5UnitLCD(uint8_t pin_sda = M5UNITLCD_SDA, uint8_t pin_scl = M5UNITLCD_SCL, uint32_t i2c_freq = M5UNITLCD_FREQ, int8_t i2c_port = -1, uint8_t i2c_addr = M5UNITLCD_ADDR)
  {
    setup(pin_sda, pin_scl, i2c_freq, i2c_port, i2c_addr);
  }

  using lgfx::LGFX_Device::init;
  bool init(uint8_t pin_sda, uint8_t pin_scl, uint32_t i2c_freq = M5UNITLCD_FREQ, int8_t i2c_port = -1, uint8_t i2c_addr = M5UNITLCD_ADDR)
  {
    setup(pin_sda, pin_scl, i2c_freq, i2c_port, i2c_addr);
    return init();
  }

  void setup(uint8_t pin_sda = M5UNITLCD_SDA, uint8_t pin_scl = M5UNITLCD_SCL, uint32_t i2c_freq = M5UNITLCD_FREQ, int8_t i2c_port = -1, uint8_t i2c_addr = M5UNITLCD_ADDR)
  {
    _board = lgfx::board_t::board_M5UnitLCD;
    if (i2c_port < 0)
    {
      i2c_port = 0;
#ifdef _M5EPD_H_
      if ((pin_sda == 25 && pin_scl == 32)  /// M5Paper
      {
        i2c_port = 1
      }
#endif
    }

    {
      _bus_cfg.freq_write = i2c_freq;
      _bus_cfg.freq_read = i2c_freq > 400000 ? 400000 + ((i2c_freq - 400000) >> 1) : i2c_freq;
      _bus_cfg.pin_scl = pin_scl;
      _bus_cfg.pin_sda = pin_sda;
      _bus_cfg.i2c_port = i2c_port;
      _bus_cfg.i2c_addr = i2c_addr;
      _bus_cfg.prefix_len = 0;
    }
  }

  bool init_impl(bool use_reset, bool use_clear)
  {
    if (_panel_last.get() != nullptr) {
      return true;
    }
    auto p = new lgfx::Panel_M5UnitLCD;
    auto b = new lgfx::Bus_I2C();
    b->config(_bus_cfg);
    {
      p->bus(b);
      auto cfg = p->config();
      cfg.memory_width     = 135;
      cfg.memory_height    = 240;
      cfg.panel_width      = 135;
      cfg.panel_height     = 240;
      cfg.offset_x         =   0;
      cfg.offset_rotation  =   0;
      cfg.bus_shared       = false;
      p->config(cfg);
    }
    setPanel(p);
    if (lgfx::LGFX_Device::init_impl(use_reset, use_clear)) {
      _panel_last.reset(p);
      _bus_last.reset(b);
      return true;
    }
    setPanel(nullptr);
    delete p;
    delete b;
    return false;
  }
#endif
};

#endif
