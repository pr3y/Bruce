
#include "common_function.hpp"

#include <string.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void memset_multi(uint8_t* buf, uint32_t c, size_t size, size_t length)
  {
    if (size == 1 || ((c & 0xFF) == ((c >> 8) & 0xFF) && (size == 2 || ((c & 0xFF) == ((c >> 16) & 0xFF)))))
    {
      memset(buf, c, size * length);
      return;
    }

    size_t l = length;
    if (l & ~0xF)
    {
      while ((l >>= 1) & ~0xF);
      ++l;
    }
    size_t len = l * size;
    length = (length * size) - len;
    uint8_t* dst = buf;
    if (size == 2) {
      do { // 2byte speed tweak
        *(uint16_t*)dst = c;
        dst += 2;
      } while (--l);
    } else {
      do {
        size_t i = 0;
        do {
          *dst++ = *(((uint8_t*)&c) + i);
        } while (++i != size);
      } while (--l);
    }
    if (!length) return;
    while (length > len) {
      memcpy(dst, buf, len);
      dst += len;
      length -= len;
      len <<= 1;
    }
    if (length) {
      memcpy(dst, buf, length);
    }
  }

//----------------------------------------------------------------------------
 }
}
