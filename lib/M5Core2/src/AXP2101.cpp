#if defined (CORE2)
#include "AXP2101.h"

bool AXP2101::begin(TwoWire* wire, uint8_t addr, uint8_t sda, uint8_t scl,
                    uint32_t speed) {
    _wire  = wire;
    _addr  = addr;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl, _speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

void AXP2101::set_bus_3v3(uint16_t voltage) {
    if (!voltage) {
        set_dcdc1_on_off(false);
        set_dcdc3_on_off(false);
    } else {
        set_dcdc1_on_off(true);
        set_dcdc3_on_off(true);
        axp2101_set_dcdc1_voltage(voltage);
        axp2101_set_dcdc3_voltage(voltage);
    }
}

void AXP2101::set_lcd_back_light_voltage(uint16_t voltage) {
    if (!voltage) {
        set_bldo1_on_off(false);
    } else {
        set_bldo1_on_off(true);
        axp2101_set_bldo1_voltage(voltage);
    }
}

void AXP2101::set_bus_5v(uint8_t sw) {
    if (sw) {
        set_blod2_on_off(true);
        axp2101_set_bldo2_voltage(3300);
    } else {
        set_blod2_on_off(false);
    }
}

bool AXP2101::set_sys_led(bool sw) {
    bool result = false;

    if (sw) {
        result = bitOn(AXP2101_ADDR, AXP2101_CHGLED_REG, 0b00110000, _speed);
    } else {
        result = bitOff(AXP2101_ADDR, AXP2101_CHGLED_REG, 0b00110000, _speed);
    }

    return result;
}

bool AXP2101::set_charger_term_current_to_zero(void) {
    bool result = false;

    result =
        bitOff(AXP2101_ADDR, AXP2101_CHARGER_SETTING_REG, 0b00001111, _speed);

    return result;
}

bool AXP2101::set_charger_constant_current_to_50mA(void) {
    bool result = false;

    result = writeRegister8(AXP2101_ADDR, AXP2101_ICC_CHARGER_SETTING_REG, 2,
                            _speed);

    return result;
}

void AXP2101::set_spk(bool sw) {
    if (sw) {
        set_aldo3_on_off(true);
        axp2101_set_aldo3_voltage(3300);
    } else {
        set_aldo3_on_off(false);
    }
}

void AXP2101::set_lcd_rst(bool sw) {
    if (sw) {
        set_aldo2_on_off(true);
        axp2101_set_aldo2_voltage(3300);
    } else {
        set_aldo2_on_off(false);
    }
}

void AXP2101::set_lcd_and_tf_voltage(uint16_t voltage) {
    if (!voltage) {
        set_aldo4_on_off(false);
    } else {
        set_aldo4_on_off(true);
        axp2101_set_aldo4_voltage(voltage);
    }
}

void AXP2101::set_vib_motor_voltage(uint16_t voltage) {
    if (!voltage) {
        set_dldo1_on_off(false);
    } else {
        set_dldo1_on_off(true);
        axp2101_set_dldo1_voltage(voltage);
    }
}

bool AXP2101::axp2101_set_dldo1_voltage(uint16_t voltage) {
    uint16_t temp;

    temp = voltage;
    if (temp < 500) temp = 500;
    if (temp > 3400) temp = 3400;
    if (writeRegister8(AXP2101_ADDR, AXP2101_DLDO1_VOLTAGE_REG,
                       (temp - 500) / 100, _speed)) {
        return true;
    } else {
        return false;
    }
}

bool AXP2101::axp2101_set_aldo3_voltage(uint16_t voltage) {
    uint16_t temp;

    temp = voltage;
    if (temp < 500) temp = 500;
    // AXP2101 BLDO1 max voltage value is 3500mV
    if (temp > 3500) temp = 3500;
    if (writeRegister8(AXP2101_ADDR, AXP2101_ALDO3_VOLTAGE_REG,
                       (temp - 500) / 100, _speed)) {
        return true;
    } else {
        return false;
    }
}

void AXP2101::set_bat_charge(bool enable) {
    uint8_t val = 0;
    if (readRegister(AXP2101_ADDR, 0x18, &val, 1, _speed)) {
        writeRegister8(AXP2101_ADDR, 0x18, (val & 0xFD) + (enable << 1),
                       _speed);
    }
}

bool AXP2101::axp2101_enable_pwrok_resets(void) {
    return (bitOn(AXP2101_ADDR, 0x10, 1 << 3, _speed));
}

void AXP2101::power_off(void) {
    // 1. AXP2101 Power off
    bitOn(AXP2101_ADDR, 0x41, 1 << 1,
          400000);  // POWERON Negative Edge IRQ(ponne_irq_en) enable
    writeRegister8(AXP2101_ADDR, 0x25, 0b00011011,
                   400000);  // sleep and wait for wakeup
    delay(100);
    writeRegister8(AXP2101_ADDR, 0x10, 0b00110001, 400000);  // power off
}

bool AXP2101::readRegister(uint8_t addr, uint8_t reg, uint8_t* result,
                           uint16_t length, uint32_t freq) {
    uint8_t index = 0;
    uint8_t err   = 0;

    _wire->beginTransmission(addr);
    _wire->write(reg);
    err = _wire->endTransmission();
    _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        result[index++] = _wire->read();
    }
    if (err == 0) {
        return true;
    } else {
        return false;
    }
}

