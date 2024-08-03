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

#include "Panel_LCD.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_ST7735 : public Panel_LCD
  {
    Panel_ST7735(void)
    {
// The ST7735 is available in three resolutions, 132x162 / 128x160 / 132x132, depending on the mode select pin status.
      _cfg.panel_width  = _cfg.memory_width  = 132;  // or 128
      _cfg.panel_height = _cfg.memory_height = 162;  // or 160 or 132

      _cfg.dummy_read_pixel = 9;

      //freq_write = 27000000;
      //freq_read  = 14000000;
      //freq_fill  = 27000000;

    }

  protected:

    static constexpr uint8_t CMD_FRMCTR1 = 0xB1;
    static constexpr uint8_t CMD_FRMCTR2 = 0xB2;
    static constexpr uint8_t CMD_FRMCTR3 = 0xB3;
    static constexpr uint8_t CMD_INVCTR  = 0xB4;
    static constexpr uint8_t CMD_DISSET5 = 0xB6;

    static constexpr uint8_t CMD_PWCTR1  = 0xC0;
    static constexpr uint8_t CMD_PWCTR2  = 0xC1;
    static constexpr uint8_t CMD_PWCTR3  = 0xC2;
    static constexpr uint8_t CMD_PWCTR4  = 0xC3;
    static constexpr uint8_t CMD_PWCTR5  = 0xC4;
    static constexpr uint8_t CMD_VMCTR1  = 0xC5;

    static constexpr uint8_t CMD_RDID1   = 0xDA;
    static constexpr uint8_t CMD_RDID2   = 0xDB;
    static constexpr uint8_t CMD_RDID3   = 0xDC;
    static constexpr uint8_t CMD_RDID4   = 0xDD;

    static constexpr uint8_t CMD_PWCTR6  = 0xFC;

    static constexpr uint8_t CMD_GMCTRP1 = 0xE0;
    static constexpr uint8_t CMD_GMCTRN1 = 0xE1;

    const uint8_t* getInitCommands(uint8_t listno) const override {
      static constexpr uint8_t Bcmd[] = {                  // Initialization commands for 7735B screens
        CMD_SWRESET,   CMD_INIT_DELAY, 50,   // Software reset, no args, w/delay
        CMD_SLPOUT ,   CMD_INIT_DELAY, 255,  // Out of sleep mode, no args, w/delay
        CMD_FRMCTR1, 3+CMD_INIT_DELAY,  // Frame rate control, 3 args + delay:
          0x00,                   //     fastest refresh
          0x06,                   //     6 lines front porch
          0x03,                   //     3 lines back porch
          10,                     //     10 ms delay
        CMD_DISSET5, 2      ,  //  Display settings #5, 2 args, no delay:
          0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate rise, 3 cycle osc equalize
          0x02,                   //     Fix on VTL
        CMD_INVCTR , 1      ,  //  Display inversion control, 1 arg:
          0x0,                    //     Line inversion
        CMD_PWCTR1 , 2+CMD_INIT_DELAY,  //  8: Power control, 2 args + delay:
          0x02,                   //     GVDD = 4.7V
          0x70,                   //     1.0uA
          10,                     //     10 ms delay
        CMD_PWCTR2 , 1      ,  //  Power control, 1 arg, no delay:
          0x05,                   //     VGH = 14.7V, VGL = -7.35V
        CMD_PWCTR3 , 2      ,  //  Power control, 2 args, no delay:
          0x01,                   //     Opamp current small
          0x02,                   //     Boost frequency
        CMD_VMCTR1 , 2+CMD_INIT_DELAY,  // 11: Power control, 2 args + delay:
          0x3C,                   //     VCOMH = 4V
          0x38,                   //     VCOML = -1.1V
          10,                     //     10 ms delay
        CMD_PWCTR6 , 2      ,  //  Power control, 2 args, no delay:
          0x11, 0x15,
        CMD_GMCTRP1,16      ,  //  Magical unicorn dust, 16 args, no delay:
          0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
          0x21, 0x1B, 0x13, 0x19, //      these config values represent)
          0x17, 0x15, 0x1E, 0x2B,
          0x04, 0x05, 0x02, 0x0E,
        CMD_GMCTRN1,16+CMD_INIT_DELAY,  // Sparkles and rainbows, 16 args + delay:
          0x0B, 0x14, 0x08, 0x1E, //     (ditto)
          0x22, 0x1D, 0x18, 0x1E,
          0x1B, 0x1A, 0x24, 0x2B,
          0x06, 0x06, 0x02, 0x0F,
          10,                     //     10 ms delay
        CMD_NORON  ,   CMD_INIT_DELAY,  // Normal display on, no args, w/delay
          10,                     //     10 ms delay
        CMD_DISPON ,   CMD_INIT_DELAY,  // Main screen turn on, no args, w/delay
          255 ,                   //     255 = 500 ms delay
        0xFF, 0xFF
      };

      switch (listno) {
      case 0: return Bcmd;
      default: return nullptr;
      }
    }
  };

  struct Panel_ST7735S : public Panel_ST7735
  {
  protected:
    const uint8_t* getInitCommands(uint8_t listno) const override {

      static constexpr uint8_t Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
        CMD_SWRESET,   CMD_INIT_DELAY,  //  1: Software reset, 0 args, w/delay
          150,                    //     150 ms delay
        CMD_SLPOUT ,   CMD_INIT_DELAY,  //  2: Out of sleep mode, 0 args, w/delay
          255,                    //     500 ms delay
        CMD_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
          0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        CMD_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
          0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        CMD_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
          0x01, 0x2C, 0x2D,       //     Dot inversion mode
          0x01, 0x2C, 0x2D,       //     Line inversion mode
        CMD_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
          0x07,                   //     No inversion
        CMD_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
          0xA2,
          0x02,                   //     -4.6V
          0x84,                   //     AUTO mode
        CMD_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
          0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
        CMD_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
          0x0A,                   //     Opamp current small
          0x00,                   //     Boost frequency
        CMD_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
          0x8A,                   //     BCLK/2, Opamp current small & Medium low
          0x2A,
        CMD_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
          0x8A, 0xEE,
        CMD_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
          0x0E,
        0xFF,0xFF
      };
      static constexpr uint8_t Rcmd2[] = {  // Init for 7735R, part 2 (red or green tab)
        CMD_GMCTRP1, 16      , //  1: 16 args, no delay:
          0x02, 0x1c, 0x07, 0x12,
          0x37, 0x32, 0x29, 0x2d,
          0x29, 0x25, 0x2B, 0x39,
          0x00, 0x01, 0x03, 0x10,
        CMD_GMCTRN1, 16      , //  2: 16 args, no delay:
          0x03, 0x1d, 0x07, 0x06,
          0x2E, 0x2C, 0x29, 0x2D,
          0x2E, 0x2E, 0x37, 0x3F,
          0x00, 0x00, 0x02, 0x10,
        CMD_NORON  ,    CMD_INIT_DELAY, //  3: Normal display on, no args, w/delay
          10,                     //     10 ms delay
        CMD_DISPON ,    CMD_INIT_DELAY, //  4: Main screen turn on, no args w/delay
          100,                    //     100 ms delay
        0xFF,0xFF
      };

      switch (listno) {
      case 0: return Rcmd1;
      case 1: return Rcmd2;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------
 }
}
