#ifndef __AXP202__
#define __AXP202__
#include <Arduino.h>

#define AXP202_SLAVE_ADDRESS    (0x35)

#define AXP202_CHIP_ID          (0x41)

#define AXP202_STATUS           (0x00)
#define AXP202_MODE_CHGSTATUS   (0x01)
#define AXP202_OTG_STATUS       (0x02)
#define AXP202_IC_TYPE          (0x03)

#define AXP202_DATA_BUFFER1     (0x04)
#define AXP202_DATA_BUFFER2     (0x05)
#define AXP202_DATA_BUFFER3     (0x06)
#define AXP202_DATA_BUFFER4     (0x07)
#define AXP202_DATA_BUFFER5     (0x08)
#define AXP202_DATA_BUFFER6     (0x09)
#define AXP202_DATA_BUFFER7     (0x0A)
#define AXP202_DATA_BUFFER8     (0x0B)
#define AXP202_DATA_BUFFER9     (0x0C)
#define AXP202_DATA_BUFFERA     (0x0D)
#define AXP202_DATA_BUFFERB     (0x0E)
#define AXP202_DATA_BUFFERC     (0x0F)

#define AXP202_LDO234_DC23_CTL  (0x12)
#define AXP202_DC2OUT_VOL       (0x23)
#define AXP202_LDO3_DC2_DVM     (0x25)
#define AXP202_DC3OUT_VOL       (0x27)
#define AXP202_LDO24OUT_VOL     (0x28)
#define AXP202_LDO3OUT_VOL      (0x29)
#define AXP202_IPS_SET          (0x30)
#define AXP202_VOFF_SET         (0x31)
#define AXP202_OFF_CTL          (0x32)
#define AXP202_CHARGE1          (0x33)
#define AXP202_CHARGE2          (0x34)
#define AXP202_BACKUP_CHG       (0x35)
#define AXP202_POK_SET          (0x36)
#define AXP202_DCDC_FREQSET     (0x37)
#define AXP202_VLTF_CHGSET      (0x38)
#define AXP202_VHTF_CHGSET      (0x39)
#define AXP202_APS_WARNING1     (0x3A)
#define AXP202_APS_WARNING2     (0x3B)
#define AXP202_TLTF_DISCHGSET   (0x3C)
#define AXP202_THTF_DISCHGSET   (0x3D)
#define AXP202_DCDC_MODESET     (0x80)
#define AXP202_ADC_EN1          (0x82)
#define AXP202_ADC_EN2          (0x83)
#define AXP202_ADC_SPEED        (0x84)
#define AXP202_ADC_INPUTRANGE   (0x85)
#define AXP202_ADC_IRQ_RETFSET  (0x86)
#define AXP202_ADC_IRQ_FETFSET  (0x87)
#define AXP202_TIMER_CTL        (0x8A)
#define AXP202_VBUS_DET_SRP     (0x8B)
#define AXP202_HOTOVER_CTL      (0x8F)

#define AXP202_DATA_BUFFER_SIZE (12)
#define AXP202_GPIO0_CTL        (0x90)
#define AXP202_GPIO0_VOL        (0x91)
#define AXP202_GPIO1_CTL        (0x92)
#define AXP202_GPIO2_CTL        (0x93)
#define AXP202_GPIO012_SIGNAL   (0x94)
#define AXP202_GPIO3_CTL        (0x95)

// INTERRUPT REGISTER
#define AXP202_INTEN1 (0x40)
#define AXP202_INTEN2 (0x41)
#define AXP202_INTEN3 (0x42)
#define AXP202_INTEN4 (0x43)
#define AXP202_INTEN5 (0x44)

// INTERRUPT STATUS REGISTER
#define AXP202_INTSTS1    (0x48)
#define AXP202_INTSTS2    (0x49)
#define AXP202_INTSTS3    (0x4A)
#define AXP202_INTSTS4    (0x4B)
#define AXP202_INTSTS5    (0x4C)
#define AXP202_INTSTS_CNT (5)

