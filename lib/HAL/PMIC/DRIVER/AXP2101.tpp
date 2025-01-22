#include "../REG/AXP2101.hpp"
#include "../IMPL/XPOWERS.tpp"
namespace HAL::PMIC
{
    class AXP2101 : public XPOWERS<class AXP2101>
    {
        friend class Base<AXP2101>;

    public:
        AXP2101(TwoWire &wire, int sda = SDA, int scl = SCL, uint8_t addr = AXP2101_SLAVE_ADDRESS)
        {
            myWire = &wire;
            mySDA  = sda;
            mySCL  = scl;
            myADDR = addr;
        }

        AXP2101()
        {
            myWire = &Wire;
            mySDA  = SDA;
            mySCL  = SCL;
            myADDR = AXP2101_SLAVE_ADDRESS;
        }

        ~AXP2101()
        {
            log_i("~AXP2101");
            deinit();
        }

        bool init(TwoWire &wire, int sda = SDA, int scl = SCL, uint8_t addr = AXP2101_SLAVE_ADDRESS)
        {
            myWire = &wire;
            mySDA  = sda;
            mySCL  = scl;
            myADDR = addr;
            return begin();
        }

        bool init()
        {
            return begin();
        }

        void deinit()
        {
            end();
        }

        /*
         * PMU status functions
         */
        uint16_t status()
        {
            uint16_t status1 = readRegister(AXP2101_STATUS1) & 0x1F;
            uint16_t status2 = readRegister(AXP2101_STATUS2) & 0x1F;
            ;
            return (status1 << 8) | (status2);
        }

        bool isVbusGood(void)
        {
            return getRegisterBit(AXP2101_STATUS1, 5);
        }

        bool getBatfetState(void)
        {
            return getRegisterBit(AXP2101_STATUS1, 4);
        }

        // getBatPresentState
        bool isBatteryConnect(void)
        {
            return getRegisterBit(AXP2101_STATUS1, 3);
        }

        bool isBatInActiveModeState(void)
        {
            return getRegisterBit(AXP2101_STATUS1, 2);
        }

        bool getThermalRegulationStatus(void)
        {
            return getRegisterBit(AXP2101_STATUS1, 1);
        }

        bool getCurrentLimitStatus(void)
        {
            return getRegisterBit(AXP2101_STATUS1, 0);
        }

        bool isCharging(void)
        {
            return (readRegister(AXP2101_STATUS2) >> 5) == 0x01;
        }

        bool isDischarge(void)
        {
            return (readRegister(AXP2101_STATUS2) >> 5) == 0x02;
        }

        bool isStandby(void)
        {
            return (readRegister(AXP2101_STATUS2) >> 5) == 0x00;
        }

        bool isPowerOn(void)
        {
            return getRegisterBit(AXP2101_STATUS2, 4);
        }

        bool isPowerOff(void)
        {
            return getRegisterBit(AXP2101_STATUS2, 4);
        }

        bool isVbusIn(void)
        {
            return getRegisterBit(AXP2101_STATUS2, 3) == 0 && isVbusGood();
        }

        pmic_chg_status getChargerStatus(void)
        {
            int val = readRegister(AXP2101_STATUS2);
            if (val == -1)
                return AXP2101_CHG_STOP_STATE;
            val &= 0x07;
            return (pmic_chg_status)val;
        }

        /*
         * Data Buffer
         */

        bool writeDataBuffer(uint8_t *data, uint8_t size)
        {
            if (size > AXP2101_DATA_BUFFER_SIZE)
                return false;
            return writeRegister(AXP2101_DATA_BUFFER1, data, size);
        }

        bool readDataBuffer(uint8_t *data, uint8_t size)
        {
            if (size > AXP2101_DATA_BUFFER_SIZE)
                return false;
            return readRegister(AXP2101_DATA_BUFFER1, data, size);
        }

        /*
         * PMU common configuration
         */

        /**
         * @brief   Internal off-discharge enable for DCDC & LDO & SWITCH
         */

        void enableInternalDischarge(void)
        {
            setRegisterBit(AXP2101_COMMON_CONFIG, 5);
        }

        void disableInternalDischarge(void)
        {
            clrRegisterBit(AXP2101_COMMON_CONFIG, 5);
        }

        /**
         * @brief   PWROK PIN pull low to Restart
         */
        void enablePwrOkPinPullLow(void)
        {
            setRegisterBit(AXP2101_COMMON_CONFIG, 3);
        }

        void disablePwrOkPinPullLow(void)
        {
            clrRegisterBit(AXP2101_COMMON_CONFIG, 3);
        }

        void enablePwronShutPMIC(void)
        {
            setRegisterBit(AXP2101_COMMON_CONFIG, 2);
        }

        void disablePwronShutPMIC(void)
        {
            clrRegisterBit(AXP2101_COMMON_CONFIG, 2);
        }

        /**
         * @brief  Restart the SoC System, POWOFF/POWON and reset the related registers
         * @retval None
         */
        void reset(void)
        {
            setRegisterBit(AXP2101_COMMON_CONFIG, 1);
        }

        /**
         * @brief  Set shutdown, calling shutdown will turn off all power channels,
         *         only VRTC belongs to normal power supply
         * @retval None
         */
        void shutdown(void)
        {
            setRegisterBit(AXP2101_COMMON_CONFIG, 0);
        }

        /**
         * @brief  BATFET control / REG 12H
         * @note   DIE Over Temperature Protection Level1 Configuration
         * @param  opt: 0:115 , 1:125 , 2:135
         * @retval None
         */
        void setBatfetDieOverTempLevel1(uint8_t opt)
        {
            int val = readRegister(AXP2101_BATFET_CTRL);
            if (val == -1)
                return;
            val &= 0xF9;
            writeRegister(AXP2101_BATFET_CTRL, val | (opt << 1));
        }

        uint8_t getBatfetDieOverTempLevel1(void)
        {
            return (readRegister(AXP2101_BATFET_CTRL) & 0x06);
        }

        void enableBatfetDieOverTempDetect(void)
        {
            setRegisterBit(AXP2101_BATFET_CTRL, 0);
        }

        void disableBatfetDieOverTempDetect(void)
        {
            clrRegisterBit(AXP2101_BATFET_CTRL, 0);
        }

        /**
         * @param  opt: 0:115 , 1:125 , 2:135
         */
        void setDieOverTempLevel1(uint8_t opt)
        {
            int val = readRegister(AXP2101_DIE_TEMP_CTRL);
            if (val == -1)
                return;
            val &= 0xF9;
            writeRegister(AXP2101_DIE_TEMP_CTRL, val | (opt << 1));
        }

        uint8_t getDieOverTempLevel1(void)
        {
            return (readRegister(AXP2101_DIE_TEMP_CTRL) & 0x06);
        }

        void enableDieOverTempDetect(void)
        {
            setRegisterBit(AXP2101_DIE_TEMP_CTRL, 0);
        }

        void disableDieOverTempDetect(void)
        {
            clrRegisterBit(AXP2101_DIE_TEMP_CTRL, 0);
        }

        // Linear Charger Vsys voltage dpm
        void setLinearChargerVsysDpm(pmic_chg_dpm opt)
        {
            int val = readRegister(AXP2101_MIN_SYS_VOL_CTRL);
            if (val == -1)
                return;
            val &= 0x8F;
            writeRegister(AXP2101_MIN_SYS_VOL_CTRL, val | (opt << 4));
        }

        uint8_t getLinearChargerVsysDpm(void)
        {
            int val = readRegister(AXP2101_MIN_SYS_VOL_CTRL);
            if (val == -1)
                return 0;
            val &= 0x70;
            return (val & 0x70) >> 4;
        }

        /**
         * @brief  Set VBUS Voltage Input Limit.
         * @param  opt: View the related chip type pmic_vbus_vol_limit enumeration
         *              parameters in "XPowersParams.hpp"
         */
        void setVbusVoltageLimit(pmic_vbus_vol_limit opt)
        {
            int val = readRegister(AXP2101_INPUT_VOL_LIMIT_CTRL);
            if (val == -1)
                return;
            val &= 0xF0;
            writeRegister(AXP2101_INPUT_VOL_LIMIT_CTRL, val | (opt & 0x0F));
        }

        /**
         * @brief  Get VBUS Voltage Input Limit.
         * @retval View the related chip type pmic_vbus_vol_limit enumeration
         *              parameters in "XPowersParams.hpp"
         */
        pmic_vbus_vol_limit getVbusVoltageLimit(void)
        {
            return (pmic_vbus_vol_limit)(readRegister(AXP2101_INPUT_VOL_LIMIT_CTRL) & 0x0F);
        }

        /**
         * @brief  Set VBUS Current Input Limit.
         * @param  opt: View the related chip type pmic_vbus_cur_limit enumeration
         *              parameters in "XPowersParams.hpp"
         * @retval true valid false invalid
         */
        bool setVbusCurrentLimit(pmic_vbus_cur_limit opt)
        {
            int val = readRegister(AXP2101_INPUT_CUR_LIMIT_CTRL);
            if (val == -1)
                return false;
            val &= 0xF8;
            return 0 == writeRegister(AXP2101_INPUT_CUR_LIMIT_CTRL, val | (opt & 0x07));
        }

        /**
         * @brief  Get VBUS Current Input Limit.
         * @retval View the related chip type pmic_vbus_cur_limit_t enumeration
         *              parameters in "XPowersParams.hpp"
         */
        pmic_vbus_cur_limit getVbusCurrentLimit(void)
        {
            return (pmic_vbus_cur_limit)(readRegister(AXP2101_INPUT_CUR_LIMIT_CTRL) & 0x07);
        }

        /**
         * @brief  Reset the fuel gauge
         */
        void resetGauge(void)
        {
            setRegisterBit(AXP2101_RESET_FUEL_GAUGE, 3);
        }

        /**
         * @brief   reset the gauge besides reset
         */
        void resetGaugeBesides(void)
        {
            setRegisterBit(AXP2101_RESET_FUEL_GAUGE, 2);
        }

