#if defined (CORE2)
/*

    Arduino library for INA3221 current and voltage sensor.

    MIT License

    Copyright (c) 2020 Beast Devices, Andrejs Bondarevs

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

#ifndef _INA3221_H_
#define _INA3221_H_

#include "Arduino.h"
#include "Wire.h"

typedef enum {
    INA3221_ADDR40_GND = 0b1000000,  // A0 pin -> GND
    INA3221_ADDR41_VCC = 0b1000001,  // A0 pin -> VCC
    INA3221_ADDR42_SDA = 0b1000010,  // A0 pin -> SDA
    INA3221_ADDR43_SCL = 0b1000011   // A0 pin -> SCL
} ina3221_addr_t;

// Channels
typedef enum {
    INA3221_CH1 = 0,
    INA3221_CH2,
    INA3221_CH3,
    INA3221_CH_NUM
} ina3221_ch_t;

// Registers
typedef enum {
    INA3221_REG_CONF = 0,
    INA3221_REG_CH1_SHUNTV,
    INA3221_REG_CH1_BUSV,
    INA3221_REG_CH2_SHUNTV,
    INA3221_REG_CH2_BUSV,
    INA3221_REG_CH3_SHUNTV,
    INA3221_REG_CH3_BUSV,
    INA3221_REG_CH1_CRIT_ALERT_LIM,
    INA3221_REG_CH1_WARNING_ALERT_LIM,
    INA3221_REG_CH2_CRIT_ALERT_LIM,
    INA3221_REG_CH2_WARNING_ALERT_LIM,
    INA3221_REG_CH3_CRIT_ALERT_LIM,
    INA3221_REG_CH3_WARNING_ALERT_LIM,
    INA3221_REG_SHUNTV_SUM,
    INA3221_REG_SHUNTV_SUM_LIM,
    INA3221_REG_MASK_ENABLE,
    INA3221_REG_PWR_VALID_HI_LIM,
    INA3221_REG_PWR_VALID_LO_LIM,
    INA3221_REG_MANUF_ID = 0xFE,
    INA3221_REG_DIE_ID   = 0xFF
} ina3221_reg_t;

// Conversion times
typedef enum {
    INA3221_REG_CONF_CT_140US = 0,
    INA3221_REG_CONF_CT_204US,
    INA3221_REG_CONF_CT_332US,
    INA3221_REG_CONF_CT_588US,
    INA3221_REG_CONF_CT_1100US,
    INA3221_REG_CONF_CT_2116US,
    INA3221_REG_CONF_CT_4156US,
    INA3221_REG_CONF_CT_8244US
} ina3221_conv_time_t;

// Averaging modes
typedef enum {
    INA3221_REG_CONF_AVG_1 = 0,
    INA3221_REG_CONF_AVG_4,
    INA3221_REG_CONF_AVG_16,
    INA3221_REG_CONF_AVG_64,
    INA3221_REG_CONF_AVG_128,
    INA3221_REG_CONF_AVG_256,
    INA3221_REG_CONF_AVG_512,
    INA3221_REG_CONF_AVG_1024
} ina3221_avg_mode_t;

class INA3221 {
    // Configuration register
    typedef struct {
        uint16_t mode_shunt_en : 1;
        uint16_t mode_bus_en : 1;
        uint16_t mode_continious_en : 1;
        uint16_t shunt_conv_time : 3;
        uint16_t bus_conv_time : 3;
        uint16_t avg_mode : 3;
        uint16_t ch3_en : 1;
        uint16_t ch2_en : 1;
        uint16_t ch1_en : 1;
        uint16_t reset : 1;
    } __attribute__((packed)) conf_reg_t;

    // Mask/Enable register
    typedef struct {
        uint16_t conv_ready : 1;
        uint16_t timing_ctrl_alert : 1;
        uint16_t pwr_valid_alert : 1;
        uint16_t warn_alert_ch3 : 1;
        uint16_t warn_alert_ch2 : 1;
        uint16_t warn_alert_ch1 : 1;
        uint16_t shunt_sum_alert : 1;
        uint16_t crit_alert_ch3 : 1;
        uint16_t crit_alert_ch2 : 1;
        uint16_t crit_alert_ch1 : 1;
        uint16_t crit_alert_latch_en : 1;
        uint16_t warn_alert_latch_en : 1;
        uint16_t shunt_sum_en_ch3 : 1;
        uint16_t shunt_sum_en_ch2 : 1;
        uint16_t shunt_sum_en_ch1 : 1;
        uint16_t reserved : 1;
    } __attribute__((packed)) masken_reg_t;

    // Arduino's I2C library
    TwoWire *_i2c;

    // I2C address
    ina3221_addr_t _i2c_addr;

    // Shunt resistance in mOhm
    uint32_t _shuntRes[INA3221_CH_NUM];

    // Series filter resistance in Ohm
    uint32_t _filterRes[INA3221_CH_NUM];

    // Value of Mask/Enable register.
    masken_reg_t _masken_reg;

    // Reads 16 bytes from a register.
    void _read(ina3221_reg_t reg, uint16_t *val);

    // Writes 16 bytes to a register.
    void _write(ina3221_reg_t reg, uint16_t *val);

   public:
    INA3221(ina3221_addr_t addr = INA3221_ADDR40_GND) : _i2c_addr(addr){};
    // Initializes INA3221
    void begin(TwoWire *theWire = &Wire);

    // Sets shunt resistor value in mOhm
    void setShuntRes(uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3);

    // Sets filter resistors value in Ohm
    void setFilterRes(uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3);

    // Sets I2C address of INA3221
    void setAddr(ina3221_addr_t addr) {
        _i2c_addr = addr;
    }

    // Gets a register value.
    uint16_t getReg(ina3221_reg_t reg);

    // Resets INA3221
    void reset();

    // Sets operating mode to power-down
    void setModePowerDown();

    // Sets operating mode to continious
    void setModeContinious();

    // Sets operating mode to triggered (single-shot)
    void setModeTriggered();

    // Enables shunt-voltage measurement
    void setShuntMeasEnable();

    // Disables shunt-voltage mesurement
    void setShuntMeasDisable();

    // Enables bus-voltage measurement
    void setBusMeasEnable();

    // Disables bus-voltage measureement
    void setBusMeasDisable();

    // Sets averaging mode. Sets number of samples that are collected
    // and averaged togehter.
    void setAveragingMode(ina3221_avg_mode_t mode);

    // Sets bus-voltage conversion time.
    void setBusConversionTime(ina3221_conv_time_t convTime);

    // Sets shunt-voltage conversion time.
    void setShuntConversionTime(ina3221_conv_time_t convTime);

    // Sets power-valid upper-limit voltage. The power-valid condition
    // is reached when all bus-voltage channels exceed the value set.
    // When the powervalid condition is met, the PV alert pin asserts high.
    void setPwrValidUpLimit(int16_t voltagemV);

    // Sets power-valid lower-limit voltage. If any bus-voltage channel drops
    // below the power-valid lower-limit, the PV alert pin pulls low.
    void setPwrValidLowLimit(int16_t voltagemV);

    // Sets the value that is compared to the Shunt-Voltage Sum register value
    // following each completed cycle of all selected channels to detect
    // for system overcurrent events.
    void setShuntSumAlertLimit(int32_t voltagemV);

    // Sets the current value that is compared to the sum all currents.
    // This function is a helper for setShuntSumAlertLim(). It onverts current
    // value to shunt voltage value.
    void setCurrentSumAlertLimit(int32_t currentmA);

    // Enables warning alert latch.
    void setWarnAlertLatchEnable();

    // Disables warning alert latch.
    void setWarnAlertLatchDisable();

    // Enables critical alert latch.
    void setCritAlertLatchEnable();

    // Disables critical alert latch.
    void setCritAlertLatchDisable();

    // Reads flags from Mask/Enable register.
    // When Mask/Enable register is read, flags are cleared.
    // Use getTimingCtrlAlertFlag(), getPwrValidAlertFlag(),
    // getCurrentSumAlertFlag() and getConvReadyFlag() to get flags after
    // readFlags() is called.
    void readFlags();

    // Gets timing-control-alert flag indicator.
    bool getTimingCtrlAlertFlag();

    // Gets power-valid-alert flag indicator.
    bool getPwrValidAlertFlag();

    // Gets summation-alert flag indicator.
    bool getCurrentSumAlertFlag();

    // Gets Conversion-ready flag.
    bool getConversionReadyFlag();

    // Gets manufacturer ID.
    // Should read 0x5449.
    uint16_t getManufID();

    // Gets die ID.
    // Should read 0x3220.
    uint16_t getDieID();

    // Enables channel measurements
    void setChannelEnable(ina3221_ch_t channel);

    // Disables channel measurements
    void setChannelDisable(ina3221_ch_t channel);

    // Sets warning alert shunt voltage limit
    void setWarnAlertShuntLimit(ina3221_ch_t channel, int32_t voltageuV);

    // Sets critical alert shunt voltage limit
    void setCritAlertShuntLimit(ina3221_ch_t channel, int32_t voltageuV);

    // Sets warning alert current limit
    void setWarnAlertCurrentLimit(ina3221_ch_t channel, int32_t currentmA);

    // Sets critical alert current limit
    void setCritAlertCurrentLimit(ina3221_ch_t channel, int32_t currentmA);

    // Includes channel to fill Shunt-Voltage Sum register.
    void setCurrentSumEnable(ina3221_ch_t channel);

    // Excludes channel from filling Shunt-Voltage Sum register.
    void setCurrentSumDisable(ina3221_ch_t channel);

    // Gets shunt voltage in uV.
    int32_t getShuntVoltage(ina3221_ch_t channel);

    // Gets warning alert flag.
    bool getWarnAlertFlag(ina3221_ch_t channel);

    // Gets critical alert flag.
    bool getCritAlertFlag(ina3221_ch_t channel);

    // Estimates offset voltage added by the series filter resitors
    int32_t estimateOffsetVoltage(ina3221_ch_t channel, uint32_t busVoltage);

    // Gets current in A.
    float getCurrent(ina3221_ch_t channel);

    // Gets current compensated with calculated offset voltage.
    float getCurrentCompensated(ina3221_ch_t channel);

    // Gets bus voltage in V.
    float getVoltage(ina3221_ch_t channel);
};

#endif
#endif