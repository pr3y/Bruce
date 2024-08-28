#if defined (CORE)
/*----------------------------------------------------------------------*
 * M5Stack Bettery/Power Control Library v1.0                           *
 *                                                                      *
 * This work is licensed under the GNU Lesser General Public            *
 * License v2.1                                                         *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html           *
 *----------------------------------------------------------------------*/
#ifndef Power_h
#define Power_h
#include <Arduino.h>
#include <Wire.h>

#define SLEEP_MSEC(us) (((uint64_t)us) * 1000L)
#define SLEEP_SEC(us)  (((uint64_t)us) * 1000000L)
#define SLEEP_MIN(us)  (((uint64_t)us) * 60L * 1000000L)
#define SLEEP_HR(us)   (((uint64_t)us) * 60L * 60L * 1000000L)

class POWER {
   public:
    POWER();
    bool canControl();
    void begin();

    // -- ShutdownTimeParam
    enum ShutdownTime {
        SHUTDOWN_8S = 0,
        SHUTDOWN_16S,
        SHUTDOWN_32S,
        SHUTDOWN_64S
    };

    // -- control for power
    bool setKeepLightLoad(bool en) __attribute__((deprecated));
    bool setPowerBoostKeepOn(bool en);
    bool setAutoBootOnLoad(bool en);
    bool setLowPowerShutdown(bool en) __attribute__((deprecated));
    bool setLowPowerShutdownTime(ShutdownTime time);
    bool setPowerBoostOnOff(bool en);
    bool setPowerBoostSet(bool en);
    bool setPowerVin(bool en);
    bool setPowerWLEDSet(bool en);
    bool setPowerBtnEn(bool en);

    // -- control for battery
    bool setVinMaxCurrent(uint8_t cur);
    bool setChargeVolt(uint8_t volt);

    bool setCharge(bool en);
    bool isChargeFull();
    bool isCharging();
    int8_t getBatteryLevel();
    bool batteryMode(bool en);

    // -- configuration for wakeup
    void setWakeupButton(uint8_t button);

    // -- get resson for startup
    bool isResetbyWatchdog();
    bool isResetbyDeepsleep();
    bool isResetbySoftware();
    bool isResetbyPowerSW();

    // -- sleep
    void deepSleep(uint64_t time_in_us = 0);
    void lightSleep(uint64_t time_in_us = 0);

    // -- power off
    void powerOFF();

    // -- software reset
    void reset();

   private:
    uint8_t _wakeupPin;
};
#endif
#endif