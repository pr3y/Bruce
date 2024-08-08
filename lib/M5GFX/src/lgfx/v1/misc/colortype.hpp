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
#include <type_traits>

#include "../../utility/pgmspace.h"


#include "enum.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

#if defined ( _MSVC_LANG )
 #define LGFX_INLINE inline static
#else
 #define LGFX_INLINE __attribute__ ((always_inline)) inline static
#endif

  LGFX_INLINE constexpr uint8_t  color332(uint8_t r, uint8_t g, uint8_t b) { return ((((r >> 5) << 3) + (g >> 5)) << 2) + (b >> 6); }
  LGFX_INLINE constexpr uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return (r >> 3) <<11 | (g >> 2) << 5 | b >> 3; }
  LGFX_INLINE constexpr uint32_t color888(uint8_t r, uint8_t g, uint8_t b) { return  r << 16 | g << 8 | b; }
  LGFX_INLINE constexpr uint16_t swap565( uint8_t r, uint8_t g, uint8_t b) { return (((r >> 3) << 3) + (g >> 5)) | (((g >> 2) << 5) | (b >> 3)) << 8; }
  LGFX_INLINE constexpr uint32_t swap888( uint8_t r, uint8_t g, uint8_t b) { return b << 16 | g << 8 | r; }

  LGFX_INLINE constexpr uint32_t no_convert(uint32_t c) { return c; }
  LGFX_INLINE constexpr uint32_t convert_uint32_to_palette8(uint32_t c) { return  c & 0xFF; }
  LGFX_INLINE constexpr uint32_t convert_uint32_to_palette4(uint32_t c) { return (c & 0x0F) * 0x11; }
  LGFX_INLINE constexpr uint32_t convert_uint32_to_palette2(uint32_t c) { return (c & 0x03) * 0x55; }
  LGFX_INLINE constexpr uint32_t convert_uint32_to_palette1(uint32_t c) { return (c & 1) ? 0xFF : 0; }

  LGFX_INLINE constexpr uint32_t getSwap16(uint32_t c) { return (uint16_t)((c << 8) + (c >> 8)); }
  LGFX_INLINE constexpr uint32_t getSwap24(uint32_t c) { return ((((c & 0xFF) << 8) + ((c >> 8) & 0xFF)) << 8) + ((c>>16) & 0xFF); }
  LGFX_INLINE           uint32_t getSwap32(uint32_t c) { c = (c >> 16) + (c << 16); return ((c >> 8) & 0xFF00FF) + ((c & 0xFF00FF) << 8); }

#undef LGFX_INLINE


