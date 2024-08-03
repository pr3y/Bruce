// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5GFX_H__
#define __M5GFX_H__

// If you want to use a set of functions to handle SD/SPIFFS/HTTP,
//  please include <SD.h>,<SPIFFS.h>,<HTTPClient.h> before <M5GFX.h>
// #include <SD.h>
// #include <SPIFFS.h>
// #include <HTTPClient.h>

#ifdef setFont
#undef setFont
#endif

#define LGFX_USE_V1
#include "lgfx/v1/gitTagVersion.h"
#include "lgfx/v1/platforms/device.hpp"
#include "lgfx/v1/platforms/common.hpp"
#include "lgfx/v1/lgfx_filesystem_support.hpp"
#include "lgfx/v1/LGFXBase.hpp"
#include "lgfx/v1/LGFX_Sprite.hpp"
#include "lgfx/v1/LGFX_Button.hpp"

#include <vector>
#include <memory>

namespace m5gfx
{
  using namespace lgfx;
//----------------------------------------------------------------------------

  namespace ili9341_colors  // Color definitions for backwards compatibility with old sketches
  {
    #ifdef ILI9341_BLACK
    #undef ILI9341_BLACK
    #undef ILI9341_NAVY
    #undef ILI9341_DARKGREEN
    #undef ILI9341_DARKCYAN
    #undef ILI9341_MAROON
    #undef ILI9341_PURPLE
    #undef ILI9341_OLIVE
    #undef ILI9341_LIGHTGREY
    #undef ILI9341_DARKGREY
    #undef ILI9341_BLUE
    #undef ILI9341_GREEN
    #undef ILI9341_CYAN
    #undef ILI9341_RED
    #undef ILI9341_MAGENTA
    #undef ILI9341_YELLOW
    #undef ILI9341_WHITE
    #undef ILI9341_ORANGE
    #undef ILI9341_GREENYELLOW
    #undef ILI9341_PINK
    #endif

    #ifdef BLACK
    #undef BLACK
    #undef NAVY
    #undef DARKGREEN
    #undef DARKCYAN
    #undef MAROON
    #undef PURPLE
    #undef OLIVE
    #undef LIGHTGREY
    #undef DARKGREY
    #undef BLUE
    #undef GREEN
    #undef CYAN
    #undef RED
    #undef MAGENTA
    #undef YELLOW
    #undef WHITE
    #undef ORANGE
    #undef GREENYELLOW
    #undef PINK
    #endif

    static constexpr int ILI9341_BLACK       = 0x0000;      /*   0,   0,   0 */
    static constexpr int ILI9341_NAVY        = 0x000F;      /*   0,   0, 128 */
    static constexpr int ILI9341_DARKGREEN   = 0x03E0;      /*   0, 128,   0 */
    static constexpr int ILI9341_DARKCYAN    = 0x03EF;      /*   0, 128, 128 */
    static constexpr int ILI9341_MAROON      = 0x7800;      /* 128,   0,   0 */
    static constexpr int ILI9341_PURPLE      = 0x780F;      /* 128,   0, 128 */
    static constexpr int ILI9341_OLIVE       = 0x7BE0;      /* 128, 128,   0 */
    static constexpr int ILI9341_LIGHTGREY   = 0xD69A;      /* 211, 211, 211 */
    static constexpr int ILI9341_DARKGREY    = 0x7BEF;      /* 128, 128, 128 */
    static constexpr int ILI9341_BLUE        = 0x001F;      /*   0,   0, 255 */
    static constexpr int ILI9341_GREEN       = 0x07E0;      /*   0, 255,   0 */
    static constexpr int ILI9341_CYAN        = 0x07FF;      /*   0, 255, 255 */
    static constexpr int ILI9341_RED         = 0xF800;      /* 255,   0,   0 */
    static constexpr int ILI9341_MAGENTA     = 0xF81F;      /* 255,   0, 255 */
    static constexpr int ILI9341_YELLOW      = 0xFFE0;      /* 255, 255,   0 */
    static constexpr int ILI9341_WHITE       = 0xFFFF;      /* 255, 255, 255 */
    static constexpr int ILI9341_ORANGE      = 0xFDA0;      /* 255, 180,   0 */
    static constexpr int ILI9341_GREENYELLOW = 0xB7E0;      /* 180, 255,   0 */
    static constexpr int ILI9341_PINK        = 0xFE19;      /* 255, 192, 203 */
    static constexpr int ILI9341_BROWN       = 0x9A60;      /* 150,  75,   0 */
    static constexpr int ILI9341_GOLD        = 0xFEA0;      /* 255, 215,   0 */
    static constexpr int ILI9341_SILVER      = 0xC618;      /* 192, 192, 192 */
    static constexpr int ILI9341_SKYBLUE     = 0x867D;      /* 135, 206, 235 */
    static constexpr int ILI9341_VIOLET      = 0x915C;      /* 180,  46, 226 */

