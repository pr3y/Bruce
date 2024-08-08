#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined (ESP_PLATFORM)

#include "BMM150_Class.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace m5
{
  BMM150_Class::~BMM150_Class() {}
  BMM150_Class::BMM150_Class(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c)
  : IMU_Base ( i2c_addr, freq, i2c )
  {}

  IMU_Base::imu_spec_t BMM150_Class::begin(I2C_Class* i2c)
  {
    if (i2c)
    {
      _i2c = i2c;
    }

    writeRegister8(0x4B, 0x83); // software reset + power on
    
    WhoAmI();
// printf("BMM150 : %02x\n", WhoAmI());
    if (WhoAmI() != 0x32) 
    {
      return imu_spec_none;
    }
// printf("BMM150 found!\n");
    writeRegister8(0x4C, 0x38); // normal mode / ODR 30Hz

    // writeRegister8(CMD_REG_ADDR, SOFT_RESET_CMD); // software reset.
    // {
    //   int retry = 16;
    //   while (0 == readRegister8(PWR_CONF_ADDR) && --retry);
    // }

    // writeRegister8(PWR_CONF_ADDR, 0x00); // Power save disabled.
    // vTaskDelay(1);
    // bool res = _upload_file(BMM150_config_file, 0, sizeof(BMM150_config_file));
    // writeRegister8(INIT_CTRL_ADDR, 0x01);

    // writeRegister8(PWR_CTRL_ADDR, 0x0F); // temp en | ACC en | GYR en | AUX en

    // if (res) {
    //   int retry = 16;
    //   do { vTaskDelay(1);}
    //   while (0 == readRegister8(INTERNAL_STATUS_ADDR) && --retry);
    //   res = retry > 0;
    // }
    _init = true;
    return imu_spec_mag;
  }

  std::uint8_t BMM150_Class::WhoAmI(void)
  {
    return readRegister8(CHIP_ID_ADDR);
  }

  IMU_Base::imu_spec_t BMM150_Class::getImuRawData(imu_raw_data_t* data) const
  {
    if (readRegister8(0x48) & 1)
    {
      std::int16_t buf[3];
      if (readRegister(0x42, (std::uint8_t*)buf, 6))
      {
        data->mag.x = buf[0] >> 2;
        data->mag.y = buf[1] >> 2;
        data->mag.z = buf[2] & 0xFFFEu;
        return imu_spec_mag;
      }
    }
    return imu_spec_none;
  }

  void BMM150_Class::getConvertParam(imu_convert_param_t* param) const
  {
    param->mag_res = 10.0f * 4912.0f / 32760.0f;
    // param->temp_offset = 23.0f;
    // param->temp_res = 1.0f / 512.0f;
  }
}
#endif
#endif