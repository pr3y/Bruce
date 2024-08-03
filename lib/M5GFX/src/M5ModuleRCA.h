#ifndef __M5GFX_M5MODULERCA__
#define __M5GFX_M5MODULERCA__

// If you want to use a set of functions to handle SD/SPIFFS/HTTP,
//  please include <SD.h>,<SPIFFS.h>,<HTTPClient.h> before <M5GFX.h>
// #include <SD.h>
// #include <SPIFFS.h>
// #include <HTTPClient.h>

#include "lgfx/v1/platforms/esp32/Panel_CVBS.hpp"
#include "M5GFX.h"

#if __has_include( <sdkconfig.h> )
#include <sdkconfig.h>
#ifndef M5MODULERCA_PIN_DAC
 #if defined ( CONFIG_IDF_TARGET_ESP32 ) || !defined ( CONFIG_IDF_TARGET )
  #define M5MODULERCA_PIN_DAC GPIO_NUM_26
 #else
  #define M5MODULERCA_PIN_DAC GPIO_NUM_NC
 #endif
#endif
#else
#include "lgfx/v1/platforms/sdl/Panel_sdl.hpp"
#ifndef M5MODULERCA_PIN_DAC
 #define M5MODULERCA_PIN_DAC -1
#endif
#endif

#ifndef M5MODULERCA_LOGICAL_WIDTH
#define M5MODULERCA_LOGICAL_WIDTH 216
#endif
#ifndef M5MODULERCA_LOGICAL_HEIGHT
#define M5MODULERCA_LOGICAL_HEIGHT 144
#endif
#ifndef M5MODULERCA_SIGNAL_TYPE
#define M5MODULERCA_SIGNAL_TYPE signal_type_t::PAL
#endif
#ifndef M5MODULERCA_OUTPUT_WIDTH
#define M5MODULERCA_OUTPUT_WIDTH 0
#endif
#ifndef M5MODULERCA_OUTPUT_HEIGHT
#define M5MODULERCA_OUTPUT_HEIGHT 0
#endif
#ifndef M5MODULERCA_USE_PSRAM
#define M5MODULERCA_USE_PSRAM use_psram_t::psram_half_use
#endif
#ifndef M5MODULERCA_OUTPUT_LEVEL
#define M5MODULERCA_OUTPUT_LEVEL 0
#endif

class M5ModuleRCA : public M5GFX
{
  lgfx::Panel_CVBS::config_t _cfg;
  lgfx::Panel_CVBS::config_detail_t _cfg_detail;

public:

  typedef lgfx::Panel_CVBS::config_detail_t::signal_type_t signal_type_t;
  enum use_psram_t {
    psram_no_use = 0,
    psram_half_use = 1,
    psram_use = 2,
  };

  struct config_t
  {
    uint16_t logical_width    = M5MODULERCA_LOGICAL_WIDTH;
    uint16_t logical_height   = M5MODULERCA_LOGICAL_HEIGHT;
    uint16_t output_width     = M5MODULERCA_OUTPUT_WIDTH;
    uint16_t output_height    = M5MODULERCA_OUTPUT_HEIGHT;
    signal_type_t signal_type = M5MODULERCA_SIGNAL_TYPE;
    use_psram_t use_psram     = M5MODULERCA_USE_PSRAM;
    uint8_t  pin_dac          = M5MODULERCA_PIN_DAC;
    uint8_t  output_level     = M5MODULERCA_OUTPUT_LEVEL;
  };

  config_t config(void) const { return config_t(); }

  M5ModuleRCA( const config_t& cfg )
  {
    _board = lgfx::board_t::board_M5ModuleRCA;
    setup(cfg.logical_width, cfg.logical_height, cfg.output_width, cfg.output_height, cfg.signal_type, cfg.use_psram, cfg.pin_dac, cfg.output_level);
  }

