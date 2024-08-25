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

#include "INA3221.h"

void INA3221::_read(ina3221_reg_t reg, uint16_t *val) {
    _i2c->beginTransmission(_i2c_addr);
    _i2c->write(reg);  // Register
    _i2c->endTransmission(false);

    _i2c->requestFrom((uint8_t)_i2c_addr, (uint8_t)2);

    if (_i2c->available()) {
        *val = ((_i2c->read() << 8) | _i2c->read());
    }
}

void INA3221::_write(ina3221_reg_t reg, uint16_t *val) {
    _i2c->beginTransmission(_i2c_addr);
    _i2c->write(reg);                 // Register
    _i2c->write((*val >> 8) & 0xFF);  // Upper 8-bits
    _i2c->write(*val & 0xFF);         // Lower 8-bits
    _i2c->endTransmission();
}

void INA3221::begin(TwoWire *theWire) {
    _i2c = theWire;

    _shuntRes[0] = 10;
    _shuntRes[1] = 10;
    _shuntRes[2] = 10;

    _filterRes[0] = 0;
    _filterRes[1] = 0;
    _filterRes[2] = 0;

    _i2c->begin();
}

void INA3221::setShuntRes(uint32_t res_ch1, uint32_t res_ch2,
                          uint32_t res_ch3) {
    _shuntRes[0] = res_ch1;
    _shuntRes[1] = res_ch2;
    _shuntRes[2] = res_ch3;
}

void INA3221::setFilterRes(uint32_t res_ch1, uint32_t res_ch2,
                           uint32_t res_ch3) {
    _filterRes[0] = res_ch1;
    _filterRes[1] = res_ch2;
    _filterRes[2] = res_ch3;
}

uint16_t INA3221::getReg(ina3221_reg_t reg) {
    uint16_t val = 0;
    _read(reg, &val);
    return val;
}

