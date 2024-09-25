#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_BMI270_CLASS_H__
#define __M5_BMI270_CLASS_H__

#include "IMU_Base.hpp"

namespace m5
{
  class BMI270_Class : public IMU_Base
  {
  public:
    static constexpr const std::uint8_t CHIP_ID_ADDR            = 0x00;
    static constexpr const std::uint8_t ERR_REG_ADDR            = 0x02;
    static constexpr const std::uint8_t STATUS_ADDR             = 0x03;
    static constexpr const std::uint8_t AUX_X_LSB_ADDR          = 0x04;
    static constexpr const std::uint8_t ACC_X_LSB_ADDR          = 0x0C;
    static constexpr const std::uint8_t GYR_X_LSB_ADDR          = 0x12;
    static constexpr const std::uint8_t SENSORTIME_ADDR         = 0x18;
    static constexpr const std::uint8_t EVENT_ADDR              = 0x1B;
    static constexpr const std::uint8_t INT_STATUS_0_ADDR       = 0x1C;
    static constexpr const std::uint8_t INT_STATUS_1_ADDR       = 0x1D;
    static constexpr const std::uint8_t SC_OUT_0_ADDR           = 0x1E;
    static constexpr const std::uint8_t SYNC_COMMAND_ADDR       = 0x1E;
    static constexpr const std::uint8_t GYR_CAS_GPIO0_ADDR      = 0x1E;
    static constexpr const std::uint8_t INTERNAL_STATUS_ADDR    = 0x21;
    static constexpr const std::uint8_t TEMPERATURE_0_ADDR      = 0x22;
    static constexpr const std::uint8_t FIFO_LENGTH_0_ADDR      = 0x24;
    static constexpr const std::uint8_t FIFO_DATA_ADDR          = 0x26;
    static constexpr const std::uint8_t FEAT_PAGE_ADDR          = 0x2F;
    static constexpr const std::uint8_t FEATURES_REG_ADDR       = 0x30;
    static constexpr const std::uint8_t ACC_CONF_ADDR           = 0x40;
    static constexpr const std::uint8_t GYR_CONF_ADDR           = 0x42;
    static constexpr const std::uint8_t AUX_CONF_ADDR           = 0x44;
    static constexpr const std::uint8_t FIFO_DOWNS_ADDR         = 0x45;
    static constexpr const std::uint8_t FIFO_WTM_0_ADDR         = 0x46;
    static constexpr const std::uint8_t FIFO_WTM_1_ADDR         = 0x47;
    static constexpr const std::uint8_t FIFO_CONFIG_0_ADDR      = 0x48;
    static constexpr const std::uint8_t FIFO_CONFIG_1_ADDR      = 0x49;
    static constexpr const std::uint8_t AUX_DEV_ID_ADDR         = 0x4B;
    static constexpr const std::uint8_t AUX_IF_CONF_ADDR        = 0x4C;
    static constexpr const std::uint8_t AUX_RD_ADDR             = 0x4D;
    static constexpr const std::uint8_t AUX_WR_ADDR             = 0x4E;
    static constexpr const std::uint8_t AUX_WR_DATA_ADDR        = 0x4F;
    static constexpr const std::uint8_t INT1_IO_CTRL_ADDR       = 0x53;
    static constexpr const std::uint8_t INT2_IO_CTRL_ADDR       = 0x54;
    static constexpr const std::uint8_t INT_LATCH_ADDR          = 0x55;
    static constexpr const std::uint8_t INT1_MAP_FEAT_ADDR      = 0x56;
    static constexpr const std::uint8_t INT2_MAP_FEAT_ADDR      = 0x57;
    static constexpr const std::uint8_t INT_MAP_DATA_ADDR       = 0x58;
    static constexpr const std::uint8_t INIT_CTRL_ADDR          = 0x59;
    static constexpr const std::uint8_t INIT_ADDR_0             = 0x5B;
    static constexpr const std::uint8_t INIT_ADDR_1             = 0x5C;
    static constexpr const std::uint8_t INIT_DATA_ADDR          = 0x5E;
    static constexpr const std::uint8_t AUX_IF_TRIM             = 0x68;
    static constexpr const std::uint8_t GYR_CRT_CONF_ADDR       = 0x69;
    static constexpr const std::uint8_t NVM_CONF_ADDR           = 0x6A;
    static constexpr const std::uint8_t IF_CONF_ADDR            = 0x6B;
    static constexpr const std::uint8_t ACC_SELF_TEST_ADDR      = 0x6D;
    static constexpr const std::uint8_t GYR_SELF_TEST_AXES_ADDR = 0x6E;
    static constexpr const std::uint8_t SELF_TEST_MEMS_ADDR     = 0x6F;
    static constexpr const std::uint8_t NV_CONF_ADDR            = 0x70;
    static constexpr const std::uint8_t ACC_OFF_COMP_0_ADDR     = 0x71;
    static constexpr const std::uint8_t GYR_OFF_COMP_3_ADDR     = 0x74;
    static constexpr const std::uint8_t GYR_OFF_COMP_6_ADDR     = 0x77;
    static constexpr const std::uint8_t GYR_USR_GAIN_0_ADDR     = 0x78;
    static constexpr const std::uint8_t PWR_CONF_ADDR           = 0x7C;
    static constexpr const std::uint8_t PWR_CTRL_ADDR           = 0x7D;
    static constexpr const std::uint8_t CMD_REG_ADDR            = 0x7E;


    static constexpr const std::uint8_t G_TRIGGER_CMD  = 0x02;
    static constexpr const std::uint8_t USR_GAIN_CMD   = 0x03;
    static constexpr const std::uint8_t NVM_PROG_CMD   = 0xA0;
    static constexpr const std::uint8_t SOFT_RESET_CMD = 0xB6;
    static constexpr const std::uint8_t FIFO_FLUSH_CMD = 0xB0;


    static constexpr std::uint8_t DEFAULT_ADDRESS = 0x69;

    virtual ~BMI270_Class();
    BMI270_Class(std::uint8_t i2c_addr = DEFAULT_ADDRESS, std::uint32_t freq = 400000, I2C_Class* i2c = &In_I2C);

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
  protected:
    bool _upload_file(const std::uint8_t *config_data, std::size_t index, std::size_t write_len);

    bool auxSetupMode(std::uint8_t i2c_addr);
    bool auxWriteRegister8(std::uint8_t reg, std::uint8_t data);
    std::uint8_t auxReadRegister8(std::uint8_t reg);
  };
}
#endif
#endif
