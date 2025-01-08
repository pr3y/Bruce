#include "../REG/AXP202.hpp"
#include "../TYPE1.tpp"
namespace HAL
{
    namespace PMIC
    {
        typedef class AXP202 THIS;
        class AXP202 : public Type1<class AXP202>
        {
            friend class Base<Type1<AXP202>, AXP202>;

            typedef enum
            {
                MONITOR_TS_PIN = _BV(0),
                MONITOR_APS_VOLTAGE = _BV(1),
                MONITOR_USB_CURRENT = _BV(2),
                MONITOR_USB_VOLTAGE = _BV(3),
                MONITOR_AC_CURRENT = _BV(4),
                MONITOR_AC_VOLTAGE = _BV(5),
                MONITOR_BAT_CURRENT = _BV(6),
                MONITOR_BAT_VOLTAGE = _BV(7),
                MONITOR_ADC_IO3 = _BV(8),
                MONITOR_ADC_IO2 = _BV(9),
                MONITOR_ADC_IO1 = _BV(10),
                MONITOR_ADC_IO0 = _BV(11),
                MONITOR_TEMPERATURE = _BV(16),
            } axp202_adc_func;

        public:
            AXP202(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = AXP202_SLAVE_ADDRESS)
            {
                this->myWire = &w;
                this->mySDA = sda;
                this->mySCL = scl;
                this->myADDR = addr;
            }

            AXP202(uint8_t addr, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback)
            {
                this->thisReadRegCallback = readRegCallback;
                this->thisWriteRegCallback = writeRegCallback;
                this->myADDR = addr;
            }

            AXP202()
            {
                this->myWire = &Wire;
                this->mySDA = SDA;
                this->mySCL = SCL;
                this->myADDR = AXP202_SLAVE_ADDRESS;
            }

            ~AXP202()
            {
                log_i("~AXP202");
                deinit();
            }

            bool init(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = AXP202_SLAVE_ADDRESS)
            {
                this->myWire = &w;
                this->mySDA = sda;
                this->mySCL = scl;
                this->myADDR = addr;
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
                return readRegister(AXP202_STATUS);
            }

            bool isAcinVbusStart()
            {
                return getRegisterBit(AXP202_STATUS, 0);
            }

            bool isDischarge()
            {
                return !getRegisterBit(AXP202_STATUS, 2);
            }

            bool isVbusIn(void)
            {
                return getRegisterBit(AXP202_STATUS, 5);
            }

            bool isAcinEfficient()
            {
                return getRegisterBit(AXP202_STATUS, 6);
            }

            bool isAcinIn()
            {
                return getRegisterBit(AXP202_STATUS, 7);
            }

            bool isOverTemperature()
            {
                return getRegisterBit(AXP202_MODE_CHGSTATUS, 7);
            }

            bool isCharging(void)
            {
                return getRegisterBit(AXP202_MODE_CHGSTATUS, 6);
            }

            bool isBatteryConnect(void)
            {
                return getRegisterBit(AXP202_MODE_CHGSTATUS, 5);
            }

            bool isBattInActiveMode()
            {
                return getRegisterBit(AXP202_MODE_CHGSTATUS, 3);
            }

            bool isChargeCurrLessPreset()
            {
                return getRegisterBit(AXP202_MODE_CHGSTATUS, 2);
            }

            void enableVbusVoltageLimit()
            {
                setRegisterBit(AXP202_IPS_SET, 6);
            }

            void disableVbusVoltageLimit()
            {
                clrRegisterBit(AXP202_IPS_SET, 6);
            }

            /**
             * @brief  Set VBUS Voltage Input Limit.
             * @param  opt: View the related chip type pmic_vbus_vol_limit enumeration
             *              parameters in "XPowersParams.hpp"
             */
            void setVbusVoltageLimit(pmic_vbus_vol_limit opt)
            {
                int val = readRegister(AXP202_IPS_SET);
                if (val == -1)
                    return;
                val &= 0xC7;
                writeRegister(AXP202_IPS_SET, val | (opt << 3));
            }

            /**
             * @brief  Get VBUS Voltage Input Limit.
             * @retval View the related chip type pmic_vbus_vol_limit enumeration
             *              parameters in "XPowersParams.hpp"
             */
            pmic_vbus_vol_limit getVbusVoltageLimit(void)
            {
                return static_cast<pmic_vbus_vol_limit>((readRegister(AXP202_IPS_SET) >> 3) & 0x7);
            }

            /**
             * @brief  Set VBUS Current Input Limit.
             * @param  opt: View the related chip type pmic_vbus_cur_limit enumeration
             *              parameters in "XPowersParams.hpp"
             * @retval true valid false invalid
             */
            bool setVbusCurrentLimit(pmic_vbus_cur_limit opt)
            {
                int val = readRegister(AXP202_IPS_SET);
                if (val == -1)
                    return false;
                val &= 0xFC;
                switch (opt)
                {
                case AXP202_VBUS_CUR_LIM_900MA:
                    return writeRegister(AXP202_IPS_SET, val);
                case AXP202_VBUS_CUR_LIM_500MA:
                    return writeRegister(AXP202_IPS_SET, val | 0x01);
                case AXP202_VBUS_CUR_LIM_100MA:
                    return writeRegister(AXP202_IPS_SET, val | 0x02);
                case AXP202_VBUS_CUR_LIM_OFF:
                    return writeRegister(AXP202_IPS_SET, val | 0x03);
                default:
                    break;
                }
                return false;
            }