// AXP ADC DATA REGISTER
#define AXP202_GPIO0_VOL_ADC_H8        (0x64)
#define AXP202_GPIO0_VOL_ADC_L4        (0x65)
#define AXP202_GPIO1_VOL_ADC_H8        (0x66)
#define AXP202_GPIO1_VOL_ADC_L4        (0x67)

#define AXP202_GPIO0_STEP              (0.5F)
#define AXP202_GPIO1_STEP              (0.5F)

#define AXP202_BAT_AVERVOL_H8          (0x78)
#define AXP202_BAT_AVERVOL_L4          (0x79)

#define AXP202_BAT_AVERCHGCUR_H8       (0x7A)
#define AXP202_BAT_AVERCHGCUR_L4       (0x7B)

#define AXP202_BAT_AVERCHGCUR_L5       (0x7B)
#define AXP202_ACIN_VOL_H8             (0x56)
#define AXP202_ACIN_VOL_L4             (0x57)
#define AXP202_ACIN_CUR_H8             (0x58)
#define AXP202_ACIN_CUR_L4             (0x59)
#define AXP202_VBUS_VOL_H8             (0x5A)
#define AXP202_VBUS_VOL_L4             (0x5B)
#define AXP202_VBUS_CUR_H8             (0x5C)
#define AXP202_VBUS_CUR_L4             (0x5D)
#define AXP202_INTERNAL_TEMP_H8        (0x5E)
#define AXP202_INTERNAL_TEMP_L4        (0x5F)
#define AXP202_TS_IN_H8                (0x62)
#define AXP202_TS_IN_L4                (0x63)
#define AXP202_GPIO0_VOL_ADC_H8        (0x64)
#define AXP202_GPIO0_VOL_ADC_L4        (0x65)
#define AXP202_GPIO1_VOL_ADC_H8        (0x66)
#define AXP202_GPIO1_VOL_ADC_L4        (0x67)

#define AXP202_BAT_AVERDISCHGCUR_H8    (0x7C)
#define AXP202_BAT_AVERDISCHGCUR_L5    (0x7D)
#define AXP202_APS_AVERVOL_H8          (0x7E)
#define AXP202_APS_AVERVOL_L4          (0x7F)
#define AXP202_INT_BAT_CHGCUR_H8       (0xA0)
#define AXP202_INT_BAT_CHGCUR_L4       (0xA1)
#define AXP202_EXT_BAT_CHGCUR_H8       (0xA2)
#define AXP202_EXT_BAT_CHGCUR_L4       (0xA3)
#define AXP202_INT_BAT_DISCHGCUR_H8    (0xA4)
#define AXP202_INT_BAT_DISCHGCUR_L4    (0xA5)
#define AXP202_EXT_BAT_DISCHGCUR_H8    (0xA6)
#define AXP202_EXT_BAT_DISCHGCUR_L4    (0xA7)
#define AXP202_BAT_CHGCOULOMB3         (0xB0)
#define AXP202_BAT_CHGCOULOMB2         (0xB1)
#define AXP202_BAT_CHGCOULOMB1         (0xB2)
#define AXP202_BAT_CHGCOULOMB0         (0xB3)
#define AXP202_BAT_DISCHGCOULOMB3      (0xB4)
#define AXP202_BAT_DISCHGCOULOMB2      (0xB5)
#define AXP202_BAT_DISCHGCOULOMB1      (0xB6)
#define AXP202_BAT_DISCHGCOULOMB0      (0xB7)
#define AXP202_COULOMB_CTL             (0xB8)
#define AXP202_BATT_PERCENTAGE         (0xB9)

#define AXP202_BAT_POWERH8             (0x70)
#define AXP202_BAT_POWERM8             (0x71)
#define AXP202_BAT_POWERL8             (0x72)

