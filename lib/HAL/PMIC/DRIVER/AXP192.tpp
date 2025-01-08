#include "../REG/AXP192.hpp"
#include "../TYPE1.tpp"
namespace HAL
{
    namespace PMIC
    {
        typedef AXP192 THIS;
        class AXP192 : public Type1<class AXP192>
        {
            friend class Base<Type1<AXP192>, AXP192>;

        public:
            AXP192(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = AXP192_SLAVE_ADDRESS)
            {
                myWire = &w;
                mySDA = sda;
                mySCL = scl;
                myADDR = addr;
            }

            AXP192(uint8_t addr, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback)
            {
                thisReadRegCallback = readRegCallback;
                thisWriteRegCallback = writeRegCallback;
                myADDR = addr;
            }

            AXP192()
            {
                myWire = &Wire;
                mySDA = SDA;
                mySCL = SCL;
                myADDR = AXP192_SLAVE_ADDRESS;
            }

            ~AXP192()
            {
                log_i("~XPowersAXP192");
                deinit();
            }

            bool init(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = AXP192_SLAVE_ADDRESS)
            {
                myWire = &w;
                mySDA = sda;
                mySCL = scl;
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

            uint16_t status()
            {
                return readRegister(AXP192_STATUS);
            }

            bool isAcinVbusStart()
            {
                return getRegisterBit(AXP192_STATUS, 0);
            }

            bool isDischarge()
            {
                return !getRegisterBit(AXP192_STATUS, 2);
            }

            bool isVbusIn(void)
            {
                return getRegisterBit(AXP192_STATUS, 5);
            }

            bool isAcinEfficient()
            {
                return getRegisterBit(AXP192_STATUS, 6);
            }

            bool isAcinIn()
            {
                return getRegisterBit(AXP192_STATUS, 7);
            }

            bool isOverTemperature()
            {
                return getRegisterBit(AXP192_MODE_CHGSTATUS, 7);
            }

            bool isCharging(void)
            {
                return getRegisterBit(AXP192_MODE_CHGSTATUS, 6);
            }

            bool isBatteryConnect(void)
            {
                return getRegisterBit(AXP192_MODE_CHGSTATUS, 5);
            }

            bool isBattInActiveMode()
            {
                return getRegisterBit(AXP192_MODE_CHGSTATUS, 3);
            }

            bool isChargeCurrLessPreset()
            {
                return getRegisterBit(AXP192_MODE_CHGSTATUS, 2);
            }

            void enableVbusVoltageLimit()
            {
                setRegisterBit(AXP192_IPS_SET, 6);
            }

            void disableVbusVoltageLimit()
            {
                clrRegisterBit(AXP192_IPS_SET, 6);
            }

            /**
             * @brief  Set VBUS Voltage Input Limit.
             * @param  opt: View the related chip type pmic_vbus_vol_limit enumeration
             *              parameters in "XPowersParams.hpp"
             */
            void setVbusVoltageLimit(pmic_vbus_vol_limit opt)
            {
                int val = readRegister(AXP192_IPS_SET);
                if (val == -1)
                    return;
                val &= 0xC7;
                writeRegister(AXP192_IPS_SET, val | (opt << 3));
            }

            /**
             * @brief  Get VBUS Voltage Input Limit.
             * @retval View the related chip type pmic_vbus_vol_limit enumeration
             *              parameters in "XPowersParams.hpp"
             */
            pmic_vbus_vol_limit getVbusVoltageLimit(void)
            {
                int val = (readRegister(AXP192_IPS_SET) >> 3) & 0x7;
                return static_cast<pmic_vbus_vol_limit>(val);
            }

            /**
             * @brief  Set VBUS Current Input Limit.
             * @param  opt: View the related chip type pmic_vbus_cur_limit enumeration
             *              parameters in "XPowersParams.hpp"
             * @retval true valid false invalid
             */
            bool setVbusCurrentLimit(pmic_vbus_cur_limit opt)
            {
                switch (opt)
                {
                case AXP192_VBUS_CUR_LIM_500MA:
                    setRegisterBit(AXP192_IPS_SET, 1);
                    return clrRegisterBit(AXP192_IPS_SET, 0);
                case AXP192_VBUS_CUR_LIM_100MA:
                    setRegisterBit(AXP192_IPS_SET, 1);
                    return setRegisterBit(AXP192_IPS_SET, 0);
                case AXP192_VBUS_CUR_LIM_OFF:
                    return clrRegisterBit(AXP192_IPS_SET, 1);
                default:
                    break;
                }
                return false;
            }

            /**
             * @brief  Get VBUS Current Input Limit.
             * @retval View the related chip type pmic_vbus_cur_limit enumeration
             *              parameters in "XPowersParams.hpp"
             */
            pmic_vbus_cur_limit getVbusCurrentLimit(void)
            {
                if (getRegisterBit(AXP192_IPS_SET, 1) == 0)
                {
                    return AXP192_VBUS_CUR_LIM_OFF;
                }
                if (getRegisterBit(AXP192_IPS_SET, 0))
                {
                    return AXP192_VBUS_CUR_LIM_100MA;
                }
                return AXP192_VBUS_CUR_LIM_500MA;
            }

            // Set the minimum system operating voltage inside the PMU,
            // below this value will shut down the PMU,Adjustment range 2600mV ~ 3300mV
            bool setSysPowerDownVoltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_SYS_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_SYS_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_VOFF_VOL_MIN)
                {
                    log_e("Mistake ! SYS minimum output voltage is  %umV", AXP192_VOFF_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP192_VOFF_VOL_MAX)
                {
                    log_e("Mistake ! SYS maximum output voltage is  %umV", AXP192_VOFF_VOL_MAX);
                    return false;
                }

                int val = readRegister(AXP192_VOFF_SET);
                if (val == -1)
                    return false;
                val &= 0xF8;
                val |= (millivolt - AXP192_VOFF_VOL_MIN) / AXP192_SYS_VOL_STEPS;
                return 0 == writeRegister(AXP192_VOFF_SET, val);
            }

            uint16_t getSysPowerDownVoltage()
            {
                int val = readRegister(AXP192_VOFF_SET);
                if (val == -1)
                    return 0;
                val &= 0x07;
                return (val * AXP192_SYS_VOL_STEPS) + AXP192_VOFF_VOL_MIN;
            }

            /**
             * @brief  Set shutdown, calling shutdown will turn off all power channels,
             *         only VRTC belongs to normal power supply
             * @retval None
             */
            void shutdown()
            {
                setRegisterBit(AXP192_OFF_CTL, 7);
            }

            /*
             * Charge setting
             */
            void enableCharge()
            {
                setRegisterBit(AXP192_CHARGE1, 7);
            }

