#include "TYPE1.hpp"
namespace HAL
{
    namespace PMIC
    {
        template <class Driver>
        bool Type1<Driver>::isChannelAvailable(PMICPowerChannel channel)
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

        template <class Driver>
        void Type1<Driver>::setProtectedChannel(PMICPowerChannel channel)
        {
            __protectedMask |= _BV(channel);
        }

        template <class Driver>
        void Type1<Driver>::setUnprotectChannel(PMICPowerChannel channel)
        {
            __protectedMask &= (~_BV(channel));
        }

        template <class Driver>
        bool Type1<Driver>::getProtectedChannel(PMICPowerChannel channel)
        {
            return __protectedMask & _BV(channel);
        }

        static uint64_t inline check_params(uint32_t opt, uint32_t params, uint64_t mask)
        {
            return ((opt & params) == params) ? mask : 0;
        }

        template <class Driver>
        bool Type1<Driver>::enableInterrupt(uint32_t option)
        {
            return setInterruptMask(option, true);
        }

        template <class Driver>
        bool Type1<Driver>::disableInterrupt(uint32_t option)
        {
            return setInterruptMask(option, false);
        }

        template <class Driver>
        bool Type1<Driver>::setInterruptMask(uint32_t option, bool enable)
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
    }
}