            /**
             * @brief  Get VBUS Current Input Limit.
             * @retval View the related chip type axp202_vbus_cur_limit_t enumeration
             *              parameters in "XPowersParams.hpp"
             */
            pmic_vbus_cur_limit getVbusCurrentLimit(void)
            {
                int val = readRegister(AXP202_IPS_SET);
                if (val == -1)
                    return pmic_vbus_cur_limit(0);
                val &= 0x03;
                return static_cast<pmic_vbus_cur_limit>(val);
            }

            // Set the minimum system operating voltage inside the PMU,
            // below this value will shut down the PMU,Adjustment range 2600mV ~ 3300mV
            bool setSysPowerDownVoltage(uint16_t millivolt)
            {
                if (millivolt % AXP202_SYS_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_SYS_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP202_VOFF_VOL_MIN)
                {
                    log_e("Mistake ! SYS minimum output voltage is  %umV", AXP202_VOFF_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP202_VOFF_VOL_MAX)
                {
                    log_e("Mistake ! SYS maximum output voltage is  %umV", AXP202_VOFF_VOL_MAX);
                    return false;
                }

                int val = readRegister(AXP202_VOFF_SET);
                if (val == -1)
                    return false;
                val &= 0xF8;
                val |= (millivolt - AXP202_VOFF_VOL_MIN) / AXP202_SYS_VOL_STEPS;
                return 0 == writeRegister(AXP202_VOFF_SET, val);
            }

            uint16_t getSysPowerDownVoltage()
            {
                int val = readRegister(AXP202_VOFF_SET);
                if (val == -1)
                    return 0;
                val &= 0x07;
                return (val * AXP202_SYS_VOL_STEPS) + AXP202_VOFF_VOL_MIN;
            }

            /**
             * @brief  Set shutdown, calling shutdown will turn off all power channels,
             *         only VRTC belongs to normal power supply
             * @retval None
             */
            void shutdown()
            {
                setRegisterBit(AXP202_OFF_CTL, 7);
            }

            /*
             * Charge setting
             */
            void enableCharge()
            {
                setRegisterBit(AXP202_CHARGE1, 7);
            }

            void disableCharge()
            {
                clrRegisterBit(AXP202_CHARGE1, 7);
            }

            /**
             * @brief Set charge target voltage.
             * @param  opt: See pmic_chg_vol enum for details.
             * @retval
             */
            bool setChargeTargetVoltage(pmic_chg_vol opt)
            {
                if (opt >= AXP202_CHG_VOL_MAX)
                    return false;
                int val = readRegister(AXP202_CHARGE1);
                if (val == -1)
                    return false;
                val &= 0x9F;
                return 0 == writeRegister(AXP202_CHARGE1, val | (opt << 5));
            }

            /**
             * @brief Get charge target voltage settings.
             * @retval See pmic_chg_vol enum for details.
             */
            pmic_chg_vol getChargeTargetVoltage()
            {
                int val = readRegister(AXP202_CHARGE1);
                if (val == -1)
                    return pmic_chg_vol(0);
                return static_cast<pmic_chg_vol>((val & 0x60) >> 5);
            }

            /**
             * @brief Set charge current settings.
             * @retval See pmic_chg_curr enum for details.
             */
            bool setChargerConstantCurr(pmic_chg_curr opt)
            {
                if (opt > 0x0F)
                    return false;
                int val = readRegister(AXP202_CHARGE1);
                if (val == -1)
                {
                    return false;
                }
                val &= 0xF0;
                return 0 == writeRegister(AXP202_CHARGE1, val | opt);
            }

            /**
             * @brief Get charge current settings.
             * @retval See pmic_chg_curr enum for details.
             */
            pmic_chg_curr getChargerConstantCurr(void)
            {
                int val = readRegister(AXP202_CHARGE1) & 0x0F;
                // if (val == -1)return AXP202_CHG_CUR_780MA;
                // todo:
                return static_cast<pmic_chg_curr>(val);
            }

            void setChargerTerminationCurr(pmic_chg_iterm opt)
            {
                switch (opt)
                {
                case AXP202_CHG_ITERM_LESS_10_PERCENT:
                    clrRegisterBit(AXP202_CHARGE1, 0);
                    break;
                case AXP202_CHG_ITERM_LESS_15_PERCENT:
                    setRegisterBit(AXP202_CHARGE1, 0);
                    break;
                default:
                    break;
                }
            }

            pmic_chg_iterm getChargerTerminationCurr()
            {
                return static_cast<pmic_chg_iterm>(getRegisterBit(AXP202_CHARGE1, 4));
            }

            bool setPrechargeTimeout(pmic_prechg_to opt)
            {
                int val = readRegister(AXP202_CHARGE2);
                if (val == -1)
                    return false;
                val &= 0x3F;
                return 0 == writeRegister(AXP202_CHARGE2, val | (opt << 6));
            }

            // External channel charge current setting,Range:300~1000mA
            bool setChargerExternChannelCurr(uint16_t milliampere)
            {
                if (milliampere % AXP202_CHG_EXT_CURR_STEP)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_CHG_EXT_CURR_STEP);
                    return false;
                }
                if (milliampere < AXP202_CHG_EXT_CURR_MIN)
                {
                    log_e("Mistake ! The minimum external path charge current setting is:  %umA", AXP202_CHG_EXT_CURR_MIN);
                    return false;
                }
                else if (milliampere > AXP202_CHG_EXT_CURR_MAX)
                {
                    log_e("Mistake ! The maximum external channel charge current setting is:  %umA", AXP202_CHG_EXT_CURR_MAX);
                    return false;
                }
                int val = readRegister(AXP202_CHARGE2);
                if (val == -1)
                    return false;
                val &= 0xC7;
                val |= ((milliampere - AXP202_CHG_EXT_CURR_MIN) / AXP202_CHG_EXT_CURR_STEP);
                return 0 == writeRegister(AXP202_CHARGE2, val);
            }

