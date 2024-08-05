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

#include "SpriteBuffer.hpp"

#include "../../internal/algorithm.h"
#include "../platforms/common.hpp"

#include <assert.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  SpriteBuffer::SpriteBuffer(size_t length, AllocationSource source) : _buffer(nullptr), _length(0), _source(source)
  {
    if (length)
    {
      assert (source != AllocationSource::Preallocated);
      this->reset(length, source);
    }
  }

  SpriteBuffer::SpriteBuffer(const SpriteBuffer& rhs) : _buffer(nullptr)
  {
    if ( rhs._source == AllocationSource::Preallocated )
    {
      this->_buffer = rhs._buffer;
      this->_length = rhs._length;
      this->_source = rhs._source;
    }
    else
    {
      this->reset(rhs._length, rhs._source);
      if( _buffer != nullptr && rhs._buffer != nullptr )
      {
        std::copy(rhs._buffer, rhs._buffer + _length, _buffer);
      }
    }
  }

  SpriteBuffer::SpriteBuffer(SpriteBuffer&& rhs) : _buffer(nullptr)
  {
    if ( rhs._source == AllocationSource::Preallocated ) {
      this->_buffer = rhs._buffer;
      this->_length = rhs._length;
      this->_source = rhs._source;
    }
    else {
      this->reset(rhs._length, rhs._source);
      if( _buffer != nullptr && rhs._buffer != nullptr ) {
        std::copy(rhs._buffer, rhs._buffer + _length, _buffer);
        rhs.release();
      }
    }
  }

  SpriteBuffer& SpriteBuffer::operator=(const SpriteBuffer& rhs)
  {
    if ( rhs._source == AllocationSource::Preallocated ) {
      this->_buffer = rhs._buffer;
      this->_length = rhs._length;
      this->_source = rhs._source;
    }
    else {
      this->reset(rhs._length, rhs._source);
      if ( _buffer != nullptr && rhs._buffer != nullptr ) {
        std::copy(rhs._buffer, rhs._buffer + _length, _buffer);
      }
    }
    return *this;
  }

  SpriteBuffer& SpriteBuffer::operator=(SpriteBuffer&& rhs)
  {
    if( rhs._source == AllocationSource::Preallocated ) {
      this->_buffer = rhs._buffer;
      this->_length = rhs._length;
      this->_source = rhs._source;
    }
    else {
      this->reset(rhs._length, rhs._source);
      if( _buffer != nullptr && rhs._buffer != nullptr ) {
        std::copy(rhs._buffer, rhs._buffer + _length, _buffer);
        rhs.release();
      }
    }
    return *this;
  }

  void SpriteBuffer::reset(void* buffer)
  {
    this->release();
    _source = AllocationSource::Preallocated;
    _buffer = reinterpret_cast<uint8_t*>(buffer);
    _length = 0;
  }

  void SpriteBuffer::reset(size_t length, AllocationSource source)
  {
    this->release();
    void* buffer = nullptr;
    _source = source;
    switch (source)
    {
      default:
      case AllocationSource::Normal:
        buffer = heap_alloc(length);
        break;
      case AllocationSource::Dma:
        buffer = heap_alloc_dma(length);
        break;
      case AllocationSource::Psram:
        buffer = heap_alloc_psram(length);
        if (!buffer)
        {
          _source = AllocationSource::Dma;
          buffer = heap_alloc_dma(length);
        }
        break;
    }
    _buffer = reinterpret_cast<uint8_t*>(buffer);
    if ( _buffer != nullptr ) {
      _length = length;
    }
  }

  void SpriteBuffer::release(void) {
    _length = 0;
    if ( _buffer != nullptr ) {
      if (_source != AllocationSource::Preallocated)
      {
        heap_free(_buffer);
      }
      _buffer = nullptr;
    }
  }

//----------------------------------------------------------------------------
 }
}
