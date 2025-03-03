#include "../IMPL/TYPE2.tpp"
#include "../REG/BQ25896.hpp"
namespace HAL::PMIC
{
    class BQ25896 : public Type2<class BQ25896>
    {
        friend class Base<BQ25896>;

    public:
        BQ25896(TwoWire &wire = Wire, int sda = SDA, int scl = SCL, uint8_t addr = BQ25896_SLAVE_ADDRESS)
        {
            myWire = &wire;
            mySDA  = sda;
            mySCL  = scl;
            myADDR = addr;
        }

        ~BQ25896()
        {
            log_d("~PowersBQ25896");
            end();
        }

        bool init(TwoWire &wire = Wire, int sda = SDA, int scl = SCL, uint8_t addr = BQ25896_SLAVE_ADDRESS)
        {
            myWire  = &wire;
            mySDA   = sda;
            mySCL   = scl;
            myADDR  = addr;
            r0C.reg = 0;
            return begin();
        }

        const char *getChipName()
        {
            return getChipID() == BQ25896_DEV_REV ? "BQ25896" : "Unkown";
        }

        // VBUS Good Status
        bool isVbusIn()
        {
            return getRegisterBit(POWERS_PPM_REG_11H, 7);
        }
        /***************************************************
         * POWERS_PPM_REG_00H ✅
         **************************************************/

