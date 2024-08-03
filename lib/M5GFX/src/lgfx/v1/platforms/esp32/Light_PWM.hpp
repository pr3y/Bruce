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

#include "../../Light.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Light_PWM : public ILight
  {
  public:
    struct config_t
    {
      uint32_t freq = 1200;
      int16_t pin_bl = -1;
      uint8_t offset = 0;
      uint8_t pwm_channel = 7;

      bool invert = false;
    };

    const config_t& config(void) const { return _cfg; }

    void config(const config_t &cfg) { _cfg = cfg; }

    bool init(uint8_t brightness) override;
    void setBrightness(uint8_t brightness) override;

  private:
    config_t _cfg;
  };

//----------------------------------------------------------------------------
 }
}
