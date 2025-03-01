#ifndef __AXP2101__
#define __AXP2101__
#include <Arduino.h>

#define AXP2101_SLAVE_ADDRESS         (0x34)

#define AXP2101_CHIP_ID               (0x4A)

#define AXP2101_STATUS1               (0x00)
#define AXP2101_STATUS2               (0x01)
#define AXP2101_IC_TYPE               (0x03)

#define AXP2101_DATA_BUFFER1          (0x04)
#define AXP2101_DATA_BUFFER2          (0x05)
#define AXP2101_DATA_BUFFER3          (0x06)
#define AXP2101_DATA_BUFFER4          (0x07)
#define AXP2101_DATA_BUFFER_SIZE      (4u)

#define AXP2101_COMMON_CONFIG         (0x10)
#define AXP2101_BATFET_CTRL           (0x12)
#define AXP2101_DIE_TEMP_CTRL         (0x13)
#define AXP2101_MIN_SYS_VOL_CTRL      (0x14)
#define AXP2101_INPUT_VOL_LIMIT_CTRL  (0x15)
#define AXP2101_INPUT_CUR_LIMIT_CTRL  (0x16)
#define AXP2101_RESET_FUEL_GAUGE      (0x17)
#define AXP2101_CHARGE_GAUGE_WDT_CTRL (0x18)

#define AXP2101_WDT_CTRL              (0x19)
#define AXP2101_LOW_BAT_WARN_SET      (0x1A)

#define AXP2101_PWRON_STATUS          (0x20)
#define AXP2101_PWROFF_STATUS         (0x21)
#define AXP2101_PWROFF_EN             (0x22)
#define AXP2101_DC_OVP_UVP_CTRL       (0x23)
#define AXP2101_VOFF_SET              (0x24)
#define AXP2101_PWROK_SEQU_CTRL       (0x25)
#define AXP2101_SLEEP_WAKEUP_CTRL     (0x26)
#define AXP2101_IRQ_OFF_ON_LEVEL_CTRL (0x27)

#define AXP2101_FAST_PWRON_SET0       (0x28)
#define AXP2101_FAST_PWRON_SET1       (0x29)
#define AXP2101_FAST_PWRON_SET2       (0x2A)
#define AXP2101_FAST_PWRON_CTRL       (0x2B)

#define AXP2101_ADC_CHANNEL_CTRL      (0x30)
#define AXP2101_ADC_DATA_RELUST0      (0x34)
#define AXP2101_ADC_DATA_RELUST1      (0x35)
#define AXP2101_ADC_DATA_RELUST2      (0x36)
#define AXP2101_ADC_DATA_RELUST3      (0x37)
#define AXP2101_ADC_DATA_RELUST4      (0x38)
#define AXP2101_ADC_DATA_RELUST5      (0x39)
#define AXP2101_ADC_DATA_RELUST6      (0x3A)
#define AXP2101_ADC_DATA_RELUST7      (0x3B)
#define AXP2101_ADC_DATA_RELUST8      (0x3C)
#define AXP2101_ADC_DATA_RELUST9      (0x3D)

// XPOWERS INTERRUPT REGISTER
#define AXP2101_INTEN1                (0x40)
#define AXP2101_INTEN2                (0x41)
#define AXP2101_INTEN3                (0x42)

// XPOWERS INTERRUPT STATUS REGISTER
#define AXP2101_INTSTS1              (0x48)
#define AXP2101_INTSTS2              (0x49)
#define AXP2101_INTSTS3              (0x4A)
#define AXP2101_INTSTS_CNT           (3)

#define AXP2101_TS_PIN_CTRL          (0x50)
#define AXP2101_TS_HYSL2H_SET        (0x52)
#define AXP2101_TS_LYSL2H_SET        (0x53)

#define AXP2101_VLTF_CHG_SET         (0x54)
#define AXP2101_VHLTF_CHG_SET        (0x55)
#define AXP2101_VLTF_WORK_SET        (0x56)
#define AXP2101_VHLTF_WORK_SET       (0x57)

#define AXP2101_JIETA_EN_CTRL        (0x58)
#define AXP2101_JIETA_SET0           (0x59)
#define AXP2101_JIETA_SET1           (0x5A)
#define AXP2101_JIETA_SET2           (0x5B)

