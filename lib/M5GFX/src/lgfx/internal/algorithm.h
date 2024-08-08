#pragma once

#if defined ( CONFIG_ARCH_BOARD_SPRESENSE )
  #include <Arduino.h>
  #include <stdio.h>
  #include <stdlib.h>
  #ifdef atof
  #undef atof
  #endif
  #ifdef atoi
  #undef atoi
  #endif
  #ifdef atol
  #undef atol
  #endif
  __attribute__((weak)) double atof(const char* nptr) { return strtod((nptr), NULL); }
  __attribute__((weak)) int atoi(const char* nptr) { return (int)strtol((nptr), NULL, 10); }
  __attribute__((weak)) long atol(const char* nptr) { return strtol((nptr), NULL, 10); }
  __attribute__((weak)) int	mblen(const char*, size_t) { return 0; }
  __attribute__((weak)) size_t mbstowcs(wchar_t*, const char*, size_t) { return 0; }
  __attribute__((weak)) int	mbtowc(wchar_t*, const char*, size_t) { return 0; }
  __attribute__((weak)) size_t wcstombs(char*, const wchar_t*, size_t) { return 0; }
  __attribute__((weak)) int wctomb(char*, wchar_t) { return 0; }

  #ifdef max
  #undef max
  #endif

  #ifdef min
  #undef min
  #endif

#endif
#include <algorithm>
