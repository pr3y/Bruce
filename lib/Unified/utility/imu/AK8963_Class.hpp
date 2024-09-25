#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_AK8963_CLASS_H__
#define __M5_AK8963_CLASS_H__

#include "IMU_Base.hpp"

namespace m5
{
  class AK8963_Class : public IMU_Base
  {
  public:
    static constexpr const std::uint8_t WHO_AM_I_ADDR = 0x00;
    static constexpr const std::uint8_t STATUS1_ADDR  = 0x02;
    static constexpr const std::uint8_t XOUT_L_ADDR   = 0x03;
    static constexpr const std::uint8_t CTRL_CMD_ADDR = 0x0A;
    static constexpr const std::uint8_t ASAX_ADDR     = 0x10;

    static constexpr const std::uint8_t POWER_DOWN_CMD = 0x00;
    static constexpr const std::uint8_t SINGLE_MEASURE_CMD = 0x01;
    static constexpr const std::uint8_t SELF_TEST_CMD = 0x08;
    static constexpr const std::uint8_t FUSE_ROM_CMD = 0x0F;

    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x0C;

    virtual ~AK8963_Class();
    AK8963_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C);

    imu_spec_t begin(I2C_Class* i2c = nullptr) override;
    imu_spec_t getImuRawData(imu_raw_data_t* data) const override;
    void getConvertParam(imu_convert_param_t* param) const override;

    std::uint8_t WhoAmI(void);
  };
}

#endif
#endif