            void disableCharge()
            {
                clrRegisterBit(AXP192_CHARGE1, 7);
            }

            /**
             * @brief Set charge target voltage.
             * @param  opt: See axp192_chg_vol enum for details.
             * @retval
             */
            bool setChargeTargetVoltage(pmic_chg_vol opt)
            {
                if (opt >= AXP192_CHG_VOL_MAX)
                    return false;
                int val = readRegister(AXP192_CHARGE1);
                if (val == -1)
                    return false;
                val &= 0x9F;
                return 0 == writeRegister(AXP192_CHARGE1, val | (opt << 5));
            }

            /**
             * @brief Get charge target voltage settings.
             * @retval See axp192_chg_vol enum for details.
             */
            pmic_chg_vol getChargeTargetVoltage()
            {
                int val = readRegister(AXP192_CHARGE1);
                if (val == -1)
                    return static_cast<pmic_chg_vol>(0);
                return static_cast<pmic_chg_vol>((val & 0x60) >> 5);
            }

            /**
             * @brief Set charge current settings.
             * @param opt: see axp192_chg_curr for details.
             */
            bool setChargerConstantCurr(pmic_chg_curr opt)
            {
                if (opt > 0x0F)
                    return false;
                int val = readRegister(AXP192_CHARGE1);
                if (val == -1)
                {
                    return false;
                }
                val &= 0xF0;
                return 0 == writeRegister(AXP192_CHARGE1, val | opt);
            }

            /**
             * @brief Get charge current settings.
             * @retval See pmic_chg_curr enum for details.
             */
            pmic_chg_curr getChargerConstantCurr()
            {
                int val = readRegister(AXP192_CHARGE1) & 0x0F;
                if (val == -1)
                    return pmic_chg_curr::AXP192_CHG_CUR_780MA;
                return static_cast<pmic_chg_curr>(val);
            }

            void setChargerTerminationCurr(pmic_chg_iterm opt)
            {
                switch (opt)
                {
                case AXP192_CHG_ITERM_LESS_10_PERCENT:
                    clrRegisterBit(AXP192_CHARGE1, 0);
                    break;
                case AXP192_CHG_ITERM_LESS_15_PERCENT:
                    setRegisterBit(AXP192_CHARGE1, 0);
                    break;
                default:
                    break;
                }
            }

            pmic_chg_iterm getChargerTerminationCurr()
            {
                int val = getRegisterBit(AXP192_CHARGE1, 4);
                return static_cast<pmic_chg_iterm>(val);
            }

            bool setPrechargeTimeout(pmic_prechg_to opt)
            {
                int val = readRegister(AXP192_CHARGE2);
                if (val == -1)
                    return false;
                val &= 0x3F;
                return 0 == writeRegister(AXP192_CHARGE2, val | (opt << 6));
            }

            // External channel charge current setting,Range:300~1000mA
            bool setChargerExternChannelCurr(uint16_t milliampere)
            {
                if (milliampere % AXP192_CHG_EXT_CURR_STEP)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_CHG_EXT_CURR_STEP);
                    return false;
                }
                if (milliampere < AXP192_CHG_EXT_CURR_MIN)
                {
                    log_e("Mistake ! The minimum external path charge current setting is:  %umA", AXP192_CHG_EXT_CURR_MIN);
                    return false;
                }
                else if (milliampere > AXP192_CHG_EXT_CURR_MAX)
                {
                    log_e("Mistake ! The maximum external channel charge current setting is:  %umA", AXP192_CHG_EXT_CURR_MAX);
                    return false;
                }
                int val = readRegister(AXP192_CHARGE2);
                if (val == -1)
                    return false;
                val &= 0xC7;
                val |= ((milliampere - AXP192_CHG_EXT_CURR_MIN) / AXP192_CHG_EXT_CURR_STEP);
                return 0 == writeRegister(AXP192_CHARGE2, val);
            }

            bool enableChargerExternChannel()
            {
                return setRegisterBit(AXP192_CHARGE2, 2);
            }

            bool disableChargerExternChannel()
            {
                return clrRegisterBit(AXP192_CHARGE2, 2);
            }

            // Timeout setting in constant current mode
            bool setChargerConstantTimeout(pmic_chg_cons_to opt)
            {
                int val = readRegister(AXP192_CHARGE2);
                if (val == -1)
                    return false;
                val &= 0xFC;
                return 0 == writeRegister(AXP192_CHARGE2, val | opt);
            }

            bool enableBackupBattCharger()
            {
                return setRegisterBit(AXP192_BACKUP_CHG, 7);
            }

            bool disableBackupBattCharger()
            {
                return clrRegisterBit(AXP192_BACKUP_CHG, 7);
            }

            bool isEnableBackupCharger()
            {
                return getRegisterBit(AXP192_BACKUP_CHG, 7);
            }

            bool setBackupBattChargerVoltage(pmic_backup_batt_vol opt)
            {
                int val = readRegister(AXP192_BACKUP_CHG);
                if (val == -1)
                    return false;
                val &= 0x9F;
                return 0 == writeRegister(AXP192_BACKUP_CHG, val | (opt << 5));
            }

            bool setBackupBattChargerCurr(pmic_backup_batt_curr opt)
            {
                int val = readRegister(AXP192_BACKUP_CHG);
                if (val == -1)
                    return false;
                val &= 0xFC;
                return 0 == writeRegister(AXP192_BACKUP_CHG, val | opt);
            }

            /*
             * Temperature
             */
            float getTemperature()
            {
                return readRegisterH8L4(AXP192_INTERNAL_TEMP_H8, AXP192_INTERNAL_TEMP_L4) * AXP192_INTERNAL_TEMP_STEP - AXP192_INERNAL_TEMP_OFFSET;
            }

            bool enableTemperatureMeasure()
            {
                return setRegisterBit(AXP192_ADC_EN2, 7);
            }

            bool disableTemperatureMeasure()
            {
                return clrRegisterBit(AXP192_ADC_EN2, 7);
            }

            /*
             * Power control LDOio functions
             */
            bool isEnableLDOio(void)
            {
                int val = readRegister(AXP192_GPIO0_CTL);
                return (val & 0x02);
            }

            bool enableLDOio(void)
            {
                int val = readRegister(AXP192_GPIO0_CTL) & 0xF8;
                return 0 == writeRegister(AXP192_GPIO0_CTL, val | 0x02);
            }

            bool disableLDOio(void)
            {
                int val = readRegister(AXP192_GPIO0_CTL) & 0xF8;
                return 0 == writeRegister(AXP192_GPIO0_CTL, val);
            }

