/*
  Keyboard_da_DK.h

  Copyright (c) 2021, Peter John

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

#ifndef KEYBOARD_DA_DK_h
#define KEYBOARD_DA_DK_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

//================================================================================
//================================================================================
//  Keyboard

// DA_DK keys
#define KEY_A_RING        (136+0x2f)
#define KEY_SLASHED_O     (136+0x34)
#define KEY_ASH           (136+0x33)
#define KEY_UMLAUT        (136+0x30)
#define KEY_ACUTE_ACC     (136+0x2e)

#endif
#endif
