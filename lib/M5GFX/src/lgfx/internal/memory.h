#pragma once

#if defined ( CONFIG_ARCH_BOARD_SPRESENSE )
  #include <stdint.h>
  #include <stdio.h>
  #include <wchar.h>
  __attribute__((weak)) int vfwscanf(void*, const wchar_t *, ...) { return 0; }
  __attribute__((weak)) int vswscanf(const wchar_t*, const wchar_t *, ...) { return 0; }
  __attribute__((weak)) int vwscanf(const wchar_t*, ...) { return 0; }

  #define _GLIBCXX_CSTDINT 1
#endif
#include <memory>
