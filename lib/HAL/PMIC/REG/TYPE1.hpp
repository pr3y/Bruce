#ifndef __REG_TYPE1__
#define __REG_TYPE1__

#define _BV(b) (1ULL << (uint64_t)(b))
namespace HAL::PMIC
{
    /*
    | CHIP       | AXP173            | AXP192            | AXP202            | AXP2101                                |
    | ---------- | ----------------- | ----------------- | ----------------- | -------------------------------------- |
    | DC1        | 0.7V-3.5V /1.2A   | 0.7V-3.5V  /1.2A  | X                 | 1.5-3.4V                        /2A    |
    | DC2        | 0.7-2.275V/0.6A   | 0.7-2.275V /1.6A  | 0.7-2.275V /1.6A  | 0.5-1.2V,1.22-1.54V             /2A    |
    | DC3        | X                 | 0.7-3.5V   /0.7A  | 0.7-3.5V   /1.2A  | 0.5-1.2V,1.22-1.54V,1.6-3.4V    /2A    |
    | DC4        | X                 | x                 | x                 | 0.5-1.2V,1.22-1.84V             /1.5A   |
    | DC5        | X                 | x                 | x                 | 1.2V,1.4-3.7V                   /1A    |
    | LDO1(VRTC) | 3.3V       /30mA  | 3.3V       /30mA  | 3.3V       /30mA  | 1.8V                            /30mA  |
    | LDO2       | 1.8V-3.3V  /200mA | 1.8V-3.3V  /200mA | 1.8V-3.3V  /200mA | x                                      |
    | LDO3       | 1.8V-3.3V  /200mA | 1.8-3.3V   /200mA | 0.7-3.5V   /200mA | x                                      |
    | LDO4       | 0.7-3.5V   /500mA | X                 | 1.8V-3.3V  /200mA | x                                      |
    | LDO5/IO0   | X                 | 1.8-3.3V   /50mA  | 1.8-3.3V   /50mA  | x                                      |
    | ALDO1      | x                 | x                 | x                 | 0.5-3.5V                        /300mA |
    | ALDO2      | x                 | x                 | x                 | 0.5-3.5V                        /300mA |
    | ALDO3      | x                 | x                 | x                 | 0.5-3.5V                        /300mA |
    | ALDO4      | x                 | x                 | x                 | 0.5-3.5V                        /300mA |
    | BLDO1      | x                 | x                 | x                 | 0.5-3.5V                        /300mA |
    | BLDO2      | x                 | x                 | x                 | 0.5-3.5V                        /300mA |
    | DLDO1      | x                 | x                 | x                 | 0.5-3.3V/ 0.5-1.4V              /300mA |
    | DLDO1      | x                 | x                 | x                 | 0.5-3.3V/ 0.5-1.4V              /300mA |
    | CPUSLDO    | x                 | x                 | x                 | 0.5-1.4V                        /30mA  |
    |            |                   |                   |                   |                                        |
    */

    /**
     * @brief Each chip resource is different,please refer to the table above.
     */
    enum PMICPowerChannel
    {
        DCDC1,
        DCDC2,
        DCDC3,
        DCDC4,
        DCDC5,
        LDO1,
        LDO2,
        LDO3,
        LDO4,
        LDO5,
        LDOIO,
        ALDO1,
        ALDO2,
        ALDO3,
        ALDO4,
        BLDO1,
        BLDO2,
        DLDO1,
        DLDO2,
        VBACKUP,
        CPULDO,
    };

    /**
     * @brief PMU PEKEY Press off time parameters.
     */
    enum pmic_press_off_time
    {
        POWEROFF_4S,
        POWEROFF_6S,
        POWEROFF_8S,
        POWEROFF_10S,
    };

    /**
     * @brief PMU PEKEY Press on time parameters.
     */
    enum pmic_press_on_time
    {
        POWERON_128MS,
        POWERON_512MS,
        POWERON_1S,
        POWERON_2S,
    };

    /**
     * @brief Charging led mode parameters.
     */
    enum pmic_chg_led_mode
    {
        CHG_LED_OFF,
        CHG_LED_BLINK_1HZ,
        CHG_LED_BLINK_4HZ,
        CHG_LED_ON,
        CHG_LED_CTRL_CHG, // The charging indicator is controlled by the charger
    };

    enum pmic_interrupt_enum
    {
        USB_INSERT_INT = _BV(0),
        USB_REMOVE_INT = _BV(1),
        BATTERY_INSERT_INT = _BV(2),
        BATTERY_REMOVE_INT = _BV(3),
        CHARGE_START_INT = _BV(4),
        CHARGE_DONE_INT = _BV(5),
        PWR_BTN_CLICK_INT = _BV(6),
        PWR_BTN_LONGPRESSED_INT = _BV(7),
        ALL_INT = _BV(8),
    };

}
#endif /* __TYPE1__ */
