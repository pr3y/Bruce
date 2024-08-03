#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "AXP192_Class.hpp"

#if __has_include(<esp_log.h>)
#include <esp_log.h>
#endif

#include <algorithm>

namespace m5
{
/*
  DCDC1 : 0.7-3.5V，  25mV/step  1200mA
  DCDC2 : 0.7-2.275V，25mV/step  1600mA
  DCDC3 : 0.7-3.5V，  25mV/step   700mA

  LDOio0: 1.8-3.3V,  100mV/step    50mA
  LDO1  :                          30mA always on
  LDO2  : 1.8-3.3V， 100mV/step   200mA
  LDO3  : 1.8-3.3V， 100mV/step   200mA
*/
  bool AXP192_Class::begin(void)
  {
    std::uint8_t val;
    _init = readRegister(0x03, &val, 1);
    if (_init)
    {
      _init = (val == 0x03);
#if defined (ESP_LOGV)
      ESP_LOGV("AXP192", "reg03h:%02x : init:%d", val, _init);
#endif
    }
    return _init;
  }

  /// @param num 0=DCDC1 / 1=DCDC2 / 2=DCDC3
  void AXP192_Class::_set_DCDC(std::uint8_t num, int voltage)
  {
    static constexpr uint8_t reg12bit_tbl[] = { 0x01, 0x10, 0x02 };
    static constexpr uint8_t volt_reg_tbl[] = { 0x26, 0x23, 0x27 };
    static constexpr uint8_t volt_max_tbl[] = { 0x7F, 0x3F, 0x7F };

    voltage -= 700;
    std::uint_fast8_t val = (voltage < 0) ? 0 : std::min(voltage / 25, (int)volt_max_tbl[num]);
    writeRegister8(volt_reg_tbl[num], val);
    if (voltage < 0)
    {
      bitOff(0x12, reg12bit_tbl[num]);
    }
    else
    {
      bitOn(0x12, reg12bit_tbl[num]);
    }
  }

  /// @param num 0:LDOio0 ; 2:LDO2 ; 3=LDO3
  void AXP192_Class::_set_LDO(std::uint8_t num, int voltage)
  {
    if (num > 3 || num == 1) return;
    std::uint8_t reg_volt = (num == 0) ? 0x91 : 0x28;
    voltage -= 1800;
    /// convert voltage to value
    std::uint_fast8_t val = (voltage < 0) ? 0 : std::min(voltage / 100, 0x0F);
    std::uint_fast8_t now = readRegister8(reg_volt);
    if (num == 3)
    { /// LDO3
      now = (now & 0xF0) + val;
    }
    else
    { /// LDOio0 , LDO2
      now = (now & 0x0F) | (val << 4);
    }
    writeRegister8(reg_volt, now);

    if (num)
    { // LDO2 , LDO3
      std::uint_fast8_t reg12bit = 1 << num;
      if (voltage < 0)
      {
        bitOff(0x12, reg12bit);
      }
      else
      {
        bitOn(0x12, reg12bit);
      }
    }
    else
    { // LDOio0
      writeRegister8(0x90, (voltage < 0) ? 0x07 : 0x02 ); /// floating or LDO
    }
  }

  /// @param num 0=LDO2 / 1=LDO3
  void AXP192_Class::_set_LDO2_LDO3(std::uint8_t num, int voltage)
  {
    voltage -= 1800;
    std::uint_fast8_t val = (voltage < 0) ? 0 : std::min(voltage / 100, 0x0F);
    std::uint_fast8_t now = readRegister8(0x28);
    if (num == 1)
    { /// LDO3
      now = (now & 0xF0) + val;
    }
    else
    { /// LDO2
      now = (now & 0x0F) | (val << 4);
    }
    writeRegister8(0x28, now);

    std::uint_fast8_t reg12bit = 1 << (num + 2);
    if (voltage < 0)
    {
      bitOff(0x12, reg12bit);
    }
    else
    {
      bitOn(0x12, reg12bit);
    }
  }

  /// @param num 0=GPIO0 / 1=GPIO1 / 2=GPIO2
  void AXP192_Class::_set_GPIO0_2(std::uint8_t num, bool state)
  {
    static constexpr uint8_t reg[] = { 0x90, 0x92, 0x93 };
    writeRegister8(reg[num], state ? 0x06 : 0x05); // floating or LOW
  }

  /// @param num 0=GPIO3 / 1=GPIO4
  void AXP192_Class::_set_GPIO3_4(std::uint8_t num, bool state)
  {
    uint32_t bit = num ? 2 : 1;
    if (state)
    {
      bitOn(0x96, bit);
    }
    else
    {
      bitOff(0x96, bit);
    }
    uint_fast8_t mask = num ? ~0x0C : ~0x03;
    uint_fast8_t reg0x95 = readRegister8(0x95) & mask;
    writeRegister8(0x95, reg0x95 | (num ? 0x84 : 0x81)); // set GPIO mode
  }

  void AXP192_Class::setBatteryCharge(bool enable)
  {
    std::uint8_t val = 0;
    if (readRegister(0x33, &val, 1))
    {
      writeRegister8(0x33, (val & 0x7F) + (enable ? 0x80 : 0x00));
    }
  }

  void AXP192_Class::setChargeCurrent(std::uint16_t max_mA)
  {
    max_mA /= 10;
    if (max_mA > 132) { max_mA = 132; }
    static constexpr std::uint8_t table[] = { 19, 28, 36, 45, 55, 63, 70, 78, 88, 96, 100, 108, 116, 124, 132, 255 };

    size_t i = 0;
    while (table[i] <= max_mA) { ++i; }

    std::uint8_t val = 0;
    if (readRegister(0x33, &val, 1))
    {
      writeRegister8(0x33, (val & 0xF0) + i);
    }
  }

