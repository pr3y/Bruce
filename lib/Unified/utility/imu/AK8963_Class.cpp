#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined (ESP_PLATFORM)

#include "AK8963_Class.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace m5
{
  AK8963_Class::~AK8963_Class() {}
  AK8963_Class::AK8963_Class(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c)
  : IMU_Base ( i2c_addr, freq, i2c )
  {}

  IMU_Base::imu_spec_t AK8963_Class::begin(I2C_Class* i2c)
  {
    if (i2c)
    {
      _i2c = i2c;
    }

    uint8_t rawData[3];  // x/y/z gyro calibration data stored here
    writeRegister8(CTRL_CMD_ADDR, POWER_DOWN_CMD);  // Power down magnetometer
    vTaskDelay(10);
    writeRegister8(CTRL_CMD_ADDR, FUSE_ROM_CMD);  // Enter Fuse ROM access mode
    vTaskDelay(10);
    readRegister(ASAX_ADDR, rawData, 3);  // Read the x-, y-, and z-axis calibration values

    writeRegister8(CTRL_CMD_ADDR, POWER_DOWN_CMD);  // Power down magnetometer
    vTaskDelay(10);
    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL
    // register, and enable continuous mode data acquisition Mmode (bits [3:0]),
    // 0010 for 8 Hz and 0110 for 100 Hz sample rates
    writeRegister8(CTRL_CMD_ADDR, 1 << 4 | 0x06);  // Power down magnetometer

    WhoAmI();
// printf("AK8963 : %02x\n", WhoAmI());
    if (WhoAmI() == 0x48) 
    {
      _init = true;
      return imu_spec_mag;
    }
    return imu_spec_none;
  }

  std::uint8_t AK8963_Class::WhoAmI(void)
  {
    return readRegister8(WHO_AM_I_ADDR);
  }

  IMU_Base::imu_spec_t AK8963_Class::getImuRawData(imu_raw_data_t* data) const
  {
    union
    {
      std::uint8_t buf8[10];
      std::int16_t buf16[5];
    };
    bool res = readRegister(STATUS1_ADDR, &buf8[1], 8) && buf8[1] && (0 == (buf8[8] & 0x08));
    if (res)
    {
      data->mag.x = buf16[1];
      data->mag.y = buf16[2];
      data->mag.z = buf16[3];
      return imu_spec_mag;
    }
    return imu_spec_none;
  }

  void AK8963_Class::getConvertParam(imu_convert_param_t* param) const
  { // Proper scale to return milliGauss
    param->mag_res = 10.0f * 4912.0f / 32760.0f;
  }
}

#endif
#endif