#pragma pack(push)
#pragma pack(1)
  struct rgb332_t;    //  8bpp
  struct rgb565_t;    // 16bpp
  struct rgb888_t;    // 24bpp
  struct argb8888_t;  // 32bpp
  struct swap565_t;   // 16bpp
  struct bgr666_t;    // 18bpp (24bpp __RRRRRR__GGGGGG__BBBBBB (for OLED SSD1351)
  struct bgr888_t;    // 24bpp
  struct bgra8888_t;  // 32bpp
  struct grayscale_t; //  8bpp grayscale

  struct rgb332_t
  {
    union
    {
      struct
      {
        uint8_t b2: 2;
        uint8_t g3: 3;
        uint8_t r3: 3;
      };
      uint8_t raw;
    };
    static constexpr uint8_t bits = 8;
    static constexpr bool swapped = true;
    static constexpr color_depth_t depth = rgb332_1Byte;
    constexpr rgb332_t(void) : raw(0) {}
    constexpr rgb332_t(const rgb332_t&) = default;
    constexpr rgb332_t(uint8_t r8, uint8_t g8, uint8_t b8) : raw(color332(r8, g8, b8)) {}
    constexpr rgb332_t(uint8_t rgb332) : raw(rgb332) {}
    inline rgb332_t& operator=(uint8_t rgb332) { *reinterpret_cast<uint8_t*>(this) = rgb332; return *this; }
    inline rgb332_t& operator=(const rgb332_t&   ) = default;
    inline rgb332_t& operator=(const rgb565_t&   );
    inline rgb332_t& operator=(const rgb888_t&   );
    inline rgb332_t& operator=(const argb8888_t& );
    inline rgb332_t& operator=(const swap565_t&  );
    inline rgb332_t& operator=(const bgr666_t&   );
    inline rgb332_t& operator=(const bgr888_t&   );
    inline rgb332_t& operator=(const bgra8888_t& );
    inline rgb332_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_byte(this); }
    explicit inline operator uint8_t(void) const { return pgm_read_byte(this); }
    explicit inline operator bool(void) const { return pgm_read_byte(this); }
    static constexpr uint8_t A8(void) { return 255; }
    inline constexpr uint8_t R8(void) const { return  (((r3 << 3) + r3) << 2) + (r3 >> 1); }
    inline constexpr uint8_t G8(void) const { return  (((g3 << 3) + g3) << 2) + (g3 >> 1); }
    inline constexpr uint8_t B8(void) const { return  b2 * 0x55; }       // (b<<6)|(b<<4)|(b<<2)|b;
    inline constexpr uint8_t R6(void) const { return  (r3 << 3) + r3; }
    inline constexpr uint8_t G6(void) const { return  (g3 << 3) + g3; }
    inline constexpr uint8_t B6(void) const { return  b2 * 0x15; }
    inline void R8(uint8_t r8) { r3 = r8 >> 5; }
    inline void G8(uint8_t g8) { g3 = g8 >> 5; }
    inline void B8(uint8_t b8) { b2 = b8 >> 6; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { *reinterpret_cast<uint8_t*>(this) = color332(r8, g8, b8); }
    inline void set(uint_fast8_t c) { *reinterpret_cast<uint8_t*>(this) = c; }
    inline uint_fast8_t get(void) const { return pgm_read_byte(this); }
  };

  struct rgb565_t
  {
    union
    {
      struct
      {
        uint16_t b5: 5;
        uint16_t g6: 6;
        uint16_t r5: 5;
      };
      uint16_t raw;
    };
    static constexpr uint8_t bits = 16;
    static constexpr bool swapped = false;
    static constexpr color_depth_t depth = rgb565_nonswapped;
    constexpr rgb565_t(void) : raw(0) {}
    constexpr rgb565_t(const rgb565_t&) = default;
    constexpr rgb565_t(uint8_t r8, uint8_t g8, uint8_t b8) : raw(color565(r8, g8, b8)) {}
    constexpr rgb565_t(uint16_t rgb565) : raw(rgb565) {}
    inline rgb565_t& operator=(uint16_t rgb565) { *reinterpret_cast<uint16_t*>(this) = rgb565; return *this; }
    inline rgb565_t& operator=(const rgb332_t&   );
    inline rgb565_t& operator=(const rgb565_t&   ) = default;
    inline rgb565_t& operator=(const rgb888_t&   );
    inline rgb565_t& operator=(const argb8888_t& );
    inline rgb565_t& operator=(const swap565_t&  );
    inline rgb565_t& operator=(const bgr666_t&   );
    inline rgb565_t& operator=(const bgr888_t&   );
    inline rgb565_t& operator=(const bgra8888_t& );
    inline rgb565_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_word(this); }
    explicit inline operator uint16_t(void) const { return pgm_read_word(this); }
    explicit inline operator bool(void) const { return pgm_read_word(this); }
    static constexpr uint8_t A8(void) { return 255; }
    inline constexpr uint8_t R8(void) const { return (r5 << 3) + (r5 >> 2); }
    inline constexpr uint8_t G8(void) const { return (g6 << 2) + (g6 >> 4); }
    inline constexpr uint8_t B8(void) const { return (b5 << 3) + (b5 >> 2); }
    inline constexpr uint8_t R6(void) const { return (r5 << 1) + (r5 >> 4); }
    inline constexpr uint8_t G6(void) const { return  g6; }
    inline constexpr uint8_t B6(void) const { return (b5 << 1) + (b5 >> 4); }
    inline void R8(uint8_t r8) { r5 = r8 >> 3; }
    inline void G8(uint8_t g8) { g6 = g8 >> 2; }
    inline void B8(uint8_t b8) { b5 = b8 >> 3; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { *reinterpret_cast<uint16_t*>(this) = color565(r8, g8, b8); }
    inline void set(uint_fast16_t c) { *reinterpret_cast<uint16_t*>(this) = c; }
    inline uint_fast16_t get(void) const { return pgm_read_word(this); }
  };

  struct rgb888_t
  {
    union
    {
      struct
      {
        uint8_t b;
        uint8_t g;
        uint8_t r;
      };
      uint16_t bg;
    };
    static constexpr uint8_t bits = 24;
    static constexpr bool swapped = false;
    static constexpr color_depth_t depth = rgb888_nonswapped;
    constexpr rgb888_t(void) : b(0), g(0), r(0) {}
    constexpr rgb888_t(const rgb888_t&) = default;
    constexpr rgb888_t(uint8_t r8, uint8_t g8, uint8_t b8) : b(b8), g(g8), r(r8) {}
    constexpr rgb888_t(uint32_t rgb888) : b(rgb888), g(rgb888>>8), r(rgb888>>16) {}
    inline rgb888_t& operator=(uint32_t rgb888) { r = rgb888>>16; g = rgb888>>8; b = rgb888; return *this; }
    inline rgb888_t& operator=(const rgb332_t&   );
    inline rgb888_t& operator=(const rgb565_t&   );
    inline rgb888_t& operator=(const rgb888_t&   ) = default;
    inline rgb888_t& operator=(const argb8888_t& );
    inline rgb888_t& operator=(const swap565_t&  );
    inline rgb888_t& operator=(const bgr666_t&   );
    inline rgb888_t& operator=(const bgr888_t&   );
    inline rgb888_t& operator=(const bgra8888_t& );
    inline rgb888_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_3byte_unaligned(this); }
    explicit inline operator bool(void) const { return pgm_read_3byte_unaligned(this); }
    static constexpr uint8_t A8(void) { return 255; }
    inline constexpr uint8_t R8(void) const { return r; }
    inline constexpr uint8_t G8(void) const { return g; }
    inline constexpr uint8_t B8(void) const { return b; }
    inline constexpr uint8_t R6(void) const { return r >> 2; }
    inline constexpr uint8_t G6(void) const { return g >> 2; }
    inline constexpr uint8_t B6(void) const { return b >> 2; }
    inline void R8(uint8_t r8) { r = r8; }
    inline void G8(uint8_t g8) { g = g8; }
    inline void B8(uint8_t b8) { b = b8; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { r = r8; g = g8; b = b8; }
    inline void set(uint_fast32_t c) { write_3byte_unaligned(this, c); }
    inline uint_fast32_t get(void) const { return pgm_read_3byte_unaligned(this); }
  };

  struct argb8888_t
  {
    union
    {
      struct
      {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
      };
      uint32_t raw;
    };
    static constexpr uint8_t bits = 32;
    static constexpr bool swapped = false;
    static constexpr color_depth_t depth = argb8888_nonswapped;
    constexpr argb8888_t(void) : raw(0) {}
    constexpr argb8888_t(const argb8888_t&) = default;
    constexpr argb8888_t(uint8_t r8, uint8_t g8, uint8_t b8) : b(b8), g(g8), r(r8), a(255) {}
    constexpr argb8888_t(uint8_t a8, uint8_t r8, uint8_t g8, uint8_t b8) : b(b8), g(g8), r(r8), a(a8) {}
    constexpr argb8888_t(uint32_t argb8888) : raw(argb8888) {}
    inline argb8888_t& operator=(uint32_t argb8888) { *reinterpret_cast<uint32_t*>(this) = argb8888; return *this; }
    inline argb8888_t& operator=(const rgb332_t&   );
    inline argb8888_t& operator=(const rgb565_t&   );
    inline argb8888_t& operator=(const rgb888_t&   );
    inline argb8888_t& operator=(const argb8888_t& ) = default;
    inline argb8888_t& operator=(const swap565_t&  );
    inline argb8888_t& operator=(const bgr666_t&   );
    inline argb8888_t& operator=(const bgr888_t&   );
    inline argb8888_t& operator=(const bgra8888_t& );
    inline argb8888_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_dword(this); }
    explicit inline operator bool(void) const { return pgm_read_dword(this); }
    inline constexpr uint8_t A8(void) const { return a; }
    inline constexpr uint8_t R8(void) const { return r; }
    inline constexpr uint8_t G8(void) const { return g; }
    inline constexpr uint8_t B8(void) const { return b; }
    inline constexpr uint8_t R6(void) const { return r >> 2; }
    inline constexpr uint8_t G6(void) const { return g >> 2; }
    inline constexpr uint8_t B6(void) const { return b >> 2; }
    inline void A8(uint8_t a8) { a = a8; }
    inline void R8(uint8_t r8) { r = r8; }
    inline void G8(uint8_t g8) { g = g8; }
    inline void B8(uint8_t b8) { b = b8; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { a = 255; r = r8; g = g8; b = b8; }
    inline void set(uint8_t a8, uint8_t r8, uint8_t g8, uint8_t b8) { a = a8; r = r8; g = g8; b = b8; }
    inline void set(uint_fast32_t c) { *reinterpret_cast<uint32_t*>(this) = c; }
    inline uint_fast32_t get(void) const { return pgm_read_dword(this); }
  };

  struct swap565_t
  {
    union
    {
      struct
      {
        uint16_t gh:3;
        uint16_t r5:5;
        uint16_t b5:5;
        uint16_t gl:3;
      };
      uint16_t raw;
    };
    static constexpr uint8_t bits = 16;
    static constexpr bool swapped = true;
    static constexpr color_depth_t depth = rgb565_2Byte;
    constexpr swap565_t(void) : raw(0) {}
    constexpr swap565_t(const swap565_t&) = default;
    constexpr swap565_t(uint8_t r8, uint8_t g8, uint8_t b8) : raw(swap565(r8, g8, b8)) {}
    constexpr swap565_t(uint16_t swap565) : raw(swap565) {}
    inline swap565_t& operator=(uint16_t swap565) { *reinterpret_cast<uint16_t*>(this) = swap565; return *this; }
    inline swap565_t& operator=(const rgb332_t&   );
    inline swap565_t& operator=(const rgb565_t&   );
    inline swap565_t& operator=(const rgb888_t&   );
    inline swap565_t& operator=(const argb8888_t& );
    inline swap565_t& operator=(const swap565_t&  ) = default;
    inline swap565_t& operator=(const bgr666_t&   );
    inline swap565_t& operator=(const bgr888_t&   );
    inline swap565_t& operator=(const bgra8888_t& );
    inline swap565_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_word(this); }
    explicit inline operator uint16_t(void) const { return pgm_read_word(this); }
    explicit inline operator bool(void) const { return pgm_read_word(this); }
    static constexpr uint8_t A8(void) { return 255; }
    inline constexpr uint8_t R8(void) const { return (r5 << 3) + (r5 >> 2); }
    inline constexpr uint8_t G8(void) const { return (((gh << 3) + gl) << 2) + (gh >> 1); }
    inline constexpr uint8_t B8(void) const { return (b5 << 3) + (b5 >> 2); }
    inline constexpr uint8_t R6(void) const { return (r5 << 1) + (r5 >> 4); }
    inline constexpr uint8_t G6(void) const { return (gh << 3) + gl; }
    inline constexpr uint8_t B6(void) const { return (b5 << 1) + (b5 >> 4); }
    inline void R8(uint8_t r8) { r5 = r8 >> 3; }
    inline void G8(uint8_t g8) { gh = g8 >> 5; gl = g8 >> 2;}
    inline void B8(uint8_t b8) { b5 = b8 >> 3; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { *reinterpret_cast<uint16_t*>(this) = swap565(r8, g8, b8); }
    inline void set(uint_fast16_t c) { *reinterpret_cast<uint16_t*>(this) = c; }
    inline uint_fast16_t get(void) const { return pgm_read_word(this); }
  };

  struct bgr666_t
  {
    union
    {
      struct
      {
        uint8_t r6;
        uint8_t g6;
        uint8_t b6;
      };
      uint16_t rg;
    };
    static constexpr uint8_t bits = 24;
    static constexpr bool swapped = true;
    static constexpr color_depth_t depth = rgb666_3Byte;
    constexpr bgr666_t(void) : r6(0), g6(0), b6(0) {};
    constexpr bgr666_t(const bgr666_t&) = default;
    constexpr bgr666_t(uint8_t r8, uint8_t g8, uint8_t b8) : r6(r8>>2), g6(g8>>2), b6(b8>>2) {}
    constexpr bgr666_t(uint32_t raw) : r6(raw), g6(raw>>8), b6(raw>>16) {}
    inline bgr666_t& operator=(uint32_t bgr666) { r6 = bgr666; g6 = bgr666 >> 8 ; b6 = bgr666 >> 16; return *this; }
    inline bgr666_t& operator=(const rgb332_t&   );
    inline bgr666_t& operator=(const rgb565_t&   );
    inline bgr666_t& operator=(const rgb888_t&   );
    inline bgr666_t& operator=(const argb8888_t& );
    inline bgr666_t& operator=(const swap565_t&  );
    inline bgr666_t& operator=(const bgr666_t&   ) = default;
    inline bgr666_t& operator=(const bgr888_t&   );
    inline bgr666_t& operator=(const bgra8888_t& );
    inline bgr666_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_3byte_unaligned(this); }
    explicit inline operator bool(void) const { return pgm_read_3byte_unaligned(this); }
    static constexpr uint8_t A8(void) { return 255; }
    inline constexpr uint8_t R8(void) const { return (r6 << 2) + (r6 >> 4); }
    inline constexpr uint8_t G8(void) const { return (g6 << 2) + (g6 >> 4); }
    inline constexpr uint8_t B8(void) const { return (b6 << 2) + (b6 >> 4); }
    inline constexpr uint8_t R6(void) const { return r6; }
    inline constexpr uint8_t G6(void) const { return g6; }
    inline constexpr uint8_t B6(void) const { return b6; }
    inline void R8(uint8_t r8) { r6 = r8 >> 2; }
    inline void G8(uint8_t g8) { g6 = g8 >> 2; }
    inline void B8(uint8_t b8) { b6 = b8 >> 2; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { r6 = r8>>2; g6 = g8>>2; b6 = b8>>2; }
    inline void set(uint_fast32_t c) { write_3byte_unaligned(this, c); }
    inline uint_fast32_t get(void) const { return pgm_read_3byte_unaligned(this); }
  };

  struct bgr888_t
  {
    union
    {
      struct
      {
        uint8_t r;
        uint8_t g;
        uint8_t b;
      };
      uint16_t rg;
    };
    static constexpr uint8_t bits = 24;
    static constexpr bool swapped = true;
    static constexpr color_depth_t depth = rgb888_3Byte;
    constexpr bgr888_t(void) : r{0}, g{0}, b{0} {};
    constexpr bgr888_t(const bgr888_t&) = default;
    constexpr bgr888_t(uint8_t r8, uint8_t g8, uint8_t b8) : r(r8), g(g8), b(b8) {}
    constexpr bgr888_t(uint32_t bgr888) : r(bgr888), g(bgr888>>8), b(bgr888>>16) {}
    inline bgr888_t& operator=(uint32_t bgr888) { r = bgr888; g = bgr888>>8 ; b = bgr888>>16; return *this; }
    inline bgr888_t& operator=(const rgb332_t&   );
    inline bgr888_t& operator=(const rgb565_t&   );
    inline bgr888_t& operator=(const rgb888_t&   );
    inline bgr888_t& operator=(const argb8888_t& );
    inline bgr888_t& operator=(const swap565_t&  );
    inline bgr888_t& operator=(const bgr666_t&   );
    inline bgr888_t& operator=(const bgr888_t&   ) = default;
    inline bgr888_t& operator=(const bgra8888_t& );
    inline bgr888_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_3byte_unaligned(this); }
    explicit inline operator bool(void) const { return pgm_read_3byte_unaligned(this); }
    static constexpr uint8_t A8() { return 255; }
    inline constexpr uint8_t R8() const { return r; }
    inline constexpr uint8_t G8() const { return g; }
    inline constexpr uint8_t B8() const { return b; }
    inline constexpr uint8_t R6() const { return r >> 2; }
    inline constexpr uint8_t G6() const { return g >> 2; }
    inline constexpr uint8_t B6() const { return b >> 2; }
    inline constexpr uint32_t RGB888(void) const { return (r<<16)+(g<<8)+b; }
    inline void R8(uint8_t r8) { r = r8; }
    inline void G8(uint8_t g8) { g = g8; }
    inline void B8(uint8_t b8) { b = b8; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { r = r8; g = g8; b = b8; }
    inline void set(uint_fast32_t c) { write_3byte_unaligned(this, c); }
    inline uint_fast32_t get(void) const { return pgm_read_3byte_unaligned(this); }
  };

  struct bgra8888_t
  {
    union
    {
      struct
      {
        uint8_t a;
        uint8_t r;
        uint8_t g;
        uint8_t b;
      };
      uint32_t raw;
    };
    static constexpr uint8_t bits = 32;
    static constexpr bool swapped = true;
    static constexpr color_depth_t depth = argb8888_4Byte;
    constexpr bgra8888_t(void) : raw{0} {};
    constexpr bgra8888_t(const bgra8888_t&) = default;
    constexpr bgra8888_t(uint8_t r8, uint8_t g8, uint8_t b8) : a(255), r(r8), g(g8), b(b8) {}
    constexpr bgra8888_t(uint8_t a8, uint8_t r8, uint8_t g8, uint8_t b8) : a(a8), r(r8), g(g8), b(b8) {}
    constexpr bgra8888_t(uint32_t bgra8888) : raw(bgra8888) {}
    inline bgra8888_t& operator=(uint32_t rhs) { *reinterpret_cast<uint32_t*>(this) = rhs; return *this; }
    inline bgra8888_t& operator=(const rgb332_t&   );
    inline bgra8888_t& operator=(const rgb565_t&   );
    inline bgra8888_t& operator=(const rgb888_t&   );
    inline bgra8888_t& operator=(const argb8888_t& );
    inline bgra8888_t& operator=(const swap565_t&  );
    inline bgra8888_t& operator=(const bgr666_t&   );
    inline bgra8888_t& operator=(const bgr888_t&   );
    inline bgra8888_t& operator=(const bgra8888_t& ) = default;
    inline bgra8888_t& operator=(const grayscale_t&);
    explicit inline operator uint32_t(void) const { return pgm_read_dword(this); }
    explicit inline operator bool(void) const { return pgm_read_dword(this); }
    inline constexpr uint8_t A8(void) const { return a; }
    inline constexpr uint8_t R8(void) const { return r; }
    inline constexpr uint8_t G8(void) const { return g; }
    inline constexpr uint8_t B8(void) const { return b; }
    inline constexpr uint8_t R6(void) const { return r >> 2; }
    inline constexpr uint8_t G6(void) const { return g >> 2; }
    inline constexpr uint8_t B6(void) const { return b >> 2; }
    inline void A8(uint8_t a8) { a = a8; }
    inline void R8(uint8_t r8) { r = r8; }
    inline void G8(uint8_t g8) { g = g8; }
    inline void B8(uint8_t b8) { b = b8; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { a = 255; r = r8; g = g8; b = b8; }
    inline void set(uint8_t a8, uint8_t r8, uint8_t g8, uint8_t b8) { a = a8; r = r8; g = g8; b = b8; }
    inline void set(uint_fast32_t c) { *reinterpret_cast<uint32_t*>(this) = c; }
    inline uint_fast32_t get(void) const { return pgm_read_dword(this); }
  };

  struct grayscale_t
  {
    union
    {
      uint8_t raw;
      uint8_t r;
      uint8_t g;
      uint8_t b;
    };
    static constexpr uint8_t bits = 8;
    static constexpr bool swapped = true;
    static constexpr color_depth_t depth = grayscale_8bit;
    constexpr grayscale_t(void) : raw{0} {};
    constexpr grayscale_t(const grayscale_t&) = default;
    constexpr grayscale_t(uint8_t r8, uint8_t g8, uint8_t b8) : raw((r8 + (g8 << 1) + b8) >> 2) {}
    constexpr grayscale_t(uint8_t gray8) : raw(gray8) {}
    inline grayscale_t& operator=(uint8_t gray8) { *reinterpret_cast<uint8_t*>(this) = gray8; return *this; }
    inline grayscale_t& operator=(const rgb332_t&   );
    inline grayscale_t& operator=(const rgb565_t&   );
    inline grayscale_t& operator=(const rgb888_t&   );
    inline grayscale_t& operator=(const argb8888_t& );
    inline grayscale_t& operator=(const swap565_t&  );
    inline grayscale_t& operator=(const bgr666_t&   );
    inline grayscale_t& operator=(const bgr888_t&   );
    inline grayscale_t& operator=(const bgra8888_t& );
    inline grayscale_t& operator=(const grayscale_t&) = default;
    explicit inline operator uint32_t(void) const { return pgm_read_byte(this); }
    explicit inline operator uint8_t(void) const { return pgm_read_byte(this); }
    explicit inline operator bool(void) const { return pgm_read_byte(this); }
    static constexpr uint8_t A8(void) { return 255; }
    inline constexpr uint8_t R8(void) const { return r; }
    inline constexpr uint8_t G8(void) const { return g; }
    inline constexpr uint8_t B8(void) const { return b; }
    inline constexpr uint8_t R6(void) const { return r >> 2; }
    inline constexpr uint8_t G6(void) const { return g >> 2; }
    inline constexpr uint8_t B6(void) const { return b >> 2; }
    inline void R8(uint8_t r8) { r = r8; }
    inline void G8(uint8_t g8) { g = g8; }
    inline void B8(uint8_t b8) { b = b8; }
    inline void set(uint8_t r8, uint8_t g8, uint8_t b8) { raw = (r8 + (g8 << 1) + b8) >> 2; }
    inline void set(uint_fast32_t c) { *reinterpret_cast<uint8_t*>(this) = c; }
    inline uint_fast8_t get(void) const { return pgm_read_byte(this); }
  };

  struct raw_color_t
  {
    union
    {
      struct
      {
        uint8_t raw0;
        uint8_t raw1;
        uint8_t raw2;
        uint8_t raw3;
      };
      struct
      {
        uint16_t rawL;
        uint16_t rawH;
      };
      uint32_t raw;
    };
    constexpr raw_color_t() : raw(0) {}
    constexpr raw_color_t(const raw_color_t&) = default;
    constexpr raw_color_t(const uint32_t& rhs) : raw(rhs) {}
  };

#pragma pack(pop)

#if defined ( _MSVC_LANG )
#define LGFX_INLINE inline
#else
#define LGFX_INLINE __attribute__ ((always_inline)) inline
#endif

  template<class TDst, class TSrc> LGFX_INLINE uint32_t color_convert(uint32_t c) { return c; }

  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , rgb565_t   >(uint32_t c) { return (((((c >>13) & 7) << 3) + ((c >> 8) & 7)) << 2) + ((c >> 3) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , rgb888_t   >(uint32_t c) { return (((((c >>21) & 7) << 3) + ((c >>13) & 7)) << 2) + ((c >> 6) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , argb8888_t >(uint32_t c) { return (((((c >>21) & 7) << 3) + ((c >>13) & 7)) << 2) + ((c >> 6) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , swap565_t  >(uint32_t c) { return (((((c >> 5) & 7) << 3) + ( c       & 7)) << 2) + ((c >>11) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , bgr666_t   >(uint32_t c) { return (((((c >> 3) & 7) << 3) + ((c >>11) & 7)) << 2) + ((c >>20) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , bgr888_t   >(uint32_t c) { return (((((c >> 5) & 7) << 3) + ((c >>13) & 7)) << 2) + ((c >>22) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , bgra8888_t >(uint32_t c) { return (((((c >>13) & 7) << 3) + ((c >>21) & 7)) << 2) + ((c >>30) & 3); }
  template<> LGFX_INLINE uint32_t color_convert<rgb332_t   , grayscale_t>(uint32_t c) { return ((c>>5)*0x49)>>1; }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c >> 5) & 0x07; r = (r << 2) + (r >> 1); uint_fast8_t g = (c >> 2) & 0x07; g = (g << 3) + g; uint_fast8_t b = c & 0x03; b = (((b << 2) + b) << 1) + (b >> 1); return (((r<<6)+g)<<5)+b; }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , rgb888_t   >(uint32_t c) { return (((((c >>19) & 0x1F) << 6) + ((c >>10) & 0x3F)) << 5) + ((c >> 3) & 0x1F); }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , argb8888_t >(uint32_t c) { return (((((c >>19) & 0x1F) << 6) + ((c >>10) & 0x3F)) << 5) + ((c >> 3) & 0x1F); }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , swap565_t  >(uint32_t c) { return getSwap16(c); }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , bgr666_t   >(uint32_t c) { return (((((c >> 1) & 0x1F) << 6) + ((c >> 8) & 0x3F)) << 5) + (c >> 17); }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , bgr888_t   >(uint32_t c) { return (((((c >> 3) & 0x1F) << 6) + ((c >>10) & 0x3F)) << 5) + (c >> 19); }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , bgra8888_t >(uint32_t c) { return (((((c >>11) & 0x1F) << 6) + ((c >>18) & 0x3F)) << 5) +((c >> 27) & 0x1F); }
  template<> LGFX_INLINE uint32_t color_convert<rgb565_t   , grayscale_t>(uint32_t c) { uint_fast8_t r = c >> 3; return ((c&0xFC) << 3) + (r | r << 11); }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c >>  5) & 0x07; r = (((r<<3)+r)<<2)+(r>>1); uint_fast8_t g = c & 0x1C; g = (g << 3) + g + (g >> 3); uint_fast8_t b = (c & 0x03); b = (b << 2) + b; b = (b << 4) + b; return (((r<<8)+g)<<8)+b; }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , rgb565_t   >(uint32_t c) { uint_fast8_t r = (c >> 11) & 0x1F; r = (r << 3) + (r >> 2); uint_fast8_t g = (c >> 5) & 0x3F; g = (g << 2) + (g >> 4); uint_fast8_t b = c & 0x1F; b = (b << 3) + (b >> 2); return (((r<<8)+g)<<8)+b; }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , argb8888_t >(uint32_t c) { return (c << 8) >> 8; }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , swap565_t  >(uint32_t c) { uint_fast16_t g = (c & 7); uint_fast16_t b = (c >> 8) & 0x1F;  b = (b << 3) + (b >> 2); uint_fast16_t r = (c >> 3) & 0x1F;  r = (r << 3) + (r >> 2); return (((((((r << 3) + g) << 3) + (c >> 13)) << 2) + (g >> 1)) << 8) + b; }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , bgr666_t   >(uint32_t c) { return getSwap24((c << 2) + ((c >> 4) & 0x030303)); }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , bgr888_t   >(uint32_t c) { return getSwap24(c); }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , bgra8888_t >(uint32_t c) { return ((c>>8) & 0xFF) << 16 | ((c>>16)&0xFF)<<8 | (c>>24); }
  template<> LGFX_INLINE uint32_t color_convert<rgb888_t   , grayscale_t>(uint32_t c) { return c*0x010101; }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c >>  5) & 0x07; r = (((r<<3)+r)<<2)+(r>>1); uint_fast8_t g = c & 0x1C; g = (g << 3) + g + (g >> 3); uint_fast8_t b = (c & 0x03); b = (b << 2) + b; b = (b << 4) + b; return (((((0xFF<<8)+r)<<8)+g)<<8)+b; }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , rgb565_t   >(uint32_t c) { uint_fast8_t r = (c >> 11) & 0x1F; r = (r << 3) + (r >> 2); uint_fast8_t g = (c >> 5) & 0x3F; g = (g << 2) + (g >> 4); uint_fast8_t b = c & 0x1F; b = (b << 3) + (b >> 2); return (((((0xFF<<8)+r)<<8)+g)<<8)+b; }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , rgb888_t   >(uint32_t c) { return c | (0xFF << 24); }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , swap565_t  >(uint32_t c) { uint_fast16_t g = (c & 7); uint_fast16_t b = (c >> 8) & 0x1F;  b = (b << 3) + (b >> 2); uint_fast16_t r = (c >> 3) & 0x1F;  r = (r << 3) + (r >> 2); return (((((((((0xFF << 8) + r) << 3) + g) << 3) + (c >> 13)) << 2) + (g >> 1)) << 8) + b; }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , bgr666_t   >(uint32_t c) { c = ((((c | 0x3FC0)<<8) +  ((c>>8)&0x3F))<<8) + ((c>>16)&0x3F); return (c << 2) + ((c >> 4) & 0x030303); }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , bgr888_t   >(uint32_t c) { return (c | 0xFF00)<<16 | (((c>>8)&0xFF))<<8  | ((c>>16)&0xFF); }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , bgra8888_t >(uint32_t c) { return getSwap32(c); }
  template<> LGFX_INLINE uint32_t color_convert<argb8888_t , grayscale_t>(uint32_t c) { return (c * 0x010101) | (0xFF<<24); }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c >> 5) & 0x07; r = (r << 2) + (r >> 1); uint_fast8_t g = (c >> 2) & 0x07; uint_fast8_t b = c & 0x03; b = (((b << 2) + b) << 1) + (b >> 1); return (((((g<<5)+b)<<5)+r)<<3)+g; }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , rgb565_t   >(uint32_t c) { return getSwap16(c); }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , rgb888_t   >(uint32_t c) { uint_fast8_t r = (c >> 19) & 0x1F; uint_fast8_t gh = (c >> 13) & 0x07; uint_fast8_t gl = (c >> 10) & 0x07; uint_fast8_t b = (c >>  3) & 0x1F; return (((((gl << 5) + b) << 5) + r) << 3) + gh; }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , argb8888_t >(uint32_t c) { uint_fast8_t r = (c >> 19) & 0x1F; uint_fast8_t gh = (c >> 13) & 0x07; uint_fast8_t gl = (c >> 10) & 0x07; uint_fast8_t b = (c >>  3) & 0x1F; return (((((gl << 5) + b) << 5) + r) << 3) + gh; }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , bgr666_t   >(uint32_t c) { uint_fast8_t r = (c >>  1) & 0x1F; uint_fast8_t gh = (c >> 11) & 0x07; uint_fast8_t gl = (c >>  8) & 0x07; uint_fast8_t b = (c >> 17) & 0x1F; return (((((gl << 5) + b) << 5) + r) << 3) + gh; }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , bgr888_t   >(uint32_t c) { uint_fast8_t r = (c >>  3) & 0x1F; uint_fast8_t gh = (c >> 13) & 0x07; uint_fast8_t gl = (c >> 10) & 0x07; uint_fast8_t b = (c >> 19) & 0x1F; return (((((gl << 5) + b) << 5) + r) << 3) + gh; }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , bgra8888_t >(uint32_t c) { uint_fast8_t r = (c >> 11) & 0x1F; uint_fast8_t gh = (c >> 21) & 0x07; uint_fast8_t gl = (c >> 18) & 0x07; uint_fast8_t b = (c >> 27) & 0x1F; return (((((gl << 5) + b) << 5) + r) << 3) + gh; }
  template<> LGFX_INLINE uint32_t color_convert<swap565_t  , grayscale_t>(uint32_t c) { uint_fast8_t rb = c >> 3; return ((((((c & 0x1C) << 3) + rb) << 5) + rb) << 3) + (c >> 5); }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c >>  5) & 0x07; r = (r << 3) +  r;       uint_fast8_t g = (c >> 2) & 0x07; g = (g << 3) + g; uint_fast8_t b = (c & 3) * 0x15; return (((b<<8)+g)<<8)+r; }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , rgb565_t   >(uint32_t c) { uint_fast8_t r = (c >> 11) & 0x1F; r = (r << 1) + (r >> 4); uint_fast8_t g = (c >> 5) & 0x3F; uint_fast8_t b = c & 0x1F; b = (b << 1) + (b >> 4); return (((b<<8)+g)<<8)+r; }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , rgb888_t   >(uint32_t c) { return ((c >> 2) & 0x3F) << 16 | ((c >> 10) & 0x3F) << 8 | ((c >> 18) & 0x3F);  }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , argb8888_t >(uint32_t c) { return ((c >> 2) & 0x3F) << 16 | ((c >> 10) & 0x3F) << 8 | ((c >> 18) & 0x3F);  }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , swap565_t  >(uint32_t c) { uint_fast16_t g = (c & 7); g = (g << 3) + (c >> 13); uint_fast16_t b = (c >> 8) & 0x1F;  b = (b << 1) + (b >> 4); uint_fast16_t r = (c >> 3) & 0x1F;  r = (r << 1) + (r >> 4); return (((b<<8)+g)<<8)+r; }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , bgr888_t   >(uint32_t c) { return (c >>  2) & 0x3F3F3F; }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , bgra8888_t >(uint32_t c) { return (c >> 10) & 0x3F3F3F; }
  template<> LGFX_INLINE uint32_t color_convert<bgr666_t   , grayscale_t>(uint32_t c) { return (c >>  2) * 0x010101; }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c >>  5) & 0x07; r = (((r<<3)+r)<<2)+(r>>1); uint_fast8_t g = c & 0x1C; g = (g << 3) + g + (g >> 3); uint_fast8_t b = (c & 0x03) * 0x55; return (((b<<8)+g)<<8)+r; }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , rgb565_t   >(uint32_t c) { uint_fast8_t r = (c >> 11) & 0x1F; r = (r << 3) + (r >> 2); uint_fast8_t g = (c >> 5) & 0x3F; g = (g << 2) + (g >> 4); uint_fast8_t b = c & 0x1F; b = (b << 3) + (b >> 2); return (((b<<8)+g)<<8)+r; }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , rgb888_t   >(uint32_t c) { return getSwap24(c); }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , argb8888_t >(uint32_t c) { return getSwap24(c); }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , swap565_t  >(uint32_t c) { uint_fast16_t g = (c & 7); uint_fast16_t b = (c >> 8) & 0x1F;  b = (b << 3) + (b >> 2); uint_fast16_t r = (c >> 3) & 0x1F;  r = (r << 3) + (r >> 2); return (((((((b << 3) + g) << 3) + (c >> 13)) << 2) + (g >> 1)) << 8) + r; }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , bgr666_t   >(uint32_t c) { return (c << 2) + ((c >> 4) & 0x030303); }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , bgra8888_t >(uint32_t c) { return c >> 8; }
  template<> LGFX_INLINE uint32_t color_convert<bgr888_t   , grayscale_t>(uint32_t c) { return ((c << 16) + c) + (c << 8); }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , rgb332_t   >(uint32_t c) { uint_fast8_t r = (c>> 5)*0x49; uint_fast8_t g = c & 0x1C; g = (g << 3) + g + (g >> 3); uint_fast8_t b = (c & 0x03) * 0x55; return (((((b<<8)+g)<<9)+r)<<7)|0xFF; }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , rgb565_t   >(uint32_t c) { uint_fast8_t r = (c >> 11) & 0x1F; r = (r << 3) + (r >> 2); uint_fast8_t g = (c >> 5) & 0x3F; g = (g << 2) + (g >> 4); uint_fast8_t b = c & 0x1F; b = (b << 3) + (b >> 2); return (((((b<<8)+g)<<8)+r)<<8)+0xFF; }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , rgb888_t   >(uint32_t c) { return (((c << 8) + ((c >> 8) & 0xFF)) << 16) + ((uint16_t)(c >> 8) | 0xFF); }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , argb8888_t >(uint32_t c) { return getSwap32(c); }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , swap565_t  >(uint32_t c) { uint_fast16_t g = (c & 7); uint_fast16_t b = (c >> 8) & 0x1F;  b = (b << 3) + (b >> 2); uint_fast16_t r = (c >> 3) & 0x1F;  r = (r << 3) + (r >> 2); return (((((((((b << 3) + g) << 3) + (c >> 13)) << 2) + (g >> 1)) << 8) + r) << 8) + 0xFF; }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , bgr666_t   >(uint32_t c) { c<<=2; return (c << 8) + ((c & 0xC0C0C0) << 2) + 0xFF; }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , bgr888_t   >(uint32_t c) { return (c << 8) + 0xFF; }
  template<> LGFX_INLINE uint32_t color_convert<bgra8888_t , grayscale_t>(uint32_t c) { return (((c << 8) + c) << 16) + (c << 8) + 0xFF; }

// ITU-R BT.601 RGB to Y convert  R 0.299 + G 0.587 + B 0.114
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, rgb332_t   >(uint32_t c) { return (((c >>  5) & 0x07) *  43 + ((c >>  2) & 0x07) *  86 +  (c        & 0x03) *  39) >> 2; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, rgb565_t   >(uint32_t c) { return (((c >> 11) & 0x1F) *  79 + ((c >>  5) & 0x3F) *  76 +  (c        & 0x1F) *  30) >> 5; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, rgb888_t   >(uint32_t c) { return (((c >> 16) & 0xFF) *  77 + ((c >>  8) & 0xFF) * 151 +  (c        & 0xFF) *  29) >> 8; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, argb8888_t >(uint32_t c) { return (((c >> 24) & 0xFF) *  77 + ((c >> 16) & 0xFF) * 151 + ((c >>  8) & 0xFF) *  29) >> 8; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, swap565_t  >(uint32_t c) { return (((c >>  3) & 0x1F) *  79+(((c<<3)+(c>>13))&0x3F)*76 + ((c >>  8) & 0x1F) *  30) >> 5; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, bgr666_t   >(uint32_t c) { return (( c        & 0x3F) *  39 + ((c >>  8) & 0x3F) *  76 + ((c >> 16) & 0x3F) *  15) >> 5; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, bgr888_t   >(uint32_t c) { return (( c        & 0xFF) *  77 + ((c >>  8) & 0xFF) * 151 + ((c >> 16) & 0xFF) *  29) >> 8; }
  template<> LGFX_INLINE uint32_t color_convert<grayscale_t, bgra8888_t >(uint32_t c) { return color_convert<grayscale_t, bgr888_t>(c>>8); }

  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const rgb565_t&    c) { set(color_convert<rgb332_t   , rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const rgb888_t&    c) { set(color_convert<rgb332_t   , rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const argb8888_t&  c) { set(color_convert<rgb332_t   , argb8888_t >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const swap565_t&   c) { set(color_convert<rgb332_t   , swap565_t  >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const bgr666_t&    c) { set(color_convert<rgb332_t   , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const bgr888_t&    c) { set(color_convert<rgb332_t   , bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const bgra8888_t&  c) { set(color_convert<rgb332_t   , bgra8888_t >(c.get())); return *this; }
  LGFX_INLINE rgb332_t&    rgb332_t   ::operator=(const grayscale_t& c) { set(color_convert<rgb332_t   , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const rgb332_t&    c) { set(color_convert<rgb565_t   , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const rgb888_t&    c) { set(color_convert<rgb565_t   , rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const argb8888_t&  c) { set(color_convert<rgb565_t   , argb8888_t >(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const swap565_t&   c) { set(getSwap16                              (c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const bgr666_t&    c) { set(color_convert<rgb565_t   , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const bgr888_t&    c) { set(color_convert<rgb565_t   , bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const bgra8888_t&  c) { set(color_convert<rgb565_t   , bgra8888_t >(c.get())); return *this; }
  LGFX_INLINE rgb565_t&    rgb565_t   ::operator=(const grayscale_t& c) { set(color_convert<rgb565_t   , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const rgb332_t&    c) { set(color_convert<rgb888_t   , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const rgb565_t&    c) { set(color_convert<rgb888_t   , rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const argb8888_t&  c) { set(color_convert<rgb888_t   , argb8888_t >(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const swap565_t&   c) { set(color_convert<rgb888_t   , swap565_t  >(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const bgr666_t&    c) { set(color_convert<rgb888_t   , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const bgr888_t&    c) { set(getSwap24                              (c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const bgra8888_t&  c) { set(color_convert<rgb888_t   , bgra8888_t >(c.get())); return *this; }
  LGFX_INLINE rgb888_t&    rgb888_t   ::operator=(const grayscale_t& c) { set(color_convert<rgb888_t   , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const rgb332_t&    c) { set(color_convert<argb8888_t , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const rgb565_t&    c) { set(color_convert<argb8888_t , rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const rgb888_t&    c) { set(color_convert<argb8888_t , rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const swap565_t&   c) { set(color_convert<argb8888_t , swap565_t  >(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const bgr666_t&    c) { set(color_convert<argb8888_t , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const bgr888_t&    c) { set(color_convert<argb8888_t , bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const bgra8888_t&  c) { set(getSwap32                              (c.get())); return *this; }
  LGFX_INLINE argb8888_t&  argb8888_t ::operator=(const grayscale_t& c) { set(color_convert<argb8888_t , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const rgb332_t&    c) { set(color_convert<swap565_t  , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const rgb565_t&    c) { set(getSwap16(                              c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const rgb888_t&    c) { set(color_convert<swap565_t  , rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const argb8888_t&  c) { set(color_convert<swap565_t  , argb8888_t >(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const bgr666_t&    c) { set(color_convert<swap565_t  , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const bgr888_t&    c) { set(color_convert<swap565_t  , bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const bgra8888_t&  c) { set(color_convert<swap565_t  , bgra8888_t >(c.get())); return *this; }
  LGFX_INLINE swap565_t&   swap565_t  ::operator=(const grayscale_t& c) { set(color_convert<swap565_t  , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const rgb332_t&    c) { set(color_convert<bgr666_t   , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const rgb565_t&    c) { set(color_convert<bgr666_t   , rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const rgb888_t&    c) { set(color_convert<bgr666_t   , rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const argb8888_t&  c) { set(color_convert<bgr666_t   , argb8888_t >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const swap565_t&   c) { set(color_convert<bgr666_t   , swap565_t  >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const bgr888_t&    c) { set(color_convert<bgr666_t   , bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const bgra8888_t&  c) { set(color_convert<bgr666_t   , bgra8888_t >(c.get())); return *this; }
  LGFX_INLINE bgr666_t&    bgr666_t   ::operator=(const grayscale_t& c) { set(color_convert<bgr666_t   , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const rgb332_t&    c) { set(color_convert<bgr888_t   , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const rgb565_t&    c) { set(color_convert<bgr888_t   , rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const rgb888_t&    c) { set(getSwap24                              (c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const argb8888_t&  c) { set(color_convert<bgr888_t   , argb8888_t >(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const swap565_t&   c) { set(color_convert<bgr888_t   , swap565_t  >(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const bgr666_t&    c) { set(color_convert<bgr888_t   , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const bgra8888_t&  c) { set(color_convert<bgr888_t   , bgra8888_t >(c.get())); return *this; }
  LGFX_INLINE bgr888_t&    bgr888_t   ::operator=(const grayscale_t& c) { set(color_convert<bgr888_t   , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const rgb332_t&    c) { set(color_convert<bgra8888_t , rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const rgb565_t&    c) { set(color_convert<bgra8888_t , rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const rgb888_t&    c) { set(color_convert<bgra8888_t , rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const argb8888_t&  c) { set(getSwap32                              (c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const swap565_t&   c) { set(color_convert<bgra8888_t , swap565_t  >(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const bgr666_t&    c) { set(color_convert<bgra8888_t , bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const bgr888_t&    c) { set(color_convert<bgra8888_t , bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE bgra8888_t&  bgra8888_t ::operator=(const grayscale_t& c) { set(color_convert<bgra8888_t , grayscale_t>(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const rgb332_t&    c) { set(color_convert<grayscale_t, rgb332_t   >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const rgb565_t&    c) { set(color_convert<grayscale_t, rgb565_t   >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const rgb888_t&    c) { set(color_convert<grayscale_t, rgb888_t   >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const argb8888_t&  c) { set(color_convert<grayscale_t, argb8888_t >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const swap565_t&   c) { set(color_convert<grayscale_t, swap565_t  >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const bgr666_t&    c) { set(color_convert<grayscale_t, bgr666_t   >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const bgr888_t&    c) { set(color_convert<grayscale_t, bgr888_t   >(c.get())); return *this; }
  LGFX_INLINE grayscale_t& grayscale_t::operator=(const bgra8888_t&  c) { set(color_convert<grayscale_t, bgra8888_t >(c.get())); return *this; }

  LGFX_INLINE bool operator==(const rgb332_t&    lhs, const rgb332_t&    rhs) { return pgm_read_byte           (&lhs) == pgm_read_byte           (&rhs); }
  LGFX_INLINE bool operator==(const rgb565_t&    lhs, const rgb565_t&    rhs) { return pgm_read_word           (&lhs) == pgm_read_word           (&rhs); }
  LGFX_INLINE bool operator==(const swap565_t&   lhs, const swap565_t&   rhs) { return pgm_read_word           (&lhs) == pgm_read_word           (&rhs); }
  LGFX_INLINE bool operator==(const bgr666_t&    lhs, const bgr666_t&    rhs) { return pgm_read_3byte_unaligned(&lhs) == pgm_read_3byte_unaligned(&rhs); }
  LGFX_INLINE bool operator==(const rgb888_t&    lhs, const rgb888_t&    rhs) { return pgm_read_3byte_unaligned(&lhs) == pgm_read_3byte_unaligned(&rhs); }
  LGFX_INLINE bool operator==(const bgr888_t&    lhs, const bgr888_t&    rhs) { return pgm_read_3byte_unaligned(&lhs) == pgm_read_3byte_unaligned(&rhs); }
  LGFX_INLINE bool operator==(const argb8888_t&  lhs, const argb8888_t&  rhs) { return pgm_read_dword          (&lhs) == pgm_read_dword          (&rhs); }
  LGFX_INLINE bool operator==(const grayscale_t& lhs, const grayscale_t& rhs) { return pgm_read_byte           (&lhs) == pgm_read_byte           (&rhs); }

  // for compare transparent color.
  LGFX_INLINE bool operator==(const rgb332_t&    lhs, uint32_t rhs) { return pgm_read_byte           (&lhs) == rhs; }
  LGFX_INLINE bool operator==(const rgb565_t&    lhs, uint32_t rhs) { return pgm_read_word           (&lhs) == rhs; }
  LGFX_INLINE bool operator==(const swap565_t&   lhs, uint32_t rhs) { return pgm_read_word           (&lhs) == rhs; }
  LGFX_INLINE bool operator==(const bgr666_t&    lhs, uint32_t rhs) { return pgm_read_3byte_unaligned(&lhs) == rhs; }
  LGFX_INLINE bool operator==(const rgb888_t&    lhs, uint32_t rhs) { return pgm_read_3byte_unaligned(&lhs) == rhs; }
  LGFX_INLINE bool operator==(const bgr888_t&    lhs, uint32_t rhs) { return pgm_read_3byte_unaligned(&lhs) == rhs; }
  LGFX_INLINE bool operator==(const argb8888_t&  lhs, uint32_t rhs) { return pgm_read_dword          (&lhs) == rhs; }
  LGFX_INLINE bool operator==(const bgra8888_t&  lhs, uint32_t rhs) { return pgm_read_dword          (&lhs) == rhs; }
  LGFX_INLINE bool operator==(const grayscale_t& lhs, uint32_t rhs) { return pgm_read_byte           (&lhs) == rhs; }

  LGFX_INLINE bool operator==(const raw_color_t& lhs, const raw_color_t& rhs) { return *reinterpret_cast<const uint32_t*>(&lhs) == *reinterpret_cast<const uint32_t*>(&rhs); }
  LGFX_INLINE bool operator!=(const raw_color_t& lhs, const raw_color_t& rhs) { return *reinterpret_cast<const uint32_t*>(&lhs) != *reinterpret_cast<const uint32_t*>(&rhs); }


  struct get_depth_impl {
  template<typename T> static constexpr std::integral_constant<color_depth_t, T::depth> check(decltype(T::depth)*);
  template<typename T> static constexpr std::integral_constant<color_depth_t, (color_depth_t)(sizeof(T) << 3) > check(...);
  };
  template<typename T> class get_depth : public decltype(get_depth_impl::check<T>(nullptr)) {};

  template <typename TSrc>
  static auto get_fp_convert_src(color_depth_t dst_depth) -> uint32_t(*)(uint32_t)
  {
    if (std::is_same<TSrc, rgb332_t>::value || std::is_same<TSrc, uint8_t>::value) {
      switch (dst_depth) {
      case argb8888_4Byte: return color_convert<bgra8888_t, rgb332_t>;
      case rgb888_3Byte  : return color_convert<bgr888_t  , rgb332_t>;
      case rgb666_3Byte  : return color_convert<bgr666_t  , rgb332_t>;
      case rgb565_2Byte  : return color_convert<swap565_t , rgb332_t>;
      case rgb332_1Byte  : return no_convert;
      case grayscale_8bit: return color_convert<grayscale_t, rgb332_t>;
      default: break;
      }
    } else if (std::is_same<TSrc, rgb888_t>::value || std::is_same<TSrc, uint32_t>::value) {
      switch (dst_depth) {
      case argb8888_4Byte: return color_convert<bgra8888_t, rgb888_t>;
      case rgb888_3Byte  : return getSwap24;
      case rgb666_3Byte  : return color_convert<bgr666_t  , rgb888_t>;
      case rgb565_2Byte  : return color_convert<swap565_t , rgb888_t>;
      case rgb332_1Byte  : return color_convert<rgb332_t  , rgb888_t>;
      case grayscale_8bit: return color_convert<grayscale_t,rgb888_t>;
      default: break;
      }
    } else if (std::is_same<TSrc, argb8888_t>::value) {
      switch (dst_depth) {
      case argb8888_4Byte: return getSwap32;
      case rgb888_3Byte  : return color_convert<bgr888_t , rgb888_t>;
      case rgb666_3Byte  : return color_convert<bgr666_t , rgb888_t>;
      case rgb565_2Byte  : return color_convert<swap565_t, rgb888_t>;
      case rgb332_1Byte  : return color_convert<rgb332_t , rgb888_t>;
      case grayscale_8bit: return color_convert<grayscale_t,rgb888_t>;
      default: break;
      }
    } else if (std::is_same<TSrc, bgr888_t>::value) {
      switch (dst_depth) {
      case argb8888_4Byte: return color_convert<bgra8888_t, bgr888_t>;
      case rgb888_3Byte  : return no_convert;
      case rgb666_3Byte  : return color_convert<bgr666_t  , bgr888_t>;
      case rgb565_2Byte  : return color_convert<swap565_t , bgr888_t>;
      case rgb332_1Byte  : return color_convert<rgb332_t  , bgr888_t>;
      case grayscale_8bit: return color_convert<grayscale_t,bgr888_t>;
      default: break;
      }
    } else { // if (std::is_same<TSrc, rgb565_t>::value || std::is_same<TSrc, uint16_t>::value || std::is_same<TSrc, int>::value)
      switch (dst_depth) {
      case argb8888_4Byte: return color_convert<bgra8888_t, rgb565_t>;
      case rgb888_3Byte  : return color_convert<bgr888_t  , rgb565_t>;
      case rgb666_3Byte  : return color_convert<bgr666_t  , rgb565_t>;
      case rgb565_2Byte  : return getSwap16;
      case rgb332_1Byte  : return color_convert<rgb332_t  , rgb565_t>;
      case grayscale_8bit: return color_convert<grayscale_t,rgb565_t>;
      default: break;
      }
    }

    switch (dst_depth & color_depth_t::bit_mask) {
    case 8:   return convert_uint32_to_palette8;
    case 4:   return convert_uint32_to_palette4;
    case 2:   return convert_uint32_to_palette2;
    case 1:   return convert_uint32_to_palette1;
    default:           return no_convert;
    }
  }

  struct color_conv_t
  {
    uint32_t (*convert_argb8888)(uint32_t);
    uint32_t (*convert_bgr888)(uint32_t);
    uint32_t (*convert_rgb888)(uint32_t);
    uint32_t (*convert_rgb565)(uint32_t);
    uint32_t (*convert_rgb332)(uint32_t);
    uint32_t (*revert_rgb888)(uint32_t);
    uint32_t colormask;
    union
    {
      color_depth_t depth;
      struct
      {
        uint8_t bits;
        uint8_t attrib;
      };
    };
    uint8_t bytes;

    color_conv_t(const color_conv_t&) = default;
    color_conv_t()
    {
      setColorDepth(color_depth_t::rgb565_2Byte);
    }
    color_conv_t(uint_fast8_t bpp, bool has_palette = false)
    {
      setColorDepth(bpp, has_palette);
    }
    color_conv_t(color_depth_t depth_)
    {
      setColorDepth(depth_);
    }

    void setColorDepth(uint_fast8_t bpp, bool has_palette)
    {
      color_depth_t d;
      if (     bpp > 24) { d = argb8888_4Byte; }
      else if (bpp > 18) { d =   rgb888_3Byte; }
      else if (bpp > 16) { d =   rgb666_3Byte; }
      else if (bpp >  8) { d =   rgb565_2Byte; }
      else if (bpp >  4) { d =   rgb332_1Byte; }
      else if (bpp >  2) { d = grayscale_4bit; }
      else if (bpp == 2) { d = grayscale_2bit; }
      else               { d = grayscale_1bit; }
      if (has_palette && bpp <= 8)
      {
        d = (color_depth_t)((d & color_depth_t::bit_mask) | color_depth_t::has_palette);
      }
      setColorDepth(d);
    }

    void setColorDepth(color_depth_t depth_)
    {
      depth = depth_;
      bytes = bits >> 3;
      colormask = (1 << bits) - 1;

      convert_argb8888 = get_fp_convert_src<argb8888_t>(depth_);
      convert_rgb888   = get_fp_convert_src<rgb888_t  >(depth_);
      convert_rgb565   = get_fp_convert_src<rgb565_t  >(depth_);
      convert_rgb332   = get_fp_convert_src<rgb332_t  >(depth_);
      convert_bgr888   = get_fp_convert_src<bgr888_t  >(depth_);

      switch (depth_) {
      case argb8888_4Byte: revert_rgb888 = color_convert<rgb888_t, bgra8888_t >; break;
      case rgb888_3Byte:   revert_rgb888 = color_convert<rgb888_t, bgr888_t   >; break;
      case rgb666_3Byte:   revert_rgb888 = color_convert<rgb888_t, bgr666_t   >; break;
      case rgb565_2Byte:   revert_rgb888 = color_convert<rgb888_t, swap565_t  >; break;
      case rgb332_1Byte:   revert_rgb888 = color_convert<rgb888_t, rgb332_t   >; break;
      case grayscale_8bit: revert_rgb888 = color_convert<rgb888_t, grayscale_t>; break;
      default:             revert_rgb888 = no_convert;
      }
    }

#define TYPECHECK(dType) template < typename T, typename std::enable_if < (sizeof(T) == sizeof(dType)) && (std::is_signed<T>::value == std::is_signed<dType>::value), std::nullptr_t >::type=nullptr > LGFX_INLINE
    TYPECHECK(int8_t  ) uint32_t convert(T c) { return convert_rgb332(c); }
    TYPECHECK(uint8_t ) uint32_t convert(T c) { return convert_rgb332(c); }
    TYPECHECK(uint16_t) uint32_t convert(T c) { return convert_rgb565(c); }
    TYPECHECK(int16_t ) uint32_t convert(T c) { return convert_rgb565(c); }
    TYPECHECK(int32_t ) uint32_t convert(T c) { return convert_rgb565(c); }
    TYPECHECK(uint32_t) uint32_t convert(T c) { return convert_rgb888(c); }

    LGFX_INLINE uint32_t convert(const rgb332_t&   c) { return convert_rgb332(  c.get()); }
    LGFX_INLINE uint32_t convert(const rgb565_t&   c) { return convert_rgb565(  c.get()); }
    LGFX_INLINE uint32_t convert(const rgb888_t&   c) { return convert_rgb888(  c.get()); }
    LGFX_INLINE uint32_t convert(const argb8888_t& c) { return convert_argb8888(c.get()); }
    LGFX_INLINE uint32_t convert(const bgr888_t&   c) { return convert_bgr888(  c.get()); }
  };

  TYPECHECK(int8_t  ) uint32_t convert_to_rgb888(T c) { return color_convert<rgb888_t, rgb332_t>(c); }
  TYPECHECK(uint8_t ) uint32_t convert_to_rgb888(T c) { return color_convert<rgb888_t, rgb332_t>(c); }
  TYPECHECK(uint16_t) uint32_t convert_to_rgb888(T c) { return color_convert<rgb888_t, rgb565_t>(c); }
  TYPECHECK(int16_t ) uint32_t convert_to_rgb888(T c) { return color_convert<rgb888_t, rgb565_t>(c); }
  TYPECHECK(int32_t ) uint32_t convert_to_rgb888(T c) { return color_convert<rgb888_t, rgb565_t>(c); }
  TYPECHECK(uint32_t) uint32_t convert_to_rgb888(T c) { return c & 0xFFFFFF; }
  LGFX_INLINE uint32_t convert_to_rgb888(const argb8888_t& c) { return color_convert<rgb888_t, argb8888_t>(c.get()); } // c.R8()<<16|c.G8()<<8|c.B8(); }
  LGFX_INLINE uint32_t convert_to_rgb888(const rgb888_t&   c) { return c.get(); }
  LGFX_INLINE uint32_t convert_to_rgb888(const rgb565_t&   c) { return color_convert<rgb888_t, rgb565_t  >(c.get()); } // c.R8()<<16|c.G8()<<8|c.B8(); }
  LGFX_INLINE uint32_t convert_to_rgb888(const rgb332_t&   c) { return color_convert<rgb888_t, rgb332_t  >(c.get()); } // c.R8()<<16|c.G8()<<8|c.B8(); }
  LGFX_INLINE uint32_t convert_to_rgb888(const bgr888_t&   c) { return getSwap24(c.get()); }
  LGFX_INLINE uint32_t convert_to_rgb888(const bgr666_t&   c) { return color_convert<rgb888_t, bgr666_t  >(c.get()); } // c.R8()<<16|c.G8()<<8|c.B8(); }
  LGFX_INLINE uint32_t convert_to_rgb888(const swap565_t&  c) { return color_convert<rgb888_t, swap565_t >(c.get()); } // c.R8()<<16|c.G8()<<8|c.B8(); }

  TYPECHECK(int8_t  ) uint32_t convert_to_bgr888(T c) { return color_convert<bgr888_t, rgb332_t>(c); }
  TYPECHECK(uint8_t ) uint32_t convert_to_bgr888(T c) { return color_convert<bgr888_t, rgb332_t>(c); }
  TYPECHECK(uint16_t) uint32_t convert_to_bgr888(T c) { return color_convert<bgr888_t, rgb565_t>(c); }
  TYPECHECK(int16_t ) uint32_t convert_to_bgr888(T c) { return color_convert<bgr888_t, rgb565_t>(c); }
  TYPECHECK(int32_t ) uint32_t convert_to_bgr888(T c) { return color_convert<bgr888_t, rgb565_t>(c); }
  TYPECHECK(uint32_t) uint32_t convert_to_bgr888(T c) { return getSwap24(c); }
  LGFX_INLINE uint32_t convert_to_bgr888(const argb8888_t& c) { return color_convert<bgr888_t, argb8888_t>(c.get()); } // return c.B8()<<16|c.G8()<<8|c.R8(); }
  LGFX_INLINE uint32_t convert_to_bgr888(const rgb888_t&   c) { return color_convert<bgr888_t, rgb888_t  >(c.get()); } // return c.B8()<<16|c.G8()<<8|c.R8(); }
  LGFX_INLINE uint32_t convert_to_bgr888(const rgb565_t&   c) { return color_convert<bgr888_t, rgb565_t  >(c.get()); } // return c.B8()<<16|c.G8()<<8|c.R8(); }
  LGFX_INLINE uint32_t convert_to_bgr888(const rgb332_t&   c) { return color_convert<bgr888_t, rgb332_t  >(c.get()); } // return c.B8()<<16|c.G8()<<8|c.R8(); }
  LGFX_INLINE uint32_t convert_to_bgr888(const bgr888_t&   c) { return c.get(); }
  LGFX_INLINE uint32_t convert_to_bgr888(const bgr666_t&   c) { return color_convert<bgr888_t, bgr666_t  >(c.get()); } // return c.B8()<<16|c.G8()<<8|c.R8(); }
  LGFX_INLINE uint32_t convert_to_bgr888(const swap565_t&  c) { return color_convert<bgr888_t, swap565_t >(c.get()); } // return c.B8()<<16|c.G8()<<8|c.R8(); }

#undef TYPECHECK

//----------------------------------------------------------------------------

  struct effect_fill_alpha
  {
    effect_fill_alpha(argb8888_t src)
      : _inv { (uint_fast16_t)(256 - src.A8()) }
      , _r8a { (uint_fast16_t)(src.R8() * (1 + src.A8())) }
      , _g8a { (uint_fast16_t)(src.G8() * (1 + src.A8())) }
      , _b8a { (uint_fast16_t)(src.B8() * (1 + src.A8())) }
    {}
    void set(argb8888_t src)
    {
      _inv = (uint_fast16_t)(256 - src.A8());
      _r8a = (uint_fast16_t)(src.R8() * (1 + src.A8()));
      _g8a = (uint_fast16_t)(src.G8() * (1 + src.A8()));
      _b8a = (uint_fast16_t)(src.B8() * (1 + src.A8()));
    }
    template <typename TDstColor>
    void operator() (int32_t x, int32_t y, TDstColor& dst)
    {
      (void)x; (void)y;
      dst.set((_r8a + dst.R8() * _inv) >> 8
             ,(_g8a + dst.G8() * _inv) >> 8
             ,(_b8a + dst.B8() * _inv) >> 8
             );
    }
    template <typename TDstColor, typename TSrcColor>
    void operator() (int32_t x, int32_t y, TDstColor& dst, TSrcColor& src)
    {
      (void)x; (void)y;
      uint_fast16_t  a8 = 1 + src.A8();
      uint_fast16_t inv = 257 - a8;
      uint_fast16_t r8a = a8 * src.R8();
      uint_fast16_t g8a = a8 * src.G8();
      uint_fast16_t b8a = a8 * src.B8();

      dst.set((r8a + dst.R8() * inv) >> 8
             ,(g8a + dst.G8() * inv) >> 8
             ,(b8a + dst.B8() * inv) >> 8
             );
    }
  private:
    uint_fast16_t _inv;
    uint_fast16_t _r8a;
    uint_fast16_t _g8a;
    uint_fast16_t _b8a;
  };


  struct colors_t
  {
    const rgb888_t *colors;
    const uint32_t count;
  };


//----------------------------------------------------------------------------
#undef LGFX_INLINE

 }
}

using RGBColor = lgfx::bgr888_t;
