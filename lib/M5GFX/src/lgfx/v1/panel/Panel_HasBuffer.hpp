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

#include "Panel_Device.hpp"
#include "../misc/range.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_HasBuffer : public Panel_Device
  {

  public:

    Panel_HasBuffer(void);
    virtual ~Panel_HasBuffer(void);

    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    void setRotation(uint_fast8_t r) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeBlock(uint32_t rawcolor, uint32_t length) override;

  protected:
    uint8_t* _buf = nullptr;
    range_rect_t _range_mod;
    int32_t _xpos = 0;
    int32_t _ypos = 0;
    bool _in_transaction = false;

    virtual size_t _get_buffer_length(void) const = 0;
    void _rotate_pos(uint_fast16_t &x, uint_fast16_t &y);
    void _rotate_pos(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye);
  };

//----------------------------------------------------------------------------
 }
}