            bool enableChargerExternChannel()
            {
                return setRegisterBit(AXP202_CHARGE2, 2);
            }

            bool disableChargerExternChannel()
            {
                return clrRegisterBit(AXP202_CHARGE2, 2);
            }

            // Timeout setting in constant current mode
            bool setChargerConstantTimeout(pmic_chg_cons_to opt)
            {
                int val = readRegister(AXP202_CHARGE2);
                if (val == -1)
                    return false;
                val &= 0xFC;
                return 0 == writeRegister(AXP202_CHARGE2, val | opt);
            }

            bool enableBackupBattCharger()
            {
                return setRegisterBit(AXP202_BACKUP_CHG, 7);
            }

            bool disableBackupBattCharger()
            {
                return clrRegisterBit(AXP202_BACKUP_CHG, 7);
            }

            bool isEnableBackupCharger()
            {
                return getRegisterBit(AXP202_BACKUP_CHG, 7);
            }

            bool setBackupBattChargerVoltage(pmic_backup_batt_vol opt)
            {
                int val = readRegister(AXP202_BACKUP_CHG);
                if (val == -1)
                    return false;
                val &= 0x9F;
                return 0 == writeRegister(AXP202_BACKUP_CHG, val | (opt << 5));
            }

            bool setBackupBattChargerCurr(pmic_backup_batt_curr opt)
            {
                int val = readRegister(AXP202_BACKUP_CHG);
                if (val == -1)
                    return false;
                val &= 0xFC;
                return 0 == writeRegister(AXP202_BACKUP_CHG, val | opt);
            }

            /*
             * Temperature
             */
            float getTemperature()
            {
                return readRegisterH8L4(AXP202_INTERNAL_TEMP_H8, AXP202_INTERNAL_TEMP_L4) * AXP202_INTERNAL_TEMP_STEP - AXP202_INERNAL_TEMP_OFFSET;
            }

            bool enableTemperatureMeasure()
            {
                return setRegisterBit(AXP202_ADC_EN2, 7);
            }

            bool disableTemperatureMeasure()
            {
                return clrRegisterBit(AXP202_ADC_EN2, 7);
            }

            /*
             * Power control LDOio functions
             */
            bool isEnableLDOio(void)
            {
                int val = readRegister(AXP202_GPIO0_CTL);
                return (val & 0x02);
            }

            bool enableLDOio(void)
            {
                int val = readRegister(AXP202_GPIO0_CTL) & 0xF8;
                return 0 == writeRegister(AXP202_GPIO0_CTL, val | 0x02);
            }

            bool disableLDOio(void)
            {
                int val = readRegister(AXP202_GPIO0_CTL) & 0xF8;
                return 0 == writeRegister(AXP202_GPIO0_CTL, val);
            }

            bool setLDOioVoltage(uint16_t millivolt)
            {
                if (millivolt % AXP202_LDOIO_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_LDOIO_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP202_LDOIO_VOL_MIN)
                {
                    log_e("Mistake ! LDOIO minimum output voltage is  %umV", AXP202_LDOIO_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP202_LDOIO_VOL_MAX)
                {
                    log_e("Mistake ! LDOIO maximum output voltage is  %umV", AXP202_LDOIO_VOL_MAX);
                    return false;
                }
                int val = readRegister(AXP202_GPIO0_VOL);
                if (val == -1)
                    return false;
                val |= (((millivolt - AXP202_LDOIO_VOL_MIN) / AXP202_LDOIO_VOL_STEPS) << 4);
                return 0 == writeRegister(AXP202_GPIO0_VOL, val);
            }

            uint16_t getLDOioVoltage(void)
            {
                int val = readRegister(AXP202_GPIO0_VOL);
                if (val == -1)
                    return 0;
                val >>= 4;
                val *= AXP202_LDOIO_VOL_STEPS;
                val += AXP202_LDOIO_VOL_MIN;
                return val;
            }

            /*
             * Power control LDO2 functions
             */
            bool isEnableLDO2(void)
            {
                return getRegisterBit(AXP202_LDO234_DC23_CTL, 2);
            }

            bool enableLDO2(void)
            {
                return setRegisterBit(AXP202_LDO234_DC23_CTL, 2);
            }

            bool disableLDO2(void)
            {
                return clrRegisterBit(AXP202_LDO234_DC23_CTL, 2);
            }

            bool setLDO2Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP202_LDO2_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_LDO2_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP202_LDO2_VOL_MIN)
                {
                    log_e("Mistake ! LDO2 minimum output voltage is  %umV", AXP202_LDO2_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP202_LDO2_VOL_MAX)
                {
                    log_e("Mistake ! LDO2 maximum output voltage is  %umV", AXP202_LDO2_VOL_MAX);
                    return false;
                }

                int val = readRegister(AXP202_LDO24OUT_VOL);
                if (val == -1)
                    return false;
                val &= 0x0F;
                return 0 == writeRegister(AXP202_LDO24OUT_VOL, val | (((millivolt - AXP202_LDO2_VOL_MIN) / AXP202_LDO2_VOL_STEPS) << AXP202_LDO2_VOL_BIT_MASK));
            }

