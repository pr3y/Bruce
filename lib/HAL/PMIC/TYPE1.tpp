#ifndef __TYPE1__
#define __TYPE1__
#include "PMIC.tpp"
#include <Arduino.h>
#ifdef PMIC_AXP192
#include "REG/AXP192.hpp"
#elif defined(PMIC_AXP202)
#include "REG/AXP202.hpp"
#elif defined(PMIC_AXP2101)
#include "REG/AXP2101.hpp"
#endif
namespace HAL
{
    namespace PMIC
    {
        template <class Driver>
        class Type1 : public Base<Type1<Driver>, Driver>
        {
            friend class Base<Type1<Driver>, Driver>;

        public:
            Type1() : myModel(UNDEFINED), __protectedMask(0) {};
            virtual bool enableSleep() = 0;
            virtual uint16_t status() = 0;
            virtual bool isDischarge() = 0;
            virtual bool enablePowerOutput(PMICPowerChannel channel) = 0;
            virtual bool disablePowerOutput(PMICPowerChannel channel) = 0;
            virtual bool isPowerChannelEnable(PMICPowerChannel channel) = 0;
            virtual uint16_t getPowerChannelVoltage(PMICPowerChannel channel) = 0;
            virtual bool setPowerChannelVoltage(PMICPowerChannel channel, uint16_t millivolt) = 0;
            virtual bool setVbusCurrentLimit(pmic_vbus_cur_limit opt) = 0;
            virtual pmic_vbus_cur_limit getVbusCurrentLimit() = 0;
            virtual void setVbusVoltageLimit(pmic_vbus_vol_limit opt) = 0;
            virtual pmic_vbus_vol_limit getVbusVoltageLimit() = 0;
            virtual bool setChargeTargetVoltage(pmic_chg_vol opt) = 0;
            virtual bool setChargerConstantCurr(pmic_chg_curr opt) = 0;
            virtual void setChargerTerminationCurr(pmic_chg_iterm opt) = 0;
            virtual pmic_chg_iterm getChargerTerminationCurr(void) = 0;
            virtual uint64_t getIrqStatus() = 0;
            virtual void clearIrqStatus() = 0;
            virtual bool enableIRQ(uint64_t opt) = 0;
            virtual bool disableIRQ(uint64_t opt) = 0;
            virtual bool isPekeyShortPressIrq() = 0;
            virtual bool isPekeyLongPressIrq() = 0;
            virtual bool isBatInsertIrq() = 0;
            virtual bool isBatRemoveIrq() = 0;
            virtual bool isVbusInsertIrq() = 0;
            virtual bool isVbusRemoveIrq() = 0;
            virtual bool isBatChargeDoneIrq() = 0;
            virtual bool isBatChargeStartIrq() = 0;
            virtual bool enableBattDetection() = 0;
            virtual bool disableBattDetection() = 0;
            virtual bool enableVbusVoltageMeasure(void) = 0;
            virtual bool disableVbusVoltageMeasure(void) = 0;
            virtual bool enableSystemVoltageMeasure(void) = 0;
            virtual bool disableSystemVoltageMeasure(void) = 0;
            virtual pmic_chg_curr getChargerConstantCurr() = 0;
            virtual pmic_chg_vol getChargeTargetVoltage() = 0;
            virtual bool enableTemperatureMeasure(void) = 0;
            virtual bool disableTemperatureMeasure(void) = 0;
            virtual bool enableBattVoltageMeasure(void) = 0;
            virtual bool disableBattVoltageMeasure(void) = 0;
            virtual bool enableTSPinMeasure(void) = 0;
            virtual bool disableTSPinMeasure(void) = 0;
            virtual void setChargingLedMode(pmic_chg_led_mode mode) = 0;
            virtual pmic_chg_led_mode getChargingLedMode() = 0;
            virtual bool setPowerKeyPressOnTime(pmic_press_on_time opt) = 0;
            virtual pmic_press_on_time getPowerKeyPressOnTime() = 0;
            virtual bool setPowerKeyPressOffTime(pmic_press_off_time opt) = 0;
            virtual pmic_press_off_time getPowerKeyPressOffTime() = 0;
            bool inline setRegisterBit(uint8_t registers, uint8_t bit) { return this->setRegisterBit(registers, bit); };
            bool inline clrRegisterBit(uint8_t registers, uint8_t bit) { return this->clrRegisterBit(registers, bit); }
            bool inline getRegisterBit(uint8_t registers, uint8_t bit) { return this->getRegisterBit(registers, bit); }
            int readRegister(uint8_t reg) { return this->readRegister(reg); }
            int readRegister(uint8_t reg, uint8_t *buf, uint8_t length) { return this->readRegister(reg, *buf, length); }
            int writeRegister(uint8_t reg, uint8_t *buf, uint8_t length) { return this->writeRegister(reg, *buf, length); }
            int writeRegister(uint8_t reg, uint8_t val) { return this->writeRegister(reg, val); }
            bool begin(TwoWire &w, uint8_t addr, int sda, int scl) { return this->begin(&w, addr, sda, scl) }
            bool begin() { return this->begin(); }
            void end() { this->end(); }
            typedef struct gpio_t
            {
                uint8_t mode;
            };

