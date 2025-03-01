#ifndef __RTC__
#define __RTC__

enum RTCChipModel
{
    BM8563,
    UNDEFINED,
};

#ifdef RTC_BM8563
#include "DRIVER/BM8563.tpp"
#endif

#endif  /* __RTC__ */