  void AXP192_Class::setChargeVoltage(std::uint16_t max_mV)
  {
    max_mV = (max_mV / 10) - 410;
    if (max_mV > 436 - 410) { max_mV = 436 - 410; }
    static constexpr std::uint8_t table[] =
      { 415 - 410  /// 4150mV
      , 420 - 410  /// 4200mV
      , 436 - 410  /// 4360mV
      , 255
      };
    size_t i = 0;
    while (table[i] <= max_mV) { ++i; }

    std::uint8_t val = 0;
    if (readRegister(0x33, &val, 1))
    {
      writeRegister8(0x33, (val & 0x9F) + (i << 5));
    }
  }

  std::int8_t AXP192_Class::getBatteryLevel(void)
  {
    std::uint8_t buf[4];
    if (!readRegister(0x78, buf, 4)) { return -1; }

    std::uint_fast16_t voltage = (buf[0] << 4) + buf[1];
    std::uint_fast16_t current = (buf[2] << 5) + buf[3];

    std::int_fast16_t res = (voltage > 3150) ? (( voltage - 3075 ) * 0.16f  )
                          : (voltage > 2690) ? (( voltage - 2690 ) * 0.027f )
                          : 0;
    if (current > 16) { res -= 16; }

    return (res < 100) ? res : 100;
  }

  bool AXP192_Class::isCharging(void)
  {
    return readRegister8(0x00) & 0x04;
  }

  void AXP192_Class::powerOff(void)
  {
    bitOn(0x32, 0x80);
  }

  void AXP192_Class::setAdcState(bool enable)
  {
    writeRegister8(0x82, enable ? 0xff : 0x00);
  }

  void AXP192_Class::setAdcRate( std::uint8_t rate )
  {
    std::uint_fast8_t buf = readRegister8(0x84);
    writeRegister8(0x84, (buf & ~(0xc0)) | (rate & 0xc0));
  }

  void AXP192_Class::setEXTEN(bool enable)
  {
    static constexpr std::uint8_t add = 0x12;
    static constexpr std::uint8_t bit = 1 << 6;
    if (enable)
    {
      bitOn(add, bit);
    }
    else
    {
      bitOff(add, bit);
    }
  }

  bool AXP192_Class::getEXTEN(void)
  {
    return readRegister8(0x12) & (1 << 6);
  }

  void AXP192_Class::setBACKUP(bool enable)
  {
    static constexpr std::uint8_t add = 0x35;
    static constexpr std::uint8_t bit = 1 << 7;
    if (enable)
    { // Enable
      bitOn(add, bit);
    }
    else
    { // Disable
      bitOff(add, bit);
    }
  }

  bool AXP192_Class::isACIN(void)
  {
    return readRegister8(0x00) & 0x80;
  }
  bool AXP192_Class::isVBUS(void)
  {
    return readRegister8(0x00) & 0x20;
  }

  bool AXP192_Class::getBatState(void)
  {
    return readRegister8(0x01) & 0x20;
  }

  std::uint8_t AXP192_Class::getPekPress(void)
  {
    std::uint8_t val = readRegister8(0x46) & 0x03;
    if (val) { writeRegister8(0x46, val); }
    return val;
  }

  float AXP192_Class::getACINVoltage(void)
  {
    return readRegister12(0x56) * (1.7f / 1000.0f);
  }

  float AXP192_Class::getACINCurrent(void)
  {
    return readRegister12(0x58) * 0.625f;
  }

  float AXP192_Class::getVBUSVoltage(void)
  {
    return readRegister12(0x5a) * (1.7f / 1000.0f);
  }

  float AXP192_Class::getVBUSCurrent(void)
  {
    return readRegister12(0x5c) * 0.375f;
  }

  float AXP192_Class::getInternalTemperature(void)
  {
    return readRegister12(0x5e) * 0.1f -144.7f;
  }

  float AXP192_Class::getBatteryPower(void)
  {
    return  readRegister24(0x70) * (1.1f * 0.5f / 1000.0f);
  }

  float AXP192_Class::getBatteryVoltage(void)
  {
    return readRegister12(0x78) * (1.1f / 1000.0f);
  }

  float AXP192_Class::getBatteryChargeCurrent(void)
  {
    return readRegister13(0x7a) * 0.5f;
  }

  float AXP192_Class::getBatteryDischargeCurrent(void)
  {
    return readRegister13(0x7c) * 0.5f;
  }

  float AXP192_Class::getAPSVoltage(void)
  {
    return readRegister12(0x7e) * (1.4f / 1000.0f);
  }


  std::size_t AXP192_Class::readRegister12(std::uint8_t addr)
  {
    std::uint8_t buf[2] = {0};
    readRegister(addr, buf, 2);
    return buf[0] << 4 | buf[1];
  }
  std::size_t AXP192_Class::readRegister13(std::uint8_t addr)
  {
    std::uint8_t buf[2] = {0};
    readRegister(addr, buf, 2);
    return buf[0] << 5 | buf[1];
  }
  std::size_t AXP192_Class::readRegister16(std::uint8_t addr)
  {
    std::uint8_t buf[2] = {0};
    readRegister(addr, buf, 2);
    return buf[0] << 8 | buf[1];
  }
  std::size_t AXP192_Class::readRegister24(std::uint8_t addr)
  {
    std::uint8_t buf[3] = {0};
    readRegister(addr, buf, 3);
    return buf[0] << 16 | buf[1] << 8 | buf[2];
  }
  std::size_t AXP192_Class::readRegister32(std::uint8_t addr)
  {
    std::uint8_t buf[4] = {0};
    readRegister(addr, buf, 4);
    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
  }
}
#endif