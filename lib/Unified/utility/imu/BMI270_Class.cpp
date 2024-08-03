#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined (ESP_PLATFORM)

#include "BMI270_Class.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace m5
{
  #include "BMI270_config.inl"

  BMI270_Class::~BMI270_Class() {}
  BMI270_Class::BMI270_Class(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c)
  : IMU_Base ( i2c_addr, freq, i2c )
  {}

  bool BMI270_Class::_upload_file(const uint8_t *config_data, size_t index, size_t write_len)
  {
    uint8_t addr_array[2] = {
      (uint8_t)((index >> 1) & 0x0F),
      (uint8_t)(index >> 5)
    };

    if (config_data != nullptr
      && writeRegister( INIT_ADDR_0, addr_array, 2 )
      && writeRegister( INIT_DATA_ADDR, (uint8_t *)config_data, write_len))
    {
      return true;
    }
    return true;
  }

  IMU_Base::imu_spec_t BMI270_Class::begin(I2C_Class* i2c)
  {
    if (i2c)
    {
      _i2c = i2c;
    }

    WhoAmI();
    if (WhoAmI() != 0x24) 
    {
      return imu_spec_none;
    }

    writeRegister8(CMD_REG_ADDR, SOFT_RESET_CMD); // software reset.
    {
      int retry = 16;
      do { vTaskDelay(1); }
      while (0 == readRegister8(PWR_CONF_ADDR) && --retry);
    }

    writeRegister8(PWR_CONF_ADDR, 0x00); // Power save disabled.
    vTaskDelay(1);
    bool res = _upload_file(bmi270_config_file, 0, sizeof(bmi270_config_file));
    writeRegister8(INIT_CTRL_ADDR, 0x01);
    writeRegister8(INT_MAP_DATA_ADDR, 0xFF);

    _init = res;
    if (!res)
    {
      return imu_spec_none;
    }

    imu_spec_t spec = (imu_spec_t)(imu_spec_accel | imu_spec_gyro);
    int retry = 16;
    do { vTaskDelay(1);}
    while (0 == readRegister8(INTERNAL_STATUS_ADDR) && --retry);
    res = retry > 0;

// 以下は AUXセンサとしてBMM150が接続されている想定の設定
    auxSetupMode(0x10); // 0x10 = BMM150 I2C Addr
    auxWriteRegister8(0x4B, 0x83); // software reset + power on
    auxReadRegister8(0x40); // 0x40 = WhoAmI
    auto who_am_i = auxReadRegister8(0x40); // 0x40 = WhoAmI
    if (who_am_i == 0x32)
    {
      auxWriteRegister8(0x4C, 0x38); // normal mode / ODR 30Hz
      spec = (imu_spec_t)(imu_spec_accel | imu_spec_gyro | imu_spec_mag);
      writeRegister8(AUX_IF_CONF_ADDR, 0x4F); // FCU_WRITE_EN + Manual BurstLength 8 + BurstLength 8
      writeRegister8(AUX_RD_ADDR     , 0x42);  // 0x42 = BMM150 I2C Data X LSB reg
      writeRegister8(PWR_CTRL_ADDR, 0x0F); // temp en | ACC en | GYR en | AUX en
    }
    return spec;
  }

  bool BMI270_Class::auxSetupMode(std::uint8_t i2c_addr)
  {
    writeRegister8(IF_CONF_ADDR, 0x20);  // AUX I2C enable.
    writeRegister8(PWR_CONF_ADDR, 0x00); // Power save disabled.
    writeRegister8(PWR_CTRL_ADDR, 0x0E); // AUX sensor disable.
    writeRegister8(AUX_IF_CONF_ADDR, 0x80);
    return writeRegister8(AUX_DEV_ID_ADDR , i2c_addr << 1);
  }

  bool BMI270_Class::auxWriteRegister8(std::uint8_t reg, std::uint8_t data)
  {
    writeRegister8(AUX_WR_DATA_ADDR, data); // AUXセンサに書き込む値
    writeRegister8(AUX_WR_ADDR, reg);       // AUXセンサに書き込むレジスタ番号
    int retry = 3;
    while ((readRegister8(STATUS_ADDR) & 0b100) && --retry) { vTaskDelay(1); }
    return retry;
  }

  std::uint8_t BMI270_Class::auxReadRegister8(std::uint8_t reg)
  {
    writeRegister8(AUX_IF_CONF_ADDR, 0x80); // enable read write. Burst length 1
    writeRegister8(AUX_RD_ADDR, reg);       // AUXセンサから読み取るレジスタ番号
    int retry = 3;
    while ((readRegister8(STATUS_ADDR) & 0b100) && --retry) { vTaskDelay(1); }
    return readRegister8(AUX_X_LSB_ADDR);
  }

  std::uint8_t BMI270_Class::WhoAmI(void)
  {
    return readRegister8(CHIP_ID_ADDR);
  }

  IMU_Base::imu_spec_t BMI270_Class::getImuRawData(imu_raw_data_t* data) const
  {
    imu_spec_t res = imu_spec_none;
    std::uint8_t intstat = readRegister8(INT_STATUS_1_ADDR);
    if (intstat & 0xE0)
    {
      std::int16_t buf[10];
      if (readRegister(AUX_X_LSB_ADDR, (std::uint8_t*)buf, 20))
      {
        if (intstat & 0x80u)
        {
          data->accel.x = buf[4];
          data->accel.y = buf[5];
          data->accel.z = buf[6];
          res = (imu_spec_t)(res | imu_spec_accel);
        }
        if (intstat & 0x40u)
        {
          data->gyro.x = buf[7];
          data->gyro.y = buf[8];
          data->gyro.z = buf[9];
          res = (imu_spec_t)(res | imu_spec_gyro);
        }
        if (intstat & 0x20u)
        {
          data->mag.x = buf[0] >> 2;
          data->mag.y = buf[1] >> 2;
          data->mag.z = buf[2] & 0xFFFE;
          res = (imu_spec_t)(res | imu_spec_mag);
        }
      }
    }
    return res;
  }

  void BMI270_Class::getConvertParam(imu_convert_param_t* param) const
  {
    param->mag_res = 10.0f * 4912.0f / 32760.0f;
    param->temp_offset = 23.0f;
    param->temp_res = 1.0f / 512.0f;
  }

  bool BMI270_Class::getTempAdc(int16_t *t) const
  {
    std::int16_t buf;
    bool res = readRegister(TEMPERATURE_0_ADDR, (std::uint8_t*)&buf, 2);
    if (res) { *t = buf; }
    return res;
  }

/*
  bool BMI270_Class::getAccelAdc(std::int16_t* ax, std::int16_t* ay, std::int16_t* az) const
  {
    std::int16_t buf[3];
    bool res = readRegister(ACC_X_LSB_ADDR, (std::uint8_t*)buf, 6);
    *ax = buf[0];
    *ay = buf[1];
    *az = buf[2];
    return res;
  }

  bool BMI270_Class::getGyroAdc(std::int16_t* gx, std::int16_t* gy, std::int16_t* gz) const
  {
    std::int16_t buf[3];
    bool res = readRegister(GYR_X_LSB_ADDR, (std::uint8_t*)buf, 6);
    *gx = buf[0];
    *gy = buf[1];
    *gz = buf[2];
    return res;
  }

  bool BMI270_Class::getAccel(float* ax, float* ay, float* az) const
  {
    static constexpr float aRes = 8.0f / 32768.0f;
    std::int16_t buf[3];
    bool res = readRegister(ACC_X_LSB_ADDR, (std::uint8_t*)buf, 6);
    *ax = buf[0] * aRes;
    *ay = buf[1] * aRes;
    *az = buf[2] * aRes;
    return res;
  }

  bool BMI270_Class::getGyro(float* gx, float* gy, float* gz) const
  {
    static constexpr float gRes = 2000.0f / 32768.0f;
    std::int16_t buf[3];
    bool res = readRegister(GYR_X_LSB_ADDR, (std::uint8_t*)buf, 6);
    *gx = buf[0] * gRes;
    *gy = buf[1] * gRes;
    *gz = buf[2] * gRes;
    return res;
  }

  bool BMI270_Class::getTemp(float *t) const
  {
    std::int16_t buf;
    bool res = readRegister(TEMPERATURE_0_ADDR, (std::uint8_t*)&buf, 2);
    *t = 23.0f + buf / 512.0f;
    return res;
  }
//*/
}
#endif
#endif