/*
  Keyboard_tr_TR.h

  Copyright (c) 2021, [Your Name]

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

#ifndef KEYBOARD_TR_TR_h
#define KEYBOARD_TR_TR_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

//================================================================================
//================================================================================
//  Keyboard

// TR_TR keys
#define KEY_A_RING        (136+0x2f)  // ğ
#define KEY_A_UMLAUT      (136+0x34)  // ö
#define KEY_O_UMLAUT      (136+0x33)  // ü
#define KEY_UMLAUT        (136+0x30)  // ş
#define KEY_ACUTE_ACC     (136+0x2e)  // ç
#define KEY_DOTLESS_I     (136+0x3a)  // ı
#define KEY_DOTTED_I      (136+0x3b)  // İ
#define KEY_Y_DOTTED      (136+0x3c)  // Y (dotted)
#define KEY_Y_DOTLESS     (136+0x3d)  // y (dotless)

#endif
#endif