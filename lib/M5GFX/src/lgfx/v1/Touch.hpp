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

#include <stdint.h>
#include <stddef.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct touch_point_t
  {
    int16_t x = -1;
    int16_t y = -1;
    uint16_t size = 0;
    uint16_t id   = 0;
  };

//----------------------------------------------------------------------------

  struct ITouch
  {
    struct config_t
    {
      uint32_t freq = 1000000;
      uint16_t x_min = 0;
      uint16_t x_max = 3600;
      uint16_t y_min = 0;
      uint16_t y_max = 3600;
      bool bus_shared = true;          /// パネルとタッチが同じバスに繋がっている場合true;
      int16_t pin_int = -1;
      int16_t pin_rst = -1;
      uint8_t offset_rotation = 0;
      union
      {
        struct
        {
          int8_t spi_host; // ESP32:spi_host_device_t VSPI_HOST or HSPI_HOST
          int16_t pin_sclk;
          int16_t pin_mosi;
          int16_t pin_miso;
        };
        struct
        {
          int8_t i2c_port; // ESP32:i2c_port_t I2C_NUM_0 or I2C_NUM_1
          int16_t pin_scl;
          int16_t pin_sda;
          int16_t i2c_addr;
        };
      };
      int16_t pin_cs = -1;
    };

    virtual ~ITouch(void) = default;

    config_t config(void) const { return _cfg; }
    void config(const config_t& config) { _cfg = config; }

    inline bool isSPI(void) const { return _cfg.pin_cs >= 0; }

    virtual bool init(void) = 0;
    virtual void wakeup(void) = 0;
    virtual void sleep(void) = 0;
    virtual bool isEnable(void) { return true; };
    virtual uint_fast8_t getTouchRaw(touch_point_t* tp, uint_fast8_t count) = 0;

  protected:
    config_t _cfg;
    bool _inited = false;
  };

//----------------------------------------------------------------------------
 }
}
