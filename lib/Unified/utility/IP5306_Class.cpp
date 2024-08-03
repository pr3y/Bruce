#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "IP5306_Class.hpp"

#if __has_include(<esp_log.h>)
#include <esp_log.h>
#endif

#include <algorithm>

namespace m5
{
  static constexpr std::uint8_t REG_SYS_CTL0 = 0x00;
  static constexpr std::uint8_t REG_SYS_CTL1 = 0x01;
  static constexpr std::uint8_t REG_SYS_CTL2 = 0x02;
  static constexpr std::uint8_t REG_READ0    = 0x70;
  static constexpr std::uint8_t REG_READ1    = 0x71;
  static constexpr std::uint8_t REG_READ2    = 0x72;
  static constexpr std::uint8_t REG_READ3    = 0x77;
  static constexpr std::uint8_t REG_READ4    = 0x78;
  static constexpr std::uint8_t REG_CHG_CTL0 = 0x20;
  static constexpr std::uint8_t REG_CHG_CTL1 = 0x21;
  static constexpr std::uint8_t REG_CHG_CTL2 = 0x22;
  static constexpr std::uint8_t REG_CHG_CTL3 = 0x23;
  static constexpr std::uint8_t REG_CHG_DIG_CTL0 = 0x24;

  static constexpr std::uint8_t BOOST_OUT_BIT = 0x02;

  bool IP5306_Class::begin(void)
  {
    std::uint8_t val = 0;
    writeRegister(0x06, &val, 1); // reg06h WLED flashlight disabled
    val = 2;
    _init = writeRegister(0x06, &val, 1);
    if (_init)
    {
#if defined (ESP_LOGV)
      ESP_LOGV("IP5306", "found");
#endif
    }
    return _init;
  }

  std::int8_t IP5306_Class::getBatteryLevel(void)
  {
    std::uint8_t data;
    if (readRegister(REG_READ4, &data, 1)) {
      switch (data >> 4) {
        case 0x00: return 100;
        case 0x08: return 75;
        case 0x0C: return 50;
        case 0x0E: return 25;
        default:   return 0;
      }
    }
    return -1;
  }

  void IP5306_Class::setBatteryCharge(bool enable)
  {
    static constexpr std::uint8_t CHARGE_OUT_BIT = 0x10;

    std::uint8_t val = 0;
    if (readRegister(REG_SYS_CTL0, &val, 1))
    {
      writeRegister8(REG_SYS_CTL0, enable ? (val | CHARGE_OUT_BIT) : (val & (~CHARGE_OUT_BIT)));
    }
  }

  void IP5306_Class::setChargeCurrent(std::uint16_t max_mA)
  {
    max_mA = (max_mA > 50) ? (max_mA - 50) / 100 : 0;
    if (max_mA > 31) { max_mA = 31; }

    std::uint8_t val = 0;
    if (readRegister(REG_CHG_DIG_CTL0, &val, 1))
    {
      writeRegister8(REG_CHG_DIG_CTL0, (val & 0xE0) + max_mA);
    }
  }

  void IP5306_Class::setChargeVoltage(std::uint16_t max_mV)
  {
    max_mV = (max_mV / 10);
    max_mV = (max_mV > 410) ? max_mV - 410 : 0;
    if (max_mV > 436 - 410) { max_mV = 436 - 410; }
    static constexpr std::uint8_t table[] = 
      { 430 - 410  /// 4300mV
      , 435 - 410  /// 4350mV
      , 440 - 410  /// 4400mV
      , 255
      };
    size_t i = 0;
    while (table[i] <= max_mV) { ++i; }

    static constexpr std::uint8_t regdata[4] =
      { 0x02 // 4.2v  + boost 28mV
      , 0x05 // 4.3v  + boost 14mV
      , 0x09 // 4.35v + boost 14mV
      , 0x0D // 4.4v  + boost 14mV
      };
    writeRegister8(REG_CHG_CTL2, regdata[i]);
  }

  bool IP5306_Class::isCharging(void)
  {
    std::uint8_t val = 0;
    return (readRegister(0x71, &val, 1)) && (val % 0x0C);
  }

  bool IP5306_Class::setPowerBoostKeepOn(bool en) {
    std::uint8_t data;
    if (readRegister(REG_SYS_CTL0, &data, 1) == true)
    {
      data = en ? (data | BOOST_OUT_BIT) : (data & (~BOOST_OUT_BIT));
      return writeRegister(REG_SYS_CTL0, &data, 1);
    }
    return false;
  }
}
#endif