#define AXP2101_IPRECHG_SET          (0x61)
#define AXP2101_ICC_CHG_SET          (0x62)
#define AXP2101_ITERM_CHG_SET_CTRL   (0x63)

#define AXP2101_CV_CHG_VOL_SET       (0x64)

#define AXP2101_THE_REGU_THRES_SET   (0x65)
#define AXP2101_CHG_TIMEOUT_SET_CTRL (0x67)

#define AXP2101_BAT_DET_CTRL         (0x68)
#define AXP2101_CHGLED_SET_CTRL      (0x69)

#define AXP2101_BTN_VOL_MIN          (2600)
#define AXP2101_BTN_VOL_MAX          (3300)
#define AXP2101_BTN_VOL_STEPS        (100)

#define AXP2101_BTN_BAT_CHG_VOL_SET  (0x6A)

#define AXP2101_DC_ONOFF_DVM_CTRL    (0x80)
#define AXP2101_DC_FORCE_PWM_CTRL    (0x81)
#define AXP2101_DC_VOL0_CTRL         (0x82)
#define AXP2101_DC_VOL1_CTRL         (0x83)
#define AXP2101_DC_VOL2_CTRL         (0x84)
#define AXP2101_DC_VOL3_CTRL         (0x85)
#define AXP2101_DC_VOL4_CTRL         (0x86)

#define AXP2101_LDO_ONOFF_CTRL0      (0x90)
#define AXP2101_LDO_ONOFF_CTRL1      (0x91)
#define AXP2101_LDO_VOL0_CTRL        (0x92)
#define AXP2101_LDO_VOL1_CTRL        (0x93)
#define AXP2101_LDO_VOL2_CTRL        (0x94)
#define AXP2101_LDO_VOL3_CTRL        (0x95)
#define AXP2101_LDO_VOL4_CTRL        (0x96)
#define AXP2101_LDO_VOL5_CTRL        (0x97)
#define AXP2101_LDO_VOL6_CTRL        (0x98)
#define AXP2101_LDO_VOL7_CTRL        (0x99)
#define AXP2101_LDO_VOL8_CTRL        (0x9A)

#define AXP2101_BAT_PARAME           (0xA1)
#define AXP2101_FUEL_GAUGE_CTRL      (0xA2)
#define AXP2101_BAT_PERCENT_DATA     (0xA4)

// DCDC 1~5
#define AXP2101_DCDC1_VOL_MIN            (1500)
#define AXP2101_DCDC1_VOL_MAX            (3400)
#define AXP2101_DCDC1_VOL_STEPS          (100u)

#define AXP2101_DCDC2_VOL1_MIN           (500u)
#define AXP2101_DCDC2_VOL1_MAX           (1200u)
#define AXP2101_DCDC2_VOL2_MIN           (1220u)
#define AXP2101_DCDC2_VOL2_MAX           (1540u)

#define AXP2101_DCDC2_VOL_STEPS1         (10u)
#define AXP2101_DCDC2_VOL_STEPS2         (20u)

#define AXP2101_DCDC2_VOL_STEPS1_BASE    (0u)
#define AXP2101_DCDC2_VOL_STEPS2_BASE    (71)

#define AXP2101_DCDC3_VOL1_MIN           (500u)
#define AXP2101_DCDC3_VOL1_MAX           (1200u)
#define AXP2101_DCDC3_VOL2_MIN           (1220u)
#define AXP2101_DCDC3_VOL2_MAX           (1540u)
#define AXP2101_DCDC3_VOL3_MIN           (1600u)
#define AXP2101_DCDC3_VOL3_MAX           (3400u)

#define AXP2101_DCDC3_VOL_MIN            (500)
#define AXP2101_DCDC3_VOL_MAX            (3400)

#define AXP2101_DCDC3_VOL_STEPS1         (10u)
#define AXP2101_DCDC3_VOL_STEPS2         (20u)
#define AXP2101_DCDC3_VOL_STEPS3         (100u)

#define AXP2101_DCDC3_VOL_STEPS1_BASE    (0u)
#define AXP2101_DCDC3_VOL_STEPS2_BASE    (71)
#define AXP2101_DCDC3_VOL_STEPS3_BASE    (88)

#define AXP2101_DCDC4_VOL1_MIN           (500u)
#define AXP2101_DCDC4_VOL1_MAX           (1200u)
#define AXP2101_DCDC4_VOL2_MIN           (1220u)
#define AXP2101_DCDC4_VOL2_MAX           (1840u)

