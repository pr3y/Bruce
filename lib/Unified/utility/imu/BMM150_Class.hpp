#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_BMM150_CLASS_H__
#define __M5_BMM150_CLASS_H__

#include "IMU_Base.hpp"

namespace m5
{
  class BMM150_Class : public IMU_Base
  {
  public:
    static constexpr const std::uint8_t CHIP_ID_ADDR            = 0x40;

    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x10;

    virtual ~BMM150_Class();
    BMM150_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C);

    imu_spec_t begin(I2C_Class* i2c = nullptr) override;
    imu_spec_t getImuRawData(imu_raw_data_t* data) const override;
    void getConvertParam(imu_convert_param_t* param) const override;

    std::uint8_t WhoAmI(void);
  };
}

#endif
#endif