#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_INA3221_CLASS_H__
#define __M5_INA3221_CLASS_H__

#include "I2C_Class.hpp"

namespace m5
{
  class INA3221_Class : public I2C_Device
  {
  public:
    static constexpr uint8_t INA3221_CH1_SHUNT_V = 0x01;
    static constexpr uint8_t INA3221_CH1_BUS_V   = 0x02;
    static constexpr uint8_t INA3221_CH2_SHUNT_V = 0x03;
    static constexpr uint8_t INA3221_CH2_BUS_V   = 0x04;
    static constexpr uint8_t INA3221_CH3_SHUNT_V = 0x05;
    static constexpr uint8_t INA3221_CH3_BUS_V   = 0x06;
    static constexpr std::uint8_t INA3221_CH_NUM_MAX = 3;

    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x40;

    INA3221_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C)
    : I2C_Device ( i2c_addr, freq, i2c )
    {
      _shunt_res[0] = 10;
      _shunt_res[1] = 10;
      _shunt_res[2] = 10;
    }

    bool begin(void);

    /// Get the voltage of the specified channel.
    /// @param channel 0=1ch / 1=2ch / 2=3ch / other=invalid
    /// @return voltage [mV]
    float getBusVoltage(uint8_t channel);
    float getShuntVoltage(uint8_t channel);
    float getCurrent(uint8_t channel);

    int_fast16_t getBusMilliVoltage(uint8_t channel);
    int32_t getShuntMilliVoltage(uint8_t channel);

    void setShuntRes(uint8_t channel, uint32_t res);

  private:
    std::size_t readRegister16(std::uint8_t addr);

    uint32_t _shunt_res[INA3221_CH_NUM_MAX];
  };
}

#endif
#endif