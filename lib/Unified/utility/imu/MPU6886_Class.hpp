#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_MPU6886_CLASS_H__
#define __M5_MPU6886_CLASS_H__

#include "IMU_Base.hpp"

namespace m5
{
  class MPU6886_Class : public IMU_Base
  {
  public:
    static constexpr const std::uint8_t DEV_ID_MPU6886 = 0x19;
    static constexpr const std::uint8_t DEV_ID_MPU6050 = 0x68;
    static constexpr const std::uint8_t DEV_ID_MPU9250 = 0x71;

    static constexpr const std::uint8_t REG_WHOAMI           = 0x75;
    static constexpr const std::uint8_t REG_ACCEL_INTEL_CTRL = 0x69;
    static constexpr const std::uint8_t REG_SMPLRT_DIV       = 0x19;
    static constexpr const std::uint8_t REG_INT_PIN_CFG      = 0x37;
    static constexpr const std::uint8_t REG_INT_ENABLE       = 0x38;
    static constexpr const std::uint8_t REG_ACCEL_XOUT_H     = 0x3B;
    static constexpr const std::uint8_t REG_ACCEL_XOUT_L     = 0x3C;
    static constexpr const std::uint8_t REG_ACCEL_YOUT_H     = 0x3D;
    static constexpr const std::uint8_t REG_ACCEL_YOUT_L     = 0x3E;
    static constexpr const std::uint8_t REG_ACCEL_ZOUT_H     = 0x3F;
    static constexpr const std::uint8_t REG_ACCEL_ZOUT_L     = 0x40;

    static constexpr const std::uint8_t REG_TEMP_OUT_H    = 0x41;
    static constexpr const std::uint8_t REG_TEMP_OUT_L    = 0x42;

    static constexpr const std::uint8_t REG_GYRO_XOUT_H   = 0x43;
    static constexpr const std::uint8_t REG_GYRO_XOUT_L   = 0x44;
    static constexpr const std::uint8_t REG_GYRO_YOUT_H   = 0x45;
    static constexpr const std::uint8_t REG_GYRO_YOUT_L   = 0x46;
    static constexpr const std::uint8_t REG_GYRO_ZOUT_H   = 0x47;
    static constexpr const std::uint8_t REG_GYRO_ZOUT_L   = 0x48;

    static constexpr const std::uint8_t REG_USER_CTRL     = 0x6A;
    static constexpr const std::uint8_t REG_PWR_MGMT_1    = 0x6B;
    static constexpr const std::uint8_t REG_PWR_MGMT_2    = 0x6C;
    static constexpr const std::uint8_t REG_CONFIG        = 0x1A;
    static constexpr const std::uint8_t REG_GYRO_CONFIG   = 0x1B;
    static constexpr const std::uint8_t REG_ACCEL_CONFIG  = 0x1C;
    static constexpr const std::uint8_t REG_ACCEL_CONFIG2 = 0x1D;
    static constexpr const std::uint8_t REG_LP_MODE_CFG   = 0x1E;
    static constexpr const std::uint8_t REG_FIFO_EN       = 0x23;

    static constexpr const std::uint8_t REG_FIFO_COUNTH   = 0x72;
    static constexpr const std::uint8_t REG_FIFO_R_W      = 0x74;

    enum Ascale
    { AFS_2G = 0
    , AFS_4G
    , AFS_8G
    , AFS_16G
    };

    enum Gscale
    { GFS_250DPS = 0
    , GFS_500DPS
    , GFS_1000DPS
    , GFS_2000DPS
    };

    enum Fodr
    { ODR_1kHz  = 0
    , ODR_500Hz = 1
    // , ODR_333Hz = 2
    , ODR_250Hz = 3
    , ODR_200Hz = 4
    // , ODR_166Hz = 5
    // , ODR_143Hz = 6
    , ODR_125Hz = 7
    // , ODR_111Hz = 8
    , ODR_100Hz = 9
    , ODR_50Hz  = 19
    , ODR_10Hz  = 99
    };

    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x68;

    virtual ~MPU6886_Class();
    MPU6886_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C);

    imu_spec_t begin(I2C_Class* i2c = nullptr) override;
    imu_spec_t getImuRawData(imu_raw_data_t* data) const override;
    void getConvertParam(imu_convert_param_t* param) const;
    bool getTempAdc(int16_t* adc) const override;
    bool sleep(void) override;
/*
    bool getAccelAdc(std::int16_t* ax, std::int16_t* ay, std::int16_t* az) const override;
    bool getGyroAdc(std::int16_t* gx, std::int16_t* gy, std::int16_t* gz) const override;
    bool getAccel(float* ax, float* ay, float* az) const override;
    bool getGyro(float* gx, float* gy, float* gz) const override;
    bool getTemp(float* t) const override;
//*/
    void enableFIFO(Fodr output_data_rate);
    // void disableFIFO(void);

    bool setGyroAdcOffset(std::int16_t gx, std::int16_t gy, std::int16_t gz);
    bool setINTPinActiveLogic(bool level) override;

    std::uint8_t whoAmI(void) const { return _device_id; }

  protected:
    void setGyroFsr(Gscale scale);
    void setAccelFsr(Ascale scale);

    bool _fifo_en;
    float _aRes, _gRes;
    Gscale _gscale;
    Ascale _ascale;
    std::uint8_t _device_id = 0;
  };
}

#endif
#endif