uint8_t AXP2101::readRegister8(uint8_t addr, uint8_t reg, uint32_t freq) {
    uint8_t result;

    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(addr, 1);
    result = _wire->read();
    return result;
}

bool AXP2101::writeRegister8(uint8_t addr, uint8_t reg, uint8_t data,
                             uint32_t freq) {
    uint8_t result;

    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->write(data);
    result = _wire->endTransmission();
    if (result == 0) {
        return true;
    } else {
        return false;
    }
}

bool AXP2101::bitOn(uint8_t addr, uint8_t reg, uint8_t data, uint32_t freq) {
    uint8_t temp;
    uint8_t write_back;

    temp       = readRegister8(addr, reg, freq);
    write_back = (temp | data);
    // Serial.printf("biton read 0x%X, data = 0x%X, write back 0x%X\r\n", temp,
    // data, write_back);
    return (writeRegister8(addr, reg, write_back, freq));
}

bool AXP2101::bitOff(uint8_t addr, uint8_t reg, uint8_t data, uint32_t freq) {
    uint8_t temp;
    uint8_t write_back;

    temp       = readRegister8(addr, reg, freq);
    write_back = (temp & (~data));
    return (writeRegister8(addr, reg, write_back, freq));
}

uint8_t AXP2101::axp2101_get_dcdc_status(void) {
    return readRegister8(_addr, AXP2101_DCDC_CTRL_REG, _speed);
}

bool AXP2101::axp2101_set_bldo1_voltage(uint16_t voltage) {
    uint16_t temp;

    temp = voltage;
    if (temp < 500) temp = 500;
    // AXP2101 BLDO1 max voltage value is 3500mV
    if (temp > 3500) temp = 3500;
    if (writeRegister8(AXP2101_ADDR, AXP2101_BLDO1_VOLTAGE_REG,
                       (temp - 500) / 100, _speed)) {
        return true;
    } else {
        return false;
    }
}

uint8_t AXP2101::axp2101_get_bldo1_voltage(void) {
    return readRegister8(_addr, AXP2101_BLDO1_VOLTAGE_REG, _speed);
}

bool AXP2101::axp2101_set_bldo2_voltage(uint16_t voltage) {
    uint16_t temp;

    temp = voltage;
    if (temp < 500) temp = 500;
    // AXP2101 BLDO1 max voltage value is 3500mV
    if (temp > 3500) temp = 3500;
    if (writeRegister8(AXP2101_ADDR, AXP2101_BLDO2_VOLTAGE_REG,
                       (temp - 500) / 100, _speed)) {
        return true;
    } else {
        return false;
    }
}