#define AXP202_BATT_VOLTAGE_STEP       (1.1F)
#define AXP202_BATT_DISCHARGE_CUR_STEP (0.5F)
#define AXP202_BATT_CHARGE_CUR_STEP    (0.5F)
#define AXP202_ACIN_VOLTAGE_STEP       (1.7F)
#define AXP202_ACIN_CUR_STEP           (0.625F)
#define AXP202_VBUS_VOLTAGE_STEP       (1.7F)
#define AXP202_VBUS_CUR_STEP           (0.375F)
#define AXP202_INTERNAL_TEMP_STEP      (0.1F)
#define AXP202_APS_VOLTAGE_STEP        (1.4F)
#define AXP202_TS_PIN_OUT_STEP         (0.8F)

#define AXP202_LDO2_VOL_MIN            (1800u)
#define AXP202_LDO2_VOL_MAX            (3300u)
#define AXP202_LDO2_VOL_STEPS          (100u)
#define AXP202_LDO2_VOL_BIT_MASK       (4u)

#define AXP202_LDO3_VOL_MIN            (1800u)
#define AXP202_LDO3_VOL_MAX            (3300u)
#define AXP202_LDO3_VOL_STEPS          (100u)

#define AXP202_DC2_VOL_STEPS           (25u)
#define AXP202_DC2_VOL_MIN             (700u)
#define AXP202_DC2_VOL_MAX             (3500u)

#define AXP202_DC3_VOL_STEPS           (25u)
#define AXP202_DC3_VOL_MIN             (700u)
#define AXP202_DC3_VOL_MAX             (3500u)

#define AXP202_LDOIO_VOL_STEPS         (100)
#define AXP202_LDOIO_VOL_MIN           (1800)
#define AXP202_LDOIO_VOL_MAX           (3300)

#define AXP202_SYS_VOL_STEPS           (100)
#define AXP202_VOFF_VOL_MIN            (2600)
#define AXP202_VOFF_VOL_MAX            (3300)

#define AXP202_CHG_EXT_CURR_MIN        (300)
#define AXP202_CHG_EXT_CURR_MAX        (1000)
#define AXP202_CHG_EXT_CURR_STEP       (100)

#define AXP202_INERNAL_TEMP_OFFSET     (144.7)

const enum pmic_boot_time
{
    AXP202_BOOT_TIME_128MS,
    AXP202_BOOT_TIME_512MS,
    AXP202_BOOT_TIME_1S,
    AXP202_BOOT_TIME_2S,
};

enum pmic_chg_iterm
{
    AXP202_CHG_ITERM_LESS_10_PERCENT,
    AXP202_CHG_ITERM_LESS_15_PERCENT,
};

const enum pmic_prechg_to
{
    AXP202_PRECHG_TIMEOUT_30MIN,
    AXP202_PRECHG_TIMEOUT_40MIN,
    AXP202_PRECHG_TIMEOUT_50MIN,
    AXP202_PRECHG_TIMEOUT_60MIN,
};

const enum pmic_pekey_poweroff_arg
{
    AXP202_POWEROFF_4S,
    AXP202_POWEROFF_65,
    AXP202_POWEROFF_8S,
    AXP202_POWEROFF_10S,
};

const enum pmic_pekey_long_press
{
    AXP202_LONGPRESS_1000MS,
    AXP202_LONGPRESS_1500MS,
    AXP202_LONGPRESS_2000MS,
    AXP202_LONGPRESS_2500MS,
};

const enum pmic_chg_cons_to
{
    AXP202_CHG_CONS_TIMEOUT_7H,
    AXP202_CHG_CONS_TIMEOUT_8H,
    AXP202_CHG_CONS_TIMEOUT_9H,
    AXP202_CHG_CONS_TIMEOUT_10H,
};