            bool setLDOioVoltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_LDOIO_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_LDOIO_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_LDOIO_VOL_MIN)
                {
                    log_e("Mistake ! LDOIO minimum output voltage is  %umV", AXP192_LDOIO_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP192_LDOIO_VOL_MAX)
                {
                    log_e("Mistake ! LDOIO maximum output voltage is  %umV", AXP192_LDOIO_VOL_MAX);
                    return false;
                }
                int val = readRegister(AXP192_GPIO0_VOL);
                if (val == -1)
                    return false;
                val |= (((millivolt - AXP192_LDOIO_VOL_MIN) / AXP192_LDOIO_VOL_STEPS) << 4);
                return 0 == writeRegister(AXP192_GPIO0_VOL, val);
            }

            uint16_t getLDOioVoltage(void)
            {
                int val = readRegister(AXP192_GPIO0_VOL);
                if (val == -1)
                    return 0;
                val >>= 4;
                val *= AXP192_LDOIO_VOL_STEPS;
                val += AXP192_LDOIO_VOL_MIN;
                return val;
            }

            /*
             * Power control LDO2 functions
             */
            bool isEnableLDO2(void)
            {
                return getRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 2);
            }

            bool enableLDO2(void)
            {
                return setRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 2);
            }

            bool disableLDO2(void)
            {
                return clrRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 2);
            }

            bool setLDO2Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_LDO2_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_LDO2_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_LDO2_VOL_MIN)
                {
                    log_e("Mistake ! LDO2 minimum output voltage is  %umV", AXP192_LDO2_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP192_LDO2_VOL_MAX)
                {
                    log_e("Mistake ! LDO2 maximum output voltage is  %umV", AXP192_LDO2_VOL_MAX);
                    return false;
                }

                int val = readRegister(AXP192_LDO23OUT_VOL);
                if (val == -1)
                    return false;
                val &= 0x0F;
                return 0 == writeRegister(AXP192_LDO23OUT_VOL, val | (((millivolt - AXP192_LDO2_VOL_MIN) / AXP192_LDO2_VOL_STEPS) << AXP192_LDO2_VOL_BIT_MASK));
            }

            uint16_t getLDO2Voltage(void)
            {
                int val = readRegister(AXP192_LDO23OUT_VOL) & 0xF0;
                return (val >> AXP192_LDO2_VOL_BIT_MASK) * AXP192_LDO2_VOL_STEPS + AXP192_LDO2_VOL_MIN;
            }

            /*
             * Power control LDO3 functions
             */
            bool isEnableLDO3(void)
            {
                return getRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 3);
            }

            bool enableLDO3(void)
            {
                return setRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 3);
            }

            bool disableLDO3(void)
            {
                return clrRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 3);
            }

            bool setLDO3Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_LDO3_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_LDO3_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_LDO3_VOL_MIN)
                {
                    log_e("Mistake ! LDO3 minimum output voltage is  %umV", AXP192_LDO3_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP192_LDO3_VOL_MAX)
                {
                    log_e("Mistake ! LDO3 maximum output voltage is  %umV", AXP192_LDO3_VOL_MAX);
                    return false;
                }

                int val = readRegister(AXP192_LDO23OUT_VOL) & 0xF0;
                return 0 == writeRegister(AXP192_LDO23OUT_VOL, val | ((millivolt - AXP192_LDO3_VOL_MIN) / AXP192_LDO3_VOL_STEPS));
            }

            uint16_t getLDO3Voltage(void)
            {
                int val = readRegister(AXP192_LDO23OUT_VOL);
                if (val == -1)
                    return 0;
                val &= 0x0F;
                return (val * AXP192_LDO3_VOL_STEPS) + AXP192_LDO3_VOL_MIN;
            }

            /*
             * Power control DCDC1 functions
             */
            void setDC1PwmMode(void)
            {
                int val = readRegister(AXP192_DCDC_MODESET) & 0xF7;
                writeRegister(AXP192_DCDC_MODESET, val | 0x08);
            }

            void setDC1AutoMode(void)
            {
                int val = readRegister(AXP192_DCDC_MODESET) & 0xF7;
                writeRegister(AXP192_DCDC_MODESET, val);
            }

            bool isEnableDC1(void)
            {
                return getRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 0);
            }

            bool enableDC1(void)
            {
                return setRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 0);
            }

            bool disableDC1(void)
            {
                return clrRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 0);
            }

            bool setDC1Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_DC1_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_DC1_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_DC1_VOL_STEPS)
                {
                    log_e("Mistake ! DCDC1 minimum output voltage is  %umV", AXP192_DC1_VOL_STEPS);
                    return false;
                }
                else if (millivolt > AXP192_DC1_VOL_MAX)
                {
                    log_e("Mistake ! DCDC1 maximum output voltage is  %umV", AXP192_DC1_VOL_MAX);
                    return false;
                }

                int val = readRegister(AXP192_DC1_VLOTAGE);
                if (val == -1)
                    return false;
                val &= 0x80;
                val |= (millivolt - AXP192_DC1_VOL_MIN) / AXP192_DC1_VOL_STEPS;
                return 0 == writeRegister(AXP192_DC1_VLOTAGE, val);
            }

            uint16_t getDC1Voltage(void)
            {
                int val = readRegister(AXP192_DC1_VLOTAGE) & 0x7F;
                return val * AXP192_DC1_VOL_STEPS + AXP192_DC1_VOL_MIN;
            }

            /*
             * Power control DCDC2 functions
             */
            void setDC2PwmMode(void)
            {
                int val = readRegister(AXP192_DCDC_MODESET) & 0xFB;
                writeRegister(AXP192_DCDC_MODESET, val | 0x04);
            }

            void setDC2AutoMode(void)
            {
                int val = readRegister(AXP192_DCDC_MODESET) & 0xFB;
                writeRegister(AXP192_DCDC_MODESET, val);
            }

            void enableDC2VRC(void)
            {
                int val = readRegister(AXP192_DC2_DVM);
                writeRegister(AXP192_DC2_DVM, val | 0x04);
            }

            void disableDC2VRC(void)
            {
                int val = readRegister(AXP192_DC2_DVM);
                writeRegister(AXP192_DC2_DVM, val & 0xFB);
            }

            bool setDC2VRC(uint8_t opts)
            {
                if (opts > 1)
                {
                    return false;
                }
                int val = readRegister(AXP192_DC2_DVM) & 0xFE;
                writeRegister(AXP192_DC2_DVM, val | opts);
            }

            bool isEnableDC2VRC(void)
            {
                return (readRegister(AXP192_DC2_DVM) & 0x04) == 0x04;
            }

            bool isEnableDC2(void)
            {
                return getRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 4);
            }

            bool enableDC2(void)
            {
                return setRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 4);
            }

            bool disableDC2(void)
            {
                return clrRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 4);
            }

            bool setDC2Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_DC2_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_DC2_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_DC2_VOL_MIN)
                {
                    log_e("Mistake ! DCDC2 minimum output voltage is  %umV", AXP192_DC2_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP192_DC2_VOL_MAX)
                {
                    log_e("Mistake ! DCDC2 maximum output voltage is  %umV", AXP192_DC2_VOL_MAX);
                    return false;
                }
                int val = readRegister(AXP192_DC2OUT_VOL);
                if (val == -1)
                    return false;
                val &= 0x80;
                val |= (millivolt - AXP192_DC2_VOL_MIN) / AXP192_DC2_VOL_STEPS;
                return 0 == writeRegister(AXP192_DC2OUT_VOL, val);
            }

            uint16_t getDC2Voltage(void)
            {
                int val = readRegister(AXP192_DC2OUT_VOL);
                if (val == -1)
                    return 0;
                return (val * AXP192_DC2_VOL_STEPS) + AXP192_DC2_VOL_MIN;
            }

            /*
             * Power control DCDC3 functions
             */
            void setDC3PwmMode(void)
            {
                int val = readRegister(AXP192_DCDC_MODESET) & 0xFD;
                writeRegister(AXP192_DCDC_MODESET, val | 0x02);
            }

            void setDC3AutoMode(void)
            {
                int val = readRegister(AXP192_DCDC_MODESET) & 0xFD;
                writeRegister(AXP192_DCDC_MODESET, val);
            }

            bool isEnableDC3(void)
            {
                return getRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 1);
            }

            bool enableDC3(void)
            {
                return setRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 1);
            }

            bool disableDC3(void)
            {
                return clrRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 1);
            }

            bool setDC3Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP192_DC3_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP192_DC3_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP192_DC3_VOL_MIN)
                {
                    log_e("Mistake ! DCDC3 minimum output voltage is  %umV", AXP192_DC3_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP192_DC3_VOL_MAX)
                {
                    log_e("Mistake ! DCDC3 maximum output voltage is  %umV", AXP192_DC3_VOL_MAX);
                    return false;
                }
                return 0 == writeRegister(AXP192_DC3OUT_VOL, (millivolt - AXP192_DC3_VOL_MIN) / AXP192_DC3_VOL_STEPS);
            }

            uint16_t getDC3Voltage(void)
            {
                int val = readRegister(AXP192_DC3OUT_VOL);
                if (val == -1)
                    return 0;
                return (val * AXP192_DC3_VOL_STEPS) + AXP192_DC3_VOL_MIN;
            }

            /*
             * Power control EXTEN functions
             */
            bool enableExternalPin(void)
            {
                return setRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 6);
            }

            bool disableExternalPin(void)
            {
                return clrRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 6);
            }

            bool isEnableExternalPin(void)
            {
                return getRegisterBit(AXP192_LDO23_DC123_EXT_CTL, 6);
            }

            /*
             * Interrupt status functions
             */

            /**
             * @brief  Get the interrupt controller mask value.
             * @retval   Mask value corresponds to axp192_irq_t ,
             */
            uint64_t getIrqStatus(void)
            {
                statusRegister[0] = readRegister(AXP192_INTSTS1);
                statusRegister[1] = readRegister(AXP192_INTSTS2);
                statusRegister[2] = readRegister(AXP192_INTSTS3);
                statusRegister[3] = readRegister(AXP192_INTSTS4);
                statusRegister[4] = readRegister(AXP192_INTSTS5);
                return ((uint64_t)statusRegister[4]) << 32 |
                       ((uint64_t)statusRegister[3]) << 24 |
                       ((uint64_t)statusRegister[2]) << 16 |
                       ((uint64_t)statusRegister[1]) << 8 |
                       ((uint64_t)statusRegister[0]);
            }

            /**
             * @brief  Clear interrupt controller state.
             */
            void clearIrqStatus(void)
            {
                for (int i = 0; i < 4; i++)
                {
                    writeRegister(AXP192_INTSTS1 + i, 0xFF);
                    statusRegister[i] = 0;
                }
                writeRegister(AXP192_INTSTS5, 0xFF);
            }

            /**
             * @brief  Enable PMU interrupt control mask .
             * @param  opt: View the related chip type axp192_irq_t enumeration
             *              parameters in "XPowersParams.hpp"
             * @retval
             */
            bool enableIRQ(uint64_t opt)
            {
                return setInterruptImpl(opt, true);
            }

            /**
             * @brief  Disable PMU interrupt control mask .
             * @param  opt: View the related chip type axp192_irq_t enumeration
             *              parameters in "XPowersParams.hpp"
             * @retval
             */
            bool disableIRQ(uint64_t opt)
            {
                return setInterruptImpl(opt, false);
            }

            bool isAcinOverVoltageIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(7));
            }

            bool isAcinInserIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(6));
            }

            bool isAcinRemoveIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(5));
            }

            bool isVbusOverVoltageIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(4));
            }

            bool isVbusInsertIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(3));
            }

            bool isVbusRemoveIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(2));
            }

            bool isVbusLowVholdIrq(void)
            {
                return (bool)(statusRegister[0] & _BV(1));
            }

            bool isBatInsertIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(7));
            }

            bool isBatRemoveIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(6));
            }

            bool isBattEnterActivateIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(5));
            }

            bool isBattExitActivateIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(4));
            }

            bool isBatChargeStartIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(3));
            }

            bool isBatChargeDoneIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(2));
            }

            bool isBattTempHighIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(1));
            }

            bool isBattTempLowIrq(void)
            {
                return (bool)(statusRegister[1] & _BV(0));
            }

            bool isChipOverTemperatureIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(7));
            }

            bool isChargingCurrentLessIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(6));
            }

            bool isDC1VoltageLessIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(5));
            }

            bool isDC2VoltageLessIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(4));
            }

            bool isDC3VoltageLessIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(3));
            }

            bool isPekeyShortPressIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(1));
            }

            bool isPekeyLongPressIrq(void)
            {
                return (bool)(statusRegister[2] & _BV(0));
            }

            bool isNOEPowerOnIrq(void)
            {
                return (bool)(statusRegister[3] & _BV(7));
            }

            bool isNOEPowerDownIrq(void)
            {
                return (bool)(statusRegister[3] & _BV(6));
            }

            bool isVbusEffectiveIrq(void)
            {
                return (bool)(statusRegister[3] & _BV(5));
            }

            bool isVbusInvalidIrq(void)
            {
                return (bool)(statusRegister[3] & _BV(4));
            }

            bool isVbusSessionIrq(void)
            {
                return (bool)(statusRegister[3] & _BV(3));
            }

            bool isVbusSessionEndIrq(void)
            {
                return (bool)(statusRegister[3] & _BV(2));
            }

            bool isLowVoltageLevel2Irq(void)
            {
                return (bool)(statusRegister[3] & _BV(0));
            }

            // IRQ5 REGISTER :
            bool isWdtExpireIrq(void)
            {
                return (bool)(statusRegister[4] & _BV(7));
            }

            bool isGpio2EdgeTriggerIrq(void)
            {
                return (bool)(statusRegister[4] & _BV(2));
            }

            bool isGpio1EdgeTriggerIrq(void)
            {
                return (bool)(statusRegister[4] & _BV(1));
            }

            bool isGpio0EdgeTriggerIrq(void)
            {
                return (bool)(statusRegister[4] & _BV(0));
            }

            /*
             *   ADC Functions
             */

            bool enableBattDetection()
            {
                return setRegisterBit(AXP192_OFF_CTL, 6);
            }

            bool disableBattDetection()
            {
                return clrRegisterBit(AXP192_OFF_CTL, 6);
            }

            bool enableVbusVoltageMeasure()
            {
                return setSignalCaptureImpl(MONITOR_USB_CURRENT | MONITOR_USB_VOLTAGE, true);
            }

            bool disableVbusVoltageMeasure()
            {
                return setSignalCaptureImpl(MONITOR_USB_CURRENT | MONITOR_USB_VOLTAGE, false);
            }

            bool enableBattVoltageMeasure()
            {
                return setSignalCaptureImpl(MONITOR_BAT_CURRENT | MONITOR_BAT_VOLTAGE, true);
            }

            bool disableBattVoltageMeasure()
            {
                return setSignalCaptureImpl(MONITOR_BAT_CURRENT | MONITOR_BAT_VOLTAGE, false);
            }

            bool enableSystemVoltageMeasure()
            {
                return setSignalCaptureImpl(MONITOR_APS_VOLTAGE, true);
            }

            bool disableSystemVoltageMeasure()
            {
                return setSignalCaptureImpl(MONITOR_APS_VOLTAGE, false);
            }

            bool enableTSPinMeasure()
            {
                return setSignalCaptureImpl(MONITOR_TS_PIN, true);
            }

            bool disableTSPinMeasure()
            {
                return setSignalCaptureImpl(MONITOR_TS_PIN, false);
            }

            bool enableAdcChannel(uint32_t opts)
            {
                return setSignalCaptureImpl(opts, true);
            }

            bool disableAdcChannel(uint32_t opts)
            {
                return setSignalCaptureImpl(opts, false);
            }

            uint16_t getVbusVoltage()
            {
                if (!isVbusIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP192_VBUS_VOL_H8,
                                        AXP192_VBUS_VOL_L4) *
                       AXP192_VBUS_VOLTAGE_STEP;
            }

            float getVbusCurrent()
            {
                if (!isVbusIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP192_VBUS_CUR_H8,
                                        AXP192_VBUS_CUR_L4) *
                       AXP192_VBUS_CUR_STEP;
            }

            uint16_t getBattVoltage()
            {
                if (!isBatteryConnect())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP192_BAT_AVERVOL_H8,
                                        AXP192_BAT_AVERVOL_L4) *
                       AXP192_BATT_VOLTAGE_STEP;
            }

            float getBattDischargeCurrent()
            {
                if (!isBatteryConnect())
                {
                    return 0;
                }
                return readRegisterH8L5(AXP192_BAT_AVERDISCHGCUR_H8,
                                        AXP192_BAT_AVERDISCHGCUR_L5) *
                       AXP192_BATT_DISCHARGE_CUR_STEP;
            }

            uint16_t getAcinVoltage()
            {
                if (!isAcinIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP192_ACIN_VOL_H8, AXP192_ACIN_VOL_L4) * AXP192_ACIN_VOLTAGE_STEP;
            }

            float getAcinCurrent()
            {
                if (!isAcinIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP192_ACIN_CUR_H8, AXP192_ACIN_CUR_L4) * AXP192_ACIN_CUR_STEP;
            }

            uint16_t getSystemVoltage()
            {
                return readRegisterH8L4(AXP192_APS_AVERVOL_H8, AXP192_APS_AVERVOL_L4) * AXP192_APS_VOLTAGE_STEP;
            }

            /*
             * Timer Control
             */
            void setTimerout(uint8_t minute)
            {
                writeRegister(AXP192_TIMER_CTL, 0x80 | minute);
            }

            void disableTimer()
            {
                writeRegister(AXP192_TIMER_CTL, 0x80);
            }

            void clearTimerFlag()
            {
                setRegisterBit(AXP192_TIMER_CTL, 7);
            }

            /*
             * Data Buffer
             */
            bool writeDataBuffer(uint8_t *data, uint8_t size)
            {
                if (size > AXP192_DATA_BUFFER_SIZE)
                    return false;
                for (int i = 0; i < size; ++i)
                {
                    writeRegister(AXP192_DATA_BUFFER1 + i, data[i]);
                }
                return true;
            }

            bool readDataBuffer(uint8_t *data, uint8_t size)
            {
                if (size > AXP192_DATA_BUFFER_SIZE)
                    return false;
                for (int i = 0; i < size; ++i)
                {
                    data[i] = readRegister(AXP192_DATA_BUFFER1 + i);
                }
                return true;
            }

            /*
             * Charge led functions
             */

            /**
             * @brief Set charging led mode.
             * @retval See chg_led_mode_t enum for details.
             */
            void setChargingLedMode(pmic_chg_led_mode mode)
            {
                int val;
                switch (mode)
                {
                case CHG_LED_OFF:
                case CHG_LED_BLINK_1HZ:
                case CHG_LED_BLINK_4HZ:
                case CHG_LED_ON:
                    val = readRegister(AXP192_OFF_CTL);
                    if (val == -1)
                        return;
                    val &= 0xC7;
                    val |= 0x08; // use manual ctrl
                    val |= (mode << 4);
                    writeRegister(AXP192_OFF_CTL, val);
                    break;
                case CHG_LED_CTRL_CHG:
                    clrRegisterBit(AXP192_OFF_CTL, 3);
                    break;
                default:
                    break;
                }
            }

            pmic_chg_led_mode getChargingLedMode()
            {
                if (!getRegisterBit(AXP192_OFF_CTL, 3))
                {
                    return CHG_LED_CTRL_CHG;
                }
                int val = readRegister(AXP192_OFF_CTL);
                if (val == -1)
                    return CHG_LED_OFF;
                val &= 0x30;
                return static_cast<pmic_chg_led_mode>(val >> 4);
            }

            /*
             * Coulomb counter control
             */
            void enableCoulomb()
            {
                setRegisterBit(AXP192_COULOMB_CTL, 7);
            }

            void disableCoulomb()
            {
                clrRegisterBit(AXP192_COULOMB_CTL, 7);
            }

            void stopCoulomb()
            {
                setRegisterBit(AXP192_COULOMB_CTL, 6);
            }

            void clearCoulomb()
            {
                setRegisterBit(AXP192_COULOMB_CTL, 5);
            }

            uint32_t getBattChargeCoulomb()
            {
                int data[4];
                data[0] = readRegister(AXP192_BAT_CHGCOULOMB3);
                data[1] = readRegister(AXP192_BAT_CHGCOULOMB2);
                data[2] = readRegister(AXP192_BAT_CHGCOULOMB1);
                data[3] = readRegister(AXP192_BAT_CHGCOULOMB0);
                for (int i = 0; i < 4; ++i)
                {
                    if (data[i] == -1)
                        return 0;
                }
                return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (uint32_t)data[3];
            }

            uint32_t getBattDischargeCoulomb()
            {
                int data[4];
                data[0] = readRegister(AXP192_BAT_DISCHGCOULOMB3);
                data[1] = readRegister(AXP192_BAT_DISCHGCOULOMB2);
                data[2] = readRegister(AXP192_BAT_DISCHGCOULOMB1);
                data[3] = readRegister(AXP192_BAT_DISCHGCOULOMB0);
                for (int i = 0; i < 4; ++i)
                {
                    if (data[i] == -1)
                        return 0;
                }
                return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (uint32_t)data[3];
            }

            uint8_t getAdcSamplingRate(void)
            {
                int val = readRegister(AXP192_ADC_SPEED);
                if (val == -1)
                    return 0;
                return 25 * (int)pow(2, (val & 0xC0) >> 6);
            }

            float getCoulombData(void)
            {
                uint32_t charge = getBattChargeCoulomb(), discharge = getBattDischargeCoulomb();
                uint8_t rate = getAdcSamplingRate();
                float result = 65536.0 * 0.5 * ((float)charge - (float)discharge) / 3600.0 / rate;
                return result;
            }

            /*
             * GPIO control functions
             */
            float getBatteryChargeCurrent(void)
            {
                return readRegisterH8L5(
                           AXP192_BAT_AVERCHGCUR_H8,
                           AXP192_BAT_AVERCHGCUR_L5) *
                       AXP192_BATT_CHARGE_CUR_STEP;
            }

            uint16_t getGpio0Voltage()
            {
                return readRegisterH8L4(AXP192_GPIO0_VOL_ADC_H8, AXP192_GPIO0_VOL_ADC_L4) * AXP192_GPIO0_STEP * 1000;
            }

            uint16_t getGpio1Voltage()
            {
                return readRegisterH8L4(AXP192_GPIO1_VOL_ADC_H8, AXP192_GPIO1_VOL_ADC_L4) * AXP192_GPIO1_STEP * 1000;
            }

            int8_t pwmSetup(uint8_t channel, uint8_t freq, uint16_t duty)
            {
                // PWM输出频率 = 2.25MHz / (X+1) / Y1
                // PWM输出占空比 = Y2 / Y1
                switch (channel)
                {
                case 0:
                    writeRegister(AXP192_PWM1_FREQ_SET, freq);
                    writeRegister(AXP192_PWM1_DUTY_SET1, duty >> 8);
                    writeRegister(AXP192_PWM1_DUTY_SET2, duty & 0xFF);
                    break;
                case 1:
                    writeRegister(AXP192_PWM2_FREQ_SET, freq);
                    writeRegister(AXP192_PWM2_DUTY_SET1, duty >> 8);
                    writeRegister(AXP192_PWM2_DUTY_SET2, duty & 0xFF);
                    break;
                default:
                    return -1;
                    break;
                }
                return 0;
            }

            int8_t pwmEnable(uint8_t channel)
            {
                int val = 0;
                switch (channel)
                {
                case 0:
                    val = readRegister(AXP192_GPIO1_CTL) & 0xF8;
                    writeRegister(AXP192_GPIO1_CTL, val | 0x02);
                    return 0;
                case 1:
                    val = readRegister(AXP192_GPIO2_CTL) & 0xF8;
                    writeRegister(AXP192_GPIO2_CTL, val | 0x02);
                    return 0;
                default:
                    break;
                }
                return -1;
            }

            uint8_t getBatteryPercent(void)
            {
                if (!isBatteryConnect())
                {
                    return -1;
                }
                const static int table[11] = {
                    3000, 3650, 3700, 3740, 3760, 3795,
                    3840, 3910, 3980, 4070, 4150};
                uint16_t voltage = getBattVoltage();
                if (voltage < table[0])
                    return 0;
                for (int i = 0; i < 11; i++)
                {
                    if (voltage < table[i])
                        return i * 10 - (10UL * (int)(table[i] - voltage)) /
                                            (int)(table[i] - table[i - 1]);
                    ;
                }
                return 100;
            }

            uint8_t getChipID(void)
            {
                return readRegister(AXP192_IC_TYPE);
            }

            /*
             * GPIO setting
             */
            int8_t pinMode(uint8_t pin, uint8_t mode)
            {
                int val = 0;
                switch (pin)
                {
                case PMU_GPIO0:
                    /*
                     * 000: NMOS open-drain output
                     * 001: Universal input function
                     * 010: Low noise LDO
                     * 011: reserved
                     * 100: ADC input
                     * 101: Low output
                     * 11X: Floating
                     * * */
                    if (mode == INPUT || mode == INPUT_PULLDOWN)
                    {
                        if (gpio[pin].mode != INPUT)
                        {
                            gpio[pin].mode = INPUT;
                        }
                        val = readRegister(AXP192_GPIO0_CTL) & 0xF8;
                        writeRegister(AXP192_GPIO0_CTL, val | 0x01);
                        // Set pull-down mode
                        val = readRegister(AXP192_GPIO012_PULLDOWN) & 0xFE;
                        if (mode == INPUT_PULLDOWN)
                        {
                            writeRegister(AXP192_GPIO012_PULLDOWN, val | 0x01);
                        }
                        else
                        {
                            writeRegister(AXP192_GPIO012_PULLDOWN, val);
                        }
                    }
                    break;

                case PMU_GPIO1:
                    /*
                     * 000: NMOS open-drain output
                     * 001: Universal input function
                     * 010: PWM1 output, high level is VINT, not Can add less than 100K pull-down resistance
                     * 011: reserved
                     * 100: ADC input
                     * 101: Low output
                     * 11X: Floating
                     * * */
                    if (mode == INPUT || mode == INPUT_PULLDOWN)
                    {
                        if (gpio[pin].mode != INPUT)
                        {
                            gpio[pin].mode = INPUT;
                        }
                        val = readRegister(AXP192_GPIO1_CTL) & 0xF8;
                        writeRegister(AXP192_GPIO1_CTL, val | 0x01);

                        // Set pull-down mode
                        val = readRegister(AXP192_GPIO012_PULLDOWN) & 0xFD;
                        if (mode == INPUT_PULLDOWN)
                        {
                            writeRegister(AXP192_GPIO012_PULLDOWN, val | 0x02);
                        }
                        else
                        {
                            writeRegister(AXP192_GPIO012_PULLDOWN, val);
                        }
                    }
                    break;

                case PMU_GPIO2:
                    /*
                     * 000: NMOS open-drain output
                     * 001: Universal input function
                     * 010: PWM2 output, high level is VINT, not Can add less than 100K pull-down resistance
                     * 011: reserved
                     * 100: ADC input
                     * 101: Low output
                     * 11X: Floating
                     * */
                    if (mode == INPUT || mode == INPUT_PULLDOWN)
                    {
                        if (gpio[pin].mode != INPUT)
                        {
                            gpio[pin].mode = INPUT;
                        }
                        val = readRegister(AXP192_GPIO2_CTL) & 0xF8;
                        writeRegister(AXP192_GPIO2_CTL, val | 0x01);

                        // Set pull-down mode
                        val = readRegister(AXP192_GPIO012_PULLDOWN) & 0xFB;
                        if (mode == INPUT_PULLDOWN)
                        {
                            writeRegister(AXP192_GPIO012_PULLDOWN, val | 0x04);
                        }
                        else
                        {
                            writeRegister(AXP192_GPIO012_PULLDOWN, val);
                        }
                    }
                    break;

                case PMU_GPIO3:
                    /*
                     * 00: External charging control
                     * 01: NMOS open-drain output port 3
                     * 10: Universal input port 3
                     * 11: ADC input
                     * * */
                    if (mode == INPUT)
                    {
                        if (gpio[pin].mode != INPUT)
                        {
                            gpio[pin].mode = INPUT;
                        }
                        val = readRegister(AXP192_GPIO34_CTL) & 0xFC;
                        writeRegister(AXP192_GPIO34_CTL, val | 0x82);
                    }
                    break;

                case PMU_GPIO4:
                    /*
                     * 00: External charging control
                     * 01: NMOS open-drain output port 4
                     * 10: Universal input port 4
                     * 11: undefined
                     * * */
                    if (mode == INPUT)
                    {
                        if (gpio[pin].mode != INPUT)
                        {
                            gpio[pin].mode = INPUT;
                        }
                        val = readRegister(AXP192_GPIO34_CTL) & 0xF3;
                        writeRegister(AXP192_GPIO34_CTL, val | 0x88);
                    }
                    break;
                case PMU_GPIO5:
                    if (mode == INPUT)
                    {
                        if (gpio[pin].mode != INPUT)
                        {
                            gpio[pin].mode = INPUT;
                        }
                        val = readRegister(AXP192_GPIO5_CTL) & 0xBF;
                        writeRegister(AXP192_GPIO5_CTL, val | 0x40);
                    }
                    break;
                default:
                    break;
                }
                return 0;
            }

            uint8_t digitalRead(pmic_num pin)
            {
                switch (pin)
                {
                case PMU_GPIO0:
                    return getRegisterBit(AXP192_GPIO012_SIGNAL, 4);
                case PMU_GPIO1:
                    return getRegisterBit(AXP192_GPIO012_SIGNAL, 5);
                case PMU_GPIO2:
                    return getRegisterBit(AXP192_GPIO012_SIGNAL, 6);
                case PMU_GPIO3:
                    return getRegisterBit(AXP192_GPIO34_SIGNAL, 4);
                case PMU_GPIO4:
                    return getRegisterBit(AXP192_GPIO34_SIGNAL, 5);
                case PMU_GPIO5:
                    return getRegisterBit(AXP192_GPIO5_CTL, 4);
                default:
                    break;
                }
                return 0;
            }

            void digitalWrite(pmic_num pin, uint8_t val)
            {
                int reg = 0;
                switch (pin)
                {
                case PMU_GPIO0:
                    if (gpio[pin].mode != OUTPUT)
                    {
                        gpio[pin].mode = OUTPUT;
                    }
                    reg = readRegister(AXP192_GPIO0_CTL) & 0xFE;
                    writeRegister(AXP192_GPIO0_CTL, val ? (reg | 0x01) : reg);
                    break;
                case PMU_GPIO1:
                    if (gpio[pin].mode != OUTPUT)
                    {
                        gpio[pin].mode = OUTPUT;
                    }
                    reg = readRegister(AXP192_GPIO1_CTL) & 0xFD;
                    writeRegister(AXP192_GPIO1_CTL, val ? (reg | 0x01) : reg);
                    break;
                case PMU_GPIO2:
                    if (gpio[pin].mode != OUTPUT)
                    {
                        gpio[pin].mode = OUTPUT;
                    }
                    reg = readRegister(AXP192_GPIO2_CTL) & 0xFB;
                    writeRegister(AXP192_GPIO2_CTL, val ? (reg | 0x01) : reg);
                    break;
                case PMU_GPIO3:
                    if (gpio[pin].mode != OUTPUT)
                    {
                        gpio[pin].mode = OUTPUT;
                        reg = readRegister(AXP192_GPIO34_CTL) & 0xFC;
                        writeRegister(AXP192_GPIO34_CTL, reg | 0x01);
                    }
                    reg = readRegister(AXP192_GPIO34_SIGNAL) & 0xF7;
                    writeRegister(AXP192_GPIO34_SIGNAL, val ? (val | 0x08) : reg);
                    break;
                case PMU_GPIO4:
                    if (gpio[pin].mode != OUTPUT)
                    {
                        gpio[pin].mode = OUTPUT;
                        reg = readRegister(AXP192_GPIO34_CTL) & 0xF3;
                        writeRegister(AXP192_GPIO34_CTL, reg | 0x04);
                    }
                    reg = readRegister(AXP192_GPIO34_SIGNAL) & 0xEF;
                    writeRegister(AXP192_GPIO34_SIGNAL, val ? (val | 0x10) : reg);
                    break;
                case PMU_GPIO5:
                    if (gpio[pin].mode != OUTPUT)
                    {
                        gpio[pin].mode = OUTPUT;
                        reg = readRegister(AXP192_GPIO5_CTL) & 0xBF;
                        writeRegister(AXP192_GPIO5_CTL, reg);
                    }
                    reg = readRegister(AXP192_GPIO5_CTL) & 0xDF;
                    writeRegister(AXP192_GPIO5_CTL, val ? (reg | 0x20) : reg);
                    break;
                default:
                    break;
                }
            }

            /**
             * Sleep function
             */
            bool enableSleep()
            {
                return setRegisterBit(AXP192_VOFF_SET, 3);
            }

            /*
             * Pekey function
             */

            /**
             * @brief Set the PEKEY power-on long press time.
             * @param opt: See pmic_press_on_time enum for details.
             * @retval
             */
            bool setPowerKeyPressOnTime(pmic_press_on_time opt)
            {
                int val = readRegister(AXP192_POK_SET);
                if (val == -1)
                    return false;
                return 0 == writeRegister(AXP192_POK_SET, (val & 0x3F) | (opt << 6));
            }

            /**
             * @brief Get the PEKEY power-on long press time.
             * @retval See press_on_time_t enum for details.
             */
            pmic_press_on_time getPowerKeyPressOnTime()
            {
                int val = readRegister(AXP192_POK_SET);
                if (val == -1)
                    return pmic_press_on_time(0);
                return static_cast<pmic_press_on_time>((val & 0xC0) >> 6);
            }

            /**
             * @brief Set the PEKEY power-off long press time.
             * @param opt: See pmic_press_off_time enum for details.
             * @retval
             */
            bool setPowerKeyPressOffTime(pmic_press_off_time opt)
            {
                int val = readRegister(AXP192_POK_SET);
                if (val == -1)
                    return false;
                return 0 == writeRegister(AXP192_POK_SET, (val & 0xFC) | opt);
            }

            /**
             * @brief Get the PEKEY power-off long press time.
             * @retval See pmic_press_off_time enum for details.
             */
            pmic_press_off_time getPowerKeyPressOffTime()
            {
                int val = readRegister(AXP192_POK_SET);
                if (val == -1)
                    return pmic_press_off_time(0);
                return static_cast<pmic_press_off_time>(val & 0x03);
            }

            void setPowerKeyLongPressOnTime(pmic_pekey_long_press opt)
            {
                int val = readRegister(AXP192_POK_SET);
                if (val == -1)
                    return;
                writeRegister(AXP192_POK_SET, (val & 0xCF) | (opt << 4));
            }

            void enablePowerKeyLongPressPowerOff()
            {
                setRegisterBit(AXP192_POK_SET, 3);
            }

            void disablePowerKeyLongPressPowerOff()
            {
                clrRegisterBit(AXP192_POK_SET, 3);
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
                case LDO2:
                    return getLDO2Voltage();
                case LDO3:
                    return getLDO3Voltage();
                case LDOIO:
                    return getLDOioVoltage();
                default:
                    break;
                }
                return 0;
            }

            bool inline enablePowerOutput(PMICPowerChannel channel)
            {
                /*
                return 0 == writeRegister(AXP192_LDO23_DC123_EXT_CTL,
                                          val | readRegister(AXP192_LDO23_DC123_EXT_CTL));
                */
                switch (channel)
                {
                case DCDC1:
                    return enableDC1();
                case DCDC2:
                    return enableDC2();
                case DCDC3:
                    return enableDC3();
                case LDO2:
                    return enableLDO2();
                case LDO3:
                    return enableLDO3();
                case LDOIO:
                    return enableLDOio();
                case VBACKUP:
                    return enableBackupBattCharger();
                default:
                    break;
                }
                return false;
            }

            bool inline disablePowerOutput(PMICPowerChannel channel)
            {
                /*
                return 0 == writeRegister(AXP192_LDO23_DC123_EXT_CTL,
                                          val & readRegister(AXP192_LDO23_DC123_EXT_CTL));
                */
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
                case LDO2:
                    return disableLDO2();
                case LDO3:
                    return disableLDO3();
                case LDOIO:
                    return disableLDOio();
                case VBACKUP:
                    return disableBackupBattCharger();
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
                case LDO2:
                    return isEnableLDO2();
                case LDO3:
                    return isEnableLDO3();
                case LDOIO:
                    return isEnableLDOio();
                case VBACKUP:
                    return isEnableBackupCharger();
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
                case LDO2:
                    return setLDO2Voltage(millivolt);
                case LDO3:
                    return setLDO3Voltage(millivolt);
                case LDOIO:
                    return setLDOioVoltage(millivolt);
                case VBACKUP:
                // TODO:
                //  return setBackupBattChargerVoltage(millivolt);
                default:
                    break;
                }
                return false;
            }

            bool inline getPowerEnable(uint8_t val)
            {
                return (bool)(readRegister(AXP192_LDO23_DC123_EXT_CTL) & val);
            }

            bool initImpl()
            {
                if (getChipID() == AXP192_CHIP_ID)
                {
                    setChipModel(PMICChipModel::AXP192);
                    return true;
                }
                return false;
            }

            /*
             * Interrupt control functions
             */
            bool setInterruptImpl(uint64_t opts, bool enable)
            {
                int res = 0;
                int data = 0, value = 0;

                log_d("%s %s - 0x%llx\n", __func__, enable ? "ENABLE" : "DISABLE", opts);

                if (opts & 0xFF)
                {
                    value = opts & 0xFF;
                    data = readRegister(AXP192_INTEN1);
                    res |= writeRegister(AXP192_INTEN1, enable ? (data | value) : (data & (~value)));
                }

                if (opts & 0xFF00)
                {
                    value = opts >> 8;
                    data = readRegister(AXP192_INTEN2);
                    res |= writeRegister(AXP192_INTEN2, enable ? (data | value) : (data & (~value)));
                }

                if (opts & 0xFF0000)
                {
                    value = opts >> 16;
                    data = readRegister(AXP192_INTEN3);
                    res |= writeRegister(AXP192_INTEN3, enable ? (data | value) : (data & (~value)));
                }

                if (opts & 0xFF000000)
                {
                    value = opts >> 24;
                    data = readRegister(AXP192_INTEN4);
                    res |= writeRegister(AXP192_INTEN4, enable ? (data | value) : (data & (~value)));
                }

                if (opts & 0xFF00000000)
                {
                    value = opts >> 32;
                    data = readRegister(AXP192_INTEN5);
                    res |= writeRegister(AXP192_INTEN5, enable ? (data | value) : (data & (~value)));
                }
                return res == 0;
            }

            /*
             * Signal Capture control functions
             */
            bool setSignalCaptureImpl(uint32_t opts, bool enable)
            {
                int value = 0;
                if (opts & 0xFF)
                {
                    value = readRegister(AXP192_ADC_EN1);
                    writeRegister(AXP192_ADC_EN1, enable ? (value | opts) : (value & (~opts)));
                }
                if (opts & 0xFF00)
                {
                    opts >>= 8;
                    value = readRegister(AXP192_ADC_EN2);
                    writeRegister(AXP192_ADC_EN2, enable ? (value | opts) : (value & (~opts)));
                }
                return true;
            }

            const char *getChipNameImpl(void)
            {
                return "AXP192";
            }

        private:
            const uint16_t chargeTargetVol[4] = {4100, 4150, 4200, 4360};
            uint8_t statusRegister[AXP192_INTSTS_CNT];
            gpio_t gpio[AXP192_GPIO_CNT];
        };
    }
}