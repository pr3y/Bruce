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

#include "Panel_HasBuffer.hpp"
#include "../misc/range.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_1bitOLED : public Panel_HasBuffer
  {
    bool init(bool use_reset) override;

    void waitDisplay(void) override;
    bool displayBusy(void) override;
    color_depth_t setColorDepth(color_depth_t depth) override;

    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool) override {}

    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }

    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;
    void setTilePattern(uint_fast8_t);

  protected:

    static constexpr uint8_t CMD_SETSTARTLINE        = 0x40;
    static constexpr uint8_t CMD_DISPLAYALLON_RESUME = 0xA4;
    static constexpr uint8_t CMD_DISPLAYALLON        = 0xA5;
    static constexpr uint8_t CMD_NORMALDISPLAY       = 0xA6;
    static constexpr uint8_t CMD_INVERTDISPLAY       = 0xA7;
    static constexpr uint8_t CMD_SETMULTIPLEX        = 0xA8;
    static constexpr uint8_t CMD_DISP_OFF            = 0xAE;
    static constexpr uint8_t CMD_DISP_ON             = 0xAF;
    static constexpr uint8_t CMD_SETPRECHARGE        = 0xD9;
    static constexpr uint8_t CMD_SETVCOMDETECT       = 0xDB;

    uint8_t _bayer_offset = 0;

    size_t _get_buffer_length(void) const override;
    bool _read_pixel(uint_fast16_t x, uint_fast16_t y);
    void _draw_pixel(uint_fast16_t x, uint_fast16_t y, uint32_t value);
    virtual void _update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye);

  };

  struct Panel_SSD1306 : public Panel_1bitOLED
  {
    Panel_SSD1306(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 64;
    }

    bool init(bool use_reset) override;
    void setBrightness(uint8_t brightness) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

    /// OLEDパネルのCOMピン配置を設定する。;
    /// This command sets the COM signals pin configuration to match the OLED panel hardware layout.
    /// see SSD1306 data sheet  ( 10.1.18 Set COM Pins Hardware Configuration (DAh)
    /// @param data SETCOMPINSで送信するデータ (0x02 or 0x12 or 0x22 or 0x32)
    void setComPins(uint8_t data = 0x02);

  protected:

    uint8_t _compins = 0x12;

    static constexpr uint8_t CMD_MEMORYMODE  = 0x20;

    static constexpr uint8_t CMD_COLUMNADDR  = 0x21;
    static constexpr uint8_t CMD_PAGEADDR    = 0x22;

    static constexpr uint8_t CMD_SETCONTRAST = 0x81;
    static constexpr uint8_t CMD_CHARGEPUMP  = 0x8D;
    static constexpr uint8_t CMD_SEGREMAP    = 0xA0;
//  static constexpr uint8_t CMD_SETMLTPLX   = 0xA8;
    static constexpr uint8_t CMD_COMSCANINC  = 0xC0;
//  static constexpr uint8_t CMD_COMSCANDEC  = 0xC8;
    static constexpr uint8_t CMD_SETOFFSET   = 0xD3;
    static constexpr uint8_t CMD_SETCLKDIV   = 0xD5;
    static constexpr uint8_t CMD_SETCOMPINS  = 0xDA;

    static constexpr uint8_t CMD_DEACTIVATE_SCROLL   = 0x2E;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        CMD_DISP_OFF           ,
        CMD_SETCLKDIV          , 0x80,
        CMD_SETMULTIPLEX       , 0x3F,
        CMD_SETOFFSET          , 0x00,
        CMD_SETSTARTLINE       ,
        CMD_MEMORYMODE         , 0x00,
        CMD_SEGREMAP           ,
        CMD_COMSCANINC         ,
        CMD_SETVCOMDETECT      , 0x10,
        CMD_DISPLAYALLON_RESUME,
        CMD_DEACTIVATE_SCROLL  ,
        CMD_CHARGEPUMP         , 0x14,
        CMD_DISP_ON            ,
        CMD_SETCONTRAST        , 0x00,
        CMD_SETPRECHARGE       , 0x11,
        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

  struct Panel_SH110x : public Panel_1bitOLED
  {
    Panel_SH110x(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 128;
      _auto_display = true;
    }

    void setBrightness(uint8_t brightness) override;

    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

  protected:
    static constexpr uint8_t CMD_PAGEADDRESSINGMODE  = 0x20;
    static constexpr uint8_t CMD_VERTADDRESSINGMODE  = 0x21;
    static constexpr uint8_t CMD_PAGEADDR            = 0x22;
    static constexpr uint8_t CMD_SETCONTRAST         = 0x81;
//  static constexpr uint8_t CMD_CHARGEPUMP          = 0x8D;
    static constexpr uint8_t CMD_SEGREMAP            = 0xA0;
    static constexpr uint8_t CMD_DCDC                = 0xAD;
    static constexpr uint8_t CMD_SETPAGEADDR         = 0xB0;
    static constexpr uint8_t CMD_COMSCANINC          = 0xC0;
    static constexpr uint8_t CMD_COMSCANDEC          = 0xC8;
    static constexpr uint8_t CMD_SETDISPLAYOFFSET    = 0xD3;
    static constexpr uint8_t CMD_SETDISPLAYCLOCKDIV  = 0xD5;
    static constexpr uint8_t CMD_SETCOMPINS          = 0xDA;
    static constexpr uint8_t CMD_SETDISPSTARTLINE    = 0xDC;
    static constexpr uint8_t CMD_SETLOWCOLUMN        = 0x00;
    static constexpr uint8_t CMD_SETHIGHCOLUMN       = 0x10;
    static constexpr uint8_t CMD_READMODIFYWRITE     = 0xE0;
    static constexpr uint8_t CMD_READMODIFYWRITE_END = 0xEE;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        CMD_DISP_OFF   ,
        CMD_SETSTARTLINE       ,
        CMD_READMODIFYWRITE_END,
        CMD_PAGEADDRESSINGMODE ,
        CMD_SETDISPSTARTLINE   , 0x00,
        CMD_SETDISPLAYCLOCKDIV , 0x50,
        CMD_SETMULTIPLEX       , 0x7F,
        CMD_SETDISPLAYOFFSET   , 0x00,
        CMD_DCDC               , 0x8B,
        CMD_SEGREMAP           ,
        CMD_COMSCANINC         ,
        CMD_SETPRECHARGE       , 0x20,
        CMD_SETVCOMDETECT      , 0x35,
        CMD_DISPLAYALLON_RESUME,
        CMD_SETCONTRAST, 0x00,
        CMD_SETCOMPINS, 0x12,
        CMD_DISP_ON    ,
        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };


  struct Panel_ST7565 : public Panel_1bitOLED
  {
    Panel_ST7565(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 64;
      _auto_display = true;
    }

    // void setBrightness(uint8_t brightness) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

  protected:
    static constexpr uint8_t CMD_SETPAGEADDR         = 0xB0;
    static constexpr uint8_t CMD_SETLOWCOLUMN        = 0x00;
    static constexpr uint8_t CMD_SETHIGHCOLUMN       = 0x10;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        0xAE,           //Display = OFF
        0xA0,           //ADC = normal
        0xC8,           //Common output = revers
        0xA3,           //LCD bias = 1/7

  //内部レギュレータON
        0x2C,
        //delay(2);
        0x2E,
        //delay(2);
        0x2F,

  //コントラスト設定
        0x23,           //Vo voltage regulator internal resistor ratio set
        0x81,           //Electronic volume mode set
        0x1C,           //Electronic volume register set

  //表示設定
        0xA4,           //Display all point ON/OFF = normal
        0x40,           //Display start line = 0
        // 0xA7,           //Display normal/revers = normal
        0xAF,           //Dsiplay = ON

        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------
 }
}
