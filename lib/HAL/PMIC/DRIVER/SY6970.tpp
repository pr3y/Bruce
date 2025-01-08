#include "../TYPE2.tpp"
#include "../REG/SY6970.hpp"
namespace HAL
{
    namespace PMIC
    {
        typedef SY6970 THIS;
        class SY6970 : public Type2<class SY6970>
        {
            friend class Base<Type2<SY6970>, SY6970>;

        public:
            SY6970(TwoWire &wire = Wire, int sda = SDA, int scl = SCL, uint8_t addr = SY6970_SLAVE_ADDRESS)
            {
                myWire = &wire;
                mySDA = sda;
                mySCL = scl;
                myADDR = addr;
            }

            SY6970(uint8_t addr, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback)
            {
                thisReadRegCallback = readRegCallback;
                thisWriteRegCallback = writeRegCallback;
                myADDR = addr;
            }

            SY6970()
            {
                myWire = &Wire;
                mySDA = SDA;
                mySCL = SCL;
                myADDR = SY6970_SLAVE_ADDRESS;
            }

            ~SY6970()
            {
                log_i("~PowersSY6970");
                deinit();
            }

            bool init(TwoWire &wire = Wire, int sda = SDA, int scl = SCL, uint8_t addr = SY6970_SLAVE_ADDRESS)
            {
                myWire = &wire;
                mySDA = sda;
                mySCL = scl;
                myADDR = addr;
                __irq_mask = 0;
                return begin();
            }

            const char *getChipName()
            {
                return getChipID() == SY6970_DEV_REV ? "SY6970" : "Unknown";
            }

            /// REG0B
            bool isVbusIn()
            {
                return getBusStatus() != BUS_STATE_NOINPUT;
            }

            bool isBatteryConnect(void) __attribute__((error("Not implemented")))
            {
                // TODO:
                return false;
            }

            bool isPowerGood()
            {
                return getRegisterBit(POWERS_PPM_REG_0BH, 2);
            }

            bool setSysPowerDownVoltage(uint16_t millivolt)
            {
                if (millivolt % POWERS_SY6970_SYS_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", POWERS_SY6970_SYS_VOL_STEPS);
                    return false;
                }
                if (millivolt < POWERS_SY6970_SYS_VOFF_VOL_MIN)
                {
                    log_e("Mistake ! SYS minimum output voltage is  %umV", POWERS_SY6970_SYS_VOFF_VOL_MIN);
                    return false;
                }
                else if (millivolt > POWERS_SY6970_SYS_VOFF_VOL_MAX)
                {
                    log_e("Mistake ! SYS maximum output voltage is  %umV", POWERS_SY6970_SYS_VOFF_VOL_MAX);
                    return false;
                }
                int val = readRegister(POWERS_PPM_REG_03H);
                if (val == -1)
                    return false;
                val &= 0xF1;
                val |= (millivolt - POWERS_SY6970_SYS_VOFF_VOL_MIN) / POWERS_SY6970_SYS_VOL_STEPS;
                val <<= 1;
                return 0 == writeRegister(POWERS_PPM_REG_03H, val);
            }

            uint16_t getSysPowerDownVoltage()
            {
                int val = readRegister(POWERS_PPM_REG_03H);
                if (val == -1)
                    return 0;
                val &= 0x0E;
                val >>= 1;
                return (val * POWERS_SY6970_SYS_VOL_STEPS) + POWERS_SY6970_SYS_VOFF_VOL_MIN;
            }

            const char *getBusStatusString()
            {
                BusStatus status = getBusStatus();
                switch (status)
                {
                case BUS_STATE_NOINPUT:
                    return "No input";
                case BUS_STATE_USB_SDP:
                    return "USB Host SDP";
                case BUS_STATE_USB_CDP:
                    return "USB CDP";
                case BUS_STATE_USB_DCP:
                    return "USB DCP";
                case BUS_STATE_HVDCP:
                    return "HVDCP";
                case BUS_STATE_ADAPTER:
                case BUS_STATE_NO_STANDARD_ADAPTER:
                    return "Adapter";
                case BUS_STATE_OTG:
                    return "OTG";
                default:
                    return "Unknown";
                }
            }

            const char *getChargeStatusString()
            {
                ChargeStatus status = chargeStatus();
                switch (status)
                {
                case CHARGE_STATE_NO_CHARGE:
                    return "Not Charging";
                case CHARGE_STATE_PRE_CHARGE:
                    return "Pre-charge";
                case CHARGE_STATE_FAST_CHARGE:
                    return "Fast Charging";
                case CHARGE_STATE_DONE:
                    return "Charge Termination Done";
                default:
                    return "Unknown";
                }
            }

