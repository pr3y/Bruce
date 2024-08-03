#pragma once

#include <stdint.h>
#include <stddef.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void memset_multi(uint8_t* buf, uint32_t c, size_t size, size_t length);

//----------------------------------------------------------------------------
 }
}
