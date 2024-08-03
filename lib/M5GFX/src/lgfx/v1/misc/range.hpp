/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [BSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include <stdint.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)
  struct range_t
  {
    int_fast16_t first;
    int_fast16_t last;

    int_fast16_t length(void) const { return last - first + 1; }
    bool empty(void) const { return last < first; }
    bool intersectsWith(const range_t& r) const { return (r.first <= last) && (first <= r.last); }
    bool intersectsWith(int_fast16_t f, int_fast16_t l) const { return (f <= last) && (first <= l); }
  };

  struct range_rect_t
  {
    union
    {
      struct
      {
        range_t horizon;
        range_t vertical;
      };
      struct
      {
        int_fast16_t left;
        int_fast16_t right;
        int_fast16_t top;
        int_fast16_t bottom;
      };
      struct
      {
        int_fast16_t l;
        int_fast16_t r;
        int_fast16_t t;
        int_fast16_t b;
      };
    };

#if defined ( _MSVC_LANG )
#define LGFX_INLINE inline
#else
#define LGFX_INLINE __attribute__ ((always_inline)) inline
#endif

    LGFX_INLINE int_fast16_t width(void) const { return right - left + 1; }
    LGFX_INLINE int_fast16_t height(void) const { return bottom - top + 1; }
    LGFX_INLINE bool empty(void) const { return left > right || top > bottom; }
    LGFX_INLINE bool contain(int_fast16_t x, int_fast16_t y) const { return left <= x && x <= right && top <= y && y <= bottom; }
    LGFX_INLINE bool intersectsWith(const range_rect_t& rect) const { return horizon.intersectsWith(rect.horizon) && vertical.intersectsWith(rect.vertical); }

#undef LGFX_INLINE
  };
#pragma pack(pop)

//----------------------------------------------------------------------------
 }
}