            bool isChannelAvailable(PMICPowerChannel channel)
            {
#ifdef PMIC_AXP192
                switch (channel)
                {
                case DCDC1:
                case DCDC2:
                case DCDC3:
                case LDO2:
                case LDO3:
                case LDOIO:
                    return true;
                default:
                    return false;
                }
#elif defined(PMIC_AXP202)

                switch (channel)
                {
                case DCDC2:
                case DCDC3:
                case LDO2:
                case LDO3:
                case LDO4:
                case LDO5:
                    return true;
                default:
                    return false;
                }
#elif defined(PMIC_AXP2101)
                switch (channel)
                {
                case DCDC1:
                case DCDC2:
                case DCDC3:
                case DCDC4:
                case DCDC5:
                case ALDO1:
                case ALDO2:
                case ALDO3:
                case ALDO4:
                case BLDO1:
                case BLDO2:
                case VBACKUP:
                case CPULDO:
                    return true;
                default:
                    // DLDO is not available, will also return false
                    return false;
                }
#endif
                return false;
            }

            void setProtectedChannel(PMICPowerChannel channel)
            {
                __protectedMask |= _BV(channel);
            }

            void setUnprotectChannel(PMICPowerChannel channel)
            {
                __protectedMask &= (~_BV(channel));
            }

            bool getProtectedChannel(PMICPowerChannel channel)
            {
                return __protectedMask & _BV(channel);
            }

            static uint64_t inline check_params(uint32_t opt, uint32_t params, uint64_t mask)
            {
                return ((opt & params) == params) ? mask : 0;
            }

            bool enableInterrupt(uint32_t option)
            {
                return setInterruptMask(option, true);
            }

            bool disableInterrupt(uint32_t option)
            {
                return setInterruptMask(option, false);
            }

            bool setInterruptMask(uint32_t option, bool enable)
            {
                uint64_t params = 0;
#ifdef PMIC_AXP172
                return false;
#elif defined(PMIC_AXP192)
                params |= check_params(option, USB_INSERT_INT, AXP192_VBUS_INSERT_IRQ);
                params |= check_params(option, USB_REMOVE_INT, AXP192_VBUS_REMOVE_IRQ);
                params |= check_params(option, BATTERY_INSERT_INT, AXP192_BAT_INSERT_IRQ);
                params |= check_params(option, BATTERY_REMOVE_INT, AXP192_BAT_REMOVE_IRQ);
                params |= check_params(option, CHARGE_START_INT, AXP192_BAT_CHG_START_IRQ);
                params |= check_params(option, CHARGE_DONE_INT, AXP192_BAT_CHG_DONE_IRQ);
                params |= check_params(option, PWR_BTN_CLICK_INT, AXP192_PKEY_SHORT_IRQ);
                params |= check_params(option, PWR_BTN_LONGPRESSED_INT, AXP192_PKEY_LONG_IRQ);
                params |= check_params(option, ALL_INT, AXP192_ALL_IRQ);
                return enable ? enableIRQ(params) : disableIRQ(params);
#elif defined(PMIC_AXP202)
                params |= check_params(option, USB_INSERT_INT, AXP202_VBUS_INSERT_IRQ);
                params |= check_params(option, USB_REMOVE_INT, AXP202_VBUS_REMOVE_IRQ);
                params |= check_params(option, BATTERY_INSERT_INT, AXP202_BAT_INSERT_IRQ);
                params |= check_params(option, BATTERY_REMOVE_INT, AXP202_BAT_REMOVE_IRQ);
                params |= check_params(option, CHARGE_START_INT, AXP202_BAT_CHG_START_IRQ);
                params |= check_params(option, CHARGE_DONE_INT, AXP202_BAT_CHG_DONE_IRQ);
                params |= check_params(option, PWR_BTN_CLICK_INT, AXP202_PKEY_SHORT_IRQ);
                params |= check_params(option, PWR_BTN_LONGPRESSED_INT, AXP202_PKEY_LONG_IRQ);
                params |= check_params(option, ALL_INT, AXP202_ALL_IRQ);
                return enable ? enableIRQ(params) : disableIRQ(params);
                break;
#elif defined(PMIC_216)
                return false;
#elif defined(PMIC_AXP2101)
                params |= check_params(option, USB_INSERT_INT, AXP2101_VBUS_INSERT_IRQ);
                params |= check_params(option, USB_REMOVE_INT, AXP2101_VBUS_REMOVE_IRQ);
                params |= check_params(option, BATTERY_INSERT_INT, AXP2101_BAT_INSERT_IRQ);
                params |= check_params(option, BATTERY_REMOVE_INT, AXP2101_BAT_REMOVE_IRQ);
                params |= check_params(option, CHARGE_START_INT, AXP2101_BAT_CHG_START_IRQ);
                params |= check_params(option, CHARGE_DONE_INT, AXP2101_BAT_CHG_DONE_IRQ);
                params |= check_params(option, PWR_BTN_CLICK_INT, AXP2101_PKEY_SHORT_IRQ);
                params |= check_params(option, PWR_BTN_LONGPRESSED_INT, AXP2101_PKEY_LONG_IRQ);
                params |= check_params(option, ALL_INT, AXP2101_ALL_IRQ);
                return enable ? enableIRQ(params) : disableIRQ(params);
#else
                return false;
#endif
            }

        protected:
            void setChipModel(PMICChipModel m) { myModel = m; }
            uint32_t __protectedMask;
            TwoWire *myWire = this->myWire;
            int mySDA = this->mySDA;
            int mySCL = this->mySCL;
            uint8_t myADDR = this->myADDR;
            PMICChipModel myModel = this->myModel
        };
    }
}

#endif /* __TYPE1__ */
