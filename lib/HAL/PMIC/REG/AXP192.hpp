#ifndef __AXP192__
#define __AXP192__
#include <Arduino.h>

#define AXP192_SLAVE_ADDRESS                            (0x34)

#define AXP192_CHIP_ID                          (0x03)

#define AXP192_STATUS                           (0x00)
#define AXP192_MODE_CHGSTATUS                   (0x01)
#define AXP192_OTG_STATUS                       (0x02)
#define AXP192_IC_TYPE                          (0x03)

#define AXP192_DATA_BUFFER1                     (0x06)
#define AXP192_DATA_BUFFER2                     (0x07)
#define AXP192_DATA_BUFFER3                     (0x08)
#define AXP192_DATA_BUFFER4                     (0x09)
#define AXP192_DATA_BUFFER5                     (0x0A)
#define AXP192_DATA_BUFFER6                     (0x0B)
#define AXP192_DATA_BUFFER_SIZE                 (6)


#define AXP192_LDO23_DC123_EXT_CTL              (0x12)
#define AXP192_DC2OUT_VOL                       (0x23)
#define AXP192_DC2_DVM                          (0x25)
#define AXP192_DC3OUT_VOL                       (0x27)
#define AXP192_LDO24OUT_VOL                     (0x28)
#define AXP192_LDO3OUT_VOL                      (0x29)
#define AXP192_IPS_SET                          (0x30)
#define AXP192_VOFF_SET                         (0x31)
#define AXP192_OFF_CTL                          (0x32)
#define AXP192_CHARGE1                          (0x33)
#define AXP192_CHARGE2                          (0x34)
#define AXP192_BACKUP_CHG                       (0x35)
#define AXP192_POK_SET                          (0x36)
#define AXP192_DCDC_FREQSET                     (0x37)
#define AXP192_VLTF_CHGSET                      (0x38)
#define AXP192_VHTF_CHGSET                      (0x39)
#define AXP192_APS_WARNING1                     (0x3A)
#define AXP192_APS_WARNING2                     (0x3B)
#define AXP192_TLTF_DISCHGSET                   (0x3C)
#define AXP192_THTF_DISCHGSET                   (0x3D)
#define AXP192_DCDC_MODESET                     (0x80)
#define AXP192_ADC_EN1                          (0x82)
#define AXP192_ADC_EN2                          (0x83)
#define AXP192_ADC_SPEED                        (0x84)
#define AXP192_ADC_INPUTRANGE                   (0x85)
#define AXP192_ADC_IRQ_RETFSET                  (0x86)
#define AXP192_ADC_IRQ_FETFSET                  (0x87)
#define AXP192_TIMER_CTL                        (0x8A)
#define AXP192_VBUS_DET_SRP                     (0x8B)
#define AXP192_HOTOVER_CTL                      (0x8F)

#define AXP192_PWM1_FREQ_SET                    (0x98)
#define AXP192_PWM1_DUTY_SET1                   (0x99)
#define AXP192_PWM1_DUTY_SET2                   (0x9A)

#define AXP192_PWM2_FREQ_SET                    (0x9B)
#define AXP192_PWM2_DUTY_SET1                   (0x9C)
#define AXP192_PWM2_DUTY_SET2                   (0x9D)


// INTERRUPT REGISTER
#define AXP192_INTEN1                           (0x40)
#define AXP192_INTEN2                           (0x41)
#define AXP192_INTEN3                           (0x42)
#define AXP192_INTEN4                           (0x43)
#define AXP192_INTEN5                           (0x4A)

// INTERRUPT STATUS REGISTER
#define AXP192_INTSTS1                          (0x44)
#define AXP192_INTSTS2                          (0x45)
#define AXP192_INTSTS3                          (0x46)
#define AXP192_INTSTS4                          (0x47)
#define AXP192_INTSTS5                          (0x4D)
#define AXP192_INTSTS_CNT                       (5)

