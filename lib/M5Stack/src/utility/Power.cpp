#if defined (CORE)
/*----------------------------------------------------------------------*
 * M5Stack Bettery/Power Control Library v1.0                           *
 *                                                                      *
 * This work is licensed under the GNU Lesser General Public            *
 * License v2.1                                                         *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html           *
 *----------------------------------------------------------------------*/
#include "Power.h"

#include <esp_bt_main.h>
#include <esp_sleep.h>
#include <esp_wifi.h>

#ifndef ESP_ARDUINO_VERSION_VAL
#define ESP_ARDUINO_VERSION_VAL(major, minor, patch) \
    ((major << 16) | (minor << 8) | (patch))
#endif

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
#include "esp32/rom/rtc.h"
#else
#include <rom/rtc.h>
#endif  // ESP_ARDUINO_VERSION

#include "../M5Stack.h"

// ================ Power IC IP5306 ===================
#define IP5306_ADDR         (117)  // 0x75
#define IP5306_REG_SYS_CTL0 (0x00)
#define IP5306_REG_SYS_CTL1 (0x01)
#define IP5306_REG_SYS_CTL2 (0x02)
#define IP5306_REG_READ0    (0x70)
#define IP5306_REG_READ1    (0x71)
#define IP5306_REG_READ3    (0x78)
#define IP5306_REG_CHG_CTL0 (0x20)
#define IP5306_REG_CHG_CTL1 (0x21)
#define IP5306_REG_CHG_DIG  (0x24)

//- REG_CTL0
#define BOOST_ENABLE_BIT    (0x20)
#define CHARGE_OUT_BIT      (0x10)
#define BOOT_ON_LOAD_BIT    (0x04)
#define BOOST_OUT_BIT       (0x02)
#define BOOST_BUTTON_EN_BIT (0x01)

//- REG_CTL1
#define BOOST_SET_BIT   (0x80)
#define WLED_SET_BIT    (0x40)
#define SHORT_BOOST_BIT (0x20)
#define VIN_ENABLE_BIT  (0x04)

//- REG_CTL2
#define SHUTDOWNTIME_MASK (0x0c)
#define SHUTDOWNTIME_64S  (0x0c)
#define SHUTDOWNTIME_32S  (0x04)
#define SHUTDOWNTIME_16S  (0x08)
#define SHUTDOWNTIME_8S   (0x00)

//- REG_READ0
#define CHARGE_ENABLE_BIT (0x08)

//- REG_READ1
#define CHARGE_FULL_BIT (0x08)

//- REG_READ2
#define LIGHT_LOAD_BIT        (0x20)
#define LOWPOWER_SHUTDOWN_BIT (0x01)

//- CHG
#define CURRENT_100MA  (0x01 << 0)
#define CURRENT_200MA  (0x01 << 1)
#define CURRENT_400MA  (0x01 << 2)
#define CURRENT_800MA  (0x01 << 3)
#define CURRENT_1600MA (0x01 << 4)

#define BAT_4_2V   (0x00)
#define BAT_4_3V   (0x01)
#define BAT_4_3_5V (0x02)
#define BAT_4_4V   (0x03)

#define CHG_CC_BIT (0x20)

extern M5Stack M5;

POWER::POWER() {
}

void POWER::begin() {
    uint8_t data;

    // Initial I2C
    Wire.begin(21, 22);

    // 450ma
    setVinMaxCurrent(CURRENT_400MA);

    setChargeVolt(BAT_4_2V);

    // End charge current 200ma
    if (M5.I2C.readByte(IP5306_ADDR, 0x21, &data) == true) {
        M5.I2C.writeByte(IP5306_ADDR, 0x21, (data & 0x3f) | 0x00);
    }

    // Add volt 28mv
    if (M5.I2C.readByte(IP5306_ADDR, 0x22, &data) == true) {
        M5.I2C.writeByte(IP5306_ADDR, 0x22, (data & 0xfc) | 0x02);
    }

    // Vin charge CC
    if (M5.I2C.readByte(IP5306_ADDR, 0x23, &data) == true) {
        M5.I2C.writeByte(IP5306_ADDR, 0x23, (data & 0xdf) | 0x20);
    }
}

bool POWER::setPowerBoostOnOff(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL1,
            en ? (data | BOOST_SET_BIT) : (data & (~BOOST_SET_BIT)));
    }
    return false;
}

