#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_Touch_Class_H__
#define __M5_Touch_Class_H__

#include <M5GFX.h>

#include <cstdint>

namespace m5
{
  enum touch_state_t : uint8_t
  { none         = 0b0000
  , touch        = 0b0001
  , touch_end    = 0b0010
  , touch_begin  = 0b0011

  , hold         = 0b0101
  , hold_end     = 0b0110
  , hold_begin   = 0b0111

  , flick        = 0b1001
  , flick_end    = 0b1010
  , flick_begin  = 0b1011

  , drag         = 0b1101
  , drag_end     = 0b1110
  , drag_begin   = 0b1111

  , mask_touch   = 0b0001
  , mask_change  = 0b0010
  , mask_holding = 0b0100
  , mask_moving  = 0b1000
  };

  class Touch_Class
  {
  public:
    static constexpr std::size_t TOUCH_MAX_POINTS = 3;
    static constexpr std::size_t TOUCH_MIN_UPDATE_MSEC = 4;

    struct point_t
    {
      std::int16_t x;
      std::int16_t y;
    };

    struct touch_detail_t : public m5gfx::touch_point_t
    {
      union
      { /// Previous point
        point_t prev;
        struct
        {
          std::int16_t prev_x;
          std::int16_t prev_y;
        };
      };
      union
      { /// Flick start point
        point_t base;
        struct
        {
          std::int16_t base_x;
          std::int16_t base_y;
        };
      };

      std::uint32_t base_msec;
      touch_state_t state = touch_state_t::none;
      std::uint8_t click_count = 0;

      inline int deltaX(void) const { return x - prev_x; }
      inline int deltaY(void) const { return y - prev_y; }
      inline int distanceX(void) const { return x - base_x; }
      inline int distanceY(void) const { return y - base_y; }
      inline bool isPressed(void) const { return state & touch_state_t::mask_touch; };
      inline bool wasPressed(void) const { return state == touch_state_t::touch_begin; };
      inline bool wasClicked(void) const { return state == touch_state_t::touch_end; };
      inline bool isReleased(void) const { return !(state & touch_state_t::mask_touch); };
      inline bool wasReleased(void) const { return (state & (touch_state_t::mask_touch | touch_state_t::mask_change)) == touch_state_t::mask_change; };
      inline bool isHolding(void) const { return (state & (touch_state_t::mask_touch | touch_state_t::mask_holding)) == (touch_state_t::mask_touch | touch_state_t::mask_holding); }
      inline bool wasHold(void) const { return state == touch_state_t::hold_begin; }
      inline bool wasFlickStart(void) const { return state == touch_state_t::flick_begin; }
      inline bool isFlicking(void) const { return (state & touch_state_t::drag) == touch_state_t::flick; }
      inline bool wasFlicked(void) const { return state == touch_state_t::flick_end; }
      inline bool wasDragStart(void) const { return state == touch_state_t::drag_begin; }
      inline bool isDragging(void) const { return (state & touch_state_t::drag) == touch_state_t::drag; }
      inline bool wasDragged(void) const { return state == touch_state_t::drag_end; }
      inline std::uint8_t getClickCount(void) const { return click_count; }
    };

    /// Get the current number of touchpoints.
    /// @return number of touchpoints.
    inline std::uint8_t getCount(void) const { return _detail_count; }

    /// 
    inline const touch_detail_t& getDetail(std::size_t index = 0) const { return _touch_detail[_touch_raw[index].id < TOUCH_MAX_POINTS ? _touch_raw[index].id : 0]; }


    inline const m5gfx::touch_point_t& getTouchPointRaw(std::size_t index = 0) const { return _touch_raw[index < _detail_count ? index : 0]; }

    void setHoldThresh(std::uint16_t msec) { _msecHold = msec; }

    void setFlickThresh(std::uint16_t distance) { _flickThresh = distance; }

    bool isEnabled(void) const { return _gfx; }

    void begin(m5gfx::LGFX_Device* gfx) { _gfx = gfx; }
    void update(std::uint32_t msec);
    void end(void) { _gfx = nullptr; }

  protected:
    std::uint32_t _last_msec = 0;
    std::int32_t _flickThresh = 8;
    std::uint32_t _msecHold = 500;
    m5gfx::LGFX_Device* _gfx = nullptr;
    touch_detail_t _touch_detail[TOUCH_MAX_POINTS];
    m5gfx::touch_point_t _touch_raw[TOUCH_MAX_POINTS];
    std::uint8_t _detail_count;

    bool update_detail(touch_detail_t* dt, std::uint32_t msec, bool pressed, m5gfx::touch_point_t* tp);
    bool update_detail(touch_detail_t* dt, std::uint32_t msec);
  };
}
#endif
#endif