#define AXP192_DC1_VLOTAGE                      (0x26)
#define AXP192_LDO23OUT_VOL                     (0x28)
#define AXP192_GPIO0_CTL                        (0x90)
#define AXP192_GPIO0_VOL                        (0x91)
#define AXP192_GPIO1_CTL                        (0X92)
#define AXP192_GPIO2_CTL                        (0x93)
#define AXP192_GPIO012_SIGNAL                   (0x94)
#define AXP192_GPIO34_CTL                       (0x95)
#define AXP192_GPIO34_SIGNAL                    (0x96)
#define AXP192_GPIO012_PULLDOWN                 (0x97)
#define AXP192_GPIO5_CTL                        (0x9E)
#define AXP192_GPIO_CNT                         (6)

#define AXP192_GPIO0_VOL_ADC_H8                 (0x64)
#define AXP192_GPIO0_VOL_ADC_L4                 (0x65)
#define AXP192_GPIO1_VOL_ADC_H8                 (0x66)
#define AXP192_GPIO1_VOL_ADC_L4                 (0x67)
#define AXP192_GPIO2_VOL_ADC_H8                 (0x68)
#define AXP192_GPIO2_VOL_ADC_L4                 (0x69)
#define AXP192_GPIO3_VOL_ADC_H8                 (0x6A)
#define AXP192_GPIO3_VOL_ADC_L4                 (0x6B)

#define AXP192_GPIO0_STEP                       (0.5F)
#define AXP192_GPIO1_STEP                       (0.5F)
#define AXP192_TS_IN_H8                         (0x62)
#define AXP192_TS_IN_L4                         (0x63)

#define AXP192_BAT_AVERCHGCUR_H8                (0x7A)
#define AXP192_BAT_AVERCHGCUR_L5                (0x7B)


#define AXP192_ACIN_VOL_H8                      (0x56)
#define AXP192_ACIN_VOL_L4                      (0x57)
#define AXP192_ACIN_CUR_H8                      (0x58)
#define AXP192_ACIN_CUR_L4                      (0x59)
#define AXP192_VBUS_VOL_H8                      (0x5A)
#define AXP192_VBUS_VOL_L4                      (0x5B)
#define AXP192_VBUS_CUR_H8                      (0x5C)
#define AXP192_VBUS_CUR_L4                      (0x5D)

#define AXP192_BAT_AVERDISCHGCUR_H8             (0x7C)
#define AXP192_BAT_AVERDISCHGCUR_L5             (0x7D)
#define AXP192_APS_AVERVOL_H8                   (0x7E)
#define AXP192_APS_AVERVOL_L4                   (0x7F)
#define AXP192_BAT_AVERVOL_H8                   (0x78)
#define AXP192_BAT_AVERVOL_L4                   (0x79)

#define AXP192_BAT_CHGCOULOMB3                  (0xB0)
#define AXP192_BAT_CHGCOULOMB2                  (0xB1)
#define AXP192_BAT_CHGCOULOMB1                  (0xB2)
#define AXP192_BAT_CHGCOULOMB0                  (0xB3)
#define AXP192_BAT_DISCHGCOULOMB3               (0xB4)
#define AXP192_BAT_DISCHGCOULOMB2               (0xB5)
#define AXP192_BAT_DISCHGCOULOMB1               (0xB6)
#define AXP192_BAT_DISCHGCOULOMB0               (0xB7)
#define AXP192_COULOMB_CTL                      (0xB8)


#define AXP192_BATT_VOLTAGE_STEP                (1.1F)
#define AXP192_BATT_DISCHARGE_CUR_STEP          (0.5F)
#define AXP192_BATT_CHARGE_CUR_STEP             (0.5F)
#define AXP192_ACIN_VOLTAGE_STEP                (1.7F)
#define AXP192_ACIN_CUR_STEP                    (0.625F)
#define AXP192_VBUS_VOLTAGE_STEP                (1.7F)
#define AXP192_VBUS_CUR_STEP                    (0.375F)
#define AXP192_APS_VOLTAGE_STEP                 (1.4F)
#define AXP192_TS_PIN_OUT_STEP                  (0.8F)