    static constexpr int BLACK       = 0x0000;      /*   0,   0,   0 */
    static constexpr int NAVY        = 0x000F;      /*   0,   0, 128 */
    static constexpr int DARKGREEN   = 0x03E0;      /*   0, 128,   0 */
    static constexpr int DARKCYAN    = 0x03EF;      /*   0, 128, 128 */
    static constexpr int MAROON      = 0x7800;      /* 128,   0,   0 */
    static constexpr int PURPLE      = 0x780F;      /* 128,   0, 128 */
    static constexpr int OLIVE       = 0x7BE0;      /* 128, 128,   0 */
    static constexpr int LIGHTGREY   = 0xD69A;      /* 211, 211, 211 */
    static constexpr int DARKGREY    = 0x7BEF;      /* 128, 128, 128 */
    static constexpr int BLUE        = 0x001F;      /*   0,   0, 255 */
    static constexpr int GREEN       = 0x07E0;      /*   0, 255,   0 */
    static constexpr int CYAN        = 0x07FF;      /*   0, 255, 255 */
    static constexpr int RED         = 0xF800;      /* 255,   0,   0 */
    static constexpr int MAGENTA     = 0xF81F;      /* 255,   0, 255 */
    static constexpr int YELLOW      = 0xFFE0;      /* 255, 255,   0 */
    static constexpr int WHITE       = 0xFFFF;      /* 255, 255, 255 */
    static constexpr int ORANGE      = 0xFDA0;      /* 255, 180,   0 */
    static constexpr int GREENYELLOW = 0xB7E0;      /* 180, 255,   0 */
    static constexpr int PINK        = 0xFE19;      /* 255, 192, 203 */
    static constexpr int BROWN       = 0x9A60;      /* 150,  75,   0 */
    static constexpr int GOLD        = 0xFEA0;      /* 255, 215,   0 */
    static constexpr int SILVER      = 0xC618;      /* 192, 192, 192 */
    static constexpr int SKYBLUE     = 0x867D;      /* 135, 206, 235 */
    static constexpr int VIOLET      = 0x915C;      /* 180,  46, 226 */
  }

  namespace tft_command
  {
#ifdef TFT_DISPOFF
    #undef TFT_DISPOFF
    #undef TFT_DISPON
    #undef TFT_SLPIN
    #undef TFT_SLPOUT
#endif

    static constexpr int TFT_DISPOFF = 0x28;
    static constexpr int TFT_DISPON  = 0x29;
    static constexpr int TFT_SLPIN   = 0x10;
    static constexpr int TFT_SLPOUT  = 0x11;
  }

  class M5GFX : public lgfx::LGFX_Device
  {
  protected:
    static M5GFX* _instance;

    struct DisplayState
    {
      const lgfx::IFont *gfxFont;
      lgfx::TextStyle style;
      lgfx::FontMetrics metrics;
      int32_t cursor_x, cursor_y;
    };

    std::shared_ptr<lgfx::Panel_Device> _panel_last;
    std::shared_ptr<lgfx::ITouch> _touch_last;
#if defined ( ESP_PLATFORM )
    std::shared_ptr<lgfx::IBus> _bus_last;
    std::shared_ptr<lgfx::ILight> _light_last;
#endif
    std::vector<DisplayState> _displayStateStack;

    bool init_impl(bool use_reset, bool use_clear) override;
    board_t autodetect(bool use_reset = false, board_t board = board_t::board_unknown);
    void _set_backlight(lgfx::ILight* bl);
    void _set_pwm_backlight(int16_t pin, uint8_t ch, uint32_t freq = 12000, bool invert = false, uint8_t offset = 0);

  public:
    M5GFX(void);

    using LGFXBase::drawBitmap;

    static M5GFX* getInstance(void) { return _instance; }

    void clearDisplay(int32_t color = TFT_BLACK) { fillScreen(color); }
    void progressBar(int x, int y, int w, int h, uint8_t val);
    void pushState(void);
    void popState(void);

    /// draw RGB565 format image.
    [[deprecated("use pushImage")]] 
    void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const void *data)
    {
      pushImage(x, y, w, h, (const rgb565_t*)data);
    }

    /// draw RGB565 format image, with transparent color.
    [[deprecated("use pushImage")]] 
    void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const void *data, uint16_t transparent)
    {
      pushImage(x, y, w, h, (const rgb565_t*)data, transparent);
    }

    bool setResolution( uint16_t logical_width  = 0
                      , uint16_t logical_height = 0
                      , float refresh_rate      = 0
                      , uint16_t output_width   = 0
                      , uint16_t output_height  = 0
                      , uint_fast8_t scale_w    = 0
                      , uint_fast8_t scale_h    = 0
                      , uint32_t pixel_clock    = 74250000
                      )
    {
      (void)logical_width;
      (void)logical_height;
      (void)refresh_rate;
      (void)output_width;
      (void)output_height;
      (void)scale_w;
      (void)scale_h;
      (void)pixel_clock;
#if defined (__M5GFX_M5ATOMDISPLAY__) || defined (__M5GFX_M5MODULEDISPLAY__)
      auto board = getBoard();
      if (board == board_t::board_M5AtomDisplay || board == board_t::board_M5ModuleDisplay)
      {
#if defined (SDL_h_)
#else
        bool res = ((lgfx::Panel_M5HDMI*)panel())->setResolution
          ( logical_width
          , logical_height
          , refresh_rate
          , output_width
          , output_height
          , scale_w
          , scale_h
          , pixel_clock
          );
        setRotation(getRotation());
        return res;
#endif
      }
#endif
      return false;
    }

    using lgfx::LGFX_Device::init;

    bool init(lgfx::Panel_Device* panel)
    {
      setPanel(panel);
      return LGFX_Device::init_impl(true, true);
    }
  };


  class M5Canvas : public lgfx::LGFX_Sprite
  {
  public:
    M5Canvas() : LGFX_Sprite() {}
    M5Canvas(LovyanGFX* parent) : LGFX_Sprite(parent) { _psram = true; }

    void* frameBuffer(uint8_t) { return getBuffer(); }
  };

//----------------------------------------------------------------------------
}

using namespace m5gfx::ili9341_colors;
using namespace m5gfx::tft_command;
using M5GFX = m5gfx::M5GFX;
using M5Canvas = m5gfx::M5Canvas;
using RGBColor = m5gfx::bgr888_t;

#endif

#ifdef USE_M5_FONT_CREATOR
using EncodeRange = lgfx::EncodeRange;
#endif
