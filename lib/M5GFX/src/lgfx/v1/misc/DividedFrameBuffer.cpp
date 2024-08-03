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

#include "DividedFrameBuffer.hpp"

#include "../../internal/algorithm.h"
#include "../platforms/common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  uint8_t** DividedFrameBuffer::create(size_t line_size, size_t total_lines, size_t block_lines, psram_setting_t use_psram)
  {
    release();
    if (line_size == 0 || total_lines < block_lines || block_lines == 0)
    {
      return nullptr;
    }
    size_t block_count = ((total_lines - 1) / block_lines) + 1;
    _block_count = block_count;
    _block_lines = block_lines;
    _total_lines = total_lines;
    _line_size = line_size;

    auto block_array_size = block_count * sizeof(uint8_t*);
    auto block_array = static_cast<uint8_t**>(heap_alloc_dma(block_array_size));
    if (block_array != nullptr)
    {
      _block_array = block_array;
      memset(block_array, 0, block_array_size);
      bool success = true;
      bool psram = use_psram != no_psram;
      for (size_t i = 0; i < block_count; ++i)
      {
        size_t block_size = line_size * (total_lines < block_lines ? total_lines : block_lines);
        total_lines -= block_lines;
        uint8_t* buf = nullptr;
        if (psram)
        {
          buf = static_cast<uint8_t*>(heap_alloc_psram(block_size));
        }
        if (buf == nullptr)
        {
          buf = static_cast<uint8_t*>(heap_alloc_dma(block_size));
        }
        block_array[i] = buf;
        if (buf == nullptr)
        {
          success = false;
          break;
        }
        if (use_psram == psram_setting_t::half_psram)
        {
          psram = !psram;
        }
      }
      if (success)
      {
        return block_array;
      }
      release();
    }
    return nullptr;
  }

  void DividedFrameBuffer::release(void)
  {
    if (_block_array != nullptr)
    {
      for (int i = _block_count - 1; i >= 0; --i)
      {
        if (_block_array[i] != nullptr)
        {
          heap_free(_block_array[i]);
        }
        _block_array[i] = nullptr;
      }
      heap_free(_block_array);
      _block_array = nullptr;
    }
    _line_size = 0;
    _total_lines = 0;
    _block_lines = 0;
    _block_count = 0;
  }

//----------------------------------------------------------------------------
 }
}
