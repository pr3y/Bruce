#ifndef __BQ25896__
#define __BQ25896__

#define BQ25896_SLAVE_ADDRESS                                   (0x6B)

#define BQ25896_DEV_REV                                         (0x02)

#define POWERS_BQ25896_VBUS_MASK_VAL(val)                        (val & 0x7F)
#define POWERS_BQ25896_VBAT_MASK_VAL(val)                        (val & 0x7F)
#define POWERS_BQ25896_VSYS_MASK_VAL(val)                        (val & 0x7F)
#define POWERS_BQ25896_NTC_MASK_VAL(val)                         (val & 0x7F)

#define POWERS_BQ25896_VBUS_BASE_VAL                             (2600)
#define POWERS_BQ25896_VBAT_BASE_VAL                             (2304)
#define POWERS_BQ25896_VSYS_BASE_VAL                             (2304)
#define POWERS_BQ25896_NTC_BASE_VAL                              (21)

#define POWERS_BQ25896_VBUS_VOL_STEP                             (100)
#define POWERS_BQ25896_VBAT_VOL_STEP                             (20)
#define POWERS_BQ25896_VSYS_VOL_STEP                             (20)
#define POWERS_BQ25896_NTC_VOL_STEP                              (0.465)

#define POWERS_BQ25896_CHG_STEP_VAL                              (50)
#define POWERS_BQ25896_FAST_CHG_CUR_STEP                         (64)
#define POWERS_BQ25896_FAST_CHG_CURRENT_MIN                     (0)
#define POWERS_BQ25896_FAST_CHG_CURRENT_MAX                     (3008)

#define POWERS_BQ25896_PRE_CHG_CUR_BASE                          (64)
#define POWERS_BQ25896_PRE_CHG_CUR_STEP                          (64)
#define POWERS_BQ25896_PRE_CHG_CURRENT_MIN                      (64)
#define POWERS_BQ25896_PRE_CHG_CURRENT_MAX                      (1024)

#define POWERS_BQ25896_TERM_CHG_CUR_BASE                          (64)
#define POWERS_BQ25896_TERM_CHG_CUR_STEP                          (64)
#define POWERS_BQ25896_TERM_CHG_CURRENT_MIN                      (64)
#define POWERS_BQ25896_TERM_CHG_CURRENT_MAX                      (1024)

#define POWERS_BQ25896_CHG_VOL_BASE                              (3840)
#define POWERS_BQ25896_CHG_VOL_STEP                              (16)
#define POWERS_BQ25896_FAST_CHG_VOL_MIN                          (3840)
#define POWERS_BQ25896_FAST_CHG_VOL_MAX                          (4608)

#define POWERS_BQ25896_SYS_VOL_STEPS                             (100)
#define POWERS_BQ25896_SYS_VOFF_VOL_MIN                          (3000)
#define POWERS_BQ25896_SYS_VOFF_VOL_MAX                          (3700)

#define POWERS_BQ25896_IN_CURRENT_STEP                           (50)
#define POWERS_BQ25896_IN_CURRENT_MIN                            (100)
#define POWERS_BQ25896_IN_CURRENT_MAX                            (3250)

#define POWERS_BQ25896_IN_CURRENT_OPT_STEP                       (50)
#define POWERS_BQ25896_IN_CURRENT_OPT_MIN                        (100)
#define POWERS_BQ25896_IN_CURRENT_OPT_MAX                        (3250)

#define POWERS_BQ25896_IN_CURRENT_OFFSET_STEP                    (100)
#define POWERS_BQ25896_IN_CURRENT_OFFSET_MAX                     (3100)


#define POWERS_BQ25896_BOOTS_VOL_BASE                            (4550)
#define POWERS_BQ25896_BOOTS_VOL_STEP                            (64)
#define POWERS_BQ25896_BOOST_VOL_MIN                             (4550)
#define POWERS_BQ25896_BOOST_VOL_MAX                             (5510)

#define POWERS_BQ25896_IRQ_WTD_FAULT(x)                          (bool)(( x & 0xFF ) >> 7)
#define POWERS_BQ25896_IRQ_BOOST_FAULT(x)                        (bool)(( x & 0xFF ) >> 6)
#define POWERS_BQ25896_IRQ_CHG_FAULT(x)                          (( x >> 4 ) & 0x03)
#define POWERS_BQ25896_IRQ_BAT_FAULT(x)                          (bool)(( x & 0xFF ) >> 3)
#define POWERS_BQ25896_IRQ_NTC_FAULT(x)                          (bool)(( x & 0xFF ) & 0x03)

#define POWERS_BQ25896_VINDPM_VOL_BASE                           (4550)
#define POWERS_BQ25896_VINDPM_VOL_STEPS                          (100)
#define POWERS_BQ25896_VINDPM_VOL_MIN                            (3900)
#define POWERS_BQ25896_VINDPM_VOL_MAX                            (15300)

#define POWERS_BQ25896_BAT_COMP_STEPS                          (20)
#define POWERS_BQ25896_BAT_COMP_MAX                            (140)

#define POWERS_BQ25896_VCLAMP_STEPS                             (32)
#define POWERS_BQ25896_VCLAMP_MAX                               (224)

enum BusStatus
{
    BUS_STATE_NOINPUT,
    BUS_STATE_USB_SDP,
    BUS_STATE_ADAPTER,
    BUS_STATE_OTG
};

enum ExitBoostModeVolt
{
    MINI_VOLT_2V9,
    MINI_VOLT_2V5
};

enum FastChargeThreshold
{
    FAST_CHG_THR_2V8,
    FAST_CHG_THR_3V0
};

enum RechargeThresholdOffset
{
    RECHARGE_OFFSET_100MV,
    RECHARGE_OFFSET_200MV
};

enum BoostCurrentLimit
{
    BOOST_CUR_LIMIT_500MA,
    BOOST_CUR_LIMIT_750MA,
    BOOST_CUR_LIMIT_1200MA,
    BOOST_CUR_LIMIT_1400MA,
    BOOST_CUR_LIMIT_1650MA,
    BOOST_CUR_LIMIT_1875MA,
    BOOST_CUR_LIMIT_2150MA,
};

enum JeitaLowTemperatureCurrent
{
    JEITA_LOW_TEMP_50, // 50% of ICHG (REG04[6:0])
    JEITA_LOW_TEMP_20, // 20% of ICHG (REG04[6:0])
};


#endif  /* __BQ25896__ */