            uint8_t getNTCStatus()
            {
                return (__irq_mask & 0x07);
            }

            const char *getNTCStatusString()
            {
                uint8_t status = getNTCStatus();

                if (isOTG())
                {
                    // Boost mode
                    switch (status)
                    {
                    case BOOST_NTC_NORMAL:
                        return "Boost mode NTC normal";
                    case BOOST_NTC_COLD:
                        return "Boost mode NTC cold";
                    case BOOST_NTC_HOT:
                        return "Boost mode NTC hot";
                    default:
                        break;
                    }
                }
                else
                {
                    // Buck mode
                    switch (status)
                    {
                    case BUCK_NTC_NORMAL:
                        return "Buck mode NTC normal";
                    case BUCK_NTC_WARM:
                        return "Buck mode NTC warm";
                    case BUCK_NTC_COOL:
                    case BUCK_NTC_COLD:
                        return "Buck mode NTC cold";
                    case BUCK_NTC_HOT:
                        return "Buck mode NTC hot";
                    default:
                        break;
                    }
                }
                return "Unknown";
            }

            bool enableADCMeasure() __attribute__((deprecated("The enableADCMeasure method will be replaced by enableMeasure in the future. Please update it to enableMeasure.")))
            {
                enableMeasure();
            }

            void enableInputCurrentLimit()
            {
                setRegisterBit(POWERS_PPM_REG_02H, 4);
            }

            void disableInputCurrentLimit()
            {
                clrRegisterBit(POWERS_PPM_REG_02H, 4);
            }

            void enableHVDCP()
            {
                setRegisterBit(POWERS_PPM_REG_02H, 3);
            }

            void disableHVDCP()
            {
                clrRegisterBit(POWERS_PPM_REG_02H, 3);
            }

            bool isEnableHVDCP()
            {
                return getRegisterBit(POWERS_PPM_REG_02H, 3);
            }

            void setHighVoltageRequestedRange(RequestRange range)
            {
                switch (range)
                {
                case REQUEST_9V:
                    clrRegisterBit(POWERS_PPM_REG_02H, 2);
                    break;
                case REQUEST_12V:
                    setRegisterBit(POWERS_PPM_REG_02H, 2);
                    break;
                default:
                    break;
                }
            }

            RequestRange getHighVoltageRequestedRange()
            {
                return getRegisterBit(POWERS_PPM_REG_02H, 2) ? REQUEST_12V : REQUEST_9V;
            }

            bool setInputCurrentLimit(uint16_t milliampere)
            {
                if (milliampere % POWERS_SY6970_IN_CURRENT_STEP)
                {
                    log_e("Mistake ! The steps is must %u mA", POWERS_SY6970_IN_CURRENT_STEP);
                    return false;
                }
                if (milliampere < POWERS_SY6970_IN_CURRENT_MIN)
                {
                    milliampere = POWERS_SY6970_IN_CURRENT_MIN;
                }
                if (milliampere > POWERS_SY6970_IN_CURRENT_MAX)
                {
                    milliampere = POWERS_SY6970_IN_CURRENT_MAX;
                }
                int val = readRegister(POWERS_PPM_REG_00H);
                if (val == -1)
                    return false;
                val &= 0xC0;
                milliampere = ((milliampere - POWERS_SY6970_IN_CURRENT_MIN) / POWERS_SY6970_IN_CURRENT_STEP);
                val |= milliampere;
                return writeRegister(POWERS_PPM_REG_00H, val) != -1;
            }

            uint32_t getInputCurrentLimit()
            {
                int val = readRegister(POWERS_PPM_REG_00H);
                if (val == -1)
                    return false;
                val &= 0x3F;
                return (val * POWERS_SY6970_IN_CURRENT_STEP) + POWERS_SY6970_IN_CURRENT_MIN;
            }

            uint16_t getVbusVoltage()
            {
                if (!isVbusIn())
                {
                    return 0;
                }
                int val = readRegister(POWERS_PPM_REG_11H);
                return (POWERS_SY6970_VBUS_MASK_VAL(val) * POWERS_SY6970_VBUS_VOL_STEP) + POWERS_SY6970_VBUS_BASE_VAL;
            }

            uint16_t getBattVoltage()
            {
                int val = readRegister(POWERS_PPM_REG_0EH);
                val = POWERS_SY6970_VBAT_MASK_VAL(val);
                if (val == 0)
                    return 0;
                return (val * POWERS_SY6970_VBAT_VOL_STEP) + POWERS_SY6970_VBAT_BASE_VAL;
            }