const enum pmic_backup_batt_vol
{
    AXP202_BACKUP_BAT_VOL_3V1,
    AXP202_BACKUP_BAT_VOL_3V,
    AXP202_BACKUP_BAT_VOL_3V0, //! NEED FIX,DATASHEET ERROR!
    AXP202_BACKUP_BAT_VOL_2V5,
};

const enum pmic_backup_batt_curr
{
    AXP202_BACKUP_BAT_CUR_50UA,
    AXP202_BACKUP_BAT_CUR_100UA,
    AXP202_BACKUP_BAT_CUR_200UA,
    AXP202_BACKUP_BAT_CUR_400UA,
};

/**
 * @brief axp202 charge target voltage parameters.
 */
const enum pmic_chg_vol
{
    AXP202_CHG_VOL_4V1,
    AXP202_CHG_VOL_4V15,
    AXP202_CHG_VOL_4V2,
    AXP202_CHG_VOL_4V36,
    AXP202_CHG_VOL_MAX,
};

/**
 * @brief axp202 charge currnet voltage parameters.
 */
const enum pmic_chg_curr
{
    AXP202_CHG_CUR_100MA,
    AXP202_CHG_CUR_190MA,
    AXP202_CHG_CUR_280MA,
    AXP202_CHG_CUR_360MA,
    AXP202_CHG_CUR_450MA,
    AXP202_CHG_CUR_550MA,
    AXP202_CHG_CUR_630MA,
    AXP202_CHG_CUR_700MA,
    AXP202_CHG_CUR_780MA,
    AXP202_CHG_CUR_880MA,
    AXP202_CHG_CUR_960MA,
    AXP202_CHG_CUR_1000MA,
    AXP202_CHG_CUR_1080MA,
    AXP202_CHG_CUR_1160MA,
    AXP202_CHG_CUR_1240MA,
    AXP202_CHG_CUR_1320MA,
};

const enum pmic_vbus_vol_limit
{
    AXP202_VBUS_VOL_LIM_4V,
    AXP202_VBUS_VOL_LIM_4V1,
    AXP202_VBUS_VOL_LIM_4V2,
    AXP202_VBUS_VOL_LIM_4V3,
    AXP202_VBUS_VOL_LIM_4V4,
    AXP202_VBUS_VOL_LIM_4V5,
    AXP202_VBUS_VOL_LIM_4V6,
    AXP202_VBUS_VOL_LIM_4V7,
};

/**
 * @brief axp202 vbus currnet limit parameters.
 */
enum pmic_vbus_cur_limit : uint8_t
{
    AXP202_VBUS_CUR_LIM_900MA,
    AXP202_VBUS_CUR_LIM_500MA,
    AXP202_VBUS_CUR_LIM_100MA,
    AXP202_VBUS_CUR_LIM_OFF,
};

/**
 * @brief axp202 interrupt control mask parameters.
 */
enum pmic_irq
{
    //! IRQ1 REG 40H
    AXP202_VBUS_VHOLD_LOW_IRQ = _BV(1), // VBUS is available, but lower than V HOLD, IRQ enable
    AXP202_VBUS_REMOVE_IRQ    = _BV(2), // VBUS removed, IRQ enable
    AXP202_VBUS_INSERT_IRQ    = _BV(3), // VBUS connected, IRQ enable
    AXP202_VBUS_OVER_VOL_IRQ  = _BV(4), // VBUS over-voltage, IRQ enable
    AXP202_ACIN_REMOVED_IRQ   = _BV(5), // ACIN removed, IRQ enable
    AXP202_ACIN_CONNECT_IRQ   = _BV(6), // ACIN connected, IRQ enable
    AXP202_ACIN_OVER_VOL_IRQ  = _BV(7), // ACIN over-voltage, IRQ enable

