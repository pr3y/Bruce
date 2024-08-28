#if defined (CORE)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.
#ifndef M5Faces_h
#define M5Faces_h

#include "M5Stack.h"

class M5Faces {
   public:
    M5Faces();
    bool canControlFaces();
    uint8_t getch(void);
    bool kbhit(void);

   private:
};

#endif
#endif