            uint16_t getSystemVoltage()
            {
                int val = readRegister(POWERS_PPM_REG_0FH);
                return (POWERS_SY6970_VSYS_MASK_VAL(val) * POWERS_SY6970_VSYS_VOL_STEP) + POWERS_SY6970_VSYS_BASE_VAL;
            }

            float getNTCPercentage()
            {
                int val = readRegister(POWERS_PPM_REG_10H);
                return (POWERS_SY6970_NTC_MASK_VAL(val) * POWERS_SY6970_NTC_VOL_STEP) + POWERS_SY6970_NTC_BASE_VAL;
            }

            uint16_t getChargeCurrent()
            {
                ChargeStatus status = chargeStatus();
                if (status == CHARGE_STATE_NO_CHARGE)
                {
                    return 0;
                }
                //* If the charger is disconnected, the value in the register
                //* will remain the last value and will not be updated to 0.
                int val = readRegister(POWERS_PPM_REG_12H);
                if (val == 0)
                    return 0;
                val = (val & 0x7F);
                return (val * POWERS_SY6970_CHG_STEP_VAL);
            }

            // Range: 64mA ~ 1024 mA ,step:64mA
            bool setPrechargeCurr(uint16_t milliampere)
            {
                if (milliampere % POWERS_SY6970_PRE_CHG_CUR_STEP)
                {
                    log_e("Mistake ! The steps is must %u mA", POWERS_SY6970_PRE_CHG_CUR_STEP);
                    return false;
                }
                if (milliampere < POWERS_SY6970_PRE_CHG_CURRENT_MIN)
                {
                    milliampere = POWERS_SY6970_PRE_CHG_CURRENT_MIN;
                }
                if (milliampere > POWERS_SY6970_PRE_CHG_CURRENT_MAX)
                {
                    milliampere = POWERS_SY6970_PRE_CHG_CURRENT_MAX;
                }
                int val = readRegister(POWERS_PPM_REG_05H);
                val &= 0x0F;
                milliampere = ((milliampere - POWERS_SY6970_PRE_CHG_CUR_BASE) / POWERS_SY6970_PRE_CHG_CUR_STEP);
                val |= milliampere << 4;
                return writeRegister(POWERS_PPM_REG_05H, val) != -1;
            }

            uint16_t getPrechargeCurr(void)
            {
                int val = readRegister(POWERS_PPM_REG_05H);
                val &= 0xF0;
                val >>= 4;
                return POWERS_SY6970_PRE_CHG_CUR_STEP + (val * POWERS_SY6970_PRE_CHG_CUR_STEP);
            }

            uint16_t getChargerConstantCurr()
            {
                int val = readRegister(POWERS_PPM_REG_04H);
                val &= 0x7F;
                return val * POWERS_SY6970_FAST_CHG_CUR_STEP;
            }

            /**
             * @brief  setChargerConstantCurr
             * @note
             * @param  milliampere: SY6970 Range:0~5056 mA / step:64mA
             * @retval true : success false : failed
             */
            bool setChargerConstantCurr(uint16_t milliampere)
            {
                if (milliampere % POWERS_SY6970_FAST_CHG_CUR_STEP)
                {
                    log_e("Mistake ! The steps is must %u mA", POWERS_SY6970_FAST_CHG_CUR_STEP);
                    return false;
                }
                if (milliampere > POWERS_SY6970_FAST_CHG_CURRENT_MAX)
                {
                    milliampere = POWERS_SY6970_FAST_CHG_CURRENT_MAX;
                }
                int val = readRegister(POWERS_PPM_REG_04H);
                val &= 0x80;
                val |= (milliampere / POWERS_SY6970_FAST_CHG_CUR_STEP);
                return writeRegister(POWERS_PPM_REG_04H, val) != -1;
            }

            uint16_t getChargeTargetVoltage()
            {
                int val = readRegister(POWERS_PPM_REG_06H);
                val = (val & 0xFC) >> 2;
                if (val > 0x30)
                {
                    return POWERS_SY6970_FAST_CHG_VOL_MAX;
                }
                return val * POWERS_SY6970_CHG_VOL_STEP + POWERS_SY6970_CHG_VOL_BASE;
            }

            // Range:3840 ~ 4608mV ,step:16 mV
            bool setChargeTargetVoltage(uint16_t millivolt)
            {
                if (millivolt % POWERS_SY6970_CHG_VOL_STEP)
                {
                    log_e("Mistake ! The steps is must %u mV", POWERS_SY6970_CHG_VOL_STEP);
                    return false;
                }
                if (millivolt < POWERS_SY6970_FAST_CHG_VOL_MIN)
                {
                    millivolt = POWERS_SY6970_FAST_CHG_VOL_MIN;
                }
                if (millivolt > POWERS_SY6970_FAST_CHG_VOL_MAX)
                {
                    millivolt = POWERS_SY6970_FAST_CHG_VOL_MAX;
                }
                int val = readRegister(POWERS_PPM_REG_06H);
                val &= 0x03;
                val |= (((millivolt - POWERS_SY6970_CHG_VOL_BASE) / POWERS_SY6970_CHG_VOL_STEP) << 2);
                return writeRegister(POWERS_PPM_REG_06H, val) != -1;
            }

