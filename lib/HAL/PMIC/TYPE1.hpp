#ifndef __TYPE1__
#define __TYPE1__

#include "PMIC.tpp"
#include "REG/TYPE1.hpp"

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
            Type1() : __protectedMask(0) {};
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
            typedef struct gpio_t
            {
                uint8_t mode;
            };
            bool isChannelAvailable(PMICPowerChannel channel);
            void setProtectedChannel(PMICPowerChannel channel);
            bool getProtectedChannel(PMICPowerChannel channel);
            void setUnprotectChannel(PMICPowerChannel channel);
            bool enableInterrupt(uint32_t option);
            bool disableInterrupt(uint32_t option);
            bool setInterruptMask(uint32_t option, bool enable);

        protected:
            void setChipModel(PMICChipModel m) { myModel = m; }
            uint32_t __protectedMask;
        };
    }
}

#endif  /* __TYPE1__ */
