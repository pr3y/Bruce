#if defined (CORE2)
#ifndef __AXP2101_H
#define __AXP2101_H

#include <Arduino.h>
#include <Wire.h>

#define AXP2101_ADDR 0x34

#define AXP2101_SLAVE_ADDRESS           0x34
#define AXP2101_DCDC1_VOLTAGE_REG       0x82
#define AXP2101_DCDC3_VOLTAGE_REG       0x84
#define AXP2101_DCDC_CTRL_REG           0x80
#define AXP2101_LDO_CTRL_REG            0x90
#define AXP2101_BLDO1_VOLTAGE_REG       0x96
#define AXP2101_BLDO2_VOLTAGE_REG       0x97
#define AXP2101_CHGLED_REG              0x69
#define AXP2101_CHARGER_SETTING_REG     0x63
#define AXP2101_ICC_CHARGER_SETTING_REG 0x62
#define AXP2101_ALDO3_VOLTAGE_REG       0x94
#define AXP2101_ALDO2_VOLTAGE_REG       0x93
#define AXP2101_ALDO4_VOLTAGE_REG       0x95
#define AXP2101_DLDO1_VOLTAGE_REG       0x99
#define AXP2101_PMU_CONFIG_REG          0x10
#define AXP2101_ADC_ENABLE_REG          0x30

#define AXP2101_DLDO1_CTRL_MASK   1 << 7
#define AXP2101_CPUSLDO_CTRL_MASK 1 << 6
#define AXP2101_BLDO2_CTRL_MASK   1 << 5
#define AXP2101_BLDO1_CTRL_MASK   1 << 4
#define AXP2101_ALDO4_CTRL_MASK   1 << 3
#define AXP2101_ALDO3_CTRL_MASK   1 << 2
#define AXP2101_ALDO2_CTRL_MASK   1 << 1
#define AXP2101_ALDO1_CTRL_MASK   1 << 0

#define AXP2101_DCDC5_CTRL_MASK 1 << 4
#define AXP2101_DCDC4_CTRL_MASK 1 << 3
#define AXP2101_DCDC3_CTRL_MASK 1 << 2
#define AXP2101_DCDC2_CTRL_MASK 1 << 1
#define AXP2101_DCDC1_CTRL_MASK 1 << 0

class AXP2101 {
   private:
    uint8_t _addr;
    TwoWire* _wire;
    uint8_t _scl;
    uint8_t _sda;
    uint8_t _speed;

   public:
    bool begin(TwoWire* wire = &Wire, uint8_t addr = AXP2101_ADDR,
               uint8_t sda = 21, uint8_t scl = 22, uint32_t speed = 400000L);

    bool readRegister(uint8_t addr, uint8_t reg, uint8_t* result,
                      uint16_t length, uint32_t freq);
    uint8_t readRegister8(uint8_t addr, uint8_t reg, uint32_t freq);
    bool writeRegister8(uint8_t addr, uint8_t reg, uint8_t data, uint32_t freq);
    bool bitOn(uint8_t addr, uint8_t reg, uint8_t data, uint32_t freq);
    bool bitOff(uint8_t addr, uint8_t reg, uint8_t data, uint32_t freq);
    void set_bus_3v3(uint16_t voltage);
    void set_lcd_back_light_voltage(uint16_t voltage);
    void set_bus_5v(uint8_t sw);
    bool set_sys_led(bool sw);
    void set_spk(bool sw);
    void set_lcd_rst(bool sw);
    void set_lcd_and_tf_voltage(uint16_t voltage);
    void set_vib_motor_voltage(uint16_t voltage);
    void set_bat_charge(bool enable);
    void power_off(void);
    bool set_charger_term_current_to_zero(void);
    bool set_charger_constant_current_to_50mA(void);

    bool axp2101_enable_pwrok_resets(void);
    uint8_t axp2101_get_dcdc_status(void);
    bool axp2101_set_bldo1_voltage(uint16_t voltage);
    uint8_t axp2101_get_bldo1_voltage(void);
    bool axp2101_set_bldo2_voltage(uint16_t voltage);
    uint8_t axp2101_get_bldo2_voltage(void);
    bool axp2101_set_dcdc1_voltage(uint16_t voltage);
    uint8_t axp2101_get_dcdc1_voltage(void);
    bool axp2101_set_dcdc3_voltage(uint16_t voltage);
    uint8_t axp2101_get_dcdc3_voltage(void);
    bool axp2101_set_aldo3_voltage(uint16_t voltage);
    uint8_t axp2101_get_aldo3_voltage(void);
    bool axp2101_set_aldo2_voltage(uint16_t voltage);
    uint8_t axp2101_get_aldo2_voltage(void);
    bool axp2101_set_aldo4_voltage(uint16_t voltage);
    uint8_t axp2101_get_aldo4_voltage(void);
    bool axp2101_set_dldo1_voltage(uint16_t voltage);
    uint8_t axp2101_get_dldo1_voltage(void);
    bool set_dldo1_on_off(bool sw);
    bool set_cpusldo_on_off(bool sw);
    bool set_blod2_on_off(bool sw);
    bool set_bldo1_on_off(bool sw);
    bool set_aldo4_on_off(bool sw);
    bool set_aldo3_on_off(bool sw);
    bool set_aldo2_on_off(bool sw);
    bool set_aldo1_on_off(bool sw);
    bool set_dcdc1_on_off(bool sw);
    bool set_dcdc2_on_off(bool sw);
    bool set_dcdc3_on_off(bool sw);
};

#endif
#endif