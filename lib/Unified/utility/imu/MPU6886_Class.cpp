#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined (ESP_PLATFORM)

#include "MPU6886_Class.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace m5
{
  MPU6886_Class::~MPU6886_Class() {}
  MPU6886_Class::MPU6886_Class(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c)
  : IMU_Base ( i2c_addr, freq, i2c )
  {}

  IMU_Base::imu_spec_t MPU6886_Class::begin(I2C_Class* i2c)
  {
    if (i2c)
    {
      _i2c = i2c;
    }

    // WHO_AM_I : IMU Check
    auto id = readRegister8(0x75);
    _device_id = id;
    if (id != DEV_ID_MPU6886
     && id != DEV_ID_MPU6050
     && id != DEV_ID_MPU9250)
    {
      return imu_spec_none;
    }
    writeRegister8(REG_PWR_MGMT_1, 0x80);

    static constexpr std::uint8_t init_cmd[] =
    { REG_PWR_MGMT_1    , 0x01
    , REG_ACCEL_CONFIG  , 0x10  // ACCEL_CONFIG(0x1C) : +-8G
    , REG_GYRO_CONFIG   , 0x18  // GYRO_CONFIG(0x1B) : +-2000dps
    , REG_CONFIG        , 0x01  // CONFIG(0x1A)
    , REG_SMPLRT_DIV    , 0x03  // SMPLRT_DIV(0x19)
    , REG_INT_PIN_CFG   , 0xC0  // INT_PIN_CFG(0x37) : active low, open-drain
    , REG_INT_ENABLE    , 0x00  // INT_ENABLE(0x38) : all disable
    , REG_ACCEL_CONFIG2 , 0x00  // ACCEL_CONFIG 2(0x1D)
    , REG_USER_CTRL     , 0x00  // USER_CTRL(0x6A)
    , REG_FIFO_EN       , 0x00  // FIFO_EN(0x23)
    , 0xFF, 0xFF  // EOF
    };

    for (int idx = -1;;)
    {
      std::uint8_t reg = init_cmd[++idx];
      std::uint8_t val = init_cmd[++idx];
      if ((reg & val) == 0xFF) { break; }
      uint32_t retry_count = 16;
      do {
        writeRegister8(reg, val);
      } while (readRegister8(reg) != val && --retry_count);
    }

    setGyroFsr(Gscale::GFS_2000DPS);
    setAccelFsr(Ascale::AFS_8G);

    _init = true;
    _fifo_en = false;

// enableFIFO(Fodr::ODR_500Hz);

    return (imu_spec_t)(imu_spec_accel | imu_spec_gyro);
  }

  bool MPU6886_Class::sleep(void)
  {
    return writeRegister8(REG_PWR_MGMT_1, 0x40u);
  }

  bool MPU6886_Class::setGyroAdcOffset(std::int16_t gx, std::int16_t gy, std::int16_t gz)
  {
    uint8_t buffer[6] =
    { (uint8_t)(gx >> 8), (uint8_t)gx
    , (uint8_t)(gy >> 8), (uint8_t)gy
    , (uint8_t)(gz >> 8), (uint8_t)gz
    };
    return writeRegister(0x13, buffer, 6);
  }

  void MPU6886_Class::enableFIFO(Fodr output_data_rate)
  {
    auto regdata = readRegister8(REG_GYRO_CONFIG);
    regdata &= 0x1C;  // Clear bits 7:5 and 0:1 of FCHOICE_B to enable sample
                      // rate divider and DLPF setting
    writeRegister8(REG_GYRO_CONFIG, regdata);
    vTaskDelay(10);

    regdata = output_data_rate & 0xFF;  // Set sample rate clock divider based on passed
                                      // value for sample rate desired
    writeRegister8(REG_SMPLRT_DIV, regdata);
    vTaskDelay(10);

    regdata = readRegister8(REG_CONFIG);
    regdata |= 0x01;  // Set DLPF_CFG to 176Hz DLPF filtering (highest value
                      // where sample rate clock divider still works)
    regdata &= 0xBF;  // Clear bit 6 to allow overflow writes to the FIFO - Use
                      // it, or lose it!
    writeRegister8(REG_CONFIG, regdata);
    vTaskDelay(10);

// MPU9250: 0x78(TEMP OUT + GYRO XOUT,YOUT,ZOUT + ACCEL)
    regdata = 0xF8;  // Set GYRO_FIFO_EN and ACCEL_FIFO_EN bits to one in FIFO
                     // Enable register to enable FIFO on ALL sensor data
    writeRegister8(REG_FIFO_EN, regdata);
    vTaskDelay(10);

    regdata = readRegister8(REG_INT_ENABLE);
    regdata |= 0x10;  // Set bit 4 to turn on interrupts on FIFO overflow events
    writeRegister8(REG_INT_ENABLE, regdata);
    vTaskDelay(10);

    regdata = 0x44;  // Set FIFO_EN and FIFO_RST bits to one in User Control
                     // register to enable FIFO mode
    writeRegister8(REG_USER_CTRL, regdata);
    vTaskDelay(10);

    _fifo_en = true;
  }
/*
  void MPU6886_Class::disableFIFO(void)
  {
    _fifo_en = false;
    unsigned char regdata;
    regdata = 0x00;  // Clear GYRO_FIFO_EN and ACCEL_FIFO_EN bits to zero in
                     // FIFO Enable register
    writeRegister8(REG_FIFO_EN, regdata);
    vTaskDelay(10);

    writeRegister8(REG_INT_ENABLE, regdata);
    regdata &=
        0xEF;  // Clear bit 4 to turn off interrupts on FIFO overflow events
    writeRegister8(REG_INT_ENABLE, regdata);
    vTaskDelay(10);

    regdata = 0x00;  // Set FIFO_EN bit to zero in User Control register to
                     // dsiable FIFO mode
    writeRegister8(REG_USER_CTRL, regdata);
    vTaskDelay(10);
  }
//*/
  void MPU6886_Class::setGyroFsr(Gscale scale)
  {
    scale = (Gscale)(scale & 3);
    _gscale = scale;
    writeRegister8(REG_GYRO_CONFIG, scale << 3);
    vTaskDelay(10);
    static constexpr const float table[] =
    {  250.0f / 32768.0f   // GFS_250DPS
    ,  500.0f / 32768.0f   // GFS_500DPS
    , 1000.0f / 32768.0f   // GFS_1000DPS
    , 2000.0f / 32768.0f   // GFS_2000DPS
    };
    _gRes = table[scale];
  }

  void MPU6886_Class::setAccelFsr(Ascale scale)
  {
    scale = (Ascale)(scale & 3);
    _ascale = scale;
    writeRegister8(REG_ACCEL_CONFIG, scale << 3);
    vTaskDelay(10);
    static constexpr const float table[] =
    {  2.0f / 32768.0f  // AFS_2G
    ,  4.0f / 32768.0f  // AFS_4G
    ,  8.0f / 32768.0f  // AFS_8G
    , 16.0f / 32768.0f  // AFS_16G
    };
    _aRes = table[scale];
  }

  bool MPU6886_Class::setINTPinActiveLogic(bool level)
  {
    // MPU6886_INT_PIN_CFG = 0x37
    std::uint8_t tmp = readRegister8(REG_INT_PIN_CFG) & 0x7F;
    tmp |= level ? 0x00 : 0x80;
    return writeRegister8(REG_INT_PIN_CFG, tmp);
  }
/*
  bool MPU6886_Class::getAccelAdc(std::int16_t* ax, std::int16_t* ay, std::int16_t* az) const
  {
    std::uint8_t buf[6];
    bool res = readRegister(0x3B, buf, 6);
    *ax = (buf[0] << 8) + buf[1];
    *ay = (buf[2] << 8) + buf[3];
    *az = (buf[4] << 8) + buf[5];
    return res;
  }

  bool MPU6886_Class::getGyroAdc(std::int16_t* gx, std::int16_t* gy, std::int16_t* gz) const
  {
    std::uint8_t buf[6];
    bool res = readRegister(0x43, buf, 6);
    *gx = (buf[0] << 8) + buf[1];
    *gy = (buf[2] << 8) + buf[3];
    *gz = (buf[4] << 8) + buf[5];
    return res;
  }

  bool MPU6886_Class::getAccel(float* ax, float* ay, float* az) const
  {
    std::uint8_t buf[6];
    bool res = readRegister(0x3B, buf, 6);
    auto aRes = _aRes;
    *ax = (std::int16_t)((buf[0] << 8) + buf[1]) * aRes;
    *ay = (std::int16_t)((buf[2] << 8) + buf[3]) * aRes;
    *az = (std::int16_t)((buf[4] << 8) + buf[5]) * aRes;
    return res;
  }

  bool MPU6886_Class::getGyro(float* gx, float* gy, float* gz) const
  {
    std::uint8_t buf[6];
    bool res = readRegister(0x43, buf, 6);
    auto gRes = _gRes;
    *gx = (std::int16_t)((buf[0] << 8) + buf[1]) * gRes;
    *gy = (std::int16_t)((buf[2] << 8) + buf[3]) * gRes;
    *gz = (std::int16_t)((buf[4] << 8) + buf[5]) * gRes;
    return res;
  }

  bool MPU6886_Class::getTemp(float *t) const
  {
    std::uint8_t buf[2];
    bool res = readRegister(0x41, buf, 2);
    *t = 25.0f + ((buf[0] << 8) + buf[1]) / 326.8f;
    return res;
  }
//*/
  IMU_Base::imu_spec_t MPU6886_Class::getImuRawData(imu_raw_data_t* data) const
  {
    std::uint8_t raw_buf[16];
    auto buf = &raw_buf[1];

    if (_fifo_en)
    {
      if (!readRegister(REG_FIFO_COUNTH, buf, 2) || (buf[0] + buf[1] == 0))
      {
        return imu_spec_none;
      }
      if (!readRegister(REG_FIFO_R_W, buf, 14) || (buf[0] == 0x7F && buf[1] == 0x7F))
      {
        return imu_spec_none;
      }
    }
    else
    {
      if (!readRegister(0x3A, raw_buf, 1) || ((raw_buf[0] & 1) == 0))
      {
        return imu_spec_none;
      }
      if (!readRegister(0x3B, &raw_buf[1], 14))
      {
        return imu_spec_none;
      }
    }

    data->accel.x = (std::int16_t)((buf[0] << 8) + buf[1]);
    data->accel.y = (std::int16_t)((buf[2] << 8) + buf[3]);
    data->accel.z = (std::int16_t)((buf[4] << 8) + buf[5]);
 // data->temp    = (std::int16_t)((buf[6] << 8) + buf[7]);

    data->gyro.x = (std::int16_t)((buf[ 8] << 8) + buf[ 9]);
    data->gyro.y = (std::int16_t)((buf[10] << 8) + buf[11]);
    data->gyro.z = (std::int16_t)((buf[12] << 8) + buf[13]);
    return (imu_spec_t)(imu_spec_accel | imu_spec_gyro);
  }

  void MPU6886_Class::getConvertParam(imu_convert_param_t* param) const
  {
    param->accel_res = _aRes;
    param->gyro_res = _gRes;
    param->temp_res = 1.0f / 326.8f;
    param->temp_offset = 25.0f;
  }

  bool MPU6886_Class::getTempAdc(int16_t* t) const
  {
    std::uint8_t buf[2];
    bool res = readRegister(0x41, buf, 2);
    if (res) { *t = (buf[0] << 8) + buf[1]; }
    return res;
  }
}
#endif
#endif