            uint16_t getLDO2Voltage(void)
            {
                int val = readRegister(AXP202_LDO24OUT_VOL) & 0xF0;
                return (val >> AXP202_LDO2_VOL_BIT_MASK) * AXP202_LDO2_VOL_STEPS + AXP202_LDO2_VOL_MIN;
            }

            /*
             * Power control LDO3 functions
             */
            bool isEnableLDO3(void)
            {
                return getRegisterBit(AXP202_LDO234_DC23_CTL, 6);
            }

            bool enableLDO3(void)
            {
                return setRegisterBit(AXP202_LDO234_DC23_CTL, 6);
            }

            bool disableLDO3(void)
            {
                return clrRegisterBit(AXP202_LDO234_DC23_CTL, 6);
            }

            bool setLDO3Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP202_LDO3_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_LDO3_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP202_LDO3_VOL_MIN)
                {
                    log_e("Mistake ! LDO3 minimum output voltage is  %umV", AXP202_LDO3_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP202_LDO3_VOL_MAX)
                {
                    log_e("Mistake ! LDO3 maximum output voltage is  %umV", AXP202_LDO3_VOL_MAX);
                    return false;
                }
                int val = readRegister(AXP202_LDO3OUT_VOL) & 0x80;
                return 0 == writeRegister(AXP202_LDO3OUT_VOL, val | ((millivolt - AXP202_LDO3_VOL_MIN) / AXP202_LDO3_VOL_STEPS));
            }

            uint16_t getLDO3Voltage(void)
            {
                int val = readRegister(AXP202_LDO3OUT_VOL);
                if (val == -1)
                    return 0;
                // Returns the voltage value of VBUS if in pass-through mode
                if (val & 0x80)
                {
                    log_i("ldo3 pass-through mode");
                    return getVbusVoltage();
                }
                val &= 0x7F;
                return (val * AXP202_LDO3_VOL_STEPS) + AXP202_LDO3_VOL_MIN;
            }

            /*
             * Power control LDO4 functions
             */
            bool isEnableLDO4(void)
            {
                return getRegisterBit(AXP202_LDO234_DC23_CTL, 3);
            }

            bool enableLDO4(void)
            {
                return setRegisterBit(AXP202_LDO234_DC23_CTL, 3);
            }

            bool disableLDO4(void)
            {
                return clrRegisterBit(AXP202_LDO234_DC23_CTL, 3);
            }

            // Support setting voltage
            // 1.25
            // 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0
            // 2.5 2.7 2.8 3.0 3.1 3.2 3.3
            bool setLDO4Voltage(uint16_t millivolt)
            {
                int index = -1;
                for (int i = 0; i < 16; ++i)
                {
                    if (ldo4_table[i] == millivolt)
                    {
                        index = i;
                        break;
                    }
                }
                if (index == -1)
                {
                    log_e("Mistake ! Out of adjustment range");
                    return false;
                }
                int val = readRegister(AXP202_LDO24OUT_VOL);
                if (val == -1)
                {
                    return false;
                }
                return 0 == writeRegister(AXP202_LDO24OUT_VOL, (val & 0xF0) | index);
            }

            uint16_t getLDO4Voltage(void)
            {
                int val = readRegister(AXP202_LDO24OUT_VOL);
                if (val == -1)
                    return 0;
                val &= 0x0F;
                return ldo4_table[val];
            }

            /*
             * Power control DCDC2 functions
             */
            void setDC2PwmMode(void)
            {
                int val = readRegister(AXP202_DCDC_MODESET) & 0xFB;
                writeRegister(AXP202_DCDC_MODESET, val | 0x04);
            }

            void setDC2AutoMode(void)
            {
                int val = readRegister(AXP202_DCDC_MODESET) & 0xFB;
                writeRegister(AXP202_DCDC_MODESET, val);
            }

            void enableDC2VRC(void)
            {
                // int val =  readRegister(AXP202_DC2_DVM);
                // writeRegister(AXP202_DC2_DVM, val | 0x04);
            }

            void disableDC2VRC(void)
            {
                // int val =  readRegister(AXP202_DC2_DVM);
                // writeRegister(AXP202_DC2_DVM, val & 0xFB);
            }

            bool setDC2VRC(uint8_t opts)
            {
                // if (opts > 1) {
                //     return false;
                // }
                // int val =  readRegister(AXP202_DC2_DVM) & 0xFE;
                // writeRegister(AXP202_DC2_DVM, val | opts);
                return false;
            }

            bool isEnableDC2VRC(void)
            {
                // return (readRegister(AXP202_DC2_DVM) & 0x04) == 0x04;
                return 0;
            }

            bool isEnableDC2(void)
            {
                return getRegisterBit(AXP202_LDO234_DC23_CTL, 4);
            }

            bool enableDC2(void)
            {
                return setRegisterBit(AXP202_LDO234_DC23_CTL, 4);
            }

            bool disableDC2(void)
            {
                return clrRegisterBit(AXP202_LDO234_DC23_CTL, 4);
            }

