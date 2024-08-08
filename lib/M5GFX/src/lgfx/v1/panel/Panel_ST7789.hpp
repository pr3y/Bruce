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

  struct Panel_ST7789  : public Panel_LCD
  {
    Panel_ST7789(void)
    {
      _cfg.panel_height = _cfg.memory_height = 320;

      _cfg.dummy_read_pixel = 16;
    }

  protected:

    static constexpr uint8_t CMD_RAMCTRL  = 0xB0;
    static constexpr uint8_t CMD_PORCTRL  = 0xB2;      // Porch control
    static constexpr uint8_t CMD_GCTRL    = 0xB7;      // Gate control
    static constexpr uint8_t CMD_VCOMS    = 0xBB;      // VCOMS setting
    static constexpr uint8_t CMD_LCMCTRL  = 0xC0;      // LCM control
    static constexpr uint8_t CMD_VDVVRHEN = 0xC2;      // VDV and VRH command enable
    static constexpr uint8_t CMD_VRHS     = 0xC3;      // VRH set
    static constexpr uint8_t CMD_VDVSET   = 0xC4;      // VDV setting
    static constexpr uint8_t CMD_FRCTR2   = 0xC6;      // FR Control 2
    static constexpr uint8_t CMD_PWCTRL1  = 0xD0;      // Power control 1
    static constexpr uint8_t CMD_PVGAMCTRL= 0xE0;      // Positive voltage gamma control
    static constexpr uint8_t CMD_NVGAMCTRL= 0xE1;      // Negative voltage gamma control

    const uint8_t* getInitCommands(uint8_t listno) const override {
      static constexpr uint8_t list0[] = {
//        CMD_SLPOUT , CMD_INIT_DELAY, 120,
//        CMD_NORON  , CMD_INIT_DELAY, 0,
//        0xB6       , 2, 0x0A,0x82,
          CMD_PORCTRL, 5, 0x0c, 0x0c, 0x00, 0x33, 0x33,
          CMD_GCTRL  , 1, 0x35,

          CMD_VCOMS  , 1, 0x28,  // JLX240 display datasheet
          CMD_LCMCTRL, 1, 0x0C,
          CMD_VDVVRHEN,2, 0x01, 0xFF,
          CMD_VRHS   , 1, 0x10,      // voltage VRHS
          CMD_VDVSET , 1, 0x20,
          CMD_FRCTR2 , 1, 0x0f,      // 0x0f=60Hz
          CMD_PWCTRL1, 2, 0xa4, 0xa1,
          CMD_RAMCTRL, 2, 0x00, 0xC0,
          //--------------------------------ST7789V gamma setting---------------------------------------//
          CMD_PVGAMCTRL,14, 0xd0,0x00,0x02,0x07,
                             0x0a,0x28,0x32,0x44,
                             0x42,0x06,0x0e,0x12,
                             0x14,0x17,
          CMD_NVGAMCTRL,14, 0xd0,0x00,0x02,0x07,
                             0x0a,0x28,0x31,0x54,
                             0x47,0x0e,0x1c,0x17,
                             0x1b,0x1e,
          CMD_SLPOUT, 0+CMD_INIT_DELAY, 130,    // Exit sleep mode
          CMD_IDMOFF, 0,
          CMD_DISPON, 0,
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