        // Input Current Limit
        // Offset: 100mA
        // Range: 100mA (000000) – 3.25A (111111)
        // Default:0001000 (500mA)
        // (Actual input current limit is the lower of I2C or ILIM pin)
        // IINLIM bits are changed automaticallly after input source
        // type detection is completed
        // bq25896
        // PSEL = Hi (USB500) = 500mA
        // PSEL = Lo = 3.25A
        bool setInputCurrentLimit(uint16_t milliampere)
        {
            if (milliampere % BQ25896_IN_CURRENT_STEP)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_IN_CURRENT_STEP);
                return false;
            }
            if (milliampere < BQ25896_IN_CURRENT_MIN)
            {
                milliampere = BQ25896_IN_CURRENT_MIN;
            }
            if (milliampere > BQ25896_IN_CURRENT_MAX)
            {
                milliampere = BQ25896_IN_CURRENT_MAX;
            }
            int val = readRegister(POWERS_PPM_REG_00H);
            if (val == -1)
                return false;
            val         &= 0xC0;
            milliampere  = ((milliampere - BQ25896_IN_CURRENT_MIN) / BQ25896_IN_CURRENT_STEP);
            val         |= milliampere;
            return writeRegister(POWERS_PPM_REG_00H, val) != -1;
        }

        uint32_t getInputCurrentLimit()
        {
            int val = readRegister(POWERS_PPM_REG_00H);
            if (val == -1)
                return false;
            val &= 0x3F;
            return (val * BQ25896_IN_CURRENT_STEP) + BQ25896_IN_CURRENT_MIN;
        }

        /***************************************************
         * POWERS_PPM_REG_01H ✅
         **************************************************/

        // Boost Mode Hot Temperature Monitor Threshold
        // 0x0 – VBHOT1 Threshold (34.75%) (default)
        // 0x01 – VBHOT0 Threshold (Typ. 37.75%)
        // 0x02 – VBHOT2 Threshold (Typ. 31.25%)
        // 0x03 – Disable boost mode thermal protection
        void setBoostModeHotTemperatureMonitorThreshold(uint8_t params)
        {
            int val = readRegister(POWERS_PPM_REG_01H);
            if (val == -1)
                return;
            val &= 0x3F;
            val |= (params << 6);
            writeRegister(POWERS_PPM_REG_01H, val);
        }

        // Boost Mode Cold Temperature Monitor Threshold
        // 0 – VBCOLD0 Threshold (Typ. 77%) (default)
        // 1 – VBCOLD1 Threshold (Typ. 80%)
        void setBoostModeColdTemperatureMonitorThreshold(uint8_t params)
        {
            int val = readRegister(POWERS_PPM_REG_01H);
            if (val == -1)
                return;
            val &= 0xDF;
            val |= ((params & 0x01) << 5);
            writeRegister(POWERS_PPM_REG_01H, val);
        }

        // Input Voltage Limit Offset
        // Default: 600mV (00110)
        // Range: 0mV – 3100mV
        // Minimum VINDPM threshold is clamped at 3.9V
        // Maximum VINDPM threshold is clamped at 15.3V
        // When VBUS at noLoad is ≤ 6V, the VINDPM_OS is used to calculate VINDPM threhold
        // When VBUS at noLoad is > 6V, the VINDPM_OS multiple by 2 is used to calculate VINDPM threshold.
        void setInputVoltageLimitOffset(uint16_t millivolt)
        {
            if (millivolt % BQ25896_IN_CURRENT_OFFSET_STEP)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_IN_CURRENT_OFFSET_STEP);
                return;
            }
            if (millivolt > BQ25896_IN_CURRENT_OFFSET_MAX)
            {
                millivolt = BQ25896_IN_CURRENT_OFFSET_MAX;
            }
            int val    = readRegister(POWERS_PPM_REG_01H);
            val       &= 0xE0;
            millivolt  = (millivolt / BQ25896_IN_CURRENT_OFFSET_STEP);
            val       |= millivolt;
            writeRegister(POWERS_PPM_REG_01H, val);
        }

        // Input Current Optimizer (ICO) Enable
        void enableInputCurrentOptimizer()
        {
            setRegisterBit(POWERS_PPM_REG_02H, 4);
        }

        // Input Current Optimizer (ICO) Disable
        void disableInputCurrentOptimizer()
        {
            clrRegisterBit(POWERS_PPM_REG_02H, 4);
        }

        /***************************************************
         * POWERS_PPM_REG_03H ✅
         **************************************************/

        bool setSysPowerDownVoltage(uint16_t millivolt)
        {
            if (millivolt % BQ25896_SYS_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", BQ25896_SYS_VOL_STEPS);
                return false;
            }
            if (millivolt < BQ25896_SYS_VOFF_VOL_MIN)
            {
                log_e("Mistake ! SYS minimum output voltage is  %umV", BQ25896_SYS_VOFF_VOL_MIN);
                return false;
            }
            else if (millivolt > BQ25896_SYS_VOFF_VOL_MAX)
            {
                log_e("Mistake ! SYS maximum output voltage is  %umV", BQ25896_SYS_VOFF_VOL_MAX);
                return false;
            }
            int val = readRegister(POWERS_PPM_REG_03H);
            if (val == -1)
                return false;
            val  &= 0xF1;
            val  |= (millivolt - BQ25896_SYS_VOFF_VOL_MIN) / BQ25896_SYS_VOL_STEPS;
            val <<= 1;
            return 0 == writeRegister(POWERS_PPM_REG_03H, val);
        }

        uint16_t getSysPowerDownVoltage()
        {
            int val = readRegister(POWERS_PPM_REG_03H);
            if (val == -1)
                return 0;
            val  &= 0x0E;
            val >>= 1;
            return (val * BQ25896_SYS_VOL_STEPS) + BQ25896_SYS_VOFF_VOL_MIN;
        }

        // Minimum Battery Voltage (falling) to exit boost mode
        void setExitBoostModeVoltage(ExitBoostModeVolt params)
        {
            switch (params)
            {
                case MINI_VOLT_2V9 :
                    clrRegisterBit(POWERS_PPM_REG_03H, 0);
                    break;
                case MINI_VOLT_2V5 :
                    setRegisterBit(POWERS_PPM_REG_03H, 0);
                    break;
                default :
                    break;
            }
        }

        /***************************************************
         * POWERS_PPM_REG_04H ✅
         **************************************************/

        void enableCurrentPulseControl()
        {
            setRegisterBit(POWERS_PPM_REG_04H, 7);
        }

        void disableCurrentPulseControl()
        {
            clrRegisterBit(POWERS_PPM_REG_04H, 7);
        }

        uint16_t getChargerConstantCurr()
        {
            int val  = readRegister(POWERS_PPM_REG_04H);
            val     &= 0x7F;
            return val * BQ25896_FAST_CHG_CUR_STEP;
        }

        /**
         * @brief  setChargerConstantCurr
         * @note
         * @param  milliampere: Range:0~3008 mA / step:64mA
         * @retval true : success false : failed
         */
        bool setChargerConstantCurr(uint16_t milliampere)
        {
            if (milliampere % BQ25896_FAST_CHG_CUR_STEP)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_FAST_CHG_CUR_STEP);
                return false;
            }
            if (milliampere > BQ25896_FAST_CHG_CURRENT_MAX)
            {
                milliampere = BQ25896_FAST_CHG_CURRENT_MAX;
            }
            int val  = readRegister(POWERS_PPM_REG_04H);
            val     &= 0x80;
            val     |= (milliampere / BQ25896_FAST_CHG_CUR_STEP);
            return writeRegister(POWERS_PPM_REG_04H, val) != -1;
        }

        /***************************************************
         * POWERS_PPM_REG_05H ✅
         **************************************************/

        // Precharge Current Limit Range: 64mA ~ 1024mA ,step:64mA
        bool setPrechargeCurr(uint16_t milliampere)
        {
            if (milliampere % BQ25896_PRE_CHG_CUR_STEP)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_PRE_CHG_CUR_STEP);
                return false;
            }
            if (milliampere < BQ25896_PRE_CHG_CURRENT_MIN)
            {
                milliampere = BQ25896_PRE_CHG_CURRENT_MIN;
            }
            if (milliampere > BQ25896_PRE_CHG_CURRENT_MAX)
            {
                milliampere = BQ25896_PRE_CHG_CURRENT_MAX;
            }
            int val      = readRegister(POWERS_PPM_REG_05H);
            val         &= 0x0F;
            milliampere  = ((milliampere - BQ25896_PRE_CHG_CUR_BASE) / BQ25896_PRE_CHG_CUR_STEP);
            val         |= milliampere << 4;
            return writeRegister(POWERS_PPM_REG_05H, val) != -1;
        }

        uint16_t getPrechargeCurr(void)
        {
            int val   = readRegister(POWERS_PPM_REG_05H);
            val      &= 0xF0;
            val     >>= 4;
            return BQ25896_PRE_CHG_CUR_STEP + (val * BQ25896_PRE_CHG_CUR_STEP);
        }

        // Precharge Current Limit Range: 64mA ~ 1024mA ,step:64mA
        bool setTerminationCurr(uint16_t milliampere)
        {
            if (milliampere % BQ25896_TERM_CHG_CUR_STEP)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_TERM_CHG_CUR_STEP);
                return false;
            }
            if (milliampere < BQ25896_TERM_CHG_CURRENT_MIN)
            {
                milliampere = BQ25896_TERM_CHG_CURRENT_MIN;
            }
            if (milliampere > BQ25896_TERM_CHG_CURRENT_MAX)
            {
                milliampere = BQ25896_TERM_CHG_CURRENT_MAX;
            }
            int val      = readRegister(POWERS_PPM_REG_05H);
            val         &= 0xF0;
            milliampere  = ((milliampere - BQ25896_TERM_CHG_CUR_BASE) / BQ25896_TERM_CHG_CUR_STEP);
            val         |= milliampere;
            return writeRegister(POWERS_PPM_REG_05H, val) != -1;
        }

        uint16_t getTerminationCurr(void)
        {
            int val  = readRegister(POWERS_PPM_REG_05H);
            val     &= 0x0F;
            return BQ25896_TERM_CHG_CUR_STEP + (val * BQ25896_TERM_CHG_CUR_STEP);
        }

        /***************************************************
         * POWERS_PPM_REG_06H ✅
         **************************************************/
        uint16_t getChargeTargetVoltage()
        {
            int val = readRegister(POWERS_PPM_REG_06H);
            val     = (val & 0xFC) >> 2;
            if (val > 0x30)
            {
                return BQ25896_FAST_CHG_VOL_MAX;
            }
            return val * BQ25896_CHG_VOL_STEP + BQ25896_CHG_VOL_BASE;
        }

        // Charge Voltage Limit Range:3840 ~ 4608mV ,step:16 mV
        bool setChargeTargetVoltage(uint16_t millivolt)
        {
            if (millivolt % BQ25896_CHG_VOL_STEP)
            {
                log_e("Mistake ! The steps is must %u mV", BQ25896_CHG_VOL_STEP);
                return false;
            }
            if (millivolt < BQ25896_FAST_CHG_VOL_MIN)
            {
                millivolt = BQ25896_FAST_CHG_VOL_MIN;
            }
            if (millivolt > BQ25896_FAST_CHG_VOL_MAX)
            {
                millivolt = BQ25896_FAST_CHG_VOL_MAX;
            }
            int val  = readRegister(POWERS_PPM_REG_06H);
            val     &= 0x03;
            val     |= (((millivolt - BQ25896_CHG_VOL_BASE) / BQ25896_CHG_VOL_STEP) << 2);
            return writeRegister(POWERS_PPM_REG_06H, val) != -1;
        }

        // Battery Precharge to Fast Charge Threshold
        void setFastChargeThreshold(FastChargeThreshold threshold)
        {
            switch (threshold)
            {
                case FAST_CHG_THR_2V8 :
                    clrRegisterBit(POWERS_PPM_REG_06H, 1);
                    break;
                case FAST_CHG_THR_3V0 :
                    setRegisterBit(POWERS_PPM_REG_06H, 1);
                    break;
                default :
                    break;
            }
        }

        // Battery Recharge Threshold Offset(below Charge Voltage Limit)
        void setBatteryRechargeThresholdOffset(RechargeThresholdOffset offset)
        {
            switch (offset)
            {
                case RECHARGE_OFFSET_100MV :
                    clrRegisterBit(POWERS_PPM_REG_06H, 0);
                    break;
                case RECHARGE_OFFSET_200MV :
                    setRegisterBit(POWERS_PPM_REG_06H, 0);
                    break;
                default :
                    break;
            }
        }

        /***************************************************
         * POWERS_PPM_REG_07H ✅
         **************************************************/

        // JEITA Low Temperature Current Setting
        // JEITA（Japan Electronics and Information Technology Industries Association）
        // https://en.wikipedia.org/wiki/Japan_Electronics_and_Information_Technology_Industries_Association
        void setJeitaLowTemperatureCurrent(enum REG07_00 params)
        {
            switch (params)
            {
                case JEITA_LOW_TEMP_50 :
                    clrRegisterBit(POWERS_PPM_REG_07H, 0);
                    break;
                case JEITA_LOW_TEMP_20 :
                    setRegisterBit(POWERS_PPM_REG_07H, 0);
                    break;
                default :
                    break;
            }
        }

        /***************************************************
         * POWERS_PPM_REG_08H ✅
         **************************************************/
        // IR Compensation Resistor Setting
        // Range: 0 – 140mΩ
        // Default: 0Ω (000) (i.e. Disable IRComp)
        void setIRCompensationResistor(uint16_t params)
        {
            if (params % BQ25896_BAT_COMP_STEPS)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_BAT_COMP_STEPS);
                return;
            }
            if (params > BQ25896_TERM_CHG_CURRENT_MAX)
            {
                params = BQ25896_TERM_CHG_CURRENT_MAX;
            }
            int val = readRegister(POWERS_PPM_REG_08H);
            if (val == -1)
                return;
            val    &= 0x1F;
            params  = (params / BQ25896_BAT_COMP_STEPS);
            val    |= (params << 5);
            writeRegister(POWERS_PPM_REG_08H, val);
        }

        // IR Compensation Voltage Clamp
        // above VREG (REG06[7:2])
        // Offset: 0mV
        // Range: 0-224mV
        // Default: 0mV (000)
        void setIRCompensationVoltageClamp(uint16_t params)
        {
            if (params % BQ25896_VCLAMP_STEPS)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_VCLAMP_STEPS);
                return;
            }
            if (params > BQ25896_TERM_CHG_CURRENT_MAX)
            {
                params = BQ25896_TERM_CHG_CURRENT_MAX;
            }
            int val = readRegister(POWERS_PPM_REG_08H);
            if (val == -1)
                return;
            val    &= 0xE3;
            params  = (params / BQ25896_VCLAMP_STEPS);
            val    |= (params << 2);
            writeRegister(POWERS_PPM_REG_08H, val);
        }

        // Thermal Regulation Threshold
        // 0x0 – 60°C
        // 0x1 – 80°C
        // 0x2 – 100°C
        // 0x3 – 120°C (default)
        void setThermalRegulationThreshold(uint8_t params)
        {
            int val = readRegister(POWERS_PPM_REG_08H);
            if (val == -1)
                return;
            val &= 0xE3;
            val |= (params);
            writeRegister(POWERS_PPM_REG_08H, val);
        }

        /***************************************************
         * POWERS_PPM_REG_09H
         **************************************************/
        // Force Start Input Current Optimizer (ICO)
        // 0 – Do not force ICO (default)
        // 1 – Force ICO
        // Note: This bit is can only be set only and always returns to 0 after ICO starts
        void forceInputCurrentOptimizer(bool force)
        {
            force ? setRegisterBit(POWERS_PPM_REG_09H, 7) : clrRegisterBit(POWERS_PPM_REG_09H, 7);
        }

        // Safety Timer Setting during DPM or Thermal Regulation
        // 0 – Safety timer not slowed by 2X during input DPM or thermal regulation
        // 1 – Safety timer slowed by 2X during input DPM or thermal regulation (default)
        void setThermalRegulation(uint8_t params)
        {
            params ? setRegisterBit(POWERS_PPM_REG_09H, 6) : clrRegisterBit(POWERS_PPM_REG_09H, 6);
        }

        // JEITA High Temperature Voltage Setting
        // JEITA（Japan Electronics and Information Technology Industries Association）
        // https://en.wikipedia.org/wiki/Japan_Electronics_and_Information_Technology_Industries_Association
        // 0 – Set Charge Voltage to VREG-200mV during JEITA hig temperature(default)
        // 1 – Set Charge Voltage to VREG during JEITA high temperature
        void setJeitaHighTemperature(uint8_t params)
        {
            params ? setRegisterBit(POWERS_PPM_REG_09H, 4) : clrRegisterBit(POWERS_PPM_REG_09H, 4);
        }

        // BATFET turn off delay control
        // 0 – BATFET turn off immediately when BATFET_DIS bit is set (default)
        // 1 – BATFET turn off delay by tSM_DLY when BATFET_DIS bit is set
        void setTurnOffDelay(uint8_t params)
        {
            params ? setRegisterBit(POWERS_PPM_REG_09H, 3) : clrRegisterBit(POWERS_PPM_REG_09H, 3);
        }

        // BATFET full system reset enable
        // 0 – Disable BATFET full system reset
        // 1 – Enable BATFET full system reset (default)
        void setFullSystemReset(uint8_t params)
        {
            params ? setRegisterBit(POWERS_PPM_REG_09H, 2) : clrRegisterBit(POWERS_PPM_REG_09H, 2);
        }

        // Current pulse control voltage up enable
        // 0 – Disable (default)
        // 1 – Enable
        // Note: This bit is can only be set when EN_PUMPX bit is set and returns to 0 after current pulse control sequence is completed
        void setCurrentPulseControlVoltageUp(uint8_t params)
        {
            params ? setRegisterBit(POWERS_PPM_REG_09H, 1) : clrRegisterBit(POWERS_PPM_REG_09H, 1);
        }

        // Current pulse control voltage down enable
        // 0 – Disable (default)
        // 1 – Enable
        // Note: This bit is can only be set when EN_PUMPX bit is set and returns to 0 after current pulse control sequence is completed
        void setCurrentPulseControlVoltageDown(uint8_t params)
        {
            params ? setRegisterBit(POWERS_PPM_REG_09H, 0) : clrRegisterBit(POWERS_PPM_REG_09H, 0);
        }

        /***************************************************
         * POWERS_PPM_REG_0AH ✅
         **************************************************/

        // Boost Mode Voltage Regulation: 4550mV ~ 5510mV
        bool setBoostVoltage(uint16_t millivolt)
        {
            if (millivolt % BQ25896_BOOTS_VOL_STEP)
            {
                log_e("Mistake ! The steps is must %u mV", BQ25896_BOOTS_VOL_STEP);
                return false;
            }
            if (millivolt < BQ25896_BOOST_VOL_MIN)
            {
                millivolt = BQ25896_BOOST_VOL_MIN;
            }
            if (millivolt > BQ25896_BOOST_VOL_MAX)
            {
                millivolt = BQ25896_BOOST_VOL_MAX;
            }
            int val  = readRegister(POWERS_PPM_REG_0AH);
            val     &= 0xF0;
            val     |= (((millivolt - BQ25896_BOOTS_VOL_BASE) / BQ25896_BOOTS_VOL_STEP) << 4);
            return writeRegister(POWERS_PPM_REG_0AH, val) != -1;
        }

        // Boost Current Limit: 500mA ~ 150 mA
        bool setBoostCurrentLimit(BoostCurrentLimit milliampere)
        {
            if (milliampere > BOOST_CUR_LIMIT_2150MA)
            {
                return false;
            }
            int val  = readRegister(POWERS_PPM_REG_0AH);
            val     &= 0x03;
            val     |= milliampere;
            return writeRegister(POWERS_PPM_REG_0AH, val) != -1;
        }

        // PFM mode allowed in boost mode
        // 0 – Allow PFM in boost mode (default)
        // 1 – Disable PFM in boost mode
        void setBoostModeUsePFM(bool enable)
        {
            enable ? clrRegisterBit(POWERS_PPM_REG_0AH, 3) : setRegisterBit(POWERS_PPM_REG_0AH, 3);
        }

        /***************************************************
         * POWERS_PPM_REG_0BH ✅
         **************************************************/

        bool isPowerGood()
        {
            return getRegisterBit(POWERS_PPM_REG_0BH, 2);
        }

        const char *getBusStatusString()
        {
            BusStatus status = getBusStatus();
            switch (status)
            {
                case BUS_STATE_NOINPUT :
                    return "No input";
                case BUS_STATE_USB_SDP :
                    return "USB Host SDP";
                case BUS_STATE_ADAPTER :
                    return "Adapter";
                case BUS_STATE_OTG :
                    return "OTG";
                default :
                    return "Unknown";
            }
        }

        const char *getChargeStatusString()
        {
            ChargeStatus status = chargeStatus();
            switch (status)
            {
                case CHARGE_STATE_NO_CHARGE :
                    return "Not Charging";
                case CHARGE_STATE_PRE_CHARGE :
                    return "Pre-charge";
                case CHARGE_STATE_FAST_CHARGE :
                    return "Fast Charging";
                case CHARGE_STATE_DONE :
                    return "Charge Termination Done";
                default :
                    return "Unknown";
            }
        }

        // VSYS Regulation Status
        // 0 – Not in VSYSMIN regulation (BAT > VSYSMIN)
        // 1 – In VSYSMIN regulation (BAT < VSYSMIN)
        bool getVsysRegulationStatus()
        {
            return getRegisterBit(POWERS_PPM_REG_0BH, 0);
        }

        const char *getVsysRegulationStatusString()
        {
            if (getVsysRegulationStatus())
            {
                return "BAT < VSYSMIN";
            }
            return "BAT > VSYSMIN";
        }

        /***************************************************
         * POWERS_PPM_REG_0CH ✅
         **************************************************/

        // After reading the register, all will be cleared
        uint8_t getFaultStatus(void)
        {
            readRegister(POWERS_PPM_REG_0CH, &r0C.reg);
            return r0C.reg;
        }

        // Watchdog Fault Status
        // 0 – Normal
        // 1- Watchdog timer expiration
        bool isWatchdogFault()
        {
            return r0C.bits.WATCHDOG;
        }

        // Boost Mode Fault Status
        // 0 – Normal
        // 1 – VBUS overloaded in OTG, or VBUS OVP, or battery is too low in boost mode
        bool isBoostFault()
        {
            return r0C.bits.BOOST;
        }

        // Charge Fault Status
        // 00 – Normal
        // 01 – Input fault (VBUS > VACOV or VBAT < VBUS < VVBUSMIN(typical 3.8V)
        // 10 - Thermal shutdown
        // 11 – Charge Safety Timer Expiration
        uint8_t isChargeFault()
        {
            return r0C.bits.CHRG;
        }

        // Battery Fault Status
        // 0 – Normal
        // 1 – BATOVP (VBAT > VBATOVP)
        bool isBatteryFault()
        {
            return r0C.bits.BAT;
        }

        // NTC Fault Status
        bool isNTCFault()
        {
            return !(r0C.bits.NTC = REG0C_20::BUCK_NTC_NORMAL);
        }

        // NTC Fault Status string
        uint8_t getNTCStatus()
        {
            return r0C.bits.NTC;
        }

        const char *getNTCStatusString()
        {
            uint8_t status = getNTCStatus();
            if (isOTG())
            {
                // Boost mode
                switch (status)
                {
                    case BOOST_NTC_NORMAL :
                        return "Boost mode NTC normal";
                    case BOOST_NTC_COLD :
                        return "Boost mode NTC cold";
                    case BOOST_NTC_HOT :
                        return "Boost mode NTC hot";
                    default :
                        break;
                }
            }
            else
            {
                // Buck mode
                switch (status)
                {
                    case BUCK_NTC_NORMAL :
                        return "Buck mode NTC normal";
                    case BUCK_NTC_WARM :
                        return "Buck mode NTC warm";
                    case BUCK_NTC_COOL :
                    case BUCK_NTC_COLD :
                        return "Buck mode NTC cold";
                    case BUCK_NTC_HOT :
                        return "Buck mode NTC hot";
                    default :
                        break;
                }
            }
            return "Unknown";
        }

        // Debug
        void getReadOnlyRegisterValue()
        {
#ifdef ARDUINO // debug ..
            static uint8_t last_val[8] = {0};
            const uint8_t  regis[]     = {
                POWERS_PPM_REG_0BH,
                POWERS_PPM_REG_0CH,
                // POWERS_PPM_REG_0EH, //BATTERY VOLTAGE
                // POWERS_PPM_REG_0FH, //SYSTEM VOLTAGE
                // POWERS_PPM_REG_10H, //NTC PERCENTAGE
                // POWERS_PPM_REG_11H, //VBUS VOLTAGE
                POWERS_PPM_REG_12H,
                POWERS_PPM_REG_13H};
            Serial.println();
            Serial.println("-------------------------");
            for (uint32_t i = 0; i < sizeof(regis) / sizeof(regis[0]); ++i)
            {
                int val = readRegister(regis[i]);
                if (val == -1)
                {
                    continue;
                }
                if (last_val[i] != val)
                {
                    Serial.printf("\t---> REG%02X Prev:0x%02X ", regis[i], last_val[i]);
                    Serial.print(" BIN:");
                    Serial.print(last_val[i], BIN);
                    Serial.printf(" Curr: 0x%02X", val);
                    Serial.print(" BIN:");
                    Serial.println(val, BIN);
                    last_val[i] = val;
                }
                Serial.printf("\tREG%02XH:0x%X BIN:0b", regis[i], val);
                Serial.println(val, BIN);
            }
            Serial.println("-------------------------");
#endif
        }

        /***************************************************
         * POWERS_PPM_REG_0DH ✅
         **************************************************/
        // VINDPM Threshold Setting Method
        // 0 – Run Relative VINDPM Threshold (default)
        // 1 – Run Absolute VINDPM Threshold
        // Note: Register is reset to default value when input source is plugged-in
        void setVinDpmThresholdSetting(bool relative)
        {
            relative ? clrRegisterBit(POWERS_PPM_REG_0DH, 7) : setRegisterBit(POWERS_PPM_REG_0DH, 7);
        }

        // Absolute VINDPM Threshold
        bool setVinDpmThreshold(uint16_t millivolt)
        {
            if (millivolt % BQ25896_VINDPM_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", BQ25896_VINDPM_VOL_STEPS);
                return false;
            }
            if (millivolt < BQ25896_VINDPM_VOL_MIN)
            {
                millivolt = BQ25896_VINDPM_VOL_MIN;
            }
            if (millivolt > BQ25896_VINDPM_VOL_MAX)
            {
                millivolt = BQ25896_VINDPM_VOL_MAX;
            }
            int val  = readRegister(POWERS_PPM_REG_0DH);
            val     &= 0x80;
            val     |= (((millivolt - BQ25896_VINDPM_VOL_BASE) / BQ25896_VINDPM_VOL_STEPS));
            return writeRegister(POWERS_PPM_REG_0DH, val) != -1;
        }

        /***************************************************
         * POWERS_PPM_REG_0EH ✅
         **************************************************/
        // Thermal Regulation Status
        // true – Normal
        // false – In Thermal Regulation
        bool isThermalRegulationNormal()
        {
            return getRegisterBit(POWERS_PPM_REG_0EH, 7) == false;
        }

        // ADC conversion of Battery Voltage /mv
        uint16_t getBattVoltage()
        {
            int val = readRegister(POWERS_PPM_REG_0EH);
            if (val == -1)
                return 0;
            val = BQ25896_VBAT_MASK_VAL(val);
            if (val == 0)
                return 0;
            return (val * BQ25896_VBAT_VOL_STEP) + BQ25896_VBAT_BASE_VAL;
        }

        /***************************************************
         * POWERS_PPM_REG_0FH ✅
         **************************************************/

        // ADC conversion of System Voltage (VSYS)
        uint16_t getSystemVoltage()
        {
            int val = readRegister(POWERS_PPM_REG_0FH);
            if (val == -1 || val == 0)
                return 0;
            return (BQ25896_VSYS_MASK_VAL(val) * BQ25896_VSYS_VOL_STEP) + BQ25896_VSYS_BASE_VAL;
        }

        /***************************************************
         * POWERS_PPM_REG_10H ✅
         **************************************************/

        // ADC conversion of TS Voltage (TS) as percentage of REGN
        float getNTCPercentage()
        {
            int val = readRegister(POWERS_PPM_REG_10H);
            if (val == -1)
                return 0;
            return (BQ25896_NTC_MASK_VAL(val) * BQ25896_NTC_VOL_STEP) + BQ25896_NTC_BASE_VAL;
        }

        /***************************************************
         * POWERS_PPM_REG_11H ✅
         **************************************************/

        // ADC conversion of VBUS voltage (VBUS)
        uint16_t getVbusVoltage()
        {
            if (!isVbusIn())
            {
                return 0;
            }
            int val = readRegister(POWERS_PPM_REG_11H);
            return (BQ25896_VBUS_MASK_VAL(val) * BQ25896_VBUS_VOL_STEP) + BQ25896_VBUS_BASE_VAL;
        }

        /***************************************************
         * POWERS_PPM_REG_12H ✅
         **************************************************/

        // ADC conversion of Charge Current (IBAT) when VBAT > VBATSHORT
        //* If the charger is disconnected, the value in the register
        //* will remain the last value and will not be updated to 0.
        uint16_t getChargeCurrent()
        {
            ChargeStatus status = chargeStatus();
            if (status == CHARGE_STATE_NO_CHARGE)
            {
                log_e("CHARGE_STATE_NO_CHARGE...");
                return 0;
            }
            int val = readRegister(POWERS_PPM_REG_12H);
            if (val == 0 || val == -1)
            {
                log_e("read reg failed !...");
                return 0;
            }
            val = (val & 0x7F);
            return (val * BQ25896_CHG_STEP_VAL);
        }

        /***************************************************
         * POWERS_PPM_REG_13H ✅
         **************************************************/
        // VINDPM Status : DynamicPower-Path Management and Dynamic Power Management
        bool isDynamicPowerManagement()
        {
            return getRegisterBit(POWERS_PPM_REG_13H, 7);
        }

        // IINDPM Status
        bool isInputCurrentLimit()
        {
            return getRegisterBit(POWERS_PPM_REG_13H, 6);
        }

        // Input Current Limit in effect while Input Current Optimizer (ICO) is enabled
        // Range: 100 ~ 3250 mA
        bool setInputCurrentLimitOptimizer(uint16_t milliampere)
        {
            if (milliampere % BQ25896_IN_CURRENT_OPT_STEP)
            {
                log_e("Mistake ! The steps is must %u mA", BQ25896_IN_CURRENT_OPT_STEP);
                return false;
            }
            if (milliampere < BQ25896_IN_CURRENT_OPT_MIN)
            {
                milliampere = BQ25896_IN_CURRENT_OPT_MIN;
            }
            if (milliampere > BQ25896_IN_CURRENT_OPT_MAX)
            {
                milliampere = BQ25896_IN_CURRENT_OPT_MAX;
            }
            int val = readRegister(POWERS_PPM_REG_13H);
            if (val == -1)
                return false;
            val         &= 0x3F;
            milliampere  = ((milliampere - BQ25896_IN_CURRENT_OPT_MIN) / BQ25896_IN_CURRENT_STEP);
            val         |= milliampere;
            return writeRegister(POWERS_PPM_REG_13H, val) != -1;
        }

        /***************************************************
         * POWERS_PPM_REG_14H ✅
         **************************************************/

        // Input Current Optimizer (ICO) Status
        // true – Optimization is in progress
        // false – Maximum Input Current Detected
        bool isInputCurrentOptimizer()
        {
            return getRegisterBit(POWERS_PPM_REG_14H, 6);
        }

        // Device Configuration
        uint8_t getDeviceConfig()
        {
            int val = readRegister(POWERS_PPM_REG_14H);
            if (val == -1)
                return 0;
            return (val >> 3) & 0x03;
        }

    protected:
        bool initImpl()
        {
            __user_disable_charge = false;
            setChipModel(PMICChipModel::BQ25896);

            uint8_t rev = getChipID();
            if (rev != BQ25896_DEV_REV)
            {
                return false;
            }
            // Set the minimum operating voltage. Below this voltage, the PMU will protect
            // setSysPowerDownVoltage(3300);

            // Default disable Watchdog
            disableWatchdog();

            return true;
        }

        union REG0C r0C;
        union REG07 r07;
        union REG0B r0B;
        union REG0E r0E;
        union REG0F r0F;
        union REG10 r10;
        union REG11 r11;
        union REG12 r12;
        union REG13 r13;
    };
} // namespace HAL::PMIC