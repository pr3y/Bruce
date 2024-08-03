#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "INA3221_Class.hpp"

#if __has_include(<esp_log.h>)
#include <esp_log.h>
#endif

#include <algorithm>

namespace m5
{
  bool INA3221_Class::begin(void)
  {
    uint16_t id = readRegister16(0xFF);
#if defined (ESP_LOGV)
      ESP_LOGV("INA3221", "regFFh:%04x", id);
#endif
    _init = (id == 0x3220);
    return _init;
  }

  void INA3221_Class::setShuntRes(uint8_t channel, uint32_t res)
  {
    if (channel < INA3221_CH_NUM_MAX) {
      _shunt_res[channel] = res;
    }
  }

  int_fast16_t INA3221_Class::getBusMilliVoltage(uint8_t channel)
  {
    int_fast16_t res = 0;
    if (channel < INA3221_CH_NUM_MAX) {
      res = (int16_t)readRegister16(INA3221_CH1_BUS_V + (channel * 2));
    }
    return res;
  }

  int32_t INA3221_Class::getShuntMilliVoltage(uint8_t channel)
  {
    int32_t res = 0;
    if (channel < INA3221_CH_NUM_MAX) {
      res = (int16_t)readRegister16(INA3221_CH1_SHUNT_V + (channel * 2));
      res *= 5;
    }
    return res;
  }

  float INA3221_Class::getBusVoltage(uint8_t channel)
  {
    return getBusMilliVoltage(channel) / 1000.0f;
  }

  float INA3221_Class::getShuntVoltage(uint8_t channel)
  {
    return getShuntMilliVoltage(channel) / 1000.0f;
  }

  float INA3221_Class::getCurrent(uint8_t channel)
  {
    float res = 0.0f;
    if (channel < INA3221_CH_NUM_MAX) {
      res = getShuntVoltage(channel) / _shunt_res[channel];
    }
    return res;
  }

  std::size_t INA3221_Class::readRegister16(std::uint8_t addr)
  {
    std::uint8_t buf[2] = {0};
    readRegister(addr, buf, 2);
    return buf[0] << 8 | buf[1];
  }
}
#endif