#define AXP2101_DCDC4_VOL_STEPS1         (10u)
#define AXP2101_DCDC4_VOL_STEPS2         (20u)

#define AXP2101_DCDC4_VOL_STEPS1_BASE    (0u)
#define AXP2101_DCDC4_VOL_STEPS2_BASE    (71)

#define AXP2101_DCDC5_VOL_1200MV         (1200)
#define AXP2101_DCDC5_VOL_VAL            (0x19)
#define AXP2101_DCDC5_VOL_MIN            (1400)
#define AXP2101_DCDC5_VOL_MAX            (3700)
#define AXP2101_DCDC5_VOL_STEPS          (100u)

#define AXP2101_VSYS_VOL_THRESHOLD_MIN   (2600)
#define AXP2101_VSYS_VOL_THRESHOLD_MAX   (3300)
#define AXP2101_VSYS_VOL_THRESHOLD_STEPS (100)

// ALDO 1~4

#define AXP2101_ALDO1_VOL_MIN   (500)
#define AXP2101_ALDO1_VOL_MAX   (3500)
#define AXP2101_ALDO1_VOL_STEPS (100u)

#define AXP2101_ALDO2_VOL_MIN   (500)
#define AXP2101_ALDO2_VOL_MAX   (3500)
#define AXP2101_ALDO2_VOL_STEPS (100u)

#define AXP2101_ALDO3_VOL_MIN   (500)
#define AXP2101_ALDO3_VOL_MAX   (3500)
#define AXP2101_ALDO3_VOL_STEPS (100u)

#define AXP2101_ALDO4_VOL_MIN   (500)
#define AXP2101_ALDO4_VOL_MAX   (3500)
#define AXP2101_ALDO4_VOL_STEPS (100u)

// BLDO 1~2

#define AXP2101_BLDO1_VOL_MIN   (500)
#define AXP2101_BLDO1_VOL_MAX   (3500)
#define AXP2101_BLDO1_VOL_STEPS (100u)

#define AXP2101_BLDO2_VOL_MIN   (500)
#define AXP2101_BLDO2_VOL_MAX   (3500)
#define AXP2101_BLDO2_VOL_STEPS (100u)

// CPUSLDO

#define AXP2101_CPUSLDO_VOL_MIN   (500)
#define AXP2101_CPUSLDO_VOL_MAX   (1400)
#define AXP2101_CPUSLDO_VOL_STEPS (50)

// DLDO 1~2
#define AXP2101_DLDO1_VOL_MIN   (500)
#define AXP2101_DLDO1_VOL_MAX   (3400)
#define AXP2101_DLDO1_VOL_STEPS (100u)

#define AXP2101_DLDO2_VOL_MIN   (500)
#define AXP2101_DLDO2_VOL_MAX   (3400)
#define AXP2101_DLDO2_VOL_STEPS (100u)

#define AXP2101_CONVERSION(raw) (22.0 + (7274 - raw) / 20.0)

enum pmic_chg_vol : uint8_t
{
    AXP2101_CHG_VOL_4V = 1,
    AXP2101_CHG_VOL_4V1,
    AXP2101_CHG_VOL_4V2,
    AXP2101_CHG_VOL_4V35,
    AXP2101_CHG_VOL_4V4,
    AXP2101_CHG_VOL_MAX
};

enum pmic_irq_time : uint8_t
{
    AXP2101_IRQ_TIME_1S,
    AXP2101_IRQ_TIME_1S5,
    AXP2101_IRQ_TIME_2S,
    AXP2101_PRESSOFF_2S5,
};

enum pmic_prechg
{
    AXP2101_PRECHARGE_0MA,
    AXP2101_PRECHARGE_25MA,
    AXP2101_PRECHARGE_50MA,
    AXP2101_PRECHARGE_75MA,
    AXP2101_PRECHARGE_100MA,
    AXP2101_PRECHARGE_125MA,
    AXP2101_PRECHARGE_150MA,
    AXP2101_PRECHARGE_175MA,
    AXP2101_PRECHARGE_200MA,
};