#define AXP192_LDO2_VOL_MIN                     (1800u)
#define AXP192_LDO2_VOL_MAX                     (3300u)
#define AXP192_LDO2_VOL_STEPS                   (100u)
#define AXP192_LDO2_VOL_BIT_MASK                (4u)

#define AXP192_LDO3_VOL_MIN                     (1800u)
#define AXP192_LDO3_VOL_MAX                     (3300u)
#define AXP192_LDO3_VOL_STEPS                   (100u)


#define AXP192_DC1_VOL_STEPS                    (25u)
#define AXP192_DC1_VOL_MIN                      (700u)
#define AXP192_DC1_VOL_MAX                      (3500u)

#define AXP192_DC2_VOL_STEPS                    (25u)
#define AXP192_DC2_VOL_MIN                      (700u)
#define AXP192_DC2_VOL_MAX                      (3500u)

#define AXP192_DC3_VOL_STEPS                    (25u)
#define AXP192_DC3_VOL_MIN                      (700u)
#define AXP192_DC3_VOL_MAX                      (3500u)

#define AXP192_LDOIO_VOL_STEPS                  (100)
#define AXP192_LDOIO_VOL_MIN                    (1800)
#define AXP192_LDOIO_VOL_MAX                    (3300)

#define AXP192_SYS_VOL_STEPS                    (100)
#define AXP192_VOFF_VOL_MIN                     (2600)
#define AXP192_VOFF_VOL_MAX                     (3300)

#define AXP192_CHG_EXT_CURR_MIN                 (300)
#define AXP192_CHG_EXT_CURR_MAX                 (1000)
#define AXP192_CHG_EXT_CURR_STEP                (100)


#define AXP192_INTERNAL_TEMP_H8                 (0x5E)
#define AXP192_INTERNAL_TEMP_L4                 (0x5F)
#define AXP192_INTERNAL_TEMP_STEP               (0.1F)
#define AXP192_INERNAL_TEMP_OFFSET              (144.7)

enum pmic_boot_time {
    AXP192_BOOT_TIME_128MS,
    AXP192_BOOT_TIME_512MS,
    AXP192_BOOT_TIME_1S,
    AXP192_BOOT_TIME_2S,
};

enum pmic_chg_iterm {
    AXP192_CHG_ITERM_LESS_10_PERCENT,
    AXP192_CHG_ITERM_LESS_15_PERCENT,
};

enum pmic_prechg_to {
    AXP192_PRECHG_TIMEOUT_30MIN,
    AXP192_PRECHG_TIMEOUT_40MIN,
    AXP192_PRECHG_TIMEOUT_50MIN,
    AXP192_PRECHG_TIMEOUT_60MIN,
};

enum pmic_pekey_poweroff_arg {
    AXP192_POWEROFF_4S,
    AXP192_POWEROFF_65,
    AXP192_POWEROFF_8S,
    AXP192_POWEROFF_10S,
};

enum pmic_pekey_long_press {
    AXP192_LONGPRESS_1000MS,
    AXP192_LONGPRESS_1500MS,
    AXP192_LONGPRESS_2000MS,
    AXP192_LONGPRESS_2500MS,
};

enum pmic_chg_cons_to {
    AXP192_CHG_CONS_TIMEOUT_7H,
    AXP192_CHG_CONS_TIMEOUT_8H,
    AXP192_CHG_CONS_TIMEOUT_9H,
    AXP192_CHG_CONS_TIMEOUT_10H,
};


enum pmic_backup_batt_vol {
    AXP192_BACKUP_BAT_VOL_3V1,
    AXP192_BACKUP_BAT_VOL_3V,
    AXP192_BACKUP_BAT_VOL_3V0, //!NEED FIX,DATASHEET ERROR!
    AXP192_BACKUP_BAT_VOL_2V5,
};

