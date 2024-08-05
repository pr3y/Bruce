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

  struct Touch_GT911 : public ITouch
  {
    static constexpr const uint8_t default_addr_1 = 0x14;
    static constexpr const uint8_t default_addr_2 = 0x5D;

    Touch_GT911(void)
    {
      _cfg.i2c_addr = default_addr_1;
      _cfg.x_min = 0;
      _cfg.x_max = 2047;
      _cfg.y_min = 0;
      _cfg.y_max = 2047;
    }

    bool init(void) override;

    void wakeup(void) override;

    void sleep(void) override;

    uint_fast8_t getTouchRaw(touch_point_t *tp, uint_fast8_t count) override;

    void setTouchNums(int_fast8_t nums);

  private:
    enum
    {
      max_touch_points = 5
    };
    uint32_t _last_update = 0;
    uint32_t _refresh_rate = 5;
    uint8_t _readdata[max_touch_points * 8 + 2]; // 5point * 8byte + 2byte

    void _freshConfig(void);
    bool _writeBytes(const uint8_t* data, size_t len);
    bool _writeReadBytes(const uint8_t* write_data, size_t write_len, uint8_t* read_data, size_t read_len);
    bool _update_data(void);
  };

//----------------------------------------------------------------------------
 }
}
