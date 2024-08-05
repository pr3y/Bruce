#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_IP5306_CLASS_H__
#define __M5_IP5306_CLASS_H__

#include "I2C_Class.hpp"

namespace m5
{
  class IP5306_Class : public I2C_Device
  {
  public:

    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x75;

    IP5306_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C)
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
    /// @param max_mA milli ampere. (150 - 3150).
    void setChargeCurrent(std::uint16_t max_mA);

    /// set battery charge voltage
    /// @param max_mV milli volt. (4200 - 4400).
    void setChargeVoltage(std::uint16_t max_mV);

    /// Get whether the battery is currently charging or not.
    bool isCharging(void);

    /// Set whether or not to continue supplying power even at low loads.
    bool setPowerBoostKeepOn(bool en);

  private:
  };
}

#endif
#endif