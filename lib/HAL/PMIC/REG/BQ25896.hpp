#ifndef __BQ25896__
#define __BQ25896__

#define BQ25896_SLAVE_ADDRESS          (0x6B)

#define BQ25896_DEV_REV                (0x02)

#define BQ25896_VBUS_MASK_VAL(val)     (val & 0x7F)
#define BQ25896_VBAT_MASK_VAL(val)     (val & 0x7F)
#define BQ25896_VSYS_MASK_VAL(val)     (val & 0x7F)
#define BQ25896_NTC_MASK_VAL(val)      (val & 0x7F)

#define BQ25896_VBUS_BASE_VAL          (2600)
#define BQ25896_VBAT_BASE_VAL          (2304)
#define BQ25896_VSYS_BASE_VAL          (2304)
#define BQ25896_NTC_BASE_VAL           (21)

#define BQ25896_VBUS_VOL_STEP          (100)
#define BQ25896_VBAT_VOL_STEP          (20)
#define BQ25896_VSYS_VOL_STEP          (20)
#define BQ25896_NTC_VOL_STEP           (0.465)

#define BQ25896_CHG_STEP_VAL           (50)
#define BQ25896_FAST_CHG_CUR_STEP      (64)
#define BQ25896_FAST_CHG_CURRENT_MIN   (0)
#define BQ25896_FAST_CHG_CURRENT_MAX   (3008)

#define BQ25896_PRE_CHG_CUR_BASE       (64)
#define BQ25896_PRE_CHG_CUR_STEP       (64)
#define BQ25896_PRE_CHG_CURRENT_MIN    (64)
#define BQ25896_PRE_CHG_CURRENT_MAX    (1024)

#define BQ25896_TERM_CHG_CUR_BASE      (64)
#define BQ25896_TERM_CHG_CUR_STEP      (64)
#define BQ25896_TERM_CHG_CURRENT_MIN   (64)
#define BQ25896_TERM_CHG_CURRENT_MAX   (1024)

#define BQ25896_CHG_VOL_BASE           (3840)
#define BQ25896_CHG_VOL_STEP           (16)
#define BQ25896_FAST_CHG_VOL_MIN       (3840)
#define BQ25896_FAST_CHG_VOL_MAX       (4608)

#define BQ25896_SYS_VOL_STEPS          (100)
#define BQ25896_SYS_VOFF_VOL_MIN       (3000)
#define BQ25896_SYS_VOFF_VOL_MAX       (3700)

#define BQ25896_IN_CURRENT_STEP        (50)
#define BQ25896_IN_CURRENT_MIN         (100)
#define BQ25896_IN_CURRENT_MAX         (3250)

#define BQ25896_IN_CURRENT_OPT_STEP    (50)
#define BQ25896_IN_CURRENT_OPT_MIN     (100)
#define BQ25896_IN_CURRENT_OPT_MAX     (3250)

#define BQ25896_IN_CURRENT_OFFSET_STEP (100)
#define BQ25896_IN_CURRENT_OFFSET_MAX  (3100)

#define BQ25896_BOOTS_VOL_BASE         (4550)
#define BQ25896_BOOTS_VOL_STEP         (64)
#define BQ25896_BOOST_VOL_MIN          (4550)
#define BQ25896_BOOST_VOL_MAX          (5510)

#define BQ25896_VINDPM_VOL_BASE        (4550)
#define BQ25896_VINDPM_VOL_STEPS       (100)
#define BQ25896_VINDPM_VOL_MIN         (3900)
#define BQ25896_VINDPM_VOL_MAX         (15300)

#define BQ25896_BAT_COMP_STEPS         (20)
#define BQ25896_BAT_COMP_MAX           (140)

#define BQ25896_VCLAMP_STEPS           (32)
#define BQ25896_VCLAMP_MAX             (224)

