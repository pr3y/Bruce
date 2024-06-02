/*
  Keyboard_pt_PT.h

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

#ifndef KEYBOARD_PT_PT_h
#define KEYBOARD_PT_PT_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

//================================================================================
//================================================================================
//  Keyboard

// pt_PT keys
#define KEY_LEFT_GUILLEMET       (136+0x2e)
#define KEY_ACUTE                (136+0x30)
#define KEY_C_CEDILLA            (136+0x33)
#define KEY_MASCULINE_ORDINAL    (136+0x34)
#define KEY_TILDE                (136+0x31)

#endif
#endif
