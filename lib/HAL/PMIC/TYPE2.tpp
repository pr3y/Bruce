#include "TYPE2.hpp"
namespace HAL
{
    namespace PMIC
    {
        template <class Driver>
        bool Type2<Driver>::init()
        {
            return begin();
        }

        template <class Driver>
        void Type2<Driver>::deinit()
        {
            end();
        }

        template <class Driver>
        bool Type2<Driver>::isEnableCharge()
        {
            return getRegisterBit(POWERS_PPM_REG_03H, 4);
        }

        template <class Driver>
        void Type2<Driver>::disableCharge()
        {
            __user_disable_charge = true;
            clrRegisterBit(POWERS_PPM_REG_03H, 4);
        }

        template <class Driver>
        void Type2<Driver>::enableCharge()
        {
            __user_disable_charge = false;
            setRegisterBit(POWERS_PPM_REG_03H, 4);
        }

        template <class Driver>
        bool Type2<Driver>::isEnableOTG()
        {
            return getRegisterBit(POWERS_PPM_REG_03H, 5);
        }

        template <class Driver>
        void Type2<Driver>::disableOTG()
        {
            clrRegisterBit(POWERS_PPM_REG_03H, 5);
            /*
             * After turning on the OTG function, the charging function will
             * be automatically disabled. If the user does not disable the charging
             * function, the charging function will be automatically enabled after
             * turning off the OTG output.
             * */
            if (!__user_disable_charge)
            {
                setRegisterBit(POWERS_PPM_REG_03H, 4);
            }
        }

        template <class Driver>
        bool Type2<Driver>::enableOTG()
        {
            if (isVbusIn())
                return false;
            return setRegisterBit(POWERS_PPM_REG_03H, 5);
        }

        template <class Driver>
        void Type2<Driver>::feedWatchdog()
        {
            setRegisterBit(POWERS_PPM_REG_03H, 6);
        }

        // Charging Termination Enable
        template <class Driver>
        void Type2<Driver>::enableChargingTermination()
        {
            setRegisterBit(POWERS_PPM_REG_07H, 7);
        }

        // Charging Termination Enable
        template <class Driver>
        void Type2<Driver>::disableChargingTermination()
        {
            clrRegisterBit(POWERS_PPM_REG_07H, 7);
        }

        // Charging Termination Enable
        template <class Driver>
        bool Type2<Driver>::isEnableChargingTermination()
        {
            return getRegisterBit(POWERS_PPM_REG_07H, 7);
        }

        // STAT Pin function
        template <class Driver>
        void Type2<Driver>::disableStatPin()
        {
            setRegisterBit(POWERS_PPM_REG_07H, 6);
        }

        template <class Driver>
        void Type2<Driver>::enableStatPin()
        {
            clrRegisterBit(POWERS_PPM_REG_07H, 6);
        }

        template <class Driver>
        bool Type2<Driver>::isEnableStatPin()
        {
            return getRegisterBit(POWERS_PPM_REG_07H, 6) == false;
        }

        // I2C Watchdog Timer Setting
        template <class Driver>
        bool Type2<Driver>::isEnableWatchdog()
        {
            int regVal = readRegister(POWERS_PPM_REG_07H);
            if (regVal == -1)
            {
                log_e("Config watch dog failed!");
                return false;
            }
            regVal >>= 4;
            return regVal & 0x03;
        }

        template <class Driver>
        void Type2<Driver>::disableWatchdog()
        {
            int regVal = readRegister(POWERS_PPM_REG_07H);
            regVal &= 0xCF;
            writeRegister(POWERS_PPM_REG_07H, regVal);
        }

        template <class Driver>
        void Type2<Driver>::enableWatchdog(enum Timeout val)
        {
            int regVal = readRegister(POWERS_PPM_REG_07H);
            regVal &= 0xCF;
            switch (val)
            {
            case TIMER_OUT_40SEC:
                writeRegister(POWERS_PPM_REG_07H, regVal | 0x10);
                break;
            case TIMER_OUT_80SEC:
                writeRegister(POWERS_PPM_REG_07H, regVal | 0x20);
                break;
            case TIMER_OUT_160SEC:
                writeRegister(POWERS_PPM_REG_07H, regVal | 0x30);
                break;
            default:
                break;
            }
        }

