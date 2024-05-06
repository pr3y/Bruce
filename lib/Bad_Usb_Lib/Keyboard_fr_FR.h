/*
  Keyboard_fr_FR.h

  Copyright (c) 2022, Edgar Bonet

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KEYBOARD_FR_FR_h
#define KEYBOARD_FR_FR_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

//================================================================================
//================================================================================
//  Keyboard

// fr_FR keys
#define KEY_SUPERSCRIPT_TWO (136+0x35)
#define KEY_E_ACUTE         (136+0x1f)
#define KEY_E_GRAVE         (136+0x24)
#define KEY_C_CEDILLA       (136+0x26)
#define KEY_A_GRAVE         (136+0x27)
#define KEY_CIRCUMFLEX      (136+0x2f)
#define KEY_U_GRAVE         (136+0x34)

#endif
#endif