        /**
         * @brief Gauge Module
         */
        void enableGauge(void)
        {
            setRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 3);
        }

        void disableGauge(void)
        {
            clrRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 3);
        }

        /**
         * @brief  Button Battery charge
         */
        bool enableButtonBatteryCharge(void)
        {
            return setRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 2);
        }

        bool disableButtonBatteryCharge(void)
        {
            return clrRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 2);
        }

        bool isEnableButtonBatteryCharge()
        {
            return getRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 2);
        }

        // Button battery charge termination voltage setting
        bool setButtonBatteryChargeVoltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_BTN_VOL_STEPS)
            {
                log_e("Mistake ! Button battery charging step voltage is %u mV", AXP2101_BTN_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_BTN_VOL_MIN)
            {
                log_e("Mistake ! The minimum charge termination voltage of the coin cell battery is %u mV", AXP2101_BTN_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_BTN_VOL_MAX)
            {
                log_e("Mistake ! The minimum charge termination voltage of the coin cell battery is %u mV", AXP2101_BTN_VOL_MAX);
                return false;
            }
            int val = readRegister(AXP2101_BTN_BAT_CHG_VOL_SET);
            if (val == -1)
                return 0;
            val &= 0xF8;
            val |= (millivolt - AXP2101_BTN_VOL_MIN) / AXP2101_BTN_VOL_STEPS;
            return 0 == writeRegister(AXP2101_BTN_BAT_CHG_VOL_SET, val);
        }

        uint16_t getButtonBatteryVoltage(void)
        {
            int val = readRegister(AXP2101_BTN_BAT_CHG_VOL_SET);
            if (val == -1)
                return 0;
            return (val & 0x07) * AXP2101_BTN_VOL_STEPS + AXP2101_BTN_VOL_MIN;
        }

        /**
         * @brief Cell Battery charge
         */
        void enableCellbatteryCharge(void)
        {
            setRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 1);
        }

        void disableCellbatteryCharge(void)
        {
            clrRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 1);
        }

        /**
         * @brief  Watchdog Module
         */
        void enableWatchdog(void)
        {
            setRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 0);
            enableIRQ(AXP2101_WDT_EXPIRE_IRQ);
        }

        void disableWatchdog(void)
        {
            disableIRQ(AXP2101_WDT_EXPIRE_IRQ);
            clrRegisterBit(AXP2101_CHARGE_GAUGE_WDT_CTRL, 0);
        }

        /**
         * @brief Watchdog Config
         * @note
         * @param  opt: 0: IRQ Only 1: IRQ and System reset  2: IRQ, System Reset and Pull down PWROK 1s  3: IRQ, System Reset, DCDC/LDO PWROFF & PWRON
         * @retval None
         */
        void setWatchdogConfig(pmic_wdt_config opt)
        {
            int val = readRegister(AXP2101_WDT_CTRL);
            if (val == -1)
                return;
            val &= 0xCF;
            writeRegister(AXP2101_WDT_CTRL, val | (opt << 4));
        }

        pmic_wdt_config getWatchConfig(void)
        {
            return (pmic_wdt_config)((readRegister(AXP2101_WDT_CTRL) & 0x30) >> 4);
        }

        void clrWatchdog(void)
        {
            setRegisterBit(AXP2101_WDT_CTRL, 3);
        }

        void setWatchdogTimeout(pmic_wdt_timeout opt)
        {
            int val = readRegister(AXP2101_WDT_CTRL);
            if (val == -1)
                return;
            val &= 0xF8;
            writeRegister(AXP2101_WDT_CTRL, val | opt);
        }

        pmic_wdt_timeout getWatchdogTimerout(void)
        {
            return (pmic_wdt_timeout)(readRegister(AXP2101_WDT_CTRL) & 0x07);
        }

        /**
         * @brief  Low battery warning threshold 5-20%, 1% per step
         * @param  percentage:   5 ~ 20
         * @retval None
         */
        void setLowBatWarnThreshold(uint8_t percentage)
        {
            if (percentage < 5 || percentage > 20)
                return;
            int val = readRegister(AXP2101_LOW_BAT_WARN_SET);
            if (val == -1)
                return;
            val &= 0x0F;
            writeRegister(AXP2101_LOW_BAT_WARN_SET, val | ((percentage - 5) << 4));
        }

        uint8_t getLowBatWarnThreshold(void)
        {
            int val = readRegister(AXP2101_LOW_BAT_WARN_SET);
            if (val == -1)
                return 0;
            val  &= 0xF0;
            val >>= 4;
            return val;
        }

        /**
         * @brief  Low battery shutdown threshold 0-15%, 1% per step
         * @param  opt:   0 ~ 15
         * @retval None
         */
        void setLowBatShutdownThreshold(uint8_t opt)
        {
            if (opt > 15)
            {
                opt = 15;
            }
            int val = readRegister(AXP2101_LOW_BAT_WARN_SET);
            if (val == -1)
                return;
            val &= 0xF0;
            writeRegister(AXP2101_LOW_BAT_WARN_SET, val | opt);
        }

        uint8_t getLowBatShutdownThreshold(void)
        {
            return (readRegister(AXP2101_LOW_BAT_WARN_SET) & 0x0F);
        }

        //!  PWRON statu  20
        // POWERON always high when EN Mode as POWERON Source
        bool isPoweronAlwaysHighSource()
        {
            return getRegisterBit(AXP2101_PWRON_STATUS, 5);
        }

        // Battery Insert and Good as POWERON Source
        bool isBattInsertOnSource()
        {
            return getRegisterBit(AXP2101_PWRON_STATUS, 4);
        }

        // Battery Voltage > 3.3V when Charged as Source
        bool isBattNormalOnSource()
        {
            return getRegisterBit(AXP2101_PWRON_STATUS, 3);
        }

        // Vbus Insert and Good as POWERON Source
        bool isVbusInsertOnSource()
        {
            return getRegisterBit(AXP2101_PWRON_STATUS, 2);
        }

        // IRQ PIN Pull-down as POWERON Source
        bool isIrqLowOnSource()
        {
            return getRegisterBit(AXP2101_PWRON_STATUS, 1);
        }

        // POWERON low for on level when POWERON Mode as POWERON Source
        bool isPwronLowOnSource()
        {
            return getRegisterBit(AXP2101_PWRON_STATUS, 0);
        }

        pmic_power_on_source getPowerOnSource()
        {
            int val = readRegister(AXP2101_PWRON_STATUS);
            if (val == -1)
                return AXP2101_POWERON_SRC_UNKONW;
            return (pmic_power_on_source)val;
        }

        //!  PWROFF status  21
        // Die Over Temperature as POWEROFF Source
        bool isOverTemperatureOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 7);
        }

        // DCDC Over Voltage as POWEROFF Source
        bool isDcOverVoltageOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 6);
        }

        // DCDC Under Voltage as POWEROFF Source
        bool isDcUnderVoltageOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 5);
        }

        // VBUS Over Voltage as POWEROFF Source
        bool isVbusOverVoltageOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 4);
        }

        // Vsys Under Voltage as POWEROFF Source
        bool isVsysUnderVoltageOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 3);
        }

        // POWERON always low when EN Mode as POWEROFF Source
        bool isPwronAlwaysLowOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 2);
        }

        // Software configuration as POWEROFF Source
        bool isSwConfigOffSource()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 1);
        }

        // POWERON Pull down for off level when POWERON Mode as POWEROFF Source
        bool isPwrSourcePullDown()
        {
            return getRegisterBit(AXP2101_PWROFF_STATUS, 0);
        }

        pmic_power_off_source getPowerOffSource()
        {
            int val = readRegister(AXP2101_PWROFF_STATUS);
            if (val == -1)
                return AXP2101_POWEROFF_SRC_UNKONW;
            return (pmic_power_off_source)val;
        }

        //! REG 22H
        void enableOverTemperatureLevel2PowerOff()
        {
            setRegisterBit(AXP2101_PWROFF_EN, 2);
        }

        void disableOverTemperaturePowerOff()
        {
            clrRegisterBit(AXP2101_PWROFF_EN, 2);
        }

        // CHANGE:  void enablePwrOnOverVolOffLevelPowerOff()
        void enableLongPressShutdown()
        {
            setRegisterBit(AXP2101_PWROFF_EN, 1);
        }

        // CHANGE:  void disablePwrOnOverVolOffLevelPowerOff()
        void disableLongPressShutdown()
        {
            clrRegisterBit(AXP2101_PWROFF_EN, 1);
        }

        // CHANGE: void enablePwrOffSelectFunction()
        void setLongPressRestart()
        {
            setRegisterBit(AXP2101_PWROFF_EN, 0);
        }

        // CHANGE: void disablePwrOffSelectFunction()
        void setLongPressPowerOFF()
        {
            clrRegisterBit(AXP2101_PWROFF_EN, 0);
        }

        //! REG 23H
        // DCDC 120%(130%) high voltage turn off PMIC function
        void enableDCHighVoltageTurnOff()
        {
            setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 5);
        }

        void disableDCHighVoltageTurnOff()
        {
            clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 5);
        }

        // DCDC5 85% low voltage turn Off PMIC function
        void enableDC5LowVoltageTurnOff()
        {
            setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 4);
        }

        void disableDC5LowVoltageTurnOff()
        {
            clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 4);
        }

        // DCDC4 85% low voltage turn Off PMIC function
        void enableDC4LowVoltageTurnOff()
        {
            setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 3);
        }

        void disableDC4LowVoltageTurnOff()
        {
            clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 3);
        }

        // DCDC3 85% low voltage turn Off PMIC function
        void enableDC3LowVoltageTurnOff()
        {
            setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 2);
        }

        void disableDC3LowVoltageTurnOff()
        {
            clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 2);
        }

        // DCDC2 85% low voltage turn Off PMIC function
        void enableDC2LowVoltageTurnOff()
        {
            setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 1);
        }

        void disableDC2LowVoltageTurnOff()
        {
            clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 1);
        }

        // DCDC1 85% low voltage turn Off PMIC function
        void enableDC1LowVoltageTurnOff()
        {
            setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 0);
        }

        void disableDC1LowVoltageTurnOff()
        {
            clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 0);
        }

        // Set the minimum system operating voltage inside the PMU,
        // below this value will shut down the PMU,Adjustment range 2600mV~3300mV
        bool setSysPowerDownVoltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_VSYS_VOL_THRESHOLD_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_VSYS_VOL_THRESHOLD_STEPS);
                return false;
            }
            if (millivolt < AXP2101_VSYS_VOL_THRESHOLD_MIN)
            {
                log_e("Mistake ! The minimum settable voltage of VSYS is %u mV", AXP2101_VSYS_VOL_THRESHOLD_MIN);
                return false;
            }
            else if (millivolt > AXP2101_VSYS_VOL_THRESHOLD_MAX)
            {
                log_e("Mistake ! The maximum settable voltage of VSYS is %u mV", AXP2101_VSYS_VOL_THRESHOLD_MAX);
                return false;
            }
            int val = readRegister(AXP2101_VOFF_SET);
            if (val == -1)
                return false;
            val &= 0xF8;
            return 0 == writeRegister(AXP2101_VOFF_SET, val | ((millivolt - AXP2101_VSYS_VOL_THRESHOLD_MIN) / AXP2101_VSYS_VOL_THRESHOLD_STEPS));
        }

        uint16_t getSysPowerDownVoltage(void)
        {
            int val = readRegister(AXP2101_VOFF_SET);
            if (val == -1)
                return false;
            return (val & 0x07) * AXP2101_VSYS_VOL_THRESHOLD_STEPS + AXP2101_VSYS_VOL_THRESHOLD_MIN;
        }

        //  PWROK setting and PWROFF sequence control 25.
        // Check the PWROK Pin enable after all dcdc/ldo output valid 128ms
        void enablePwrOk()
        {
            setRegisterBit(AXP2101_PWROK_SEQU_CTRL, 4);
        }

        void disablePwrOk()
        {
            clrRegisterBit(AXP2101_PWROK_SEQU_CTRL, 4);
        }

        // POWEROFF Delay 4ms after PWROK enable
        void enablePowerOffDelay()
        {
            setRegisterBit(AXP2101_PWROK_SEQU_CTRL, 3);
        }

        // POWEROFF Delay 4ms after PWROK disable
        void disablePowerOffDelay()
        {
            clrRegisterBit(AXP2101_PWROK_SEQU_CTRL, 3);
        }

        // POWEROFF Sequence Control the reverse of the Startup
        void enablePowerSequence()
        {
            setRegisterBit(AXP2101_PWROK_SEQU_CTRL, 2);
        }

        // POWEROFF Sequence Control at the same time
        void disablePowerSequence()
        {
            clrRegisterBit(AXP2101_PWROK_SEQU_CTRL, 2);
        }

        // Delay of PWROK after all power output good
        bool setPwrOkDelay(pmic_pwrok_delay opt)
        {
            int val = readRegister(AXP2101_PWROK_SEQU_CTRL);
            if (val == -1)
                return false;
            val &= 0xFC;
            return 0 == writeRegister(AXP2101_PWROK_SEQU_CTRL, val | opt);
        }

        pmic_pwrok_delay getPwrOkDelay()
        {
            int val = readRegister(AXP2101_PWROK_SEQU_CTRL);
            if (val == -1)
                return AXP2101_PWROK_DELAY_8MS;
            return (pmic_pwrok_delay)(val & 0x03);
        }

        //  Sleep and 26
        void wakeupControl(pmic_wakeup opt, bool enable)
        {
            int val = readRegister(AXP2101_SLEEP_WAKEUP_CTRL);
            if (val == -1)
                return;
            enable ? (val |= opt) : (val &= (~opt));
            writeRegister(AXP2101_SLEEP_WAKEUP_CTRL, val);
        }

        bool enableWakeup(void)
        {
            return setRegisterBit(AXP2101_SLEEP_WAKEUP_CTRL, 1);
        }

        bool disableWakeup(void)
        {
            return clrRegisterBit(AXP2101_SLEEP_WAKEUP_CTRL, 1);
        }

        bool enableSleep(void)
        {
            return setRegisterBit(AXP2101_SLEEP_WAKEUP_CTRL, 0);
        }

        bool disableSleep(void)
        {
            return clrRegisterBit(AXP2101_SLEEP_WAKEUP_CTRL, 0);
        }

        //  RQLEVEL/OFFLEVEL/ONLEVEL setting 27
        /**
         * @brief  IRQLEVEL configur
         * @param  opt: 0:1s  1:1.5s  2:2s 3:2.5s
         */
        void setIrqLevel(uint8_t opt)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return;
            val &= 0xFC;
            writeRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 4));
        }

        /**
         * @brief  OFFLEVEL configuration
         * @param  opt:  0:4s 1:6s 2:8s 3:10s
         */
        void setOffLevel(uint8_t opt)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return;
            writeRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 2));
        }

        /**
         * @brief  ONLEVEL configuration
         * @param  opt: 0:128ms 1:512ms 2:1s  3:2s
         */
        void setOnLevel(uint8_t opt)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return;
            writeRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | opt);
        }

        // Fast pwron setting 0  28
        // Fast Power On Start Sequence
        void setDc4FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET0);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET0, val | ((opt & 0x3) << 6));
        }

        void setDc3FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET0);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET0, val | ((opt & 0x3) << 4));
        }
        void setDc2FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET0);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET0, val | ((opt & 0x3) << 2));
        }
        void setDc1FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET0);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET0, val | (opt & 0x3));
        }

        //  Fast pwron setting 1  29
        void setAldo3FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET1);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET1, val | ((opt & 0x3) << 6));
        }
        void setAldo2FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET1);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET1, val | ((opt & 0x3) << 4));
        }
        void setAldo1FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET1);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET1, val | ((opt & 0x3) << 2));
        }

        void setDc5FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET1);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET1, val | (opt & 0x3));
        }

        //  Fast pwron setting 2  2A
        void setCpuldoFastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET2);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET2, val | ((opt & 0x3) << 6));
        }

        void setBldo2FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET2);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET2, val | ((opt & 0x3) << 4));
        }

        void setBldo1FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET2);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET2, val | ((opt & 0x3) << 2));
        }

        void setAldo4FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_SET2);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_SET2, val | (opt & 0x3));
        }

        //  Fast pwron setting 3  2B
        void setDldo2FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_CTRL);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_CTRL, val | ((opt & 0x3) << 2));
        }

        void setDldo1FastStartSequence(pmic_start_sequence opt)
        {
            int val = readRegister(AXP2101_FAST_PWRON_CTRL);
            if (val == -1)
                return;
            writeRegister(AXP2101_FAST_PWRON_CTRL, val | (opt & 0x3));
        }

        /**
         * @brief   Setting Fast Power On Start Sequence
         */
        void setFastPowerOnLevel(pmic_fast_on_opt opt, pmic_start_sequence seq_level)
        {
            uint8_t val = 0;
            switch (opt)
            {
                case AXP2101_FAST_DCDC1:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val | seq_level);
                    break;
                case AXP2101_FAST_DCDC2:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val | (seq_level << 2));
                    break;
                case AXP2101_FAST_DCDC3:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val | (seq_level << 4));
                    break;
                case AXP2101_FAST_DCDC4:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val | (seq_level << 6));
                    break;
                case AXP2101_FAST_DCDC5:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val | seq_level);
                    break;
                case AXP2101_FAST_ALDO1:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val | (seq_level << 2));
                    break;
                case AXP2101_FAST_ALDO2:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val | (seq_level << 4));
                    break;
                case AXP2101_FAST_ALDO3:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val | (seq_level << 6));
                    break;
                case AXP2101_FAST_ALDO4:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val | seq_level);
                    break;
                case AXP2101_FAST_BLDO1:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val | (seq_level << 2));
                    break;
                case AXP2101_FAST_BLDO2:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val | (seq_level << 4));
                    break;
                case AXP2101_FAST_CPUSLDO:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val | (seq_level << 6));
                    break;
                case AXP2101_FAST_DLDO1:
                    val = readRegister(AXP2101_FAST_PWRON_CTRL);
                    writeRegister(AXP2101_FAST_PWRON_CTRL, val | seq_level);
                    break;
                case AXP2101_FAST_DLDO2:
                    val = readRegister(AXP2101_FAST_PWRON_CTRL);
                    writeRegister(AXP2101_FAST_PWRON_CTRL, val | (seq_level << 2));
                    break;
                default:
                    break;
            }
        }

        void disableFastPowerOn(pmic_fast_on_opt opt)
        {
            uint8_t val = 0;
            switch (opt)
            {
                case AXP2101_FAST_DCDC1:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val & 0xFC);
                    break;
                case AXP2101_FAST_DCDC2:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val & 0xF3);
                    break;
                case AXP2101_FAST_DCDC3:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val & 0xCF);
                    break;
                case AXP2101_FAST_DCDC4:
                    val = readRegister(AXP2101_FAST_PWRON_SET0);
                    writeRegister(AXP2101_FAST_PWRON_SET0, val & 0x3F);
                    break;
                case AXP2101_FAST_DCDC5:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val & 0xFC);
                    break;
                case AXP2101_FAST_ALDO1:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val & 0xF3);
                    break;
                case AXP2101_FAST_ALDO2:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val & 0xCF);
                    break;
                case AXP2101_FAST_ALDO3:
                    val = readRegister(AXP2101_FAST_PWRON_SET1);
                    writeRegister(AXP2101_FAST_PWRON_SET1, val & 0x3F);
                    break;
                case AXP2101_FAST_ALDO4:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val & 0xFC);
                    break;
                case AXP2101_FAST_BLDO1:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val & 0xF3);
                    break;
                case AXP2101_FAST_BLDO2:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val & 0xCF);
                    break;
                case AXP2101_FAST_CPUSLDO:
                    val = readRegister(AXP2101_FAST_PWRON_SET2);
                    writeRegister(AXP2101_FAST_PWRON_SET2, val & 0x3F);
                    break;
                case AXP2101_FAST_DLDO1:
                    val = readRegister(AXP2101_FAST_PWRON_CTRL);
                    writeRegister(AXP2101_FAST_PWRON_CTRL, val & 0xFC);
                    break;
                case AXP2101_FAST_DLDO2:
                    val = readRegister(AXP2101_FAST_PWRON_CTRL);
                    writeRegister(AXP2101_FAST_PWRON_CTRL, val & 0xF3);
                    break;
                default:
                    break;
            }
        }

        void enableFastPowerOn(void)
        {
            setRegisterBit(AXP2101_FAST_PWRON_CTRL, 7);
        }

        void disableFastPowerOn(void)
        {
            clrRegisterBit(AXP2101_FAST_PWRON_CTRL, 7);
        }

        void enableFastWakeup(void)
        {
            setRegisterBit(AXP2101_FAST_PWRON_CTRL, 6);
        }

        void disableFastWakeup(void)
        {
            clrRegisterBit(AXP2101_FAST_PWRON_CTRL, 6);
        }

        // DCDC 120%(130%) high voltage turn off PMIC function
        void setDCHighVoltagePowerDown(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 5) : clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 5);
        }

        bool getDCHighVoltagePowerDownEn()
        {
            return getRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 5);
        }

        // DCDCS force PWM control
        void setDcUVPDebounceTime(uint8_t opt)
        {
            int val  = readRegister(AXP2101_DC_FORCE_PWM_CTRL);
            val     &= 0xFC;
            writeRegister(AXP2101_DC_FORCE_PWM_CTRL, val | opt);
        }

        void settDC1WorkModeToPwm(uint8_t enable)
        {
            enable ? setRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 2)
                   : clrRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 2);
        }

        void settDC2WorkModeToPwm(uint8_t enable)
        {
            enable ? setRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 3)
                   : clrRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 3);
        }

        void settDC3WorkModeToPwm(uint8_t enable)
        {
            enable ? setRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 4)
                   : clrRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 4);
        }

        void settDC4WorkModeToPwm(uint8_t enable)
        {
            enable ? setRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 5)
                   : clrRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 5);
        }

        // 1 = 100khz 0=50khz
        void setDCFreqSpreadRange(uint8_t opt)
        {
            opt ? setRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 6)
                : clrRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 6);
        }

        void setDCFreqSpreadRangeEn(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 7)
               : clrRegisterBit(AXP2101_DC_FORCE_PWM_CTRL, 7);
        }

        void enableCCM()
        {
            setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 6);
        }

        void disableCCM()
        {
            clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 6);
        }

        bool isenableCCM()
        {
            return getRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 6);
        }

        // args:enum DVMRamp
        void setDVMRamp(DVMRamp opt)
        {
            if (opt > 2)
                return;
            opt == 0 ? clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 5) : setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 5);
        }

        /*
         * Power control DCDC1 functions
         */
        bool isEnableDC1(void)
        {
            return getRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 0);
        }

        bool enableDC1(void)
        {
            return setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 0);
        }

        bool disableDC1(void)
        {
            return clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 0);
        }

        bool setDC1Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_DCDC1_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_DCDC1_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_DCDC1_VOL_MIN)
            {
                log_e("Mistake ! DC1 minimum voltage is %u mV", AXP2101_DCDC1_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_DCDC1_VOL_MAX)
            {
                log_e("Mistake ! DC1 maximum voltage is %u mV", AXP2101_DCDC1_VOL_MAX);
                return false;
            }
            return 0 == writeRegister(AXP2101_DC_VOL0_CTRL, (millivolt - AXP2101_DCDC1_VOL_MIN) / AXP2101_DCDC1_VOL_STEPS);
        }

        uint16_t getDC1Voltage(void)
        {
            return (readRegister(AXP2101_DC_VOL0_CTRL) & 0x1F) * AXP2101_DCDC1_VOL_STEPS + AXP2101_DCDC1_VOL_MIN;
        }

        // DCDC1 85% low voltage turn off PMIC function
        void setDC1LowVoltagePowerDown(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 0) : clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 0);
        }

        bool getDC1LowVoltagePowerDownEn()
        {
            return getRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 0);
        }

        /*
         * Power control DCDC2 functions
         */
        bool isEnableDC2(void)
        {
            return getRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 1);
        }

        bool enableDC2(void)
        {
            return setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 1);
        }

        bool disableDC2(void)
        {
            return clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 1);
        }

        bool setDC2Voltage(uint16_t millivolt)
        {
            int val = readRegister(AXP2101_DC_VOL1_CTRL);
            if (val == -1)
                return 0;
            val &= 0x80;
            if (millivolt >= AXP2101_DCDC2_VOL1_MIN && millivolt <= AXP2101_DCDC2_VOL1_MAX)
            {
                if (millivolt % AXP2101_DCDC2_VOL_STEPS1)
                {
                    log_e("Mistake !  The steps is must %umV", AXP2101_DCDC2_VOL_STEPS1);
                    return false;
                }
                return 0 == writeRegister(AXP2101_DC_VOL1_CTRL, val | (millivolt - AXP2101_DCDC2_VOL1_MIN) / AXP2101_DCDC2_VOL_STEPS1);
            }
            else if (millivolt >= AXP2101_DCDC2_VOL2_MIN && millivolt <= AXP2101_DCDC2_VOL2_MAX)
            {
                if (millivolt % AXP2101_DCDC2_VOL_STEPS2)
                {
                    log_e("Mistake !  The steps is must %umV", AXP2101_DCDC2_VOL_STEPS2);
                    return false;
                }
                val |= (((millivolt - AXP2101_DCDC2_VOL2_MIN) / AXP2101_DCDC2_VOL_STEPS2) + AXP2101_DCDC2_VOL_STEPS2_BASE);
                return 0 == writeRegister(AXP2101_DC_VOL1_CTRL, val);
            }
            return false;
        }

        uint16_t getDC2Voltage(void)
        {
            int val = readRegister(AXP2101_DC_VOL1_CTRL);
            if (val == -1)
                return 0;
            val &= 0x7F;
            if (val < AXP2101_DCDC2_VOL_STEPS2_BASE)
            {
                return (val * AXP2101_DCDC2_VOL_STEPS1) + AXP2101_DCDC2_VOL1_MIN;
            }
            else
            {
                return (val * AXP2101_DCDC2_VOL_STEPS2) - 200;
            }
            return 0;
        }

        uint8_t getDC2WorkMode(void)
        {
            return getRegisterBit(AXP2101_DCDC2_VOL_STEPS2, 7);
        }

        void setDC2LowVoltagePowerDown(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 1) : clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 1);
        }

        bool getDC2LowVoltagePowerDownEn()
        {
            return getRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 1);
        }

        /*
         * Power control DCDC3 functions
         */

        bool isEnableDC3(void)
        {
            return getRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 2);
        }

        bool enableDC3(void)
        {
            return setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 2);
        }

        bool disableDC3(void)
        {
            return clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 2);
        }

        /**
            0.5~1.2V,10mV/step,71steps
            1.22~1.54V,20mV/step,17steps
            1.6~3.4V,100mV/step,19steps
         */
        bool setDC3Voltage(uint16_t millivolt)
        {
            int val = readRegister(AXP2101_DC_VOL2_CTRL);
            if (val == -1)
                return false;
            val &= 0x80;
            if (millivolt >= AXP2101_DCDC3_VOL1_MIN && millivolt <= AXP2101_DCDC3_VOL1_MAX)
            {
                if (millivolt % AXP2101_DCDC3_VOL_STEPS1)
                {
                    log_e("Mistake ! The steps is must %umV", AXP2101_DCDC3_VOL_STEPS1);
                    return false;
                }
                return 0 == writeRegister(AXP2101_DC_VOL2_CTRL, val | (millivolt - AXP2101_DCDC3_VOL_MIN) / AXP2101_DCDC3_VOL_STEPS1);
            }
            else if (millivolt >= AXP2101_DCDC3_VOL2_MIN && millivolt <= AXP2101_DCDC3_VOL2_MAX)
            {
                if (millivolt % AXP2101_DCDC3_VOL_STEPS2)
                {
                    log_e("Mistake ! The steps is must %umV", AXP2101_DCDC3_VOL_STEPS2);
                    return false;
                }
                val |= (((millivolt - AXP2101_DCDC3_VOL2_MIN) / AXP2101_DCDC3_VOL_STEPS2) + AXP2101_DCDC3_VOL_STEPS2_BASE);
                return 0 == writeRegister(AXP2101_DC_VOL2_CTRL, val);
            }
            else if (millivolt >= AXP2101_DCDC3_VOL3_MIN && millivolt <= AXP2101_DCDC3_VOL3_MAX)
            {
                if (millivolt % AXP2101_DCDC3_VOL_STEPS3)
                {
                    log_e("Mistake ! The steps is must %umV", AXP2101_DCDC3_VOL_STEPS3);
                    return false;
                }
                val |= (((millivolt - AXP2101_DCDC3_VOL3_MIN) / AXP2101_DCDC3_VOL_STEPS3) + AXP2101_DCDC3_VOL_STEPS3_BASE);
                return 0 == writeRegister(AXP2101_DC_VOL2_CTRL, val);
            }
            return false;
        }

        uint16_t getDC3Voltage(void)
        {
            int val = readRegister(AXP2101_DC_VOL2_CTRL) & 0x7F;
            if (val == -1)
                return 0;
            if (val < AXP2101_DCDC3_VOL_STEPS2_BASE)
            {
                return (val * AXP2101_DCDC3_VOL_STEPS1) + AXP2101_DCDC3_VOL_MIN;
            }
            else if (val >= AXP2101_DCDC3_VOL_STEPS2_BASE && val < AXP2101_DCDC3_VOL_STEPS3_BASE)
            {
                return (val * AXP2101_DCDC3_VOL_STEPS2) - 200;
            }
            else
            {
                return (val * AXP2101_DCDC3_VOL_STEPS3) - 7200;
            }
            return 0;
        }

        uint8_t getDC3WorkMode(void)
        {
            return getRegisterBit(AXP2101_DC_VOL2_CTRL, 7);
        }

        // DCDC3 85% low voltage turn off PMIC function
        void setDC3LowVoltagePowerDown(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 2) : clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 2);
        }

        bool getDC3LowVoltagePowerDownEn()
        {
            return getRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 2);
        }

        /*
         * Power control DCDC4 functions
         */
        /**
            0.5~1.2V,10mV/step,71steps
            1.22~1.84V,20mV/step,32steps
         */
        bool isEnableDC4(void)
        {
            return getRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 3);
        }

        bool enableDC4(void)
        {
            return setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 3);
        }

        bool disableDC4(void)
        {
            return clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 3);
        }

        bool setDC4Voltage(uint16_t millivolt)
        {
            int val = readRegister(AXP2101_DC_VOL3_CTRL);
            if (val == -1)
                return false;
            val &= 0x80;
            if (millivolt >= AXP2101_DCDC4_VOL1_MIN && millivolt <= AXP2101_DCDC4_VOL1_MAX)
            {
                if (millivolt % AXP2101_DCDC4_VOL_STEPS1)
                {
                    log_e("Mistake ! The steps is must %umV", AXP2101_DCDC4_VOL_STEPS1);
                    return false;
                }
                return 0 == writeRegister(AXP2101_DC_VOL3_CTRL, val | (millivolt - AXP2101_DCDC4_VOL1_MIN) / AXP2101_DCDC4_VOL_STEPS1);
            }
            else if (millivolt >= AXP2101_DCDC4_VOL2_MIN && millivolt <= AXP2101_DCDC4_VOL2_MAX)
            {
                if (millivolt % AXP2101_DCDC4_VOL_STEPS2)
                {
                    log_e("Mistake ! The steps is must %umV", AXP2101_DCDC4_VOL_STEPS2);
                    return false;
                }
                val |= (((millivolt - AXP2101_DCDC4_VOL2_MIN) / AXP2101_DCDC4_VOL_STEPS2) + AXP2101_DCDC4_VOL_STEPS2_BASE);
                return 0 == writeRegister(AXP2101_DC_VOL3_CTRL, val);
            }
            return false;
        }

        uint16_t getDC4Voltage(void)
        {
            int val = readRegister(AXP2101_DC_VOL3_CTRL);
            if (val == -1)
                return 0;
            val &= 0x7F;
            if (val < AXP2101_DCDC4_VOL_STEPS2_BASE)
            {
                return (val * AXP2101_DCDC4_VOL_STEPS1) + AXP2101_DCDC4_VOL1_MIN;
            }
            else
            {
                return (val * AXP2101_DCDC4_VOL_STEPS2) - 200;
            }
            return 0;
        }

        // DCDC4 85% low voltage turn off PMIC function
        void setDC4LowVoltagePowerDown(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 3) : clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 3);
        }

        bool getDC4LowVoltagePowerDownEn()
        {
            return getRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 3);
        }

        /*
         * Power control DCDC5 functions,Output to gpio pin
         */
        bool isEnableDC5(void)
        {
            return getRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 4);
        }

        bool enableDC5(void)
        {
            return setRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 4);
        }

        bool disableDC5(void)
        {
            return clrRegisterBit(AXP2101_DC_ONOFF_DVM_CTRL, 4);
        }

        bool setDC5Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_DCDC5_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_DCDC5_VOL_STEPS);
                return false;
            }
            if (millivolt != AXP2101_DCDC5_VOL_1200MV && millivolt < AXP2101_DCDC5_VOL_MIN)
            {
                log_e("Mistake ! DC5 minimum voltage is %umV ,%umV", AXP2101_DCDC5_VOL_1200MV, AXP2101_DCDC5_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_DCDC5_VOL_MAX)
            {
                log_e("Mistake ! DC5 maximum voltage is %umV", AXP2101_DCDC5_VOL_MAX);
                return false;
            }

            int val = readRegister(AXP2101_DC_VOL4_CTRL);
            if (val == -1)
                return false;
            val &= 0xE0;
            if (millivolt == AXP2101_DCDC5_VOL_1200MV)
            {
                return 0 == writeRegister(AXP2101_DC_VOL4_CTRL, val | AXP2101_DCDC5_VOL_VAL);
            }
            val |= (millivolt - AXP2101_DCDC5_VOL_MIN) / AXP2101_DCDC5_VOL_STEPS;
            return 0 == writeRegister(AXP2101_DC_VOL4_CTRL, val);
        }

        uint16_t getDC5Voltage(void)
        {
            int val = readRegister(AXP2101_DC_VOL4_CTRL);
            if (val == -1)
                return 0;
            val &= 0x1F;
            if (val == AXP2101_DCDC5_VOL_VAL)
                return AXP2101_DCDC5_VOL_1200MV;
            return (val * AXP2101_DCDC5_VOL_STEPS) + AXP2101_DCDC5_VOL_MIN;
        }

        bool isDC5FreqCompensationEn(void)
        {
            return getRegisterBit(AXP2101_DC_VOL4_CTRL, 5);
        }

        void enableDC5FreqCompensation()
        {
            setRegisterBit(AXP2101_DC_VOL4_CTRL, 5);
        }

        void disableFreqCompensation()
        {
            clrRegisterBit(AXP2101_DC_VOL4_CTRL, 5);
        }

        // DCDC4 85% low voltage turn off PMIC function
        void setDC5LowVoltagePowerDown(bool en)
        {
            en ? setRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 4) : clrRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 4);
        }

        bool getDC5LowVoltagePowerDownEn()
        {
            return getRegisterBit(AXP2101_DC_OVP_UVP_CTRL, 4);
        }

        /*
         * Power control ALDO1 functions
         */
        bool isEnableALDO1(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 0);
        }

        bool enableALDO1(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 0);
        }

        bool disableALDO1(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 0);
        }

        bool setALDO1Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_ALDO1_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_ALDO1_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_ALDO1_VOL_MIN)
            {
                log_e("Mistake ! ALDO1 minimum output voltage is  %umV", AXP2101_ALDO1_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_ALDO1_VOL_MAX)
            {
                log_e("Mistake ! ALDO1 maximum output voltage is  %umV", AXP2101_ALDO1_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL0_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_ALDO1_VOL_MIN) / AXP2101_ALDO1_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL0_CTRL, val);
        }

        uint16_t getALDO1Voltage(void)
        {
            uint16_t val = readRegister(AXP2101_LDO_VOL0_CTRL) & 0x1F;
            return val * AXP2101_ALDO1_VOL_STEPS + AXP2101_ALDO1_VOL_MIN;
        }

        /*
         * Power control ALDO2 functions
         */
        bool isEnableALDO2(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 1);
        }

        bool enableALDO2(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 1);
        }

        bool disableALDO2(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 1);
        }

        bool setALDO2Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_ALDO2_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_ALDO2_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_ALDO2_VOL_MIN)
            {
                log_e("Mistake ! ALDO2 minimum output voltage is  %umV", AXP2101_ALDO2_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_ALDO2_VOL_MAX)
            {
                log_e("Mistake ! ALDO2 maximum output voltage is  %umV", AXP2101_ALDO2_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL1_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_ALDO2_VOL_MIN) / AXP2101_ALDO2_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL1_CTRL, val);
        }

        uint16_t getALDO2Voltage(void)
        {
            uint16_t val = readRegister(AXP2101_LDO_VOL1_CTRL) & 0x1F;
            return val * AXP2101_ALDO2_VOL_STEPS + AXP2101_ALDO2_VOL_MIN;
        }

        /*
         * Power control ALDO3 functions
         */
        bool isEnableALDO3(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 2);
        }

        bool enableALDO3(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 2);
        }

        bool disableALDO3(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 2);
        }

        bool setALDO3Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_ALDO3_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_ALDO3_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_ALDO3_VOL_MIN)
            {
                log_e("Mistake ! ALDO3 minimum output voltage is  %umV", AXP2101_ALDO3_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_ALDO3_VOL_MAX)
            {
                log_e("Mistake ! ALDO3 maximum output voltage is  %umV", AXP2101_ALDO3_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL2_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_ALDO3_VOL_MIN) / AXP2101_ALDO3_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL2_CTRL, val);
        }

        uint16_t getALDO3Voltage(void)
        {
            uint16_t val = readRegister(AXP2101_LDO_VOL2_CTRL) & 0x1F;
            return val * AXP2101_ALDO3_VOL_STEPS + AXP2101_ALDO3_VOL_MIN;
        }

        /*
         * Power control ALDO4 functions
         */
        bool isEnableALDO4(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 3);
        }

        bool enableALDO4(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 3);
        }

        bool disableALDO4(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 3);
        }

        bool setALDO4Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_ALDO4_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_ALDO4_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_ALDO4_VOL_MIN)
            {
                log_e("Mistake ! ALDO4 minimum output voltage is  %umV", AXP2101_ALDO4_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_ALDO4_VOL_MAX)
            {
                log_e("Mistake ! ALDO4 maximum output voltage is  %umV", AXP2101_ALDO4_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL3_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_ALDO4_VOL_MIN) / AXP2101_ALDO4_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL3_CTRL, val);
        }

        uint16_t getALDO4Voltage(void)
        {
            uint16_t val = readRegister(AXP2101_LDO_VOL3_CTRL) & 0x1F;
            return val * AXP2101_ALDO4_VOL_STEPS + AXP2101_ALDO4_VOL_MIN;
        }

        /*
         * Power control BLDO1 functions
         */
        bool isEnableBLDO1(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 4);
        }

        bool enableBLDO1(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 4);
        }

        bool disableBLDO1(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 4);
        }

        bool setBLDO1Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_BLDO1_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_BLDO1_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_BLDO1_VOL_MIN)
            {
                log_e("Mistake ! BLDO1 minimum output voltage is  %umV", AXP2101_BLDO1_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_BLDO1_VOL_MAX)
            {
                log_e("Mistake ! BLDO1 maximum output voltage is  %umV", AXP2101_BLDO1_VOL_MAX);
                return false;
            }
            int val = readRegister(AXP2101_LDO_VOL4_CTRL);
            if (val == -1)
                return false;
            val &= 0xE0;
            val |= (millivolt - AXP2101_BLDO1_VOL_MIN) / AXP2101_BLDO1_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL4_CTRL, val);
        }

        uint16_t getBLDO1Voltage(void)
        {
            int val = readRegister(AXP2101_LDO_VOL4_CTRL);
            if (val == -1)
                return 0;
            val &= 0x1F;
            return val * AXP2101_BLDO1_VOL_STEPS + AXP2101_BLDO1_VOL_MIN;
        }

        /*
         * Power control BLDO2 functions
         */
        bool isEnableBLDO2(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 5);
        }

        bool enableBLDO2(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 5);
        }

        bool disableBLDO2(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 5);
        }

        bool setBLDO2Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_BLDO2_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_BLDO2_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_BLDO2_VOL_MIN)
            {
                log_e("Mistake ! BLDO2 minimum output voltage is  %umV", AXP2101_BLDO2_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_BLDO2_VOL_MAX)
            {
                log_e("Mistake ! BLDO2 maximum output voltage is  %umV", AXP2101_BLDO2_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL5_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_BLDO2_VOL_MIN) / AXP2101_BLDO2_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL5_CTRL, val);
        }

        uint16_t getBLDO2Voltage(void)
        {
            int val = readRegister(AXP2101_LDO_VOL5_CTRL);
            if (val == -1)
                return 0;
            val &= 0x1F;
            return val * AXP2101_BLDO2_VOL_STEPS + AXP2101_BLDO2_VOL_MIN;
        }

        /*
         * Power control CPUSLDO functions
         */
        bool isEnableCPUSLDO(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 6);
        }

        bool enableCPUSLDO(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 6);
        }

        bool disableCPUSLDO(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 6);
        }

        bool setCPUSLDOVoltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_CPUSLDO_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_CPUSLDO_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_CPUSLDO_VOL_MIN)
            {
                log_e("Mistake ! CPULDO minimum output voltage is  %umV", AXP2101_CPUSLDO_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_CPUSLDO_VOL_MAX)
            {
                log_e("Mistake ! CPULDO maximum output voltage is  %umV", AXP2101_CPUSLDO_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL6_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_CPUSLDO_VOL_MIN) / AXP2101_CPUSLDO_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL6_CTRL, val);
        }

        uint16_t getCPUSLDOVoltage(void)
        {
            int val = readRegister(AXP2101_LDO_VOL6_CTRL);
            if (val == -1)
                return 0;
            val &= 0x1F;
            return val * AXP2101_CPUSLDO_VOL_STEPS + AXP2101_CPUSLDO_VOL_MIN;
        }

        /*
         * Power control DLDO1 functions
         */
        bool isEnableDLDO1(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 7);
        }

        bool enableDLDO1(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 7);
        }

        bool disableDLDO1(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL0, 7);
        }

        bool setDLDO1Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_DLDO1_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_DLDO1_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_DLDO1_VOL_MIN)
            {
                log_e("Mistake ! DLDO1 minimum output voltage is  %umV", AXP2101_DLDO1_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_DLDO1_VOL_MAX)
            {
                log_e("Mistake ! DLDO1 maximum output voltage is  %umV", AXP2101_DLDO1_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL7_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_DLDO1_VOL_MIN) / AXP2101_DLDO1_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL7_CTRL, val);
        }

        uint16_t getDLDO1Voltage(void)
        {
            int val = readRegister(AXP2101_LDO_VOL7_CTRL);
            if (val == -1)
                return 0;
            val &= 0x1F;
            return val * AXP2101_DLDO1_VOL_STEPS + AXP2101_DLDO1_VOL_MIN;
        }

        /*
         * Power control DLDO2 functions
         */
        bool isEnableDLDO2(void)
        {
            return getRegisterBit(AXP2101_LDO_ONOFF_CTRL1, 0);
        }

        bool enableDLDO2(void)
        {
            return setRegisterBit(AXP2101_LDO_ONOFF_CTRL1, 0);
        }

        bool disableDLDO2(void)
        {
            return clrRegisterBit(AXP2101_LDO_ONOFF_CTRL1, 0);
        }

        bool setDLDO2Voltage(uint16_t millivolt)
        {
            if (millivolt % AXP2101_DLDO2_VOL_STEPS)
            {
                log_e("Mistake ! The steps is must %u mV", AXP2101_DLDO2_VOL_STEPS);
                return false;
            }
            if (millivolt < AXP2101_DLDO2_VOL_MIN)
            {
                log_e("Mistake ! DLDO2 minimum output voltage is  %umV", AXP2101_DLDO2_VOL_MIN);
                return false;
            }
            else if (millivolt > AXP2101_DLDO2_VOL_MAX)
            {
                log_e("Mistake ! DLDO2 maximum output voltage is  %umV", AXP2101_DLDO2_VOL_MAX);
                return false;
            }
            uint16_t val  = readRegister(AXP2101_LDO_VOL8_CTRL) & 0xE0;
            val          |= (millivolt - AXP2101_DLDO2_VOL_MIN) / AXP2101_DLDO2_VOL_STEPS;
            return 0 == writeRegister(AXP2101_LDO_VOL8_CTRL, val);
        }

        uint16_t getDLDO2Voltage(void)
        {
            int val = readRegister(AXP2101_LDO_VOL8_CTRL);
            if (val == -1)
                return 0;
            val &= 0x1F;
            return val * AXP2101_DLDO2_VOL_STEPS + AXP2101_DLDO2_VOL_MIN;
        }

        /*
         * Power ON OFF IRQ TIMMING Control method
         */

        void setIrqLevelTime(pmic_irq_time opt)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return;
            val &= 0xCF;
            writeRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 4));
        }

        pmic_irq_time getIrqLevelTime(void)
        {
            return (pmic_irq_time)((readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL) & 0x30) >> 4);
        }

        /**
         * @brief Set the PEKEY power-on long press time.
         * @param opt: See pmic_press_on_time_t enum for details.
         * @retval
         */
        bool setPowerKeyPressOnTime(pmic_press_on_time opt)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return false;
            val &= 0xFC;
            return 0 == writeRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | opt);
        }

        /**
         * @brief Get the PEKEY power-on long press time.
         * @retval See pmic_press_on_time_t enum for details.
         */
        pmic_press_on_time getPowerKeyPressOnTime(void)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return pmic_press_on_time(0);
            return static_cast<pmic_press_on_time>(val & 0x03);
        }

        /**
         * @brief Set the PEKEY power-off long press time.
         * @param opt: See pmic_press_off_time_t enum for details.
         * @retval
         */
        bool setPowerKeyPressOffTime(pmic_press_off_time opt)
        {
            int val = readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL);
            if (val == -1)
                return false;
            val &= 0xF3;
            return 0 == writeRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 2));
        }

        /**
         * @brief Get the PEKEY power-off long press time.
         * @retval See pmic_press_off_time_t enum for details.
         */
        pmic_press_off_time getPowerKeyPressOffTime(void)
        {
            return (pmic_press_off_time)((readRegister(AXP2101_IRQ_OFF_ON_LEVEL_CTRL) & 0x0C) >> 2);
        }

        /*
         * ADC Control method
         */
        bool enableGeneralAdcChannel(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 5);
        }

        bool disableGeneralAdcChannel(void)
        {
            return clrRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 5);
        }

        bool enableTemperatureMeasure(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 4);
        }

        bool disableTemperatureMeasure(void)
        {
            return clrRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 4);
        }

        float getTemperature(void)
        {
            uint16_t raw = readRegisterH6L8(AXP2101_ADC_DATA_RELUST8, AXP2101_ADC_DATA_RELUST9);
            return AXP2101_CONVERSION(raw);
        }

        bool enableSystemVoltageMeasure(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 3);
        }

        bool disableSystemVoltageMeasure(void)
        {
            return clrRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 3);
        }

        uint16_t getSystemVoltage(void)
        {
            return readRegisterH6L8(AXP2101_ADC_DATA_RELUST6, AXP2101_ADC_DATA_RELUST7);
        }

        bool enableVbusVoltageMeasure(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 2);
        }

        bool disableVbusVoltageMeasure(void)
        {
            return clrRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 2);
        }

        uint16_t getVbusVoltage(void)
        {
            if (!isVbusIn())
            {
                return 0;
            }
            return readRegisterH6L8(AXP2101_ADC_DATA_RELUST4, AXP2101_ADC_DATA_RELUST5);
        }

        bool enableTSPinMeasure(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 1);
        }

        bool disableTSPinMeasure(void)
        {
            return clrRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 1);
        }

        bool enableTSPinLowFreqSample(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 7);
        }

        bool disableTSPinLowFreqSample(void)
        {
            return clrRegisterBit(AXP2101_ADC_DATA_RELUST2, 7);
        }

        uint16_t getTsTemperature(void)
        {
            return readRegisterH6L8(AXP2101_ADC_DATA_RELUST2, AXP2101_ADC_DATA_RELUST3);
        }

        bool enableBattVoltageMeasure(void)
        {
            return setRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 0);
        }

        bool disableBattVoltageMeasure(void)
        {
            return clrRegisterBit(AXP2101_ADC_CHANNEL_CTRL, 0);
        }

        bool enableBattDetection(void)
        {
            return setRegisterBit(AXP2101_BAT_DET_CTRL, 0);
        }

        bool disableBattDetection(void)
        {
            return clrRegisterBit(AXP2101_BAT_DET_CTRL, 0);
        }

        uint16_t getBattVoltage(void)
        {
            if (!isBatteryConnect())
            {
                return 0;
            }
            return readRegisterH5L8(AXP2101_ADC_DATA_RELUST0, AXP2101_ADC_DATA_RELUST1);
        }

        uint8_t getBatteryPercent(void)
        {
            if (!isBatteryConnect())
            {
                return -1;
            }
            return readRegister(AXP2101_BAT_PERCENT_DATA);
        }

        /*
         * CHG LED setting and control
         */
        // void enableChargingLed(void)
        // {
        //     setRegisterBit(AXP2101_CHGLED_SET_CTRL, 0);
        // }

        // void disableChargingLed(void)
        // {
        //     clrRegisterBit(AXP2101_CHGLED_SET_CTRL, 0);
        // }

        /**
         * @brief Set charging led mode.
         * @retval See pmic_chg_led_mode_t enum for details.
         */
        void setChargingLedMode(pmic_chg_led_mode mode)
        {
            int val;
            switch (mode)
            {
                case CHG_LED_OFF:
                // clrRegisterBit(AXP2101_CHGLED_SET_CTRL, 0);
                // break;
                case CHG_LED_BLINK_1HZ:
                case CHG_LED_BLINK_4HZ:
                case CHG_LED_ON:
                    val = readRegister(AXP2101_CHGLED_SET_CTRL);
                    if (val == -1)
                        return;
                    val &= 0xC8;
                    val |= 0x05; // use manual ctrl
                    val |= (mode << 4);
                    writeRegister(AXP2101_CHGLED_SET_CTRL, val);
                    break;
                case CHG_LED_CTRL_CHG:
                    val = readRegister(AXP2101_CHGLED_SET_CTRL);
                    if (val == -1)
                        return;
                    val &= 0xF9;
                    writeRegister(AXP2101_CHGLED_SET_CTRL, val | 0x01); // use type A mode
                    // writeRegister(AXP2101_CHGLED_SET_CTRL, val | 0x02); // use type B mode
                    break;
                default:
                    break;
            }
        }

        pmic_chg_led_mode getChargingLedMode()
        {
            int val = readRegister(AXP2101_CHGLED_SET_CTRL);
            if (val == -1)
                return CHG_LED_OFF;
            val >>= 1;
            if ((val & 0x02) == 0x02)
            {
                val >>= 4;
                return static_cast<pmic_chg_led_mode>(val & 0x03);
            }
            return CHG_LED_CTRL_CHG;
        }

        /**
         * @brief 预充电充电电流限制
         * @note  Precharge current limit 25*N mA
         * @param  opt: 25 * opt
         * @retval None
         */
        void setPrechargeCurr(pmic_prechg opt)
        {
            int val = readRegister(AXP2101_IPRECHG_SET);
            if (val == -1)
                return;
            val &= 0xFC;
            writeRegister(AXP2101_IPRECHG_SET, val | opt);
        }

        pmic_prechg getPrechargeCurr(void)
        {
            return (pmic_prechg)(readRegister(AXP2101_IPRECHG_SET) & 0x03);
        }

        /**
         * @brief Set charge current.
         * @param  opt: See pmic_chg_curr_t enum for details.
         * @retval
         */
        bool setChargerConstantCurr(pmic_chg_curr opt)
        {
            if (opt > AXP2101_CHG_CUR_1000MA)
                return false;
            int val = readRegister(AXP2101_ICC_CHG_SET);
            if (val == -1)
                return false;
            val &= 0xE0;
            return 0 == writeRegister(AXP2101_ICC_CHG_SET, val | opt);
        }

        /**
         * @brief Get charge current settings.
         *  @retval See pmic_chg_curr_t enum for details.
         */
        pmic_chg_curr getChargerConstantCurr(void)
        {
            int val = readRegister(AXP2101_ICC_CHG_SET);
            if (val == -1)
                return pmic_chg_curr(0);
            return static_cast<pmic_chg_curr>(val & 0x1F);
        }

        /**
         * @brief  充电终止电流限制
         * @note   Charging termination of current limit
         * @retval
         */
        void setChargerTerminationCurr(pmic_chg_iterm opt)
        {
            int val = readRegister(AXP2101_ITERM_CHG_SET_CTRL);
            if (val == -1)
                return;
            val &= 0xF0;
            writeRegister(AXP2101_ITERM_CHG_SET_CTRL, val | opt);
        }

        pmic_chg_iterm getChargerTerminationCurr(void)
        {
            return (pmic_chg_iterm)(readRegister(AXP2101_ITERM_CHG_SET_CTRL) & 0x0F);
        }

        void enableChargerTerminationLimit(void)
        {
            int val = readRegister(AXP2101_ITERM_CHG_SET_CTRL);
            if (val == -1)
                return;
            writeRegister(AXP2101_ITERM_CHG_SET_CTRL, val | 0x10);
        }

        void disableChargerTerminationLimit(void)
        {
            int val = readRegister(AXP2101_ITERM_CHG_SET_CTRL);
            if (val == -1)
                return;
            writeRegister(AXP2101_ITERM_CHG_SET_CTRL, val & 0xEF);
        }

        bool isChargerTerminationLimit(void)
        {
            return getRegisterBit(AXP2101_ITERM_CHG_SET_CTRL, 4);
        }

        /**
         * @brief Set charge target voltage.
         * @param  opt: See pmic_chg_vol enum for details.
         * @retval
         */
        bool setChargeTargetVoltage(pmic_chg_vol opt)
        {
            if (opt >= AXP2101_CHG_VOL_MAX)
                return false;
            int val = readRegister(AXP2101_CV_CHG_VOL_SET);
            if (val == -1)
                return false;
            val &= 0xF8;
            return 0 == writeRegister(AXP2101_CV_CHG_VOL_SET, val | opt);
        }

        /**
         * @brief Get charge target voltage settings.
         * @retval See pmic_chg_vol enum for details.
         */
        pmic_chg_vol getChargeTargetVoltage(void)
        {
            return (pmic_chg_vol)(readRegister(AXP2101_CV_CHG_VOL_SET) & 0x07);
        }

        /**
         * @brief  设定热阈值
         * @note   Thermal regulation threshold setting
         */
        void setThermaThreshold(pmic_thermal opt)
        {
            int val = readRegister(AXP2101_THE_REGU_THRES_SET);
            if (val == -1)
                return;
            val &= 0xFC;
            writeRegister(AXP2101_THE_REGU_THRES_SET, val | opt);
        }

        pmic_thermal getThermaThreshold(void)
        {
            return (pmic_thermal)(readRegister(AXP2101_THE_REGU_THRES_SET) & 0x03);
        }

        uint8_t getBatteryParameter()
        {
            return readRegister(AXP2101_BAT_PARAME);
        }

        void fuelGaugeControl(bool writeROM, bool enable)
        {
            if (writeROM)
            {
                clrRegisterBit(AXP2101_FUEL_GAUGE_CTRL, 4);
            }
            else
            {
                setRegisterBit(AXP2101_FUEL_GAUGE_CTRL, 4);
            }
            if (enable)
            {
                setRegisterBit(AXP2101_FUEL_GAUGE_CTRL, 0);
            }
            else
            {
                clrRegisterBit(AXP2101_FUEL_GAUGE_CTRL, 0);
            }
        }

        /*
         * Interrupt status/control functions
         */

        /**
         * @brief  Get the interrupt controller mask value.
         * @retval   Mask value corresponds to pmic_irq ,
         */
        uint64_t getIrqStatus(void)
        {
            statusRegister[0] = readRegister(AXP2101_INTSTS1);
            statusRegister[1] = readRegister(AXP2101_INTSTS2);
            statusRegister[2] = readRegister(AXP2101_INTSTS3);
            return (uint32_t)(statusRegister[0] << 16) | (uint32_t)(statusRegister[1] << 8) | (uint32_t)(statusRegister[2]);
        }

        /**
         * @brief  Clear interrupt controller state.
         */
        void clearIrqStatus()
        {
            for (int i = 0; i < AXP2101_INTSTS_CNT; i++)
            {
                writeRegister(AXP2101_INTSTS1 + i, 0xFF);
                statusRegister[i] = 0;
            }
        }

        /*
         *  @brief  Debug interrupt setting register
         * */
        void printIntRegister(Stream *stream)
        {
            for (int i = 0; i < AXP2101_INTSTS_CNT; i++)
            {
                uint8_t val = readRegister(AXP2101_INTEN1 + i);
                stream->print("INT[");
                stream->print(i);
                stream->print(']');
                stream->print("  HEX: ");
                stream->print(val, HEX);
                stream->print(" BIN:0b");
                stream->println(val, BIN);
            }
        }

        /**
         * @brief  Enable PMU interrupt control mask .
         * @param  opt: View the related chip type pmic_irq enumeration
         *              parameters in "XPowersParams.hpp"
         * @retval
         */
        bool enableIRQ(pmic_irq opt)
        {
            return setInterruptImpl(opt, true);
        }

        /**
         * @brief  Disable PMU interrupt control mask .
         * @param  opt: View the related chip type pmic_irq enumeration
         *              parameters in "XPowersParams.hpp"
         * @retval
         */
        bool disableIRQ(pmic_irq opt)
        {
            return setInterruptImpl(opt, false);
        }

        // IRQ STATUS 0
        bool isDropWarningLevel2Irq(void)
        {
            uint8_t mask = AXP2101_WARNING_LEVEL2_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        bool isDropWarningLevel1Irq(void)
        {
            uint8_t mask = AXP2101_WARNING_LEVEL1_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        bool isGaugeWdtTimeoutIrq()
        {
            uint8_t mask = AXP2101_WDT_TIMEOUT_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        bool isBatChargerOverTemperatureIrq(void)
        {
            uint8_t mask = AXP2101_BAT_CHG_OVER_TEMP_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        bool isBatChargerUnderTemperatureIrq(void)
        {
            uint8_t mask = AXP2101_BAT_CHG_UNDER_TEMP_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        bool isBatWorkOverTemperatureIrq(void)
        {
            uint8_t mask = AXP2101_BAT_NOR_OVER_TEMP_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        bool isBatWorkUnderTemperatureIrq(void)
        {
            uint8_t mask = AXP2101_BAT_NOR_UNDER_TEMP_IRQ;
            if (intRegister[0] & mask)
            {
                return IS_BIT_SET(statusRegister[0], mask);
            }
            return false;
        }

        // IRQ STATUS 1
        bool isVbusInsertIrq(void)
        {
            uint8_t mask = AXP2101_VBUS_INSERT_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isVbusRemoveIrq(void)
        {
            uint8_t mask = AXP2101_VBUS_REMOVE_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isBatInsertIrq(void)
        {
            uint8_t mask = AXP2101_BAT_INSERT_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isBatRemoveIrq(void)
        {
            uint8_t mask = AXP2101_BAT_REMOVE_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isPekeyShortPressIrq(void)
        {
            uint8_t mask = AXP2101_PKEY_SHORT_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isPekeyLongPressIrq(void)
        {
            uint8_t mask = AXP2101_PKEY_LONG_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isPekeyNegativeIrq(void)
        {
            uint8_t mask = AXP2101_PKEY_NEGATIVE_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        bool isPekeyPositiveIrq(void)
        {
            uint8_t mask = AXP2101_PKEY_POSITIVE_IRQ >> 8;
            if (intRegister[1] & mask)
            {
                return IS_BIT_SET(statusRegister[1], mask);
            }
            return false;
        }

        // IRQ STATUS 2
        bool isWdtExpireIrq(void)
        {
            uint8_t mask = AXP2101_WDT_EXPIRE_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isLdoOverCurrentIrq(void)
        {
            uint8_t mask = AXP2101_LDO_OVER_CURR_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isBatfetOverCurrentIrq(void)
        {
            uint8_t mask = AXP2101_BATFET_OVER_CURR_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isBatChargeDoneIrq(void)
        {
            uint8_t mask = AXP2101_BAT_CHG_DONE_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isBatChargeStartIrq(void)
        {
            uint8_t mask = AXP2101_BAT_CHG_START_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isBatDieOverTemperatureIrq(void)
        {
            uint8_t mask = AXP2101_DIE_OVER_TEMP_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isChargeOverTimeoutIrq(void)
        {
            uint8_t mask = AXP2101_CHAGER_TIMER_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        bool isBatOverVoltageIrq(void)
        {
            uint8_t mask = AXP2101_BAT_OVER_VOL_IRQ >> 16;
            if (intRegister[2] & mask)
            {
                return IS_BIT_SET(statusRegister[2], mask);
            }
            return false;
        }

        uint8_t getChipID(void)
        {
            return readRegister(AXP2101_IC_TYPE);
        }

    protected:
        uint16_t getPowerChannelVoltage(PMICPowerChannel channel)
        {
            switch (channel)
            {
                case DCDC1:
                    return getDC1Voltage();
                case DCDC2:
                    return getDC2Voltage();
                case DCDC3:
                    return getDC3Voltage();
                case DCDC4:
                    return getDC4Voltage();
                case DCDC5:
                    return getDC5Voltage();
                case ALDO1:
                    return getALDO1Voltage();
                case ALDO2:
                    return getALDO2Voltage();
                case ALDO3:
                    return getALDO3Voltage();
                case ALDO4:
                    return getALDO4Voltage();
                case BLDO1:
                    return getBLDO1Voltage();
                case BLDO2:
                    return getBLDO2Voltage();
                case DLDO1:
                    return getDLDO1Voltage();
                case DLDO2:
                    return getDLDO2Voltage();
                case VBACKUP:
                    return getButtonBatteryVoltage();
                default:
                    break;
            }
            return 0;
        }

        bool inline enablePowerOutput(PMICPowerChannel channel)
        {
            switch (channel)
            {
                case DCDC1:
                    return enableDC1();
                case DCDC2:
                    return enableDC2();
                case DCDC3:
                    return enableDC3();
                case DCDC4:
                    return enableDC4();
                case DCDC5:
                    return enableDC5();
                case ALDO1:
                    return enableALDO1();
                case ALDO2:
                    return enableALDO2();
                case ALDO3:
                    return enableALDO3();
                case ALDO4:
                    return enableALDO4();
                case BLDO1:
                    return enableBLDO1();
                case BLDO2:
                    return enableBLDO2();
                case DLDO1:
                    return enableDLDO1();
                case DLDO2:
                    return enableDLDO2();
                case VBACKUP:
                    return enableButtonBatteryCharge();
                default:
                    break;
            }
            return false;
        }

        bool inline disablePowerOutput(PMICPowerChannel channel)
        {
            if (getProtectedChannel(channel))
            {
                log_e("Failed to disable the power channel, the power channel has been protected");
                return false;
            }
            switch (channel)
            {
                case DCDC1:
                    return disableDC1();
                case DCDC2:
                    return disableDC2();
                case DCDC3:
                    return disableDC3();
                case DCDC4:
                    return disableDC4();
                case DCDC5:
                    return disableDC5();
                case ALDO1:
                    return disableALDO1();
                case ALDO2:
                    return disableALDO2();
                case ALDO3:
                    return disableALDO3();
                case ALDO4:
                    return disableALDO4();
                case BLDO1:
                    return disableBLDO1();
                case BLDO2:
                    return disableBLDO2();
                case DLDO1:
                    return disableDLDO1();
                case DLDO2:
                    return disableDLDO2();
                case VBACKUP:
                    return disableButtonBatteryCharge();
                case CPULDO:
                    return disableCPUSLDO();
                default:
                    break;
            }
            return false;
        }

        bool inline isPowerChannelEnable(PMICPowerChannel channel)
        {
            switch (channel)
            {
                case DCDC1:
                    return isEnableDC1();
                case DCDC2:
                    return isEnableDC2();
                case DCDC3:
                    return isEnableDC3();
                case DCDC4:
                    return isEnableDC4();
                case DCDC5:
                    return isEnableDC5();
                case ALDO1:
                    return isEnableALDO1();
                case ALDO2:
                    return isEnableALDO2();
                case ALDO3:
                    return isEnableALDO3();
                case ALDO4:
                    return isEnableALDO4();
                case BLDO1:
                    return isEnableBLDO1();
                case BLDO2:
                    return isEnableBLDO2();
                case DLDO1:
                    return isEnableDLDO1();
                case DLDO2:
                    return isEnableDLDO2();
                case VBACKUP:
                    return isEnableButtonBatteryCharge();
                case CPULDO:
                    return isEnableCPUSLDO();
                default:
                    break;
            }
            return false;
        }

        bool inline setPowerChannelVoltage(PMICPowerChannel channel, uint16_t millivolt)
        {
            if (getProtectedChannel(channel))
            {
                log_e("Failed to set the power channel, the power channel has been protected");
                return false;
            }
            switch (channel)
            {
                case DCDC1:
                    return setDC1Voltage(millivolt);
                case DCDC2:
                    return setDC2Voltage(millivolt);
                case DCDC3:
                    return setDC3Voltage(millivolt);
                case DCDC4:
                    return setDC4Voltage(millivolt);
                case DCDC5:
                    return setDC5Voltage(millivolt);
                case ALDO1:
                    return setALDO1Voltage(millivolt);
                case ALDO2:
                    return setALDO2Voltage(millivolt);
                case ALDO3:
                    return setALDO3Voltage(millivolt);
                case ALDO4:
                    return setALDO4Voltage(millivolt);
                case BLDO1:
                    return setBLDO1Voltage(millivolt);
                case BLDO2:
                    return setBLDO1Voltage(millivolt);
                case DLDO1:
                    return setDLDO1Voltage(millivolt);
                case DLDO2:
                    return setDLDO1Voltage(millivolt);
                case VBACKUP:
                    return setButtonBatteryChargeVoltage(millivolt);
                case CPULDO:
                    return setCPUSLDOVoltage(millivolt);
                default:
                    break;
            }
            return false;
        }

        bool initImpl()
        {
            if (getChipID() == AXP2101_CHIP_ID)
            {
                setChipModel(PMICChipModel::AXP2101);
                disableTSPinMeasure(); // Disable NTC temperature detection by default
                return true;
            }
            return false;
        }

        /*
         * Interrupt control functions
         */
        bool setInterruptImpl(uint32_t opts, bool enable)
        {
            int     res  = 0;
            uint8_t data = 0, value = 0;
            log_d("%s - HEX:0x%x \n", enable ? "ENABLE" : "DISABLE", opts);
            if (opts & 0x0000FF)
            {
                value = opts & 0xFF;
                // log_d("Write INT0: %x\n", value);
                data            = readRegister(AXP2101_INTEN1);
                intRegister[0]  = enable ? (data | value) : (data & (~value));
                res            |= writeRegister(AXP2101_INTEN1, intRegister[0]);
            }
            if (opts & 0x00FF00)
            {
                value = opts >> 8;
                // log_d("Write INT1: %x\n", value);
                data            = readRegister(AXP2101_INTEN2);
                intRegister[1]  = enable ? (data | value) : (data & (~value));
                res            |= writeRegister(AXP2101_INTEN2, intRegister[1]);
            }
            if (opts & 0xFF0000)
            {
                value = opts >> 16;
                // log_d("Write INT2: %x\n", value);
                data            = readRegister(AXP2101_INTEN3);
                intRegister[2]  = enable ? (data | value) : (data & (~value));
                res            |= writeRegister(AXP2101_INTEN3, intRegister[2]);
            }
            return res == 0;
        }

        const char *getChipNameImpl(void)
        {
            return "AXP2101";
        }

    private:
        uint8_t statusRegister[AXP2101_INTSTS_CNT];
        uint8_t intRegister[AXP2101_INTSTS_CNT];
    };
} // namespace HAL::PMIC