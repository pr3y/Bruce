#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_Power_Class_H__
#define __M5_Power_Class_H__

#include "m5unified_common.h"

#include "I2C_Class.hpp"
#include "AXP192_Class.hpp"
#include "AXP2101_Class.hpp"
#include "IP5306_Class.hpp"
#include "INA3221_Class.hpp"
#include "RTC8563_Class.hpp"

#if __has_include (<driver/adc.h>)
#include <driver/adc.h>
#endif

namespace m5
{
  class M5Unified;

  enum ext_port_mask_t
  { ext_none = 0
  // For individual control of external ports of M5Station
  , ext_PA   = 0b00000001
  , ext_PB1  = 0b00000010
  , ext_PB2  = 0b00000100
  , ext_PC1  = 0b00001000
  , ext_PC2  = 0b00010000
  , ext_USB  = 0b00100000 // M5Station external USB.   ※ Not for CoreS3 main USB.
  , ext_MAIN = 0b10000000
  };

  class Power_Class
  {
  friend M5Unified;
  public:

    enum pmic_t
    { pmic_unknown
    , pmic_adc
    , pmic_axp192
    , pmic_ip5306
    , pmic_axp2101
    };

    enum is_charging_t
    { is_discharging = 0
    , is_charging
    , charge_unknown
    };

    bool begin(void);

    /// Set power output of the external ports.
    /// @param enable true=output / false=input
    /// @param port_mask for M5Station. ext_port (bitmask).
    void setExtOutput(bool enable, ext_port_mask_t port_mask = (ext_port_mask_t)0xFF);

    /// deprecated : Change to "setExtOutput"
    [[deprecated("Change to setExtOutput")]]
    void setExtPower(bool enable, ext_port_mask_t port_mask = (ext_port_mask_t)0xFF) { setExtOutput(enable, port_mask); }

    /// Get power output of the external ports.
    /// @return true=output enabled / false=output disabled
    bool getExtOutput(void);

    /// Set power output of the main USB port.
    /// @param enable true=output / false=input
    /// @attention for M5Stack CoreS3 main USB port.
    /// @attention ※ Not for M5Station external USB.
    void setUsbOutput(bool enable);

    /// Get power output of the main USB port.
    /// @return true=output enabled / false=output disabled
    /// @attention for M5Stack CoreS3 main USB port.
    /// @attention ※ Not for M5Station external USB.
    bool getUsbOutput(void);

    /// Turn on/off the power LED.
    /// @param brightness 0=OFF: 1~255=ON (Set brightness if possible.)
    void setLed(uint8_t brightness = 255);

    /// all power off.
    void powerOff(void);

    /// sleep and timer boot. The boot condition can be specified by the argument.
    /// @param seconds Number of seconds to boot.
    void timerSleep(int seconds);

    /// sleep and timer boot. The boot condition can be specified by the argument.
    /// @param time Time to boot. (only minutes and hours can be specified. Ignore seconds)
    /// @attention CoreInk and M5Paper can't alarm boot because it can't be turned off while connected to USB.
    /// @attention CoreInk と M5Paper は USB接続中はRTCタイマー起動が出来ない。;
    void timerSleep(const rtc_time_t& time);

    /// sleep and timer boot. The boot condition can be specified by the argument.
    /// @param date Date to boot. (only date and weekDay can be specified. Ignore year and month)
    /// @param time Time to boot. (only minutes and hours can be specified. Ignore seconds)
    /// @attention CoreInk and M5Paper can't alarm boot because it can't be turned off while connected to USB.
    /// @attention CoreInk と M5Paper は USB接続中はRTCタイマー起動が出来ない。;
    void timerSleep(const rtc_date_t& date, const rtc_time_t& time);

    /// ESP32 deepsleep
    /// @param seconds Number of micro seconds to wakeup.
    void deepSleep(std::uint64_t micro_seconds = 0, bool touch_wakeup = true);

    /// ESP32 lightsleep
    /// @param seconds Number of micro seconds to wakeup.
    void lightSleep(std::uint64_t micro_seconds = 0, bool touch_wakeup = true);

    /// Get the remaining battery power.
    /// @return 0-100 level
    std::int32_t getBatteryLevel(void);

    /// set battery charge enable.
    /// @param enable true=enable / false=disable
    void setBatteryCharge(bool enable);

    /// set battery charge current
    /// @param max_mA milli ampere.
    /// @attention Non-functioning models : CoreInk , M5Paper , M5Stack(with non I2C IP5306)
    void setChargeCurrent(std::uint16_t max_mA);

    /// set battery charge voltage
    /// @param max_mV milli volt.
    /// @attention Non-functioning models : CoreInk , M5Paper , M5Stack(with non I2C IP5306)
    void setChargeVoltage(std::uint16_t max_mV);

    /// Get whether the battery is currently charging or not.
    /// @attention Non-functioning models : CoreInk , M5Paper , M5Stack(with non I2C IP5306)
    is_charging_t isCharging(void);

    /// Get battery voltage
    /// @return battery voltage [mV]
    int16_t getBatteryVoltage(void);

    /// get battery current
    /// @return battery current [mA] ( +=charge / -=discharge )
    int32_t getBatteryCurrent(void);

    /// Get Power Key Press condition.
    /// @return 0=none / 1=long pressed / 2=short clicked / 3=both
    /// @attention Only for models with AXP192 or AXP2101
    /// @attention Once this function is called, the value is reset to 0, and the next time it is pressed on, the value changes.
    uint8_t getKeyState(void);

    /// Operate the vibration motor
    /// @param level Vibration strength of the motor. (0=stop)
    void setVibration(uint8_t level);

    pmic_t getType(void) const { return _pmic; }

#if defined (CONFIG_IDF_TARGET_ESP32S3)

    AXP2101_Class Axp2101;

#elif defined (CONFIG_IDF_TARGET_ESP32C3)

#else

    AXP2101_Class Axp2101;
    AXP192_Class Axp192;
    IP5306_Class Ip5306;
    // secondery INA3221 for M5Station.
    INA3221_Class Ina3221[2] = { { 0x40 }, { 0x41 } };

#endif

  private:
    void _powerOff(bool withTimer);
    void _timerSleep(void);

    float _adc_ratio = 0;
    std::uint8_t _wakeupPin = 255;
    std::uint8_t _rtcIntPin = 255;
    pmic_t _pmic = pmic_t::pmic_unknown;
#if !defined (M5UNIFIED_PC_BUILD)
    uint8_t _batAdcCh;
    uint8_t _batAdcUnit;
#endif
  };
}

#endif
#endif