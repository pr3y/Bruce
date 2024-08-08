#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "M5Timer.h"
#include <M5GFX.h>

M5Timer::M5Timer(void) {
    _enable_count = 0;
    for (uint_fast8_t i = 0; i < MAX_TIMERS; i++) {
        _timer_info[i].clear();
    }
}

void M5Timer::timer_info_t::clear(void) {
    _callback  = 0;
    _enabled   = false;
    _interval  = 0;
    _remain    = 0;
}

void M5Timer::timer_info_t::set(uint32_t interval_msec, timer_callback cb, uint32_t times) {
    _callback  = cb;
    _interval  = interval_msec;
    _remain    = times;
    _enabled   = true;
    _prev_msec = m5gfx::millis();
}

bool M5Timer::timer_info_t::run(uint32_t current_msec) {
    if (_callback != nullptr && current_msec - _prev_msec >= _interval) {
        _prev_msec += _interval;

        if (_enabled) {
            _callback();
            // Check the remaining number of executions; if 0, assume unlimited.
            if (_remain != 0) {
                return --_remain;
            }
        }
    }
    return true;
}

void M5Timer::run(void) {
    auto msec = m5gfx::millis();

    for (auto &t : _timer_info) {
        if (!t.getCallback()) { continue; }
        if (!t.run(msec)) {
            t.clear();
            _enable_count--;
        }
    }
}

int_fast8_t M5Timer::setTimer(uint32_t interval, timer_callback cb, uint32_t times) {
    if (cb == nullptr || _enable_count >= MAX_TIMERS) {
        return -1;
    }
    for (uint_fast8_t i = 0; i < MAX_TIMERS; i++) {
        if (_timer_info[i].getCallback()) { continue; }
        _timer_info[i].set(interval, cb, times);
        _enable_count++;
        return i;
    }
    return -1;
}

void M5Timer::deleteTimer(int_fast8_t id) {
    if ((uint_fast8_t)id < MAX_TIMERS && _timer_info[id].getCallback() != nullptr) {
        _timer_info[id].clear();
        _enable_count--;
    }
}

void M5Timer::restartTimer(int_fast8_t id) {
    if ((uint_fast8_t)id < MAX_TIMERS) {
        _timer_info[id].setPreviousMsec(m5gfx::millis());
    }
}
#endif