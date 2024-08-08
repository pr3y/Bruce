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

#include "../../Bus.hpp"
#include "../common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Bus_I2C : public Bus_NULL
  {
  public:
    struct config_t
    {
      uint32_t freq_write = 400000;
      uint32_t freq_read = 400000;
      int16_t pin_scl = 22;
      int16_t pin_sda = 21;
      uint8_t i2c_port = 0;
      uint8_t i2c_addr = 0x3C;
      uint32_t prefix_cmd = 0x00;
      uint32_t prefix_data = 0x40;
      uint32_t prefix_len = 1;
    };

    const config_t& config(void) const { return _cfg; }

    void config(const config_t& config);

    bus_type_t busType(void) const override { return bus_type_t::bus_i2c; }

  protected:

    config_t _cfg;
  };

//----------------------------------------------------------------------------
 }
}