enum pmic_backup_batt_curr {
    AXP192_BACKUP_BAT_CUR_50UA,
    AXP192_BACKUP_BAT_CUR_100UA,
    AXP192_BACKUP_BAT_CUR_200UA,
    AXP192_BACKUP_BAT_CUR_400UA,
};

struct pmic_gpio{
    uint8_t mode;
};

/**
 * @brief axp192 charge target voltage parameters.
 */
enum pmic_chg_vol {
    AXP192_CHG_VOL_4V1,
    AXP192_CHG_VOL_4V15,
    AXP192_CHG_VOL_4V2,
    AXP192_CHG_VOL_4V36,
    AXP192_CHG_VOL_MAX,
};

/**
 * @brief axp192 charge currnet voltage parameters.
 */
enum pmic_chg_curr {
    AXP192_CHG_CUR_100MA,
    AXP192_CHG_CUR_190MA,
    AXP192_CHG_CUR_280MA,
    AXP192_CHG_CUR_360MA,
    AXP192_CHG_CUR_450MA,
    AXP192_CHG_CUR_550MA,
    AXP192_CHG_CUR_630MA,
    AXP192_CHG_CUR_700MA,
    AXP192_CHG_CUR_780MA,
    AXP192_CHG_CUR_880MA,
    AXP192_CHG_CUR_960MA,
    AXP192_CHG_CUR_1000MA,
    AXP192_CHG_CUR_1080MA,
    AXP192_CHG_CUR_1160MA,
    AXP192_CHG_CUR_1240MA,
    AXP192_CHG_CUR_1320MA,
};

/**
 * @brief axp192 vbus currnet limit parameters.
 */
enum pmic_vbus_cur_limit {
    AXP192_VBUS_CUR_LIM_500MA,
    AXP192_VBUS_CUR_LIM_100MA,
    AXP192_VBUS_CUR_LIM_OFF,
};
enum pmic_vbus_vol_limit{
    AXP192_VBUS_VOL_LIM_4V,
    AXP192_VBUS_VOL_LIM_4V1,
    AXP192_VBUS_VOL_LIM_4V2,
    AXP192_VBUS_VOL_LIM_4V3,
    AXP192_VBUS_VOL_LIM_4V4,
    AXP192_VBUS_VOL_LIM_4V5,
    AXP192_VBUS_VOL_LIM_4V6,
    AXP192_VBUS_VOL_LIM_4V7,
};

enum pmic_irq
{
    //! IRQ1 REG 40H
    AXP192_VBUS_VHOLD_LOW_IRQ = _BV(1), // VBUS is available, but lower than V HOLD, IRQ enable
    AXP192_VBUS_REMOVE_IRQ = _BV(2),    // VBUS removed, IRQ enable
    AXP192_VBUS_INSERT_IRQ = _BV(3),    // VBUS connected, IRQ enable
    AXP192_VBUS_OVER_VOL_IRQ = _BV(4),  // VBUS over-voltage, IRQ enable
    AXP192_ACIN_REMOVED_IRQ = _BV(5),   // ACIN removed, IRQ enable
    AXP192_ACIN_CONNECT_IRQ = _BV(6),   // ACIN connected, IRQ enable
    AXP192_ACIN_OVER_VOL_IRQ = _BV(7),  // ACIN over-voltage, IRQ enable

    //! IRQ2 REG 41H
    AXP192_BATT_LOW_TEMP_IRQ = _BV(8),       // Battery low-temperature, IRQ enable
    AXP192_BATT_OVER_TEMP_IRQ = _BV(9),      // Battery over-temperature, IRQ enable
    AXP192_BAT_CHG_DONE_IRQ = _BV(10),       // Charge finished, IRQ enable
    AXP192_BAT_CHG_START_IRQ = _BV(11),      // Be charging, IRQ enable
    AXP192_BATT_EXIT_ACTIVATE_IRQ = _BV(12), // Exit battery activate mode, IRQ enable
    AXP192_BATT_ACTIVATE_IRQ = _BV(13),      // Battery activate mode, IRQ enable
    AXP192_BAT_REMOVE_IRQ = _BV(14),         // Battery removed, IRQ enable
    AXP192_BAT_INSERT_IRQ = _BV(15),         // Battery connected, IRQ enable