void INA3221::reset() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.reset = 1;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setModePowerDown() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_bus_en        = 0;
    conf_reg.mode_continious_en = 0;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setModeContinious() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_continious_en = 1;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setModeTriggered() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_continious_en = 0;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setShuntMeasEnable() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_shunt_en = 1;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setShuntMeasDisable() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_shunt_en = 0;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setBusMeasEnable() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_bus_en = 1;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setBusMeasDisable() {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.mode_bus_en = 0;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setAveragingMode(ina3221_avg_mode_t mode) {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.avg_mode = mode;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setBusConversionTime(ina3221_conv_time_t convTime) {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.bus_conv_time = convTime;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setShuntConversionTime(ina3221_conv_time_t convTime) {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);
    conf_reg.shunt_conv_time = convTime;
    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setPwrValidUpLimit(int16_t voltagemV) {
    _write(INA3221_REG_PWR_VALID_HI_LIM, (uint16_t *)&voltagemV);
}

void INA3221::setPwrValidLowLimit(int16_t voltagemV) {
    _write(INA3221_REG_PWR_VALID_LO_LIM, (uint16_t *)&voltagemV);
}

void INA3221::setShuntSumAlertLimit(int32_t voltageuV) {
    int16_t val = 0;
    val         = voltageuV / 20;
    _write(INA3221_REG_SHUNTV_SUM_LIM, (uint16_t *)&val);
}

void INA3221::setCurrentSumAlertLimit(int32_t currentmA) {
    int16_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)_shuntRes[INA3221_CH1];
    setShuntSumAlertLimit(shuntuV);
}

void INA3221::setWarnAlertLatchEnable() {
    masken_reg_t masken_reg;

    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    masken_reg.warn_alert_latch_en = 1;
    _write(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221::setWarnAlertLatchDisable() {
    masken_reg_t masken_reg;

    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    masken_reg.warn_alert_latch_en = 1;
    _write(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221::setCritAlertLatchEnable() {
    masken_reg_t masken_reg;

    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    masken_reg.crit_alert_latch_en = 1;
    _write(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221::setCritAlertLatchDisable() {
    masken_reg_t masken_reg;

    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    masken_reg.crit_alert_latch_en = 1;
    _write(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221::readFlags() {
    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&_masken_reg);
}

bool INA3221::getTimingCtrlAlertFlag() {
    return _masken_reg.timing_ctrl_alert;
}

bool INA3221::getPwrValidAlertFlag() {
    return _masken_reg.pwr_valid_alert;
}

bool INA3221::getCurrentSumAlertFlag() {
    return _masken_reg.shunt_sum_alert;
}

bool INA3221::getConversionReadyFlag() {
    return _masken_reg.conv_ready;
}

uint16_t INA3221::getManufID() {
    uint16_t id = 0;
    _read(INA3221_REG_MANUF_ID, &id);
    return id;
}

uint16_t INA3221::getDieID() {
    uint16_t id = 0;
    _read(INA3221_REG_DIE_ID, &id);
    return id;
}

void INA3221::setChannelEnable(ina3221_ch_t channel) {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);

    switch (channel) {
        case INA3221_CH1:
            conf_reg.ch1_en = 1;
            break;
        case INA3221_CH2:
            conf_reg.ch2_en = 1;
            break;
        case INA3221_CH3:
            conf_reg.ch3_en = 1;
            break;
        default:
            break;
    }

    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setChannelDisable(ina3221_ch_t channel) {
    conf_reg_t conf_reg;

    _read(INA3221_REG_CONF, (uint16_t *)&conf_reg);

    switch (channel) {
        case INA3221_CH1:
            conf_reg.ch1_en = 0;
            break;
        case INA3221_CH2:
            conf_reg.ch2_en = 0;
            break;
        case INA3221_CH3:
            conf_reg.ch3_en = 0;
            break;
        default:
            break;
    }

    _write(INA3221_REG_CONF, (uint16_t *)&conf_reg);
}

void INA3221::setWarnAlertShuntLimit(ina3221_ch_t channel, int32_t voltageuV) {
    ina3221_reg_t reg = INA3221_REG_CONF;
    int16_t val       = 0;

    switch (channel) {
        case INA3221_CH1:
            reg = INA3221_REG_CH1_WARNING_ALERT_LIM;
            break;
        case INA3221_CH2:
            reg = INA3221_REG_CH2_WARNING_ALERT_LIM;
            break;
        case INA3221_CH3:
            reg = INA3221_REG_CH3_WARNING_ALERT_LIM;
            break;
        default:
            break;
    }

    val = voltageuV / 5;
    _write(reg, (uint16_t *)&val);
}

void INA3221::setCritAlertShuntLimit(ina3221_ch_t channel, int32_t voltageuV) {
    ina3221_reg_t reg = INA3221_REG_CONF;
    int16_t val       = 0;

    switch (channel) {
        case INA3221_CH1:
            reg = INA3221_REG_CH1_CRIT_ALERT_LIM;
            break;
        case INA3221_CH2:
            reg = INA3221_REG_CH2_CRIT_ALERT_LIM;
            break;
        case INA3221_CH3:
            reg = INA3221_REG_CH3_CRIT_ALERT_LIM;
            break;
        default:
            break;
    }

    val = voltageuV / 5;
    _write(reg, (uint16_t *)&val);
}

void INA3221::setWarnAlertCurrentLimit(ina3221_ch_t channel,
                                       int32_t currentmA) {
    int32_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)_shuntRes[channel];
    setWarnAlertShuntLimit(channel, shuntuV);
}

void INA3221::setCritAlertCurrentLimit(ina3221_ch_t channel,
                                       int32_t currentmA) {
    int32_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)_shuntRes[channel];
    setCritAlertShuntLimit(channel, shuntuV);
}

void INA3221::setCurrentSumEnable(ina3221_ch_t channel) {
    masken_reg_t masken_reg;

    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);

    switch (channel) {
        case INA3221_CH1:
            masken_reg.shunt_sum_en_ch1 = 1;
            break;
        case INA3221_CH2:
            masken_reg.shunt_sum_en_ch2 = 1;
            break;
        case INA3221_CH3:
            masken_reg.shunt_sum_en_ch3 = 1;
            break;
        default:
            break;
    }

    _write(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    _masken_reg = masken_reg;
}

void INA3221::setCurrentSumDisable(ina3221_ch_t channel) {
    masken_reg_t masken_reg;

    _read(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);

    switch (channel) {
        case INA3221_CH1:
            masken_reg.shunt_sum_en_ch1 = 0;
            break;
        case INA3221_CH2:
            masken_reg.shunt_sum_en_ch2 = 0;
            break;
        case INA3221_CH3:
            masken_reg.shunt_sum_en_ch3 = 0;
            break;
        default:
            break;
    }

    _write(INA3221_REG_MASK_ENABLE, (uint16_t *)&masken_reg);
    _masken_reg = masken_reg;
}

int32_t INA3221::getShuntVoltage(ina3221_ch_t channel) {
    int32_t res;
    ina3221_reg_t reg = INA3221_REG_CONF;
    uint16_t val_raw  = 0;

    switch (channel) {
        case INA3221_CH1:
            reg = INA3221_REG_CH1_SHUNTV;
            break;
        case INA3221_CH2:
            reg = INA3221_REG_CH2_SHUNTV;
            break;
        case INA3221_CH3:
            reg = INA3221_REG_CH3_SHUNTV;
            break;
        default:
            break;
    }

    _read(reg, &val_raw);

    // 1 LSB = 5uV
    res = (int16_t)val_raw * 5;

    return res;
}

bool INA3221::getWarnAlertFlag(ina3221_ch_t channel) {
    switch (channel) {
        case INA3221_CH1:
            return _masken_reg.warn_alert_ch1;
        case INA3221_CH2:
            return _masken_reg.warn_alert_ch2;
        case INA3221_CH3:
            return _masken_reg.warn_alert_ch3;
        default:
            return false;
    }
}

bool INA3221::getCritAlertFlag(ina3221_ch_t channel) {
    switch (channel) {
        case INA3221_CH1:
            return _masken_reg.crit_alert_ch1;
        case INA3221_CH2:
            return _masken_reg.crit_alert_ch2;
        case INA3221_CH3:
            return _masken_reg.crit_alert_ch3;
        default:
            return false;
    }
}

int32_t INA3221::estimateOffsetVoltage(ina3221_ch_t channel, uint32_t busV) {
    float bias_in     = 10.0;   // Input bias current at IN– in uA
    float r_in        = 0.670;  // Input resistance at IN– in MOhm
    uint32_t adc_step = 40;     // smallest shunt ADC step in uV
    float shunt_res   = _shuntRes[channel] / 1000.0;  // convert to Ohm
    float filter_res  = _filterRes[channel];
    int32_t offset    = 0.0;
    float reminder;

    offset = (shunt_res + filter_res) * (busV / r_in + bias_in) -
             bias_in * filter_res;

    // Round the offset to the closest shunt ADC value
    reminder = offset % adc_step;
    if (reminder < adc_step / 2) {
        offset -= reminder;
    } else {
        offset += adc_step - reminder;
    }

    return offset;
}

float INA3221::getCurrent(ina3221_ch_t channel) {
    int32_t shunt_uV = 0;
    float current_A  = 0;

    shunt_uV  = getShuntVoltage(channel);
    current_A = shunt_uV / (int32_t)_shuntRes[channel] / 1000.0;
    return current_A;
}

float INA3221::getCurrentCompensated(ina3221_ch_t channel) {
    int32_t shunt_uV  = 0;
    int32_t bus_V     = 0;
    float current_A   = 0.0;
    int32_t offset_uV = 0;

    shunt_uV  = getShuntVoltage(channel);
    bus_V     = getVoltage(channel);
    offset_uV = estimateOffsetVoltage(channel, bus_V);

    current_A = (shunt_uV - offset_uV) / (int32_t)_shuntRes[channel] / 1000.0;

    return current_A;
}

float INA3221::getVoltage(ina3221_ch_t channel) {
    float voltage_V   = 0.0;
    ina3221_reg_t reg = INA3221_REG_CONF;
    uint16_t val_raw  = 0;

    switch (channel) {
        case INA3221_CH1:
            reg = INA3221_REG_CH1_BUSV;
            break;
        case INA3221_CH2:
            reg = INA3221_REG_CH2_BUSV;
            break;
        case INA3221_CH3:
            reg = INA3221_REG_CH3_BUSV;
            break;
        default:
            break;
    }

    _read(reg, &val_raw);

    voltage_V = val_raw / 1000.0;

    return voltage_V;
}
#endif