#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined (ESP_PLATFORM)

#include "SH200Q_Class.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

namespace m5
{
  SH200Q_Class::~SH200Q_Class() {}
  SH200Q_Class::SH200Q_Class(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c)
  : IMU_Base ( i2c_addr, freq, i2c )
  {}

  IMU_Base::imu_spec_t SH200Q_Class::begin(I2C_Class* i2c)
  {
    if (i2c)
    {
      _i2c = i2c;
    }

    // WHO_AM_I : IMU Check
    if (WhoAmI() != 0x18)
    {
      return imu_spec_none;
    }
    vTaskDelay(1);

    bitOn(0xC2, 0x04);
    vTaskDelay(1);

    bitOff(0xC2, 0x04);
    vTaskDelay(1);

    bitOn(0xD8, 0x80);
    vTaskDelay(1);

    bitOff(0xD8, 0x80);
    vTaskDelay(1);

    static constexpr std::uint8_t init_cmd[] =
    { 0x78, 0x61
    , 0x78, 0x00
    , 0x0E, 0x91  // ACC_CONFIG(0x0E) : 256Hz
    , 0x0F, 0x13  // GYRO_CONFIG(0x0F) : 500Hz
    , 0x11, 0x03  // GYRO_DLPF(0x11) : 50Hz
    , 0x12, 0x00  // FIFO_CONFIG(0x12)
    // , 0x12, 0x80  // FIFO_CONFIG(0x12) : FIFO MODE
, 0x14, 0x20 // data ready interrupt en
    , 0x16, 0x01  // ACC_RANGE(0x16) : +-8G
    , 0x2B, 0x00  // GYRO_RANGE(0x2B) : +-2000
    , 0xBA, 0xC0  // REG_SET1(0xBA)
    , 0xFF, 0xFF
    };

    for (int idx = -1;;)
    {
      std::uint8_t reg = init_cmd[++idx];
      std::uint8_t val = init_cmd[++idx];
      if ((reg & val) == 0xFF) { break; }
      writeRegister8(reg, val);
      vTaskDelay(1);
    }

    // REG_SET2(0xCA)
    bitOn(0xCA, 0x10);
    vTaskDelay(1);

    // REG_SET2(0xCA)
    bitOff(0xCA, 0x10);
    vTaskDelay(1);

    _init = true;
    return (imu_spec_t)(imu_spec_accel | imu_spec_gyro);
  }

  IMU_Base::imu_spec_t SH200Q_Class::getImuRawData(imu_raw_data_t* data) const
  {
    // static constexpr float aRes = 8.0f / 32768.0f;
    // static constexpr float gRes = 2000.0f / 32768.0f;
    uint8_t st = readRegister8(0x2C);
// printf("st:%d\n",st);
    if (st & 0x20u)
    {
      std::int16_t buf[6];
      bool res = readRegister(0x00, (std::uint8_t*)buf, 14);
      if (res)
      {
        memcpy(data->value, buf, 12);
        return (imu_spec_t)(imu_spec_accel | imu_spec_gyro);
      }
    }
    return imu_spec_none;
  }
/*
  bool SH200Q_Class::getImuRawData(imu_raw_data_t* data) const
  {
    // static constexpr float aRes = 8.0f / 32768.0f;
    // static constexpr float gRes = 2000.0f / 32768.0f;
    std::uint8_t state[2];
    std::int16_t buf[6];

    if (readRegister(0x2E, (std::uint8_t*)state, 2))
    {
      // 0x2E:accel FIFO Status
      // 0x2F:gyro FIFO Status
      uint32_t len = 7;
      uint32_t idx = 0;
      if (0 == (state[0] & 0x3F))
      {
        idx += 3;
        len -= 3;
      }
      if (0 == (state[1] & 0x3F))
      {
        len -= 4;
      }
      if (len > 2)
      {
        if (readRegister(idx << 1, (std::uint8_t*)(&buf[idx]), len << 1))
        {
          if (idx == 0)
          {
            data->accel.x = buf[0];
            data->accel.y = buf[1];
            data->accel.z = buf[2];
printf("idx:%d len:%d ax:%d ay:%d az:%d gx:%d gy:%d gz:%d\n", idx, len, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
          }
          if (len > 3)
          {
            data->gyro.x  = buf[3];
            data->gyro.y  = buf[4];
            data->gyro.z  = buf[5];
            data->temp    = buf[6];
            return true;
          }
        }
      }
    }
    return false;
  }

*/

  std::uint8_t SH200Q_Class::WhoAmI(void)
  {
    return readRegister8(0x30);
  }
/*
  bool SH200Q_Class::getAccelAdc(std::int16_t* ax, std::int16_t* ay, std::int16_t* az) const
  {
    std::int16_t buf[3];
    bool res = readRegister(0x00, (std::uint8_t*)buf, 6);
    *ax = buf[0];
    *ay = buf[1];
    *az = buf[2];
    return res;
  }

  bool SH200Q_Class::getGyroAdc(std::int16_t* gx, std::int16_t* gy, std::int16_t* gz) const
  {
    std::int16_t buf[3];
    bool res = readRegister(0x06, (std::uint8_t*)buf, 6);
    *gx = buf[0];
    *gy = buf[1];
    *gz = buf[2];
    return res;
  }

  bool SH200Q_Class::getAccel(float* ax, float* ay, float* az) const
  {
    static constexpr float aRes = 8.0f / 32768.0f;
    std::int16_t buf[3];
    bool res = readRegister(0x00, (std::uint8_t*)buf, 6);
    *ax = buf[0] * aRes;
    *ay = buf[1] * aRes;
    *az = buf[2] * aRes;
    return res;
  }

  bool SH200Q_Class::getGyro(float* gx, float* gy, float* gz) const
  {
    static constexpr float gRes = 2000.0f / 32768.0f;
    std::int16_t buf[3];
    bool res = readRegister(0x06, (std::uint8_t*)buf, 6);
    *gx = buf[0] * gRes;
    *gy = buf[1] * gRes;
    *gz = buf[2] * gRes;
    return res;
  }

  bool SH200Q_Class::getTemp(float *t) const
  {
    std::int16_t buf;
    bool res = readRegister(0x0C, (std::uint8_t*)&buf, 2);
    *t = 21.0f + buf / 333.87f;
    return res;
  }
//*/
  void SH200Q_Class::getConvertParam(imu_convert_param_t* param) const
  {
    param->accel_res = 8.0f / 32768.0f;
    param->gyro_res = 2000.0f / 32768.0f;
    param->temp_res = 1.0f / 333.87f;
    param->temp_offset = 21.0f;
  }

  bool SH200Q_Class::getTempAdc(int16_t *t) const
  {
    std::int16_t buf;
    bool res = readRegister(0x0C, (std::uint8_t*)&buf, 2);
    if (res) { *t = buf; }
    return res;
  }
}
#endif
#endif