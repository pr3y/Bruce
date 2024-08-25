#if defined (CORE)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "M5Faces.h"

// ================ Faces Keyboard ===================
#define KEYBOARD_I2C_ADDR 0x08
#define KEYBOARD_INT      5
#define READI2CSUBADDR    0

/*----------------------------------------------------------------------*
 * Extentions for Facces                                                *
 *----------------------------------------------------------------------*/
bool M5Faces::canControlFaces() {
    return M5.I2C.writeCommand(KEYBOARD_I2C_ADDR, READI2CSUBADDR);
}

M5Faces::M5Faces() {
    pinMode(KEYBOARD_INT, INPUT_PULLUP);
}

uint8_t M5Faces::getch(void) {
    uint8_t data = 0x00;

    if (kbhit()) {
        while (data == 0x00) {
            if (!M5.I2C.readByte(KEYBOARD_I2C_ADDR, &data)) {
                return 0x00;
            }
            delay(1);
        }
        return data;
    }
    return 0x00;
}

bool M5Faces::kbhit(void) {
    return (digitalRead(KEYBOARD_INT) == LOW);
}
#endif