            // Turn off the battery power supply path. It can only be turned off when the
            // battery is powered. It cannot be turned off when USB is connected.
            // The device can only be powered on by pressing the PWR button or by connecting the power supply.

            // Boost Mode Voltage Regulation: 4550 mV ~ 5510 mV
            bool setBoostVoltage(uint16_t millivolt)
            {
                if (millivolt % POWERS_SY6970_BOOTS_VOL_STEP)
                {
                    log_e("Mistake ! The steps is must %u mV", POWERS_SY6970_BOOTS_VOL_STEP);
                    return false;
                }
                if (millivolt < POWERS_SY6970_BOOST_VOL_MIN)
                {
                    millivolt = POWERS_SY6970_BOOST_VOL_MIN;
                }
                if (millivolt > POWERS_SY6970_BOOST_VOL_MAX)
                {
                    millivolt = POWERS_SY6970_BOOST_VOL_MAX;
                }
                int val = readRegister(POWERS_PPM_REG_0AH);
                val &= 0xF0;
                val |= (((millivolt - POWERS_SY6970_BOOTS_VOL_BASE) / POWERS_SY6970_BOOTS_VOL_STEP) << 4);
                return writeRegister(POWERS_PPM_REG_0AH, val) != -1;
            }

            // Boost Current Limit: 500mA ~ 2450mA
            bool setBoostCurrentLimit(BoostCurrentLimit milliampere)
            {
                if (milliampere > BOOST_CUR_LIMIT_2450MA)
                {
                    return false;
                }
                int val = readRegister(POWERS_PPM_REG_0AH);
                val &= 0x03;
                val |= milliampere;
                return writeRegister(POWERS_PPM_REG_0AH, val) != -1;
            }

            uint8_t getFaultStatus(void)
            {
                int val = readRegister(POWERS_PPM_REG_0CH);
                if (val == -1)
                {
                    return 0;
                }
                __irq_mask = val;
                return __irq_mask;
            }

            void getReadOnlyRegisterValue()
            {
                static uint8_t last_val[8] = {0};
                const uint8_t regis[] = {
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
            }

            bool isWatchdogFault()
            {
                return POWERS_SY6970_IRQ_WTD_FAULT(__irq_mask);
            }

            bool isBoostFault()
            {
                return POWERS_SY6970_IRQ_BOOST_FAULT(__irq_mask);
            }

            bool isChargeFault()
            {
                return POWERS_SY6970_IRQ_CHG_FAULT(__irq_mask);
            }

            bool isBatteryFault()
            {
                return POWERS_SY6970_IRQ_BAT_FAULT(__irq_mask);
            }

            bool isNTCFault()
            {
                return POWERS_SY6970_IRQ_NTC_FAULT(__irq_mask);
            }

            bool setVinDpmThreshold(uint16_t millivolt)
            {
                if (millivolt % POWERS_SY6970_VINDPM_VOL_STEPS)
                {
                    log_e("Mistake ! The steps is must %u mV", POWERS_SY6970_VINDPM_VOL_STEPS);
                    return false;
                }
                if (millivolt < POWERS_SY6970_VINDPM_VOL_MIN)
                {
                    millivolt = POWERS_SY6970_VINDPM_VOL_MIN;
                }
                if (millivolt > POWERS_SY6970_VINDPM_VOL_MAX)
                {
                    millivolt = POWERS_SY6970_VINDPM_VOL_MAX;
                }
                int val = readRegister(POWERS_PPM_REG_0DH);
                val &= 0x80;
                val |= (((millivolt - POWERS_SY6970_VINDPM_VOL_BASE) / POWERS_SY6970_VINDPM_VOL_STEPS));
                return writeRegister(POWERS_PPM_REG_0DH, val) != -1;
            }

        private:
            bool initImpl()
            {
                __user_disable_charge = false;

                uint8_t rev = getChipID();
                if (rev != SY6970_DEV_REV)
                {
                    return false;
                }
                // Set the minimum operating voltage. Below this voltage, the PMU will protect
                // setSysPowerDownVoltage(3300);

                // Default disable Watchdog
                disableWatchdog();

                return true;
            }

            uint32_t __irq_mask;
        };
    }
}