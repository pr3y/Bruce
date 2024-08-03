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

  struct Panel_ILI9342 : public Panel_LCD
  {
    Panel_ILI9342(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 320;
      _cfg.memory_height = _cfg.panel_height = 240;
    }

  protected:

    static constexpr uint8_t CMD_DFUNCTR = 0xB6;
    static constexpr uint8_t CMD_PWCTR1  = 0xC0;
    static constexpr uint8_t CMD_PWCTR2  = 0xC1;
    static constexpr uint8_t CMD_VMCTR1  = 0xC5;
    static constexpr uint8_t CMD_SETEXTC = 0xC8;
    static constexpr uint8_t CMD_GMCTRP1 = 0xE0; // Positive Gamma Correction (E0h)
    static constexpr uint8_t CMD_GMCTRN1 = 0xE1; // Negative Gamma Correction (E1h)

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] =
      {
          CMD_SETEXTC, 3, 0xFF,0x93,0x42,   // Turn on the external command
          CMD_PWCTR1 , 2, 0x12, 0x12,
          CMD_PWCTR2 , 1, 0x03,
          CMD_VMCTR1 , 1, 0xF2,
          0xB0       , 1, 0xE0,
          0xF6       , 3, 0x01, 0x00, 0x00,
          CMD_GMCTRP1,15, 0x00,0x0C,0x11,0x04,0x11,0x08,0x37,0x89,0x4C,0x06,0x0C,0x0A,0x2E,0x34,0x0F,
          CMD_GMCTRN1,15, 0x00,0x0B,0x11,0x05,0x13,0x09,0x33,0x67,0x48,0x07,0x0E,0x0B,0x2E,0x33,0x0F,
          CMD_DFUNCTR, 4, 0x08,0x82,0x1D,0x04,
          CMD_IDMOFF , 0,
          CMD_DISPON , 0,
          CMD_SLPOUT , 0 + CMD_INIT_DELAY, 120,    // Exit sleep mode
          0xFF,0xFF, // end
      };
      switch (listno)
      {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------
 }
}
