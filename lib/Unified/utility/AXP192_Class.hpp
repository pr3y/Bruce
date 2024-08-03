#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_AXP192_CLASS_H__
#define __M5_AXP192_CLASS_H__

#include "I2C_Class.hpp"

namespace m5
{
  class AXP192_Class : public I2C_Device
  {
  public:

    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x34;

    AXP192_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C)
    : I2C_Device ( i2c_addr, freq, i2c )
    {}

    bool begin(void);

    /// Get the remaining battery power.
    /// @return 0-100 level
    std::int8_t getBatteryLevel(void);

    /// set battery charge enable.
    /// @param enable true=enable / false=disable
    void setBatteryCharge(bool enable);

    /// set battery charge current
    /// @param max_mA milli ampere. (100 - 1320).
    void setChargeCurrent(std::uint16_t max_mA);

    /// set battery charge voltage
    /// @param max_mV milli volt. (4100 - 4360).
    void setChargeVoltage(std::uint16_t max_mV);

    /// Get whether the battery is currently charging or not.
    bool isCharging(void);

    inline void setDCDC1(int voltage) { _set_DCDC(0, voltage); }
    inline void setDCDC2(int voltage) { _set_DCDC(1, voltage); }
    inline void setDCDC3(int voltage) { _set_DCDC(2, voltage); }

    /// set LDOio0 voltage
    /// @param voltage milli volt. (0 - 3300).
    inline void setLDO0(int voltage) { _set_LDO(0, voltage); }

    /// set LDO2 voltage
    /// @param voltage milli volt. (0 - 3300).
    inline void setLDO2(int voltage) { _set_LDO(2, voltage); }

    /// set LDO3 voltage
    /// @param voltage milli volt. (0 - 3300).
    inline void setLDO3(int voltage) { _set_LDO(3, voltage); }

    inline void setGPIO(uint8_t gpio_num, bool state) { if (gpio_num < 3) { _set_GPIO0_2(gpio_num, state); } else { _set_GPIO3_4(gpio_num - 3, state); } }
    inline void setGPIO0(bool state) { _set_GPIO0_2(0, state); }
    inline void setGPIO1(bool state) { _set_GPIO0_2(1, state); }
    inline void setGPIO2(bool state) { _set_GPIO0_2(2, state); }
    inline void setGPIO3(bool state) { _set_GPIO3_4(0, state); }
    inline void setGPIO4(bool state) { _set_GPIO3_4(1, state); }

    void powerOff(void);

    void setAdcState(bool enable);
    void setAdcRate( std::uint8_t rate );

    void setEXTEN(bool enable);
    void setBACKUP(bool enable);

    bool isACIN(void);
    bool isVBUS(void);
    bool getBatState(void);
    bool getEXTEN(void);

    float getBatteryVoltage(void);
    float getBatteryDischargeCurrent(void);
    float getBatteryChargeCurrent(void);
    float getBatteryPower(void);
    float getACINVoltage(void);
    float getACINCurrent(void);
    float getVBUSVoltage(void);
    float getVBUSCurrent(void);
    float getAPSVoltage(void);
    float getInternalTemperature(void);

    std::uint8_t getPekPress(void);

    [[deprecated("use getACINVoltage()")]]
    inline float getACINVolatge(void) { return getACINVoltage(); }

  private:
    std::size_t readRegister12(std::uint8_t addr);
    std::size_t readRegister13(std::uint8_t addr);
    std::size_t readRegister16(std::uint8_t addr);
    std::size_t readRegister24(std::uint8_t addr);
    std::size_t readRegister32(std::uint8_t addr);

    void _set_DCDC(std::uint8_t num, int voltage);
    void _set_LDO(std::uint8_t num, int voltage);
    void _set_LDO2_LDO3(std::uint8_t num, int voltage);
    void _set_GPIO0_2(std::uint8_t num, bool state);
    void _set_GPIO3_4(std::uint8_t num, bool state);
  };
}

#endif
#endif