  M5ModuleRCA( uint16_t logical_width    = M5MODULERCA_LOGICAL_WIDTH
             , uint16_t logical_height   = M5MODULERCA_LOGICAL_HEIGHT
             , uint16_t output_width     = M5MODULERCA_OUTPUT_WIDTH
             , uint16_t output_height    = M5MODULERCA_OUTPUT_HEIGHT
             , signal_type_t signal_type = M5MODULERCA_SIGNAL_TYPE
             , use_psram_t use_psram     = M5MODULERCA_USE_PSRAM
             , uint8_t  pin_dac          = M5MODULERCA_PIN_DAC
             , uint8_t  output_level     = M5MODULERCA_OUTPUT_LEVEL
             )
  {
    _board = lgfx::board_t::board_M5ModuleRCA;
    setup(logical_width, logical_height, output_width, output_height, signal_type, use_psram, pin_dac, output_level);
  }

  using lgfx::LGFX_Device::init;
  bool init( uint16_t logical_width
           , uint16_t logical_height
           , uint16_t output_width     = M5MODULERCA_OUTPUT_WIDTH
           , uint16_t output_height    = M5MODULERCA_OUTPUT_HEIGHT
           , signal_type_t signal_type = M5MODULERCA_SIGNAL_TYPE
           , use_psram_t use_psram     = M5MODULERCA_USE_PSRAM
           , uint8_t  pin_dac          = M5MODULERCA_PIN_DAC
           , uint8_t  output_level     = M5MODULERCA_OUTPUT_LEVEL
           )
  {
    setup(logical_width, logical_height, output_width, output_height, signal_type, use_psram, pin_dac, output_level);
    return init();
  }

  void setup(uint16_t logical_width    = M5MODULERCA_LOGICAL_WIDTH
           , uint16_t logical_height   = M5MODULERCA_LOGICAL_HEIGHT
           , uint16_t output_width     = M5MODULERCA_OUTPUT_WIDTH
           , uint16_t output_height    = M5MODULERCA_OUTPUT_HEIGHT
           , signal_type_t signal_type = M5MODULERCA_SIGNAL_TYPE
           , use_psram_t use_psram     = M5MODULERCA_USE_PSRAM
           , uint8_t  pin_dac          = M5MODULERCA_PIN_DAC
           , uint8_t  output_level     = M5MODULERCA_OUTPUT_LEVEL
           )
  {
    if (output_width  < logical_width ) { output_width  = logical_width;  }
    if (output_height < logical_height) { output_height = logical_height; }

    _cfg.panel_width   = logical_width;
    _cfg.panel_height  = logical_height;
    _cfg.memory_width  = output_width;
    _cfg.memory_height = output_height;
    _cfg.offset_x = (output_width  - logical_width ) >> 1;
    _cfg.offset_y = (output_height - logical_height) >> 1;

    _cfg_detail.signal_type = signal_type;
    _cfg_detail.use_psram = use_psram;
    _cfg_detail.pin_dac = pin_dac;
    _cfg_detail.output_level = output_level;
  }

#if defined (SDL_h_)
  bool init_impl(bool use_reset, bool use_clear)
  {
    if (_panel_last.get() != nullptr) {
      return true;
    }
    auto p = new lgfx::Panel_sdl();
    if (!p) {
      return false;
    }
    {
      auto pnl_cfg = p->config();
      pnl_cfg.memory_width = _cfg.memory_width;
      pnl_cfg.panel_width = _cfg.panel_width;
      pnl_cfg.memory_height = _cfg.memory_height;
      pnl_cfg.panel_height = _cfg.panel_height;
      pnl_cfg.bus_shared = false;
      pnl_cfg.offset_rotation = 3;
      p->config(pnl_cfg);
      p->setWindowTitle("ModuleRCA");
      p->setScaling(864/_cfg.panel_width, 576/_cfg.panel_height);
      p->setRotation(1);
      setPanel(p);
      _panel_last.reset(p);
    }

    if (lgfx::LGFX_Device::init_impl(use_reset, use_clear))
    {
      return true;
    }
    setPanel(nullptr);
    _panel_last.reset();
    return false;
  }
  void setOutputLevel(uint8_t output_level) { }
  inline void setOutputBoost(bool boost = true) { }
  void setOutputPin(uint8_t pin_dac) { }
  void setSignalType(signal_type_t signal_type) { }
  void setPsram(use_psram_t use_psram) { }
  inline void setPsram(uint8_t use_psram) { }

#else