enum pmic_chg_iterm : uint8_t
{
    AXP2101_CHG_ITERM_0MA,
    AXP2101_CHG_ITERM_25MA,
    AXP2101_CHG_ITERM_50MA,
    AXP2101_CHG_ITERM_75MA,
    AXP2101_CHG_ITERM_100MA,
    AXP2101_CHG_ITERM_125MA,
    AXP2101_CHG_ITERM_150MA,
    AXP2101_CHG_ITERM_175MA,
    AXP2101_CHG_ITERM_200MA,
};

enum pmic_vbus_cur_limit : uint8_t
{
    AXP2101_VBUS_CUR_LIM_100MA,
    AXP2101_VBUS_CUR_LIM_500MA,
    AXP2101_VBUS_CUR_LIM_900MA,
    AXP2101_VBUS_CUR_LIM_1000MA,
    AXP2101_VBUS_CUR_LIM_1500MA,
    AXP2101_VBUS_CUR_LIM_2000MA,
};

enum pmic_sys_vol_min
{
    AXP2101_SYS_VOL_MIN_4V1,
    AXP2101_SYS_VOL_MIN_4V2,
    AXP2101_SYS_VOL_MIN_4V3,
    AXP2101_SYS_VOL_MIN_4V4,
    AXP2101_SYS_VOL_MIN_4V5,
    AXP2101_SYS_VOL_MIN_4V6,
    AXP2101_SYS_VOL_MIN_4V7, // Default
    AXP2101_SYS_VOL_MIN_4V8,
};

enum pmic_vbus_vol_limit
{
    AXP2101_VBUS_VOL_LIM_3V88,
    AXP2101_VBUS_VOL_LIM_3V96,
    AXP2101_VBUS_VOL_LIM_4V04,
    AXP2101_VBUS_VOL_LIM_4V12,
    AXP2101_VBUS_VOL_LIM_4V20,
    AXP2101_VBUS_VOL_LIM_4V28,
    AXP2101_VBUS_VOL_LIM_4V36,
    AXP2101_VBUS_VOL_LIM_4V44,
    AXP2101_VBUS_VOL_LIM_4V52,
    AXP2101_VBUS_VOL_LIM_4V60,
    AXP2101_VBUS_VOL_LIM_4V68,
    AXP2101_VBUS_VOL_LIM_4V76,
    AXP2101_VBUS_VOL_LIM_4V84,
    AXP2101_VBUS_VOL_LIM_4V92,
    AXP2101_VBUS_VOL_LIM_5V,
    AXP2101_VBUS_VOL_LIM_5V08,
};

enum pmic_thermal : uint8_t
{
    AXP2101_THREMAL_60DEG,
    AXP2101_THREMAL_80DEG,
    AXP2101_THREMAL_100DEG,
    AXP2101_THREMAL_120DEG,
};

enum pmic_chg_status
{
    AXP2101_CHG_TRI_STATE,  // tri_charge
    AXP2101_CHG_PRE_STATE,  // pre_charge
    AXP2101_CHG_CC_STATE,   // constant charge
    AXP2101_CHG_CV_STATE,   // constant voltage
    AXP2101_CHG_DONE_STATE, // charge done
    AXP2101_CHG_STOP_STATE, // not charge
};

enum pmic_wakeup
{
    AXP2101_WAKEUP_IRQ_PIN_TO_LOW = _BV(4),
    AXP2101_WAKEUP_PWROK_TO_LOW   = _BV(3),
    AXP2101_WAKEUP_DC_DLO_SELECT  = _BV(2),
};

enum pmic_fast_on_opt
{
    AXP2101_FAST_DCDC1,
    AXP2101_FAST_DCDC2,
    AXP2101_FAST_DCDC3,
    AXP2101_FAST_DCDC4,
    AXP2101_FAST_DCDC5,
    AXP2101_FAST_ALDO1,
    AXP2101_FAST_ALDO2,
    AXP2101_FAST_ALDO3,
    AXP2101_FAST_ALDO4,
    AXP2101_FAST_BLDO1,
    AXP2101_FAST_BLDO2,
    AXP2101_FAST_CPUSLDO,
    AXP2101_FAST_DLDO1,
    AXP2101_FAST_DLDO2,
};

enum pmic_start_sequence : uint8_t
{
    AXP2101_SEQUENCE_LEVEL_0,
    AXP2101_SEQUENCE_LEVEL_1,
    AXP2101_SEQUENCE_LEVEL_2,
    AXP2101_SEQUENCE_DISABLE,
};