    //! IRQ2 REG 41H
    AXP202_BATT_LOW_TEMP_IRQ      = _BV(8),  // Battery low-temperature, IRQ enable
    AXP202_BATT_OVER_TEMP_IRQ     = _BV(9),  // Battery over-temperature, IRQ enable
    AXP202_BAT_CHG_DONE_IRQ       = _BV(10), // Charge finished, IRQ enable
    AXP202_BAT_CHG_START_IRQ      = _BV(11), // Be charging, IRQ enable
    AXP202_BATT_EXIT_ACTIVATE_IRQ = _BV(12), // Exit battery activate mode, IRQ enable
    AXP202_BATT_ACTIVATE_IRQ      = _BV(13), // Battery activate mode, IRQ enable
    AXP202_BAT_REMOVE_IRQ         = _BV(14), // Battery removed, IRQ enable
    AXP202_BAT_INSERT_IRQ         = _BV(15), // Battery connected, IRQ enable

    //! IRQ3 REG 42H
    AXP202_PKEY_LONG_IRQ    = _BV(16), // PEK long press, IRQ enable
    AXP202_PKEY_SHORT_IRQ   = _BV(17), // PEK short press, IRQ enable
    AXP202_LDO3_LOW_VOL_IRQ = _BV(18), // LDO3output voltage is lower than the set value, IRQ enable
    AXP202_DC3_LOW_VOL_IRQ  = _BV(19), // DC-DC3output voltage is lower than the set value, IRQ enable
    AXP202_DC2_LOW_VOL_IRQ  = _BV(20), // DC-DC2 output voltage is lower than the set value, IRQ enable
    //**Reserved and unchangeable BIT 5
    AXP202_CHARGE_LOW_CUR_IRQ = _BV(22), // Charge current is lower than the set current, IRQ enable
    AXP202_CHIP_TEMP_HIGH_IRQ = _BV(23), // AXP202 internal over-temperature, IRQ enable

    //! IRQ4 REG 43H
    AXP202_APS_LOW_VOL_LEVEL2_IRQ = _BV(24), // APS low-voltage, IRQ enable（LEVEL2）
    APX202_APS_LOW_VOL_LEVEL1_IRQ = _BV(25), // APS low-voltage, IRQ enable（LEVEL1）
    AXP202_VBUS_SESSION_END_IRQ   = _BV(26), // VBUS Session End IRQ enable
    AXP202_VBUS_SESSION_AB_IRQ    = _BV(27), // VBUS Session A/B IRQ enable
    AXP202_VBUS_INVALID_IRQ       = _BV(28), // VBUS invalid, IRQ enable
    AXP202_VBUS_VAILD_IRQ         = _BV(29), // VBUS valid, IRQ enable
    AXP202_NOE_OFF_IRQ            = _BV(30), // N_OE shutdown, IRQ enable
    AXP202_NOE_ON_IRQ             = _BV(31), // N_OE startup, IRQ enable

    //! IRQ5 REG 44H
    AXP202_GPIO0_EDGE_TRIGGER_IRQ = _BV(32), // G PIO0 input edge trigger, IRQ enable
    AXP202_GPIO1_EDGE_TRIGGER_IRQ = _BV(33), // GPIO1input edge trigger or ADC input, IRQ enable
    AXP202_GPIO2_EDGE_TRIGGER_IRQ = _BV(34), // GPIO2input edge trigger, IRQ enable
    AXP202_GPIO3_EDGE_TRIGGER_IRQ = _BV(35), // GPIO3 input edge trigger, IRQ enable
    //**Reserved and unchangeable BIT 4
    AXP202_PKEY_NEGATIVE_IRQ = _BV(37), // PEK press falling edge, IRQ enable
    AXP202_PKEY_POSITIVE_IRQ = _BV(38), // PEK press rising edge, IRQ enable
    AXP202_TIMER_TIMEOUT_IRQ = _BV(39), // Timer timeout, IRQ enable

    AXP202_ALL_IRQ = (0xFFFFFFFFFFULL)
};

#endif /* __AXP202__ */
