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

#include "../Touch.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Touch_FT5x06 : public ITouch
  {
    Touch_FT5x06(void)
    {
      _cfg.i2c_addr = 0x38;
      _cfg.x_min = 0;
      _cfg.x_max = 319;
      _cfg.y_min = 0;
      _cfg.y_max = 319;
      _cfg.freq  = 400000;
    }

    bool init(void) override;

    void wakeup(void) override;

    void sleep(void) override;

    uint_fast8_t getTouchRaw(touch_point_t* tp, uint_fast8_t count) override;

  private:
    enum
    {
      max_touch_points = 5
    };

    bool _flg_released = false;

    bool _check_init(void);
    bool _write_reg(uint8_t reg, uint8_t val);
    bool _read_reg(uint8_t reg, uint8_t *data, size_t length);
    size_t _read_data(uint8_t* data);
  };

//----------------------------------------------------------------------------
 }
}
