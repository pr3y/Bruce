#ifndef __LGFX_BOARDS_HPP__
#define __LGFX_BOARDS_HPP__

namespace lgfx // This should not be changed to "m5gfx"
{
  namespace boards
  { // Be careful not to change existing board numbers when adding values.
    enum board_t
    { board_unknown = 0
    , board_M5Stack
    , board_M5StackCore2
    , board_M5StickC
    , board_M5StickCPlus
    , board_M5StickCPlus2
    , board_M5StackCoreInk
    , board_M5Paper
    , board_M5Tough
    , board_M5Station
    , board_M5StackCoreS3
    , board_M5AtomS3
    , board_M5Dial
    , board_M5DinMeter
    , board_M5Cardputer
    , board_M5AirQ
    , board_M5VAMeter
    , board_M5StackCoreS3SE

/// non display boards 
    , board_M5Atom = 128
    , board_M5ATOM = board_M5Atom
    , board_M5AtomPsram
    , board_M5AtomU
    , board_M5Camera
    , board_M5TimerCam
    , board_M5StampPico
    , board_M5StampC3
    , board_M5StampC3U
    , board_M5StampS3
    , board_M5AtomS3Lite
    , board_M5AtomS3U
    , board_M5Capsule
    , board_M5NanoC6

/// external displays
    , board_M5AtomDisplay = 192
    , board_M5ATOMDisplay = board_M5AtomDisplay
    , board_M5UnitLCD
    , board_M5UnitOLED
    , board_M5UnitMiniOLED
    , board_M5UnitGLASS
    , board_M5UnitGLASS2
    , board_M5UnitRCA
    , board_M5ModuleDisplay
    , board_M5ModuleRCA
    };
  }
  using namespace boards;
}

#endif