            bool setDC2Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP202_DC2_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_DC2_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP202_DC2_VOL_MIN)
                {
                    log_e("Mistake ! DCDC2 minimum output voltage is  %umV", AXP202_DC2_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP202_DC2_VOL_MAX)
                {
                    log_e("Mistake ! DCDC2 maximum output voltage is  %umV", AXP202_DC2_VOL_MAX);
                    return false;
                }
                int val = readRegister(AXP202_DC2OUT_VOL);
                if (val == -1)
                    return false;
                val &= 0x80;
                val |= (millivolt - AXP202_DC2_VOL_MIN) / AXP202_DC2_VOL_STEPS;
                return 0 == writeRegister(AXP202_DC2OUT_VOL, val);
            }

            uint16_t getDC2Voltage(void)
            {
                int val = readRegister(AXP202_DC2OUT_VOL);
                if (val == -1)
                    return 0;
                return (val * AXP202_DC2_VOL_STEPS) + AXP202_DC2_VOL_MIN;
            }

            /*
             * Power control DCDC3 functions
             */
            void setDC3PwmMode(void)
            {
                int val = readRegister(AXP202_DCDC_MODESET) & 0xFD;
                writeRegister(AXP202_DCDC_MODESET, val | 0x02);
            }

            void setDC3AutoMode(void)
            {
                int val = readRegister(AXP202_DCDC_MODESET) & 0xFD;
                writeRegister(AXP202_DCDC_MODESET, val);
            }

            bool isEnableDC3(void)
            {
                return getRegisterBit(AXP202_LDO234_DC23_CTL, 1);
            }

            bool enableDC3(void)
            {
                return setRegisterBit(AXP202_LDO234_DC23_CTL, 1);
            }

            bool disableDC3(void)
            {
                return clrRegisterBit(AXP202_LDO234_DC23_CTL, 1);
            }

            bool setDC3Voltage(uint16_t millivolt)
            {
                if (millivolt % AXP202_DC3_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", AXP202_DC3_VOL_STEPS);
                    return false;
                }
                if (millivolt < AXP202_DC3_VOL_MIN)
                {
                    log_e("Mistake ! DCDC3 minimum output voltage is  %umV", AXP202_DC3_VOL_MIN);
                    return false;
                }
                else if (millivolt > AXP202_DC3_VOL_MAX)
                {
                    log_e("Mistake ! DCDC3 maximum output voltage is  %umV", AXP202_DC3_VOL_MAX);
                    return false;
                }
                return 0 == writeRegister(AXP202_DC3OUT_VOL, (millivolt - AXP202_DC3_VOL_MIN) / AXP202_DC3_VOL_STEPS);
            }

            uint16_t getDC3Voltage(void)
            {
                int val = readRegister(AXP202_DC3OUT_VOL);
                if (val == -1)
                    return 0;
                return (val * AXP202_DC3_VOL_STEPS) + AXP202_DC3_VOL_MIN;
            }

            /*
             * Power control EXTEN functions
             */
            bool enableExternalPin(void)
            {
                return setRegisterBit(AXP202_LDO234_DC23_CTL, 6);
            }

            bool disableExternalPin(void)
            {
                return clrRegisterBit(AXP202_LDO234_DC23_CTL, 6);
            }

            bool isEnableExternalPin(void)
            {
                return getRegisterBit(AXP202_LDO234_DC23_CTL, 6);
            }

            /*
             * Interrupt status functions
             */

            /**
             * @brief  Get the interrupt controller mask value.
             * @retval   Mask value corresponds to axp202_irq ,
             */
            uint64_t getIrqStatus(void)
            {
                statusRegister[0] = readRegister(AXP202_INTSTS1);
                statusRegister[1] = readRegister(AXP202_INTSTS2);
                statusRegister[2] = readRegister(AXP202_INTSTS3);
                statusRegister[3] = readRegister(AXP202_INTSTS4);
                statusRegister[4] = readRegister(AXP202_INTSTS5);
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
                    writeRegister(AXP202_INTSTS1 + i, 0xFF);
                    statusRegister[i] = 0;
                }
                writeRegister(AXP202_INTSTS5, 0xFF);
            }

            /**
             * @brief  Enable PMU interrupt control mask .
             * @param  opt: View the related chip type axp202_irq_t enumeration
             *              parameters in "XPowersParams.hpp"
             * @retval
             */
            bool enableIRQ(pmic_irq opt)
            {
                return setInterruptImpl(opt, true);
            }

            /**
             * @brief  Disable PMU interrupt control mask .
             * @param  opt: View the related chip type axp202_irq_t enumeration
             *              parameters in "XPowersParams.hpp"
             * @retval
             */
            bool disableIRQ(pmic_irq opt)
            {
                return setInterruptImpl(opt, false);
            }

            bool isAcinOverVoltageIrq(void)
            {
                uint8_t mask = AXP202_ACIN_OVER_VOL_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isAcinInserIrq(void)
            {
                uint8_t mask = AXP202_ACIN_CONNECT_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isAcinRemoveIrq(void)
            {
                uint8_t mask = AXP202_ACIN_REMOVED_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isVbusOverVoltageIrq(void)
            {
                uint8_t mask = AXP202_VBUS_OVER_VOL_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isVbusInsertIrq(void)
            {
                uint8_t mask = AXP202_VBUS_INSERT_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isVbusRemoveIrq(void)
            {
                uint8_t mask = AXP202_VBUS_REMOVE_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isVbusLowVholdIrq(void)
            {
                uint8_t mask = AXP202_VBUS_VHOLD_LOW_IRQ;
                if (intRegister[0] & mask)
                {
                    return IS_BIT_SET(statusRegister[0], mask);
                }
                return false;
            }

            bool isBatInsertIrq(void)
            {
                uint8_t mask = AXP202_BAT_INSERT_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBatRemoveIrq(void)
            {
                uint8_t mask = AXP202_BAT_REMOVE_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBattEnterActivateIrq(void)
            {
                uint8_t mask = AXP202_BATT_ACTIVATE_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBattExitActivateIrq(void)
            {
                uint8_t mask = AXP202_BATT_EXIT_ACTIVATE_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBatChargeStartIrq(void)
            {
                uint8_t mask = AXP202_BAT_CHG_START_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBatChargeDoneIrq(void)
            {
                uint8_t mask = AXP202_BAT_CHG_DONE_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBattTempHighIrq(void)
            {
                uint8_t mask = AXP202_BATT_OVER_TEMP_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isBattTempLowIrq(void)
            {
                uint8_t mask = AXP202_BATT_LOW_TEMP_IRQ >> 8;
                if (intRegister[1] & mask)
                {
                    return IS_BIT_SET(statusRegister[1], mask);
                }
                return false;
            }

            bool isChipOverTemperatureIrq(void)
            {
                uint8_t mask = AXP202_CHIP_TEMP_HIGH_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isChargingCurrentLessIrq(void)
            {
                uint8_t mask = AXP202_CHARGE_LOW_CUR_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isDC2VoltageLessIrq(void)
            {
                uint8_t mask = AXP202_DC2_LOW_VOL_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isDC3VoltageLessIrq(void)
            {
                uint8_t mask = AXP202_DC3_LOW_VOL_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isLDO3VoltageLessIr1(void)
            {
                uint8_t mask = AXP202_LDO3_LOW_VOL_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isPekeyShortPressIrq(void)
            {
                uint8_t mask = AXP202_PKEY_SHORT_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isPekeyLongPressIrq(void)
            {
                uint8_t mask = AXP202_PKEY_LONG_IRQ >> 16;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isNOEPowerOnIrq(void)
            {
                uint8_t mask = AXP202_NOE_ON_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isNOEPowerDownIrq(void)
            {
                uint8_t mask = AXP202_NOE_OFF_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isVbusEffectiveIrq(void)
            {
                uint8_t mask = AXP202_VBUS_VAILD_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isVbusInvalidIrq(void)
            {
                uint8_t mask = AXP202_VBUS_INVALID_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isVbusSessionIrq(void)
            {
                uint8_t mask = AXP202_VBUS_SESSION_AB_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isVbusSessionEndIrq(void)
            {
                uint8_t mask = AXP202_VBUS_SESSION_END_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isLowVoltageLevel1Irq(void)
            {
                uint8_t mask = APX202_APS_LOW_VOL_LEVEL1_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            bool isLowVoltageLevel2Irq(void)
            {
                uint8_t mask = AXP202_APS_LOW_VOL_LEVEL2_IRQ >> 24;
                if (intRegister[3] & mask)
                {
                    return IS_BIT_SET(statusRegister[3], mask);
                }
                return false;
            }

            // IRQ5 REGISTER :
            bool isWdtExpireIrq(void)
            {
                uint8_t mask = AXP202_GPIO3_EDGE_TRIGGER_IRQ >> 32;
                if (intRegister[4] & mask)
                {
                    return IS_BIT_SET(statusRegister[4], mask);
                }
                return false;
            }

            bool isGpio2EdgeTriggerIrq(void)
            {
                uint8_t mask = AXP202_GPIO2_EDGE_TRIGGER_IRQ >> 32;
                if (intRegister[4] & mask)
                {
                    return IS_BIT_SET(statusRegister[4], mask);
                }
                return false;
            }

            bool isGpio1EdgeTriggerIrq(void)
            {
                uint8_t mask = AXP202_GPIO1_EDGE_TRIGGER_IRQ >> 32;
                if (intRegister[2] & mask)
                {
                    return IS_BIT_SET(statusRegister[2], mask);
                }
                return false;
            }

            bool isGpio0EdgeTriggerIrq(void)
            {
                uint8_t mask = AXP202_GPIO0_EDGE_TRIGGER_IRQ >> 32;
                if (intRegister[4] & mask)
                {
                    return IS_BIT_SET(statusRegister[4], mask);
                }
                return false;
            }

            /*
             *   ADC Functions
             */

            bool enableBattDetection()
            {
                return setRegisterBit(AXP202_OFF_CTL, 6);
            }

            bool disableBattDetection()
            {
                return clrRegisterBit(AXP202_OFF_CTL, 6);
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
                return readRegisterH8L4(AXP202_VBUS_VOL_H8,
                                        AXP202_VBUS_VOL_L4) *
                       AXP202_VBUS_VOLTAGE_STEP;
            }

            float getVbusCurrent()
            {
                if (!isVbusIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP202_VBUS_CUR_H8,
                                        AXP202_VBUS_CUR_L4) *
                       AXP202_VBUS_CUR_STEP;
            }

            uint16_t getBattVoltage()
            {
                if (!isBatteryConnect())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP202_BAT_AVERVOL_H8,
                                        AXP202_BAT_AVERVOL_L4) *
                       AXP202_BATT_VOLTAGE_STEP;
            }

            float getBattDischargeCurrent()
            {
                if (!isBatteryConnect())
                {
                    return 0;
                }
                return readRegisterH8L5(AXP202_BAT_AVERDISCHGCUR_H8,
                                        AXP202_BAT_AVERDISCHGCUR_L5) *
                       AXP202_BATT_DISCHARGE_CUR_STEP;
            }

            uint16_t getAcinVoltage()
            {
                if (!isAcinIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP202_ACIN_VOL_H8, AXP202_ACIN_VOL_L4) * AXP202_ACIN_VOLTAGE_STEP;
            }

            float getAcinCurrent()
            {
                if (!isAcinIn())
                {
                    return 0;
                }
                return readRegisterH8L4(AXP202_ACIN_CUR_H8, AXP202_ACIN_CUR_L4) * AXP202_ACIN_CUR_STEP;
            }

            uint16_t getSystemVoltage()
            {
                return readRegisterH8L4(AXP202_APS_AVERVOL_H8, AXP202_APS_AVERVOL_L4) * AXP202_APS_VOLTAGE_STEP;
            }

            /*
             * Timer Control
             */
            void setTimerout(uint8_t minute)
            {
                writeRegister(AXP202_TIMER_CTL, 0x80 | minute);
            }

            void disableTimer()
            {
                writeRegister(AXP202_TIMER_CTL, 0x80);
            }

            void clearTimerFlag()
            {
                setRegisterBit(AXP202_TIMER_CTL, 7);
            }

            /*
             * Data Buffer
             */
            bool writeDataBuffer(uint8_t *data, uint8_t size)
            {
                if (size > AXP202_DATA_BUFFER_SIZE)
                    return false;
                for (int i = 0; i < size; ++i)
                {
                    writeRegister(AXP202_DATA_BUFFER1 + i, data[i]);
                }
                return true;
            }

            bool readDataBuffer(uint8_t *data, uint8_t size)
            {
                if (size > AXP202_DATA_BUFFER_SIZE)
                    return false;
                for (int i = 0; i < size; ++i)
                {
                    data[i] = readRegister(AXP202_DATA_BUFFER1 + i);
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
                    val = readRegister(AXP202_OFF_CTL);
                    if (val == -1)
                        return;
                    val &= 0xC7;
                    val |= 0x08; // use manual ctrl
                    val |= (mode << 4);
                    writeRegister(AXP202_OFF_CTL, val);
                    break;
                case CHG_LED_CTRL_CHG:
                    clrRegisterBit(AXP202_OFF_CTL, 3);
                    break;
                default:
                    break;
                }
            }

            pmic_chg_led_mode getChargingLedMode()
            {
                if (!getRegisterBit(AXP202_OFF_CTL, 3))
                {
                    return CHG_LED_CTRL_CHG;
                }
                int val = readRegister(AXP202_OFF_CTL);
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
                setRegisterBit(AXP202_COULOMB_CTL, 7);
            }

            void disableCoulomb()
            {
                clrRegisterBit(AXP202_COULOMB_CTL, 7);
            }

            void stopCoulomb()
            {
                setRegisterBit(AXP202_COULOMB_CTL, 6);
            }

            void clearCoulomb()
            {
                setRegisterBit(AXP202_COULOMB_CTL, 5);
            }

            uint32_t getBattChargeCoulomb()
            {
                int data[4];
                data[0] = readRegister(AXP202_BAT_CHGCOULOMB3);
                data[1] = readRegister(AXP202_BAT_CHGCOULOMB2);
                data[2] = readRegister(AXP202_BAT_CHGCOULOMB1);
                data[3] = readRegister(AXP202_BAT_CHGCOULOMB0);
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
                data[0] = readRegister(AXP202_BAT_DISCHGCOULOMB3);
                data[1] = readRegister(AXP202_BAT_DISCHGCOULOMB2);
                data[2] = readRegister(AXP202_BAT_DISCHGCOULOMB1);
                data[3] = readRegister(AXP202_BAT_DISCHGCOULOMB0);
                for (int i = 0; i < 4; ++i)
                {
                    if (data[i] == -1)
                        return 0;
                }
                return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (uint32_t)data[3];
            }

            uint8_t getAdcSamplingRate(void)
            {
                int val = readRegister(AXP202_ADC_SPEED);
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
                           AXP202_BAT_AVERCHGCUR_H8,
                           AXP202_BAT_AVERCHGCUR_L5) *
                       AXP202_BATT_CHARGE_CUR_STEP;
            }

            uint16_t getGpio0Voltage()
            {
                return readRegisterH8L4(AXP202_GPIO0_VOL_ADC_H8, AXP202_GPIO0_VOL_ADC_L4) * AXP202_GPIO0_STEP * 1000;
            }

            uint16_t getGpio1Voltage()
            {
                return readRegisterH8L4(AXP202_GPIO1_VOL_ADC_H8, AXP202_GPIO1_VOL_ADC_L4) * AXP202_GPIO1_STEP * 1000;
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
                return readRegister(AXP202_IC_TYPE);
            }

            /**
             * Sleep function
             */
            bool enableSleep()
            {
                return setRegisterBit(AXP202_VOFF_SET, 3);
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
                int val = readRegister(AXP202_POK_SET);
                if (val == -1)
                    return false;
                return 0 == writeRegister(AXP202_POK_SET, (val & 0x3F) | (opt << 6));
            }

            /**
             * @brief Get the PEKEY power-on long press time.
             * @retval See pmic_press_on_time enum for details.
             */
            pmic_press_on_time getPowerKeyPressOnTime()
            {
                int val = readRegister(AXP202_POK_SET);
                if (val == -1)
                    return pmic_press_on_time(0);
                return pmic_press_on_time((val & 0xC0) >> 6);
            }

            /**
             * @brief Set the PEKEY power-off long press time.
             * @param opt: See pmic_press_off_time enum for details.
             * @retval
             */
            bool setPowerKeyPressOffTime(pmic_press_off_time opt)
            {
                int val = readRegister(AXP202_POK_SET);
                if (val == -1)
                    return false;
                return 0 == writeRegister(AXP202_POK_SET, (val & 0xFC) | opt);
            }

            /**
             * @brief Get the PEKEY power-off long press time.
             * @retval See pmic_press_off_time enum for details.
             */
            pmic_press_off_time getPowerKeyPressOffTime()
            {
                int val = readRegister(AXP202_POK_SET);
                if (val == -1)
                    return pmic_press_off_time(0);
                return static_cast<pmic_press_off_time>(val & 0x03);
            }

            void setPowerKeyLongPressOnTime(pmic_pekey_long_press opt)
            {
                int val = readRegister(AXP202_POK_SET);
                if (val == -1)
                    return;
                writeRegister(AXP202_POK_SET, (val & 0xCF) | (opt << 4));
            }

            void enablePowerKeyLongPressPowerOff()
            {
                setRegisterBit(AXP202_POK_SET, 3);
            }

            void disablePowerKeyLongPressPowerOff()
            {
                clrRegisterBit(AXP202_POK_SET, 3);
            }

        protected:
            uint16_t getPowerChannelVoltage(PMICPowerChannel channel)
            {
                switch (channel)
                {
                case DCDC2:
                    return getDC2Voltage();
                case DCDC3:
                    return getDC3Voltage();
                case LDO2:
                    return getLDO2Voltage();
                case LDO3:
                    return getLDO3Voltage();
                case LDO4:
                    return getLDO4Voltage();
                case LDOIO:
                    return getLDOioVoltage();
                default:
                    break;
                }
                return 0;
            }

            bool inline enablePowerOutput(PMICPowerChannel channel)
            {
                switch (channel)
                {
                case DCDC2:
                    return enableDC2();
                case DCDC3:
                    return enableDC3();
                case LDO2:
                    return enableLDO2();
                case LDO3:
                    return enableLDO3();
                case LDO4:
                    return enableLDO4();
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
                if (getProtectedChannel(channel))
                {
                    log_e("Failed to disable the power channel, the power channel has been protected");
                    return false;
                }
                switch (channel)
                {
                case DCDC2:
                    return disableDC2();
                case DCDC3:
                    return disableDC3();
                case LDO2:
                    return disableLDO2();
                case LDO3:
                    return disableLDO3();
                case LDO4:
                    return disableLDO4();
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
                case DCDC2:
                    return isEnableDC2();
                case DCDC3:
                    return isEnableDC3();
                case LDO2:
                    return isEnableLDO2();
                case LDO3:
                    return isEnableLDO3();
                case LDO4:
                    return isEnableLDO4();
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
                case DCDC2:
                    return setDC2Voltage(millivolt);
                case DCDC3:
                    return setDC3Voltage(millivolt);
                case LDO2:
                    return setLDO2Voltage(millivolt);
                case LDO3:
                    return setLDO3Voltage(millivolt);
                case LDO4:
                    return setLDO4Voltage(millivolt);
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

            bool initImpl()
            {
                if (getChipID() == AXP202_CHIP_ID)
                {
                    setChipModel(PMICChipModel::AXP202);
                    return true;
                }
                return false;
            }

            /*
             * Interrupt control functions
             */
            bool setInterruptImpl(pmic_irq opts, bool enable)
            {
                int res = 0;
                uint8_t data = 0, value = 0;

                log_d("%s %s - 0x%llx\n", __func__, enable ? "ENABLE" : "DISABLE", opts);

                if (opts & 0x00000000FF)
                {
                    value = opts & 0xFF;
                    // log_d("Write INT0: %x\n", value);
                    data = readRegister(AXP202_INTEN1);
                    intRegister[0] = enable ? (data | value) : (data & (~value));
                    res |= writeRegister(AXP202_INTEN1, intRegister[0]);
                }

                if (opts & 0x000000FF00)
                {
                    value = opts >> 8;
                    // log_d("Write INT1: %x\n", value);
                    data = readRegister(AXP202_INTEN2);
                    intRegister[1] = enable ? (data | value) : (data & (~value));
                    res |= writeRegister(AXP202_INTEN2, intRegister[1]);
                }

                if (opts & 0x0000FF0000)
                {
                    value = opts >> 16;
                    // log_d("Write INT1: %x\n", value);
                    data = readRegister(AXP202_INTEN3);
                    intRegister[2] = enable ? (data | value) : (data & (~value));
                    res |= writeRegister(AXP202_INTEN3, intRegister[2]);
                }

                if (opts & 0x00FF000000)
                {
                    value = opts >> 24;
                    // log_d("Write INT1: %x\n", value);
                    data = readRegister(AXP202_INTEN4);
                    intRegister[3] = enable ? (data | value) : (data & (~value));
                    res |= writeRegister(AXP202_INTEN4, intRegister[3]);
                }

                if (opts & 0xFF00000000)
                {
                    value = opts >> 32;
                    // log_d("Write INT1: %x\n", value);
                    data = readRegister(AXP202_INTEN4);
                    intRegister[4] = enable ? (data | value) : (data & (~value));
                    res |= writeRegister(AXP202_INTEN4, intRegister[4]);
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
                    value = readRegister(AXP202_ADC_EN1);
                    writeRegister(AXP202_ADC_EN1, enable ? (value | opts) : (value & (~opts)));
                }
                if (opts & 0xFF00)
                {
                    opts >>= 8;
                    value = readRegister(AXP202_ADC_EN2);
                    writeRegister(AXP202_ADC_EN2, enable ? (value | opts) : (value & (~opts)));
                }
                return true;
            }

            const char *getChipNameImpl(void)
            {
                return "AXP202";
            }

        private:
            const uint16_t chargeTargetVol[4] = {4100, 4150, 4200, 4360};
            uint8_t statusRegister[AXP202_INTSTS_CNT];
            uint8_t intRegister[AXP202_INTSTS_CNT];
            const uint16_t ldo4_table[16] = {
                1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
                2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300};
        };
    }
}