bool POWER::setPowerBoostSet(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL1,
            en ? (data | SHORT_BOOST_BIT) : (data & (~SHORT_BOOST_BIT)));
    }
    return false;
}

bool POWER::setPowerVin(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL1,
            en ? (data | VIN_ENABLE_BIT) : (data & (~VIN_ENABLE_BIT)));
    }
    return false;
}

bool POWER::setPowerWLEDSet(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL1,
            en ? (data | WLED_SET_BIT) : (data & (~WLED_SET_BIT)));
    }
    return false;
}

bool POWER::setPowerBtnEn(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
        return M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0,
                                en ? (data | BOOST_BUTTON_EN_BIT)
                                   : (data & (~BOOST_BUTTON_EN_BIT)));
    }
    return false;
}

bool POWER::setLowPowerShutdownTime(ShutdownTime time) {
    uint8_t data;
    bool ret;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL2, &data) == true) {
        switch (time) {
            case ShutdownTime::SHUTDOWN_8S:
                ret = M5.I2C.writeByte(
                    IP5306_ADDR, IP5306_REG_SYS_CTL2,
                    ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_8S));
                break;
            case ShutdownTime::SHUTDOWN_16S:
                ret = M5.I2C.writeByte(
                    IP5306_ADDR, IP5306_REG_SYS_CTL2,
                    ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_16S));
                break;
            case ShutdownTime::SHUTDOWN_32S:
                ret = M5.I2C.writeByte(
                    IP5306_ADDR, IP5306_REG_SYS_CTL2,
                    ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_32S));
                break;
            case ShutdownTime::SHUTDOWN_64S:
                ret = M5.I2C.writeByte(
                    IP5306_ADDR, IP5306_REG_SYS_CTL2,
                    ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_64S));
                break;
            default:
                ret = false;
                break;
        }
        return ret;
    }
    return false;
}

/*
  default: false
  false: when the current is too small, ip5306 will automatically shut down
  note: it seem not work and has problems
        Function has disabled.(Stab for compatibility)
        This function will be removed in a future release.
*/
bool POWER::setKeepLightLoad(bool en) {
    // uint8_t data;
    // if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
    //     return M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, !en ? (data
    //     | LIGHT_LOAD_BIT) : (data & (~LIGHT_LOAD_BIT)));
    // }
    return false;
}

// true: Output normally open
bool POWER::setPowerBoostKeepOn(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL0,
            en ? (data | BOOST_OUT_BIT) : (data & (~BOOST_OUT_BIT)));
    }
    return false;
}

/**
 * Function has disabled.(Stab for compatibility)
 * This function will be removed in a future release.
 */
bool POWER::setLowPowerShutdown(bool en) {
    // uint8_t data;
    // if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
    //  return M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, en ? (data |
    //  LOWPOWER_SHUTDOWN_BIT) : (data & (~LOWPOWER_SHUTDOWN_BIT)));
    //}
    return setPowerBoostKeepOn(!en);
}
/*
  default: true
  true: If enough load is connected, the power will turn on.
*/
bool POWER::setAutoBootOnLoad(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL0,
            en ? (data | BOOT_ON_LOAD_BIT) : (data & (~BOOT_ON_LOAD_BIT)));
    }
    return false;
}

bool POWER::setVinMaxCurrent(uint8_t cur) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_CHG_DIG, &data) == true) {
        return M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_CHG_DIG,
                                (data & 0xe0) | cur);
    }
    return false;
}

bool POWER::setChargeVolt(uint8_t volt) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_CHG_CTL0, &data) == true) {
        return M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_CHG_CTL0,
                                (data & 0xfc) | volt);
    }
    return false;
}

// if charge full,try set charge enable->disable->enable,can be recharged
bool POWER::setCharge(bool en) {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
        return M5.I2C.writeByte(
            IP5306_ADDR, IP5306_REG_SYS_CTL0,
            en ? (data | CHARGE_OUT_BIT) : (data & (~CHARGE_OUT_BIT)));
    }
    return false;
}

// full return true, else return false
bool POWER::isChargeFull() {
    uint8_t data;
    return (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_READ1, &data))
               ? (data & CHARGE_FULL_BIT)
               : false;
}

// test if ip5306 is an i2c version
bool POWER::canControl() {
    return M5.I2C.writeCommand(IP5306_ADDR, IP5306_REG_READ0);
}

// true:charge, false:discharge
bool POWER::isCharging() {
    uint8_t data;
    return (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_READ0, &data))
               ? (data & CHARGE_ENABLE_BIT)
               : false;
}