namespace HAL::PMIC
{
    #pragma region REG03
    ////////////
    // REG 03 //
    //////////////////
    // REG 03 ENUMS //
    //////////////////
    enum REG03_00 : uint8_t
    {
        MINI_VOLT_2V9,
        MINI_VOLT_2V5,
    };
    typedef REG03_00 ExitBoostModeVolt;
    // END REG 03 ENUMS
    // REG 03 UNIONS
    union REG03
    {
        struct bits
        {
            REG03_00 MIN_VBAT_SEL : 1;
            uint8_t  SYS_MIN      : 3;
            bool     CHG_CONFIG   : 1;
            bool     OTG_CONFIG   : 1;
            bool     WD_RST       : 1;
            bool     BAT_LOADEN   : 1;
        };
        uint8_t reg;
    };
    ////////////////
    // END REG 03 //
    ////////////////
    #pragma endregion

    #pragma region REG06
    ////////////
    // REG 06 //
    //////////////////
    // REG 06 ENUMS //
    //////////////////
    enum REG06_11
    {
        FAST_CHG_THR_2V8,
        FAST_CHG_THR_3V0
    };
    typedef REG06_11 FastChargeThreshold;
    enum REG06_00 : uint8_t
    {
        RECHARGE_OFFSET_100MV,
        RECHARGE_OFFSET_200MV
    };
    typedef REG06_00 RechargeThresholdOffset;
    // END REG 06 ENUMS
    // REG 06 UNION
    union REG06
    {
        struct bits
        {
            REG06_00 VRECHG  : 1;
            REG06_11 BATLOWV : 1;
            uint8_t  VREG    : 6;
        };
        uint8_t reg;
    };
    ////////////////
    // END REG 06 //
    ////////////////
    #pragma endregion

    #pragma region REG07
    ////////////
    // REG 07 //
    //////////////////
    // REG 07 ENUMS //
    //////////////////
    enum REG07_00 : uint8_t
    {
        JEITA_LOW_TEMP_50, // 50% of ICHG (REG04[6:0])
        JEITA_LOW_TEMP_20, // 20% of ICHG (REG04[6:0])
    };
    enum REG07_21 : uint8_t
    {
        FAST_CHARGE_TIMER_5H,
        FAST_CHARGE_TIMER_8H,
        FAST_CHARGE_TIMER_12H,
        FAST_CHARGE_TIMER_20H,
    };
    typedef REG07_21 FastChargeTimer;
    enum REG07_54 : uint8_t
    {
        TIMER_OUT_OFF,
        TIMER_OUT_40SEC,  // 40 Second
        TIMER_OUT_80SEC,  // 80 Second
        TIMER_OUT_160SEC, // 160 Second
    };
    typedef REG07_54 Timeout;
    // END REG 07 ENUMS
    // REG 07 UNIONS
    union REG07
    {
        struct bits
        {
            REG07_00 JEITA_ISET : 1;
            REG07_21 CHG_TIMER  : 2;
            bool     EN_TIMER   : 1;
            REG07_54 WATCHDOG   : 2;
            bool     STAT_DIS   : 1;
            bool     EN_TERM    : 1;
        };
        uint8_t reg;
    };
    ////////////////
    // END REG 07 //
    ////////////////
    #pragma endregion

    #pragma region REG09
    ////////////
    // REG 09 //
    ////////////
    union REG09
    {
        struct bits
        {
            bool PUMPX_DN      : 1;
            bool PUMPX_UP      : 1;
            bool BATFET_RST_EN : 1;
            bool BATFET_DLY    : 1;
            bool JEITA_VSET    : 1;
            bool BATFET_DIS    : 1;
            bool TMR2X_EN      : 1;
            bool FORCE_ICO     : 1;
        };
        uint8_t reg;
    };
    ////////////////
    // END REG 09 //
    ////////////////
    #pragma endregion