        template <class Driver>
        void Type2<Driver>::disableChargingSafetyTimer()
        {
            clrRegisterBit(POWERS_PPM_REG_07H, 3);
        }

        template <class Driver>
        void Type2<Driver>::enableChargingSafetyTimer()
        {
            setRegisterBit(POWERS_PPM_REG_07H, 3);
        }

        template <class Driver>
        bool Type2<Driver>::isEnableChargingSafetyTimer()
        {
            return getRegisterBit(POWERS_PPM_REG_07H, 3);
        }

        template <class Driver>
        void Type2<Driver>::setFastChargeTimer(FastChargeTimer timer)
        {
            int val;
            switch (timer)
            {
            case FAST_CHARGE_TIMER_5H:
            case FAST_CHARGE_TIMER_8H:
            case FAST_CHARGE_TIMER_12H:
            case FAST_CHARGE_TIMER_20H:
                val = readRegister(POWERS_PPM_REG_07H);
                if (val == -1)
                    return;
                val &= 0xF1;
                val |= (timer << 1);
                writeRegister(POWERS_PPM_REG_07H, val);
                break;
            default:
                break;
            }
        }

        template <class Driver>
        FastChargeTimer Type2<Driver>::getFastChargeTimer()
        {
            int val = readRegister(POWERS_PPM_REG_07H);
            return static_cast<FastChargeTimer>((val & 0x0E) >> 1);
        }

        // Return  Battery Load status
        template <class Driver>
        bool Type2<Driver>::isEnableBatLoad()
        {
            return getRegisterBit(POWERS_PPM_REG_03H, 7);
        }

        // Battery Load (10mA) Disable
        template <class Driver>
        void Type2<Driver>::disableBatLoad()
        {
            clrRegisterBit(POWERS_PPM_REG_03H, 7);
        }

        // Battery Load (10mA) Enable
        template <class Driver>
        void Type2<Driver>::enableBatLoad()
        {
            setRegisterBit(POWERS_PPM_REG_03H, 7);
        }

        template <class Driver>
        bool Type2<Driver>::enableMeasure(MeasureMode mode = CONTINUOUS)
        {
            int val = readRegister(POWERS_PPM_REG_02H);
            switch (mode)
            {
            case CONTINUOUS:
                val |= _BV(6);
                break;
            case ONE_SHORT:
                val &= (~_BV(6));
            default:
                break;
            }
            val |= _BV(7);
            return writeRegister(POWERS_PPM_REG_02H, val) != -1;
        }

        template <class Driver>
        bool Type2<Driver>::disableADCMeasure()
        {
            int val = readRegister(POWERS_PPM_REG_02H);
            if (val == -1)
            {
                return false;
            }
            val &= (~_BV(7));
            return writeRegister(POWERS_PPM_REG_02H, val) != 1;
        }

        template <class Driver>
        bool Type2<Driver>::setBoostFreq(BoostFreq freq)
        {
            switch (freq)
            {
            case BOOST_FREQ_500KHZ:
                return setRegisterBit(POWERS_PPM_REG_02H, 5);
            case BOOST_FREQ_1500KHZ:
                return clrRegisterBit(POWERS_PPM_REG_02H, 5);
            default:
                break;
            }
            return false;
        }

        template <class Driver>
        BoostFreq Type2<Driver>::getBoostFreq()
        {
            return getRegisterBit(POWERS_PPM_REG_02H, 5) ? BOOST_FREQ_500KHZ : BOOST_FREQ_1500KHZ;
        }

        // Enable Force Input Detection , Force PSEL detection
        template <class Driver>
        void Type2<Driver>::enableInputDetection()
        {
            setRegisterBit(POWERS_PPM_REG_02H, 1);
        }