// Return percentage * 100
int8_t POWER::getBatteryLevel() {
    uint8_t data;
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_READ3, &data) == true) {
        switch (data & 0xF0) {
            case 0x00:
                return 100;
            case 0x80:
                return 75;
            case 0xC0:
                return 50;
            case 0xE0:
                return 25;
            default:
                return 0;
        }
    }
    return -1;
}

void POWER::setWakeupButton(uint8_t button) {
    _wakeupPin = button;
}

void POWER::reset() {
    esp_restart();
}

bool POWER::isResetbySoftware() {
    RESET_REASON reset_reason = rtc_get_reset_reason(0);
    return (reset_reason == SW_RESET || reset_reason == SW_CPU_RESET);
}

bool POWER::isResetbyWatchdog() {
    RESET_REASON reset_reason = rtc_get_reset_reason(0);
    return (
        reset_reason == TG0WDT_SYS_RESET || reset_reason == TG1WDT_SYS_RESET ||
        reset_reason == OWDT_RESET || reset_reason == RTCWDT_SYS_RESET ||
        reset_reason == RTCWDT_CPU_RESET || reset_reason == RTCWDT_RTC_RESET ||
        reset_reason == TGWDT_CPU_RESET);
}

bool POWER::isResetbyDeepsleep() {
    RESET_REASON reset_reason = rtc_get_reset_reason(0);
    return (reset_reason == DEEPSLEEP_RESET);
}

bool POWER::isResetbyPowerSW() {
    RESET_REASON reset_reason = rtc_get_reset_reason(0);
    return (reset_reason == POWERON_RESET);
}

// note:
// If the IP5306 I2C communication is not available,
// such as the old model, there is a limit to the maximum time for sleep return.
// When using this function, pay attention to the constraints.
void POWER::deepSleep(uint64_t time_in_us) {
    // Keep power keep boost on
    setPowerBoostKeepOn(true);

    // power off the Lcd
    M5.Lcd.setBrightness(0);
    M5.Lcd.sleep();

    // ESP32 into deep sleep
    esp_sleep_enable_ext0_wakeup((gpio_num_t)_wakeupPin, LOW);

    if (time_in_us > 0) {
        esp_sleep_enable_timer_wakeup(time_in_us);
    } else {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }

    while (digitalRead(_wakeupPin) == LOW) {
        delay(10);
    }

    (time_in_us == 0) ? esp_deep_sleep_start() : esp_deep_sleep(time_in_us);
}

// note:
// If the IP5306 I2C communication is not available,
// such as the old model, there is a limit to the maximum time for sleep return.
// When using this function, pay attention to the constraints.
void POWER::lightSleep(uint64_t time_in_us) {
    // Keep power keep boost on
    setPowerBoostKeepOn(true);

    // power off the Lcd
    M5.Lcd.setBrightness(0);
    M5.Lcd.sleep();

    // ESP32 into deep sleep
    esp_sleep_enable_ext0_wakeup((gpio_num_t)_wakeupPin, LOW);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);

    while (digitalRead(_wakeupPin) == LOW) {
        delay(10);
    }
    if (time_in_us > 0) {
        esp_sleep_enable_timer_wakeup(time_in_us);
    } else {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }
    esp_light_sleep_start();

    // power on the Lcd
    M5.Lcd.wakeup();
    M5.Lcd.setBrightness(200);
}

// note:
// To ensure that the power is turned off,
// reduce the power consumption according to the specifications of the power
// supply IC. Otherwise, the power supply IC will continue to supply power.
void POWER::powerOFF() {
    uint8_t data;
    // power off the Lcd
    M5.Lcd.setBrightness(0);
    M5.Lcd.sleep();

    // Power off request
    if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
        M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1,
                         (data & (~BOOST_ENABLE_BIT)));
    }

    // if wifi was initialized, stop it
    wifi_mode_t mode;
    if (esp_wifi_get_mode(&mode) == ESP_OK) {
        esp_wifi_disconnect();
        esp_wifi_stop();
    }

    // stop bt
    esp_bluedroid_disable();

    // disable interrupt/peripheral
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    gpio_deep_sleep_hold_dis();

    // Shutdown setting
    setPowerBoostKeepOn(false);
    setLowPowerShutdownTime(ShutdownTime::SHUTDOWN_8S);
    setPowerBtnEn(true);

    // wait shutdown from IP5306 (low-current shutdown)
    esp_deep_sleep_start();
}
#endif