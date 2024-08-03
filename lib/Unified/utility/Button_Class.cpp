#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Button_Class.hpp"

namespace m5
{
  void Button_Class::setState(std::uint32_t msec, button_state_t state)
  {
    if (_currentState == state_decide_click_count)
    {
      _clickCount = 0;
    }

    _lastMsec = msec;
    bool flg_timeout = (msec - _lastClicked > _msecHold);
    switch (state)
    {
    case state_nochange:
      if (flg_timeout && !_press && _clickCount)
      {
        if (_oldPress == 0 && _currentState == state_nochange)
        {
          state = state_decide_click_count;
        }
        else { _clickCount = 0; }
      }
      break;

    case state_clicked:
      ++_clickCount;
      _lastClicked = msec;
      break;

    default:
      break;
    }
    _currentState = state;
  }

  void Button_Class::setRawState(std::uint32_t msec, bool press)
  {
    button_state_t state = button_state_t::state_nochange;
    bool disable_db = (msec - _lastMsec) > _msecDebounce;
    auto oldPress = _press;
    _oldPress = oldPress;
    if (_raw_press != press)
    {
      _raw_press = press;
      _lastRawChange = msec;
    }
    if (disable_db || msec - _lastRawChange >= _msecDebounce)
    {
      if (press != (0 != oldPress))
      {
        _lastChange = msec;
      }

      if (press)
      {
        std::uint32_t holdPeriod = msec - _lastChange;
        _lastHoldPeriod = holdPeriod;
        if (!oldPress)
        {
          _press = 1;
        } else 
        if (oldPress == 1 && (holdPeriod >= _msecHold))
        {
          _press = 2;
          state = button_state_t::state_hold;
        }
      }
      else
      {
        _press = 0;
        if (oldPress == 1)
        {
          state = button_state_t::state_clicked;
        }
      }
    }
    setState(msec, state);
  }
}
#endif