        // Disable Force Input Detection ,  Not in PSEL detection (default)
        template <class Driver>
        void Type2<Driver>::disableInputDetection()
        {
            clrRegisterBit(POWERS_PPM_REG_02H, 1);
        }

        // Get Force DP/DM detection
        template <class Driver>
        bool Type2<Driver>::isEnableInputDetection()
        {
            return getRegisterBit(POWERS_PPM_REG_02H, 1);
        }

        // Enable PSEL detection when VBUS is plugged-in (default)
        template <class Driver>
        void Type2<Driver>::enableAutomaticInputDetection()
        {
            setRegisterBit(POWERS_PPM_REG_02H, 0);
        }

        // Disable PSEL detection when VBUS is plugged-in
        template <class Driver>
        void Type2<Driver>::disableAutomaticInputDetection()
        {
            clrRegisterBit(POWERS_PPM_REG_02H, 0);
        }

        // Get DPDM detection when BUS is plugged-in.
        template <class Driver>
        bool Type2<Driver>::isEnableAutomaticInputDetection()
        {
            return getRegisterBit(POWERS_PPM_REG_02H, 0);
        }

        template <class Driver>
        void Type2<Driver>::enterHizMode()
        {
            setRegisterBit(POWERS_PPM_REG_00H, 7);
        }

        template <class Driver>
        void Type2<Driver>::exitHizMode()
        {
            clrRegisterBit(POWERS_PPM_REG_00H, 7);
        }

        template <class Driver>
        bool Type2<Driver>::isHizMode()
        {
            return getRegisterBit(POWERS_PPM_REG_00H, 7);
        }

        // Enable ILIM Pin
        template <class Driver>
        void Type2<Driver>::enableCurrentLimitPin()
        {
            setRegisterBit(POWERS_PPM_REG_00H, 6);
        }

        template <class Driver>
        void Type2<Driver>::disableCurrentLimitPin()
        {
            clrRegisterBit(POWERS_PPM_REG_00H, 6);
        }

        template <class Driver>
        bool Type2<Driver>::isEnableCurrentLimitPin()
        {
            return getRegisterBit(POWERS_PPM_REG_00H, 6);
        }

        template <class Driver>
        bool Type2<Driver>::isOTG()
        {
            return getBusStatus() == BUS_STATE_OTG;
        }

        template <class Driver>
        bool Type2<Driver>::isCharging(void)
        {
            return chargeStatus() != CHARGE_STATE_NO_CHARGE;
        }

        template <class Driver>
        bool Type2<Driver>::isChargeDone()
        {
            return chargeStatus() != CHARGE_STATE_DONE;
        }

        template <class Driver>
        BusStatus Type2<Driver>::getBusStatus()
        {
            int val = readRegister(POWERS_PPM_REG_0BH);
            return (BusStatus)((val >> 5) & 0x07);
        }

        template <class Driver>
        ChargeStatus Type2<Driver>::chargeStatus()
        {
            int val = readRegister(POWERS_PPM_REG_0BH);
            if (val == -1)
                return CHARGE_STATE_UNKOWN;
            return static_cast<ChargeStatus>((val >> 3) & 0x03);
        }

        template <class Driver>
        void Type2<Driver>::shutdown()
        {
            disableBatterPowerPath();
        }

        // Close battery power path
        template <class Driver>
        void Type2<Driver>::disableBatterPowerPath()
        {
            setRegisterBit(POWERS_PPM_REG_09H, 5); // Force BATFET Off : BATFET_DIS
        }

        // Enable battery power path
        template <class Driver>
        void Type2<Driver>::enableBatterPowerPath()
        {
            clrRegisterBit(POWERS_PPM_REG_09H, 5); // Force BATFET Off : BATFET_DIS
        }

        template <class Driver>
        void Type2<Driver>::resetDefault()
        {
            setRegisterBit(POWERS_PPM_REG_14H, 7);
        }

        template <class Driver>
        uint8_t Type2<Driver>::getChipID()
        {
            int val = readRegister(POWERS_PPM_REG_14H);
            if (val == -1)
                return 0;
            return (val & 0x03);
        }
    }
}