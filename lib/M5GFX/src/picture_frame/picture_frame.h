// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined ( SDL_h_ )

namespace m5gfx
{
  #include "frame_image_M5Stack.h"
  #include "frame_image_M5StackCore2.h"
  #include "frame_image_M5StackCoreS3.h"
  #include "frame_image_M5StackCoreInk.h"
  #include "frame_image_M5StickCPlus.h"
  #include "frame_image_M5StickCPlus2.h"
  #include "frame_image_M5Dial.h"

  struct picture_frame_t
  {
  uint16_t w;
  uint16_t h;
  uint16_t x;
  uint16_t y;
  const uint32_t* img;
  };
  static constexpr const picture_frame_t picture_frame_M5Stack = { 400, 400, 40, 80, frame_image_M5Stack };
  static constexpr const picture_frame_t picture_frame_M5StackCore2 = { 400, 400, 40, 80, frame_image_M5StackCore2 };
  static constexpr const picture_frame_t picture_frame_M5StackCoreS3 = { 400, 400, 40, 80, frame_image_M5StackCoreS3 };
  static constexpr const picture_frame_t picture_frame_M5StackCoreInk = { 280, 395, 40, 80, frame_image_M5StackCoreInk };
  static constexpr const picture_frame_t picture_frame_M5StickCPlus = { 235, 468, 50, 40, frame_image_M5StickCPlus };
  static constexpr const picture_frame_t picture_frame_M5StickCPlus2 = { 235, 468, 50, 40, frame_image_M5StickCPlus2 };
  static constexpr const picture_frame_t picture_frame_M5Dial = { 350, 350, 55, 55, frame_image_M5Dial };

  const picture_frame_t* getPictureFrame(board_t b)
  {
    switch (b) {
    case board_M5Stack:         return &picture_frame_M5Stack;
    case board_M5StackCore2:    return &picture_frame_M5StackCore2;
    case board_M5StackCoreS3:   return &picture_frame_M5StackCoreS3;
    case board_M5StackCoreInk:  return &picture_frame_M5StackCoreInk;
    case board_M5StickCPlus:    return &picture_frame_M5StickCPlus;
    case board_M5StickCPlus2:   return &picture_frame_M5StickCPlus2;
    case board_M5Dial:          return &picture_frame_M5Dial;
    default: return nullptr;
    }
  }
}

#endif