enum pmic_wdt_config : uint8_t
{
    AXP2101_WDT_IRQ_TO_PIN,            // Just interrupt to pin
    AXP2101_WDT_IRQ_AND_RSET,          // IRQ to pin and reset pmu system
    AXP2101_WDT_IRQ_AND_RSET_PD_PWROK, // IRQ to pin and reset pmu system,pull down pwrok
    AXP2101_WDT_IRQ_AND_RSET_ALL_OFF,  // IRQ to pin and reset pmu system,turn off dcdc & ldo ,pull down pwrok
};

enum pmic_wdt_timeout : uint8_t
{
    AXP2101_WDT_TIMEOUT_1S,
    AXP2101_WDT_TIMEOUT_2S,
    AXP2101_WDT_TIMEOUT_4S,
    AXP2101_WDT_TIMEOUT_8S,
    AXP2101_WDT_TIMEOUT_16S,
    AXP2101_WDT_TIMEOUT_32S,
    AXP2101_WDT_TIMEOUT_64S,
    AXP2101_WDT_TIMEOUT_128S,
};

enum pmic_chg_dpm
{
    AXP2101_VSYS_VOL_4V1,
    AXP2101_VSYS_VOL_4V2,
    AXP2101_VSYS_VOL_4V3,
    AXP2101_VSYS_VOL_4V4,
    AXP2101_VSYS_VOL_4V5,
    AXP2101_VSYS_VOL_4V6,
    AXP2101_VSYS_VOL_4V7,
    AXP2101_VSYS_VOL_4V8,
};

enum pmic_power_on_source
{
    AXP2101_POWERON_SRC_POWERON_LOW, // POWERON low for on level when POWERON Mode as POWERON Source
    AXP2101_POWERON_SRC_IRQ_LOW,     // IRQ PIN Pull-down as POWERON Source
    AXP2101_POWERON_SRC_VBUS_INSERT, // Vbus Insert and Good as POWERON Source
    AXP2101_POWERON_SRC_BAT_CHARGE,  // Vbus Insert and Good as POWERON Source
    AXP2101_POWERON_SRC_BAT_INSERT,  // Battery Insert and Good as POWERON Source
    AXP2101_POWERON_SRC_ENMODE,      // POWERON always high when EN Mode as POWERON Source
    AXP2101_POWERON_SRC_UNKONW,      // Unkonw
};

enum pmic_power_off_source
{
    AXP2101_POWEROFF_SRC_PWEKEY_PULLDOWN, // POWERON Pull down for off level when POWERON Mode as POWEROFF Source
    AXP2101_POWEROFF_SRC_SOFT_OFF,        // Software configuration as POWEROFF Source
    AXP2101_POWEROFF_SRC_PWEKEY_LOW,      // POWERON always low when EN Mode as POWEROFF Source
    AXP2101_POWEROFF_SRC_UNDER_VSYS,      // Vsys Under Voltage as POWEROFF Source
    AXP2101_POWEROFF_SRC_OVER_VBUS,       // VBUS Over Voltage as POWEROFF Source
    AXP2101_POWEROFF_SRC_UNDER_VOL,       // DCDC Under Voltage as POWEROFF Source
    AXP2101_POWEROFF_SRC_OVER_VOL,        // DCDC Over Voltage as POWEROFF Source
    AXP2101_POWEROFF_SRC_OVER_TEMP,       // Die Over Temperature as POWEROFF Source
    AXP2101_POWEROFF_SRC_UNKONW,          // Unkonw
};

enum pmic_pwrok_delay : uint8_t
{
    AXP2101_PWROK_DELAY_8MS,
    AXP2101_PWROK_DELAY_16MS,
    AXP2101_PWROK_DELAY_32MS,
    AXP2101_PWROK_DELAY_64MS,
};

/**
 * @brief axp2101 charge currnet voltage parameters.
 */
enum pmic_chg_curr
{
    AXP2101_CHG_CUR_0MA,
    AXP2101_CHG_CUR_100MA = 4,
    AXP2101_CHG_CUR_125MA,
    AXP2101_CHG_CUR_150MA,
    AXP2101_CHG_CUR_175MA,
    AXP2101_CHG_CUR_200MA,
    AXP2101_CHG_CUR_300MA,
    AXP2101_CHG_CUR_400MA,
    AXP2101_CHG_CUR_500MA,
    AXP2101_CHG_CUR_600MA,
    AXP2101_CHG_CUR_700MA,
    AXP2101_CHG_CUR_800MA,
    AXP2101_CHG_CUR_900MA,
    AXP2101_CHG_CUR_1000MA,
};