  bool init_impl(bool use_reset, bool use_clear)
  {
    if (_panel_last.get() != nullptr) {
      return true;
    }
    auto p = new lgfx::Panel_CVBS();
    if (!p) {
      return false;
    }

#if defined (CONFIG_IDF_TARGET_ESP32S3)
#elif !defined (CONFIG_IDF_TARGET) || defined (CONFIG_IDF_TARGET_ESP32)

    if (_cfg_detail.output_level == 0) {
      if (0x03 == m5gfx::i2c::readRegister8(1, 0x34, 0x03, 400000))
      { // M5Stack Core2 / Tough
#if defined ( ESP_LOGD )
        ESP_LOGD("LGFX","ModuleRCA with Core2/Tough");
#endif
        _cfg_detail.output_level = 200;
      }
      else
      { // M5Stack BASIC / FIRE / GO
#if defined ( ESP_LOGD )
        ESP_LOGD("LGFX","ModuleRCA with Core Basic/Fire/Go");
#endif
        _cfg_detail.output_level = 128;
      }
    }
#endif

    _cfg.offset_rotation = 3;
    _cfg.bus_shared = false;

    p->config_detail(_cfg_detail);
    p->config(_cfg);
    p->setRotation(1);
    setPanel(p);

    if (lgfx::LGFX_Device::init_impl(use_reset, use_clear))
    {
      _panel_last.reset(p);
      return true;
    }
    setPanel(nullptr);
    delete p;
    return false;
  }

  // Adjusts the voltage level of the output signal. Initial value 128
  // If the GPIO has a protection resistor, the output voltage will drop, so set a higher value in this function to adjust it.
  // e.g. For M5Stack Core2, specify 200.
  void setOutputLevel(uint8_t output_level) {
    auto p = (lgfx::Panel_CVBS*)_panel_last.get();
    if (p) {
      _cfg_detail = p->config_detail();
      _cfg_detail.output_level = output_level;
      p->config_detail(_cfg_detail);
    }
  }

  // for M5Stack Core2
  inline void setOutputBoost(bool boost = true) {
    setOutputLevel(boost ? 200 : 128);
  }

  void setOutputPin(uint8_t pin_dac) {
    auto p = (lgfx::Panel_CVBS*)_panel_last.get();
    if (p) {
      _cfg_detail = p->config_detail();
      _cfg_detail.pin_dac = pin_dac;
      p->config_detail(_cfg_detail);
    }
  }

  // 出力信号の種類を設定する
  // NTSC,    // black = 7.5IRE
  // NTSC_J,  // black = 0IRE (for Japan)
  // PAL,
  // PAL_M,
  // PAL_N,
  void setSignalType(signal_type_t signal_type) {
    auto p = (lgfx::Panel_CVBS*)_panel_last.get();
    if (p) {
      _cfg_detail = p->config_detail();
      _cfg_detail.signal_type = signal_type;
      p->config_detail(_cfg_detail);
    }
  }

  // PSRAMの使用方法を設定する
  // psram_level_t::psram_no_use   = PSRAM不使用
  // psram_level_t::psram_half_use = SRAMとPSRAMを半分ずつ使用
  // psram_level_t::psram_use      = PSRAM使用
  void setPsram(use_psram_t use_psram) {
    auto p = (lgfx::Panel_CVBS*)_panel_last.get();
    if (p) {
      _cfg_detail = p->config_detail();
      _cfg_detail.use_psram = use_psram;
      p->config_detail(_cfg_detail);
    }
  }

  // PSRAM使用設定
  // 0 = PSRAM不使用
  // 1 = SRAMとPSRAMを半分ずつ使用
  // 2 = PSRAM使用
  inline void setPsram(uint8_t use_psram) {
    setPsram((use_psram_t)use_psram);
  }
#endif
};

#endif
