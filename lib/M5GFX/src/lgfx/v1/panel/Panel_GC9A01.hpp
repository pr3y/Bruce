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
#include "../Bus.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_GC9xxx  : public Panel_LCD
  {
    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override
    {
      if (xs != _xs || xe != _xe || ys != _ys || ye != _ye)
      {
        if (_internal_rotation & 1)
        {
          _bus->writeCommand(CMD_RASET, 8);
          _bus->writeData(~0u, 32);
        }
        _xs = xs;
        _xe = xe;
        _bus->writeCommand(CMD_CASET, 8);
        xs += _colstart;
        xe += _colstart;
        _bus->writeData(xs >> 8 | (xs & 0xFF) << 8 | (xe << 8 | xe >> 8) << 16, 32);

        _ys = ys;
        _ye = ye;
        _bus->writeCommand(CMD_RASET, 8);
        ys += _rowstart;
        ye += _rowstart;
        _bus->writeData(ys >> 8 | (ys & 0xFF) << 8 | (ye << 8 | ye >> 8) << 16, 32);
      }
      _bus->writeCommand(CMD_RAMWR, 8);
    }

  protected:

    uint8_t getMadCtl(uint8_t r) const override
    {
      static constexpr uint8_t madctl_table[] =
      {
                           0,
        MAD_MV|MAD_MX       ,
               MAD_MX|MAD_MY,
        MAD_MV|       MAD_MY,
                      MAD_MY,
        MAD_MV              ,
               MAD_MX       ,
        MAD_MV|MAD_MX|MAD_MY,
      };
      return madctl_table[r];
    }
  };

//----------------------------------------------------------------------------

  struct Panel_GC9A01  : public Panel_GC9xxx
  {
    Panel_GC9A01(void)
    {
      _cfg.panel_width  = _cfg.memory_width  = 240;
      _cfg.panel_height = _cfg.memory_height = 240;

      _cfg.dummy_read_pixel = 16;

      // GC9A01 malfunctions when sending NOP.
      // Therefore, the function to send a NOP at the end of communication should be disabled.
      _nop_closing = false;
    }

  protected:

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
          0xEF, 0,
          0xEB, 1, 0x14,
          0xFE, 0,
          0xEF, 0,
          0xEB, 1, 0x14,
          0x84, 1, 0x40,
          0x85, 1, 0xFF,
          0x86, 1, 0xFF,
          0x87, 1, 0xFF,
          0x8E, 1, 0xFF,
          0x8F, 1, 0xFF,
          0x88, 1, 0x0A,
          0x89, 1, 0x21,
          0x8A, 1, 0x00,
          0x8B, 1, 0x80,
          0x8C, 1, 0x01,
          0x8D, 1, 0x01,
          0xB6, 2, 0x00, 0x20,
          0x90, 4, 0x08, 0x08, 0x08, 0x08,
          0xBD, 1, 0x06,
          0xBC, 1, 0x00,
          0xFF, 3, 0x60, 0x01, 0x04,
          0xC3, 1, 0x13,
          0xC4, 1, 0x13,
          0xC9, 1, 0x22,
          0xBE, 1, 0x11,
          0xE1, 2, 0x10, 0x0E,
          0xDF, 3, 0x21, 0x0c, 0x02,
          0xF0, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
          0xF1, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
          0xF2, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
          0xF3, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
          0xED, 2, 0x1B, 0x0B,
          0xAE, 1, 0x77,
          0xCD, 1, 0x63,
          0x70, 9, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03,
          0xE8, 1, 0x34,
          0x62,12, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70,
          0x63,12, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70,
          0x64, 7, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07,
          0x66,10, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
          0x67,10, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
          0x74, 7, 0x10, 0x68, 0x80, 0x00, 0x00, 0x4E, 0x00,
          0x98, 2, 0x3e, 0x07,
          0x35, 1, 0x00,       // Tearing Effect Line ON
          0x11, 0+CMD_INIT_DELAY, 120,
          0x29, 0,
          0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------

  struct Panel_GC9107  : public Panel_GC9xxx
  {
    Panel_GC9107(void)
    {
      _cfg.panel_width  = _cfg.memory_width  = 128;
      _cfg.panel_height = _cfg.memory_height = 160;

      _cfg.dummy_read_pixel = 16;
    }

  protected:

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        0xFE, 0+CMD_INIT_DELAY, 5,
        0xEF, 0+CMD_INIT_DELAY, 5,
        0xB0, 1, 0xC0,
        0xB2, 1, 0x2F,
        0xB3, 1, 0x03,
        0xB6, 1, 0x19,
        0xB7, 1, 0x01,
        0xAC, 1, 0xCB,
        0xAB, 1, 0x0E,
        0xB4, 1, 0x04,
        0xA8, 1, 0x19,
        0xB8, 1, 0x08,
        0xE8, 1, 0x24,
        0xE9, 1, 0x48,
        0xEA, 1, 0x22,
        0xC6, 1, 0x30,
        0xC7, 1, 0x18,
        0xF0, 14, 0x01,0x2b,0x23,0x3c,0xb7,0x12,0x17,0x60,0x00,0x06,0x0c,0x17,0x12,0x1f,
        0xF1, 14, 0x05,0x2e,0x2d,0x44,0xd6,0x15,0x17,0xa0,0x02,0x0d,0x0d,0x1a,0x18,0x1f,
        0x11, 0+CMD_INIT_DELAY, 120,
        0x29, 0,
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
