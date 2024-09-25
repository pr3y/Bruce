#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_SH200Q_CLASS_H__
#define __M5_SH200Q_CLASS_H__

#include "IMU_Base.hpp"

namespace m5
{
  class SH200Q_Class : public IMU_Base
  {
  public:
    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x6C;

    virtual ~SH200Q_Class();
    SH200Q_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C);

    imu_spec_t begin(I2C_Class* i2c = nullptr) override;
    imu_spec_t getImuRawData(imu_raw_data_t* data) const override;
    void getConvertParam(imu_convert_param_t* param) const override;
    bool getTempAdc(int16_t* adc) const override;

    std::uint8_t WhoAmI(void);
/*
    bool getAccelAdc(std::int16_t* ax, std::int16_t* ay, std::int16_t* az) const override;
    bool getGyroAdc(std::int16_t* gx, std::int16_t* gy, std::int16_t* gz) const override;
    bool getAccel(float* ax, float* ay, float* az) const override;
    bool getGyro(float* gx, float* gy, float* gz) const override;
    bool getTemp(float *t) const override;
//*/
  };
}

#endif
#endif