enum DVMRamp : uint8_t
{
    AXP2101_DVM_RAMP_15_625US,
    AXP2101_DVM_RAMP_31_250US,
};

/**
 * @brief axp2101 interrupt control mask parameters.
 */
enum pmic_irq : uint_fast64_t
{
    //! IRQ1 REG 40H
    AXP2101_BAT_NOR_UNDER_TEMP_IRQ = _BV(0), // Battery Under Temperature in Work
    AXP2101_BAT_NOR_OVER_TEMP_IRQ  = _BV(1), // Battery Over Temperature in Work mode
    AXP2101_BAT_CHG_UNDER_TEMP_IRQ = _BV(2), // Battery Under Temperature in Charge mode IRQ(bcut_irq)
    AXP2101_BAT_CHG_OVER_TEMP_IRQ  = _BV(3), // Battery Over Temperature in Charge mode IRQ(bcot_irq) enable
    AXP2101_GAUGE_NEW_SOC_IRQ      = _BV(4), // Gauge New SOC IRQ(lowsoc_irq) enable ???
    AXP2101_WDT_TIMEOUT_IRQ        = _BV(5), // Gauge Watchdog Timeout IRQ(gwdt_irq) enable
    AXP2101_WARNING_LEVEL1_IRQ     = _BV(6), // SOC drop to Warning Level1 IRQ(socwl1_irq) enable
    AXP2101_WARNING_LEVEL2_IRQ     = _BV(7), // SOC drop to Warning Level2 IRQ(socwl2_irq) enable

    //! IRQ2 REG 41H
    AXP2101_PKEY_POSITIVE_IRQ = _BV(8),  // POWERON Positive Edge IRQ(ponpe_irq_en) enable
    AXP2101_PKEY_NEGATIVE_IRQ = _BV(9),  // POWERON Negative Edge IRQ(ponne_irq_en) enable
    AXP2101_PKEY_LONG_IRQ     = _BV(10), // POWERON Long PRESS IRQ(ponlp_irq) enable
    AXP2101_PKEY_SHORT_IRQ    = _BV(11), // POWERON Short PRESS IRQ(ponsp_irq_en) enable
    AXP2101_BAT_REMOVE_IRQ    = _BV(12), // Battery Remove IRQ(bremove_irq) enable
    AXP2101_BAT_INSERT_IRQ    = _BV(13), // Battery Insert IRQ(binsert_irq) enabl
    AXP2101_VBUS_REMOVE_IRQ   = _BV(14), // VBUS Remove IRQ(vremove_irq) enabl
    AXP2101_VBUS_INSERT_IRQ   = _BV(15), // VBUS Insert IRQ(vinsert_irq) enable

    //! IRQ3 REG 42H
    AXP2101_BAT_OVER_VOL_IRQ     = _BV(16), // Battery Over Voltage Protection IRQ(bovp_irq) enable
    AXP2101_CHAGER_TIMER_IRQ     = _BV(17), // Charger Safety Timer1/2 expire IRQ(chgte_irq) enable
    AXP2101_DIE_OVER_TEMP_IRQ    = _BV(18), // DIE Over Temperature level1 IRQ(dotl1_irq) enable
    AXP2101_BAT_CHG_START_IRQ    = _BV(19), // Charger start IRQ(chgst_irq) enable
    AXP2101_BAT_CHG_DONE_IRQ     = _BV(20), // Battery charge done IRQ(chgdn_irq) enable
    AXP2101_BATFET_OVER_CURR_IRQ = _BV(21), // BATFET Over Current Protection IRQ(bocp_irq) enable
    AXP2101_LDO_OVER_CURR_IRQ    = _BV(22), // LDO Over Current IRQ(ldooc_irq) enable
    AXP2101_WDT_EXPIRE_IRQ       = _BV(23), // Watchdog Expire IRQ(wdexp_irq) enable

    AXP2101_ALL_IRQ = (0xFFFFFFFFUL)
};

#endif /* __AXP2101__ */
