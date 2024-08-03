#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_Timer_H__
#define __M5_Timer_H__

#include <functional>
#include <stdint.h>
#include <stddef.h>

typedef std::function<void(void)> timer_callback;

class M5Timer {
   public:
    /// maximum number of timers
    static constexpr const uint32_t MAX_TIMERS = 10;

    /// constructor
    M5Timer(void);

    /// this function must be called inside loop()
    void run(void);

    /// Call function every interval [msec] for specified times.
    /// @param interval_msec interval [msec]
    /// @param function target function.
    /// @return -1 = failed / 0 or more = target timer id.
    int_fast8_t setTimer(uint32_t interval_msec, timer_callback function, uint32_t times);

    /// Call function every interval [msec].
    /// @param interval_msec interval [msec]
    /// @param function target function.
    /// @return -1 = failed / 0 or more = target timer id.
    inline int_fast8_t setInterval(uint32_t interval_msec, timer_callback function) {
        return setTimer(interval_msec, function, 0);
    }

    /// Call function once after interval [msec].
    /// @param interval_msec interval [msec]
    /// @param function target function.
    /// @return -1 = failed / 0 or more = target timer id.
    inline int_fast8_t setTimeout(uint32_t interval_msec, timer_callback function) {
        return setTimer(interval_msec, function, 1);
    }

    /// Destroy the specified timer.
    /// @param id target timer id.
    void deleteTimer(int_fast8_t id);

    /// Restart the specified timer.
    /// @param id target timer id.
    void restartTimer(int_fast8_t id);

    /// Enables the specified timer.
    /// @param id target timer id.
    void enable(int_fast8_t id) {
        if ((uint_fast8_t)id < MAX_TIMERS) {
            _timer_info[id].setEnabled(true);
        }
    }

    /// Disables the specified timer.
    /// @param id target timer id.
    void disable(int_fast8_t id) {
        if ((uint_fast8_t)id < MAX_TIMERS) {
            _timer_info[id].setEnabled(false);
        }
    }

    /// Enables the specified timer if it is currently disabled, and vice versa.
    /// @param id target timer id.
    void toggle(int_fast8_t id) {
        if ((uint_fast8_t)id < MAX_TIMERS) {
            _timer_info[id].toggle();
        }
    }

    /// gets true if the specified timer is enabled
    /// @param id target timer id.
    /// @return false=disabled / true=enabled.
    bool isEnabled(int_fast8_t id) const {
        return ((uint_fast8_t)id < MAX_TIMERS) ? _timer_info[id].getEnabled() : false;
    }

    /// gets the number of used timers.
    /// @return number of used timers.
    uint_fast8_t getNumTimers(void) const {
        return _enable_count;
    }

    /// gets the number of available timers.
    /// @return number of available timers.
    uint_fast8_t getNumAvailableTimers(void) const {
        return MAX_TIMERS - _enable_count;
    }

   private:

    class timer_info_t {
    public:
        void set(uint32_t interval_msec, timer_callback function, uint32_t times);
        bool run(uint32_t interval_msec);
        void clear(void);

        inline timer_callback getCallback(void) const { return _callback; }
        inline bool getEnabled(void) const { return _enabled; }
        inline void setEnabled(bool flg) { _enabled = flg; }
        inline void toggle(void) { _enabled = !_enabled; }
        inline void setPreviousMsec(uint32_t msec) { _prev_msec = msec; }

    private:
        timer_callback _callback = nullptr;
        uint32_t _prev_msec = 0;
        uint32_t _interval = 0;
        uint32_t _remain = 0;
        bool _enabled = false;
    };

    timer_info_t _timer_info[MAX_TIMERS];

    uint8_t _enable_count = 0;
};

#endif
#endif