/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include <stdint.h>
#include <stddef.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct bgr888_t;

  enum AllocationSource
  {
    Normal,
    Dma,
    Psram,
    Preallocated,
  };

  class SpriteBuffer
  {
  private:
    uint8_t* _buffer;
    size_t _length;
    AllocationSource _source;

  public:
    SpriteBuffer(void) : _buffer(nullptr), _length(0), _source(Dma) {}

    SpriteBuffer(size_t length, AllocationSource source = AllocationSource::Dma);

    SpriteBuffer(uint8_t* buffer, size_t length) : _buffer(buffer), _length(length), _source(AllocationSource::Preallocated)
    {
    }

    SpriteBuffer(const SpriteBuffer& rhs);

    SpriteBuffer(SpriteBuffer&& rhs);

    SpriteBuffer& operator=(const SpriteBuffer& rhs);

    SpriteBuffer& operator=(SpriteBuffer&& rhs);

    operator uint8_t*() const { return _buffer; }
    operator bool() const { return _buffer != nullptr; }

    uint8_t* get() const { return _buffer; }
    uint8_t* img8() const { return _buffer; }
    uint16_t* img16() const { return reinterpret_cast<uint16_t*>(_buffer); }
    bgr888_t* img24() const { return reinterpret_cast<bgr888_t*>(_buffer); }
    uint32_t* img32() const { return reinterpret_cast<uint32_t*>(_buffer); }

    void reset(void* buffer);

    void reset(size_t length, AllocationSource source);

    void release(void);

    bool use_dma(void) const { return _source == AllocationSource::Dma; }
    bool use_memcpy(void) const { return _source != AllocationSource::Psram; }
  };

//----------------------------------------------------------------------------
 }
}
