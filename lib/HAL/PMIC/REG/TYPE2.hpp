#ifndef __REG_TYPE2__
#define __REG_TYPE2__
#define POWERS_PPM_REG_00H                                   (0x00)
#define POWERS_PPM_REG_01H                                   (0x01)
#define POWERS_PPM_REG_02H                                   (0x02)
#define POWERS_PPM_REG_03H                                   (0x03)
#define POWERS_PPM_REG_04H                                   (0x04)
#define POWERS_PPM_REG_05H                                   (0x05)
#define POWERS_PPM_REG_06H                                   (0x06)
#define POWERS_PPM_REG_07H                                   (0x07)
#define POWERS_PPM_REG_08H                                   (0x08)
#define POWERS_PPM_REG_09H                                   (0x09)
#define POWERS_PPM_REG_0AH                                   (0x0A)
// Read only STATUS REG
#define POWERS_PPM_REG_0BH                                   (0x0B)
// Read only CHARGE IRQ REG
#define POWERS_PPM_REG_0CH                                   (0x0C)
// Absolute VINDPM Threshold
#define POWERS_PPM_REG_0DH                                   (0x0D)
// Read only BATTERY VOLTAGE
#define POWERS_PPM_REG_0EH                                   (0x0E)
// Read only SYSTEM VOLTAGE
#define POWERS_PPM_REG_0FH                                   (0x0F)
// Read only NTC PERCENTAGE
#define POWERS_PPM_REG_10H                                   (0x10)
// Read only VBUS VOLTAGE
#define POWERS_PPM_REG_11H                                   (0x11)
// Read only CHARGE CURRENT
#define POWERS_PPM_REG_12H                                   (0x12)
// Read only VINDPM/IINDPM STATUS/CURR LIMIT SETTING 
#define POWERS_PPM_REG_13H                                   (0x13)
// RESET REG
#define POWERS_PPM_REG_14H                                   (0x14)

/**
 * @brief chip type
 */
enum PMICChipModel {
    BQ25896,
    SY6970,
    UNDEFINED,
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
    BUCK_NTC_WARM = 2,
    BUCK_NTC_COOL = 3,
    BUCK_NTC_COLD = 5,
    BUCK_NTC_HOT = 6,
};

enum BoostNTCStatus
{
    BOOST_NTC_NORMAL = 0,
    BOOST_NTC_COLD = 5,
    BOOST_NTC_HOT = 6,
};

enum Timeout
{
    TIMER_OUT_40SEC,  // 40 Second
    TIMER_OUT_80SEC,  // 80 Second
    TIMER_OUT_160SEC, // 160 Second
};

enum MeasureMode
{
    ONE_SHORT,
    CONTINUOUS,
};

enum BoostFreq
{
    BOOST_FREQ_1500KHZ,
    BOOST_FREQ_500KHZ,
};

enum FastChargeTimer
{
    FAST_CHARGE_TIMER_5H,
    FAST_CHARGE_TIMER_8H,
    FAST_CHARGE_TIMER_12H,
    FAST_CHARGE_TIMER_20H,
};


#endif  /* __TYPE2__ */