    #pragma region REG0A
    ////////////
    // REG 0A //
    ////////////
    enum REG0A_02
    {
        BOOST_CUR_LIMIT_500MA,
        BOOST_CUR_LIMIT_750MA,
        BOOST_CUR_LIMIT_1200MA,
        BOOST_CUR_LIMIT_1400MA,
        BOOST_CUR_LIMIT_1650MA,
        BOOST_CUR_LIMIT_1875MA,
        BOOST_CUR_LIMIT_2150MA,
    };
    typedef REG0A_02 BoostCurrentLimit;
    // END REG 0A ENUMS
    // REG 0A UNIONS
    union REG0A
    {
        struct bits
        {
            REG0A_02 BOOST_LIM       : 3;
            bool     PFM_OTG_DISABLE : 1;
            uint8_t  BOOSTV          : 4;
        };
        uint8_t reg;
    };
    ////////////////
    // END REG 0A //
    ////////////////
    #pragma endregion

    //
    // REG 0B
    //
    enum REG0B_43 : uint8_t
    {
        CHARGE_STATE_NO_CHARGE,
        CHARGE_STATE_PRE_CHARGE,
        CHARGE_STATE_FAST_CHARGE,
        CHARGE_STATE_DONE,
        CHARGE_STATE_UNKOWN,
    };
    typedef REG0B_43 ChargeStatus;
    enum REG0B_75 : uint8_t
    {
        BUS_STATE_NOINPUT,
        BUS_STATE_USB_SDP,
        BUS_STATE_ADAPTER,
        BUS_STATE_OTG = 7,
    };
    typedef REG0B_75 BusStatus;
    union REG0B
    {
        struct bits
        {
            bool     VSYS_STAT : 1;
            bool     RESV      : 1;
            bool     PG_STAT   : 1;
            REG0B_43 CHRG_STAT : 2;
            REG0B_75 VBUS_STAT : 3;
        } bits;
        uint8_t reg;
    };
    // END REG 0B

    //
    // REG 0C
    //
    enum REG0C_20 : uint8_t
    {
        BUCK_NTC_NORMAL = 0,
        BUCK_NTC_WARM   = 2,
        BUCK_NTC_COOL   = 3,
        BUCK_NTC_COLD   = 5,
        BUCK_NTC_HOT    = 6,
    };
    typedef REG0C_20 NTCStatus;
    enum REG0C_54 : uint8_t
    {
        NORMAL,
        INPUT_FAULT,
        THERMAL_SHUTDOWN,
        CG_SFTY_TMR_EXP,
    };
    union REG0C
    {
        struct bits
        {
            REG0C_20 NTC      : 3;
            bool     BAT      : 1;
            REG0C_54 CHRG     : 2;
            bool     BOOST    : 1;
            bool     WATCHDOG : 1;
        } bits;
        uint8_t reg;
    };

    //
    // REG 0E
    //
    union REG0E
    {
        struct bits
        {
            uint8_t BATV       : 7;
            bool    THERM_STAT : 1;
        } bits;
        uint8_t reg;
    };
    // END REG 0E

    //
    // REG 0F
    //
    union REG0F
    {
        struct bits
        {
            uint8_t SYSV : 7;
            bool    RESV : 1;
        } bits;
        uint8_t reg;
    };
    // END REG 0F

    //
    // REG 10
    //
    union REG10
    {
        struct bits
        {
            uint8_t TSPCT : 7;
            bool    RESV  : 1;
        } bits;
        uint8_t reg;
    };
    // END REG 10

    //
    // REG 11
    //
    union REG11
    {
        struct bits
        {
            uint8_t VBUSV   : 7;
            bool    VBUS_GD : 1;
        } bits;
        uint8_t reg;
    };
    // END REG 11

    //
    // REG 12
    //
    union REG12
    {
        struct bits
        {
            uint8_t ICHGR : 7;
            bool    RESV  : 1;
        } bits;
        uint8_t reg;
    };
    // END REG 12

    //
    // REG 13
    //
    union REG13
    {
        struct bits
        {
            uint8_t IDPM_LIM  : 6;
            bool    IDPM_STAT : 1;
            bool    VDPM_STAT : 1;
        } bits;
        uint8_t reg;
    };
    // END REG 13
} // namespace HAL::PMIC
#endif /* __BQ25896__ */