uint8_t AXP2101::axp2101_get_bldo2_voltage(void) {
    return readRegister8(_addr, AXP2101_BLDO2_VOLTAGE_REG, _speed);
}

bool AXP2101::set_dldo1_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_DLDO1_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_DLDO1_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_cpusldo_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result = bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_CPUSLDO_CTRL_MASK,
                       _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_CPUSLDO_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_blod2_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_BLDO2_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_BLDO2_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_bldo1_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_BLDO1_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_BLDO1_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_aldo4_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO4_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO4_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_aldo3_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO3_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO3_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_aldo2_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO2_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO2_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_aldo1_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result =
            bitOn(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO1_CTRL_MASK, _speed);
    } else {
        result = bitOff(_addr, AXP2101_LDO_CTRL_REG, AXP2101_ALDO1_CTRL_MASK,
                        _speed);
    }

    return result;
}

bool AXP2101::set_dcdc1_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result = bitOn(AXP2101_ADDR, AXP2101_DCDC_CTRL_REG,
                       AXP2101_DCDC1_CTRL_MASK, _speed);
    } else {
        result = bitOff(AXP2101_ADDR, AXP2101_DCDC_CTRL_REG,
                        AXP2101_DCDC1_CTRL_MASK, _speed);
    }

    return result;
}

bool AXP2101::set_dcdc2_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result = bitOn(AXP2101_ADDR, AXP2101_DCDC_CTRL_REG,
                       AXP2101_DCDC2_CTRL_MASK, _speed);
    } else {
        result = bitOff(AXP2101_ADDR, AXP2101_DCDC_CTRL_REG,
                        AXP2101_DCDC2_CTRL_MASK, _speed);
    }

    return result;
}

bool AXP2101::set_dcdc3_on_off(bool sw) {
    bool result = false;

    if (sw) {
        result = bitOn(AXP2101_ADDR, AXP2101_DCDC_CTRL_REG,
                       AXP2101_DCDC3_CTRL_MASK, _speed);
    } else {
        result = bitOff(AXP2101_ADDR, AXP2101_DCDC_CTRL_REG,
                        AXP2101_DCDC3_CTRL_MASK, _speed);
    }

    return result;
}

bool AXP2101::axp2101_set_aldo4_voltage(uint16_t voltage) {
    uint16_t temp;

    temp = voltage;
    if (temp < 500) temp = 500;
    // AXP2101 BLDO1 max voltage value is 3500mV
    if (temp > 3500) temp = 3500;
    if (writeRegister8(AXP2101_ADDR, AXP2101_ALDO4_VOLTAGE_REG,
                       (temp - 500) / 100, _speed)) {
        return true;
    } else {
        return false;
    }
}

bool AXP2101::axp2101_set_dcdc1_voltage(uint16_t voltage) {
    uint16_t temp;

    temp = voltage;
    if (temp < 1500) temp = 1500;
    // AXP2101 DCDC1 max voltage value is 3400mV
    if (temp > 3400) temp = 3400;
    if (writeRegister8(AXP2101_ADDR, AXP2101_DCDC1_VOLTAGE_REG,
                       (temp - 1500) / 100, _speed)) {
        return true;
    } else {
        return false;
    }
}

bool AXP2101::axp2101_set_dcdc3_voltage(uint16_t voltage) {
    uint16_t temp;
    uint8_t data;

    temp = voltage;
    if (temp < 1500)
        temp = 1500;
    else if (temp > 1540 && temp < 1600)
        temp = 1540;
    else if (temp > 3400)
        temp = 3400;

    if (temp <= 1540)
        data = ((temp - 1220) / 20 + (uint8_t)0b01000111);
    else
        data = ((temp - 1600) / 100 + (uint8_t)0b01011000);
    if (writeRegister8(AXP2101_ADDR, AXP2101_DCDC3_VOLTAGE_REG, data, _speed)) {
        return true;
    } else {
        return false;
    }
}
#endif