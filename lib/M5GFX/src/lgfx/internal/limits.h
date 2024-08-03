#pragma once

#if defined ( CONFIG_ARCH_BOARD_SPRESENSE )
  #ifdef max
  #undef max
  #endif

  #ifdef min
  #undef min
  #endif

#endif
#include <limits>