    //! IRQ3 REG 42H
    AXP192_PKEY_LONG_IRQ = _BV(16),  // PEK long press, IRQ enable
    AXP192_PKEY_SHORT_IRQ = _BV(17), // PEK short press, IRQ enable
    //**Reserved and unchangeable BIT 2
    AXP192_DC3_LOW_VOL_IRQ = _BV(19),    // DC-DC3output voltage is lower than the set value, IRQ enable
    AXP192_DC2_LOW_VOL_IRQ = _BV(20),    // DC-DC2 output voltage is lower than the set value, IRQ enable
    AXP192_DC1_LOW_VOL_IRQ = _BV(21),    // DC-DC1 output voltage is lower than the set value, IRQ enable
    AXP192_CHARGE_LOW_CUR_IRQ = _BV(22), // Charge current is lower than the set current, IRQ enable
    AXP192_CHIP_TEMP_HIGH_IRQ = _BV(23), // PMIC internal over-temperature, IRQ enable

    //! IRQ4 REG 43H
    AXP192_APS_LOW_VOL_LEVEL_IRQ = _BV(24), // APS low-voltage, IRQ enable
    //**Reserved and unchangeable BIT 1
    AXP192_VBUS_SESSION_END_IRQ = _BV(26), // VBUS Session End IRQ enable
    AXP192_VBUS_SESSION_AB_IRQ = _BV(27),  // VBUS Session A/B IRQ enable
    AXP192_VBUS_INVALID_IRQ = _BV(28),     // VBUS invalid, IRQ enable
    AXP192_VBUS_VAILD_IRQ = _BV(29),       // VBUS valid, IRQ enable
    AXP192_NOE_OFF_IRQ = _BV(30),          // N_OE shutdown, IRQ enable
    AXP192_NOE_ON_IRQ = _BV(31),           // N_OE startup, IRQ enable

    //! IRQ5 REG 4AH
    AXP192_GPIO0_EDGE_TRIGGER_IRQ = _BV(32), // GPIO0 input edge trigger, IRQ enable
    AXP192_GPIO1_EDGE_TRIGGER_IRQ = _BV(33), // GPIO1input edge trigger or ADC input, IRQ enable
    AXP192_GPIO2_EDGE_TRIGGER_IRQ = _BV(34), // GPIO2input edge trigger, IRQ enable
    //**Reserved and unchangeable BIT 3
    //**Reserved and unchangeable BIT 4
    //**Reserved and unchangeable BIT 5
    //**Reserved and unchangeable BIT 6
    AXP192_TIMER_TIMEOUT_IRQ = _BV(39), // Timer timeout, IRQ enable

    AXP192_ALL_IRQ = (0xFFFFFFFFFFULL)
};

enum pmic_num {
    PMU_GPIO0,
    PMU_GPIO1,
    PMU_GPIO2,
    PMU_GPIO3,
    PMU_GPIO4,
    PMU_GPIO5,
    PMU_TS_PIN
};

enum pmic_adc_func {
    MONITOR_TS_PIN = _BV(0),
    MONITOR_APS_VOLTAGE = _BV(1),
    MONITOR_USB_CURRENT = _BV(2),
    MONITOR_USB_VOLTAGE = _BV(3),
    MONITOR_AC_CURRENT = _BV(4),
    MONITOR_AC_VOLTAGE = _BV(5),
    MONITOR_BAT_CURRENT = _BV(6),
    MONITOR_BAT_VOLTAGE = _BV(7),
    MONITOR_ADC_IO3 = _BV(8),
    MONITOR_ADC_IO2 = _BV(9),
    MONITOR_ADC_IO1 = _BV(10),
    MONITOR_ADC_IO0 = _BV(11),
    MONITOR_TEMPERATURE = _BV(16),
};


#endif  /* __AXP192__ */
