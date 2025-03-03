#ifndef __SY6970__
#define __SY6970__

#define SY6970_SLAVE_ADDRESS        (0x6A)
#define SY6970_DEV_REV              (0x00)

#define SY6970_VBUS_MASK_VAL(val)   (val & 0x7F)
#define SY6970_VBAT_MASK_VAL(val)   (val & 0x7F)
#define SY6970_VSYS_MASK_VAL(val)   (val & 0x7F)
#define SY6970_NTC_MASK_VAL(val)    (val & 0x7F)

#define SY6970_VBUS_BASE_VAL        (2600)
#define SY6970_VBAT_BASE_VAL        (2304)
#define SY6970_VSYS_BASE_VAL        (2304)
#define SY6970_NTC_BASE_VAL         (21)

#define SY6970_VBUS_VOL_STEP        (100)
#define SY6970_VBAT_VOL_STEP        (20)
#define SY6970_VSYS_VOL_STEP        (20)
#define SY6970_NTC_VOL_STEP         (0.465)
#define SY6970_CHG_STEP_VAL         (50)

#define SY6970_PRE_CHG_CUR_BASE     (64)
#define SY6970_FAST_CHG_CUR_STEP    (64)
#define SY6970_PRE_CHG_CUR_STEP     (64)

#define SY6970_FAST_CHG_CURRENT_MAX (5056)

#define SY6970_PRE_CHG_CURRENT_MIN  (64)
#define SY6970_PRE_CHG_CURRENT_MAX  (1024)

#define SY6970_CHG_VOL_BASE         (3840)
#define SY6970_CHG_VOL_STEP         (16)
#define SY6970_FAST_CHG_VOL_MIN     (3840)
#define SY6970_FAST_CHG_VOL_MAX     (4608)

#define SY6970_SYS_VOL_STEPS        (100)
#define SY6970_SYS_VOFF_VOL_MIN     (3000)
#define SY6970_SYS_VOFF_VOL_MAX     (3700)

#define SY6970_IN_CURRENT_STEP      (50)
#define SY6970_IN_CURRENT_MIN       (100)
#define SY6970_IN_CURRENT_MAX       (3250)

#define SY6970_BOOTS_VOL_BASE       (4550)
#define SY6970_BOOTS_VOL_STEP       (64)
#define SY6970_BOOST_VOL_MIN        (4550)
#define SY6970_BOOST_VOL_MAX        (5510)

#define SY6970_IRQ_WTD_FAULT(x)     (bool)((x & 0xFF) >> 7)
#define SY6970_IRQ_BOOST_FAULT(x)   (bool)((x & 0xFF) >> 6)
#define SY6970_IRQ_CHG_FAULT(x)     (bool)((x & 0xFF) >> 5)
#define SY6970_IRQ_BAT_FAULT(x)     (bool)((x & 0xFF) >> 4)
#define SY6970_IRQ_NTC_FAULT(x)     (bool)((x & 0xFF) & 0x03)

#define SY6970_VINDPM_VOL_BASE      (4550)
#define SY6970_VINDPM_VOL_STEPS     (100)
#define SY6970_VINDPM_VOL_MIN       (3900)
#define SY6970_VINDPM_VOL_MAX       (15300)

enum BusStatus
{
    BUS_STATE_NOINPUT,
    BUS_STATE_USB_SDP,
    BUS_STATE_USB_CDP,
    BUS_STATE_USB_DCP,
    BUS_STATE_HVDCP,
    BUS_STATE_ADAPTER,
    BUS_STATE_NO_STANDARD_ADAPTER,
    BUS_STATE_OTG
};

enum RequestRange
{
    REQUEST_9V,
    REQUEST_12V,
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
    BOOST_CUR_LIMIT_2450MA,
};

enum Timeout
{
    TIMER_OUT_40SEC,  // 40 Second
    TIMER_OUT_80SEC,  // 80 Second
    TIMER_OUT_160SEC, // 160 Second
};

enum ChargeStatus
{
    CHARGE_STATE_NO_CHARGE,
    CHARGE_STATE_PRE_CHARGE,
    CHARGE_STATE_FAST_CHARGE,
    CHARGE_STATE_DONE,
    CHARGE_STATE_UNKOWN,
};

enum NTCStatus
{
    BUCK_NTC_NORMAL = 0,
    BUCK_NTC_WARM   = 2,
    BUCK_NTC_COOL   = 3,
    BUCK_NTC_COLD   = 5,
    BUCK_NTC_HOT    = 6,
};

enum FastChargeTimer
{
    FAST_CHARGE_TIMER_5H,
    FAST_CHARGE_TIMER_8H,
    FAST_CHARGE_TIMER_12H,
    FAST_CHARGE_TIMER_20H,
};

#endif /* __SY6970__ */
