/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** nesinput.c
**
** Event handling system routines
** $Id: nesinput.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "nesinput.h"
#include "../log.h"

/* TODO: make a linked list of inputs sources, so they
**       can be removed if need be
*/

static nesinput_t *nes_input[MAX_CONTROLLERS];
static int active_entries = 0;

/* read counters */
static int pad0_readcount, pad1_readcount, ppad_readcount, ark_readcount;

static int retrieve_type(int type)
{
   int i, value = 0;

   for (i = 0; i < active_entries; i++)
   {
      ASSERT(nes_input[i]);

      if (type == nes_input[i]->type)
         value |= nes_input[i]->data;
   }

   return value;
}

static uint8 get_pad0(void)
{
   uint8 value;

   value = (uint8)retrieve_type(INP_JOYPAD0);

   /* mask out left/right simultaneous keypresses */
   if ((value & INP_PAD_UP) && (value & INP_PAD_DOWN))
      value &= ~(INP_PAD_UP | INP_PAD_DOWN);

   if ((value & INP_PAD_LEFT) && (value & INP_PAD_RIGHT))
      value &= ~(INP_PAD_LEFT | INP_PAD_RIGHT);

   /* return (0x40 | value) due to bus conflicts */
   return (0x40 | ((value >> pad0_readcount++) & 1));
}

static uint8 get_pad1(void)
{
   uint8 value;

   value = (uint8)retrieve_type(INP_JOYPAD1);

   /* mask out left/right simultaneous keypresses */
   if ((value & INP_PAD_UP) && (value & INP_PAD_DOWN))
      value &= ~(INP_PAD_UP | INP_PAD_DOWN);

   if ((value & INP_PAD_LEFT) && (value & INP_PAD_RIGHT))
      value &= ~(INP_PAD_LEFT | INP_PAD_RIGHT);

   /* return (0x40 | value) due to bus conflicts */
   return (0x40 | ((value >> pad1_readcount++) & 1));
}

static uint8 get_zapper(void)
{
   return (uint8)(retrieve_type(INP_ZAPPER));
}

static uint8 get_powerpad(void)
{
   int value;
   uint8 ret_val = 0;

   value = retrieve_type(INP_POWERPAD);

   if (((value >> 8) >> ppad_readcount) & 1)
      ret_val |= 0x10;
   if (((value & 0xFF) >> ppad_readcount) & 1)
      ret_val |= 0x08;

   ppad_readcount++;

   return ret_val;
}

static uint8 get_vsdips0(void)
{
   return (retrieve_type(INP_VSDIPSW0));
}

static uint8 get_vsdips1(void)
{
   return (retrieve_type(INP_VSDIPSW1));
}

static uint8 get_arkanoid(void)
{
   uint8 value = retrieve_type(INP_ARKANOID);

   if ((value >> (7 - ark_readcount++)) & 1)
      return 0x02;
   else
      return 0;
}

/* return input state for all types indicated (can be ORed together) */
uint8 input_get(int types)
{
   uint8 value = 0;

   if (types & INP_JOYPAD0)
      value |= get_pad0();
   if (types & INP_JOYPAD1)
      value |= get_pad1();
   if (types & INP_ZAPPER)
      value |= get_zapper();
   if (types & INP_POWERPAD)
      value |= get_powerpad();
   if (types & INP_VSDIPSW0)
      value |= get_vsdips0();
   if (types & INP_VSDIPSW1)
      value |= get_vsdips1();
   if (types & INP_ARKANOID)
      value |= get_arkanoid();

   return value;
}

/* register an input type */
void input_register(nesinput_t *input)
{
   if (NULL == input)
      return;

   nes_input[active_entries] = input;
   active_entries++;
}

void input_event(nesinput_t *input, int state, int value)
{
   ASSERT(input);

   if (state == INP_STATE_MAKE)
      input->data |= value;  /* OR it in */
   else                      /* break state */
      input->data &= ~value; /* mask it out */
}

void input_strobe(void)
{
   pad0_readcount = 0;
   pad1_readcount = 0;
   ppad_readcount = 0;
   ark_readcount = 0;
}

/*
** $Log: nesinput.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.9  2000/09/10 23:25:03  matt
** minor changes
**
** Revision 1.8  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.7  2000/07/23 15:11:45  matt
** removed unused variables
**
** Revision 1.6  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.5  2000/07/04 04:56:50  matt
** include changes
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
