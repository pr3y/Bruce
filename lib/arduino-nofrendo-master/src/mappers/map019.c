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
** map19.c
**
** mapper 19 interface
** $Id: map019.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"

/* TODO: shouldn't there be an h-blank IRQ handler??? */

/* Special mirroring macro for mapper 19 */
#define N_BANK1(table, value)                                                                                                               \
   {                                                                                                                                        \
      if ((value) < 0xE0)                                                                                                                   \
         ppu_setpage(1, (table) + 8, &mmc_getinfo()->vrom[((value) % (mmc_getinfo()->vrom_banks * 8)) << 10] - (0x2000 + ((table) << 10))); \
      else                                                                                                                                  \
         ppu_setpage(1, (table) + 8, &mmc_getinfo()->vram[((value)&7) << 10] - (0x2000 + ((table) << 10)));                                 \
      ppu_mirrorhipages();                                                                                                                  \
   }

static struct
{
   int counter, enabled;
} irq;

static void map19_init(void)
{
   irq.counter = irq.enabled = 0;
}

/* mapper 19: Namcot 106 */
static void map19_write(uint32 address, uint8 value)
{
   int reg = address >> 11;
   switch (reg)
   {
   case 0xA:
      irq.counter &= ~0xFF;
      irq.counter |= value;
      break;

   case 0xB:
      irq.counter = ((value & 0x7F) << 8) | (irq.counter & 0xFF);
      irq.enabled = (value & 0x80) ? true : false;
      break;

   case 0x10:
   case 0x11:
   case 0x12:
   case 0x13:
   case 0x14:
   case 0x15:
   case 0x16:
   case 0x17:
      mmc_bankvrom(1, (reg & 7) << 10, value);
      break;

   case 0x18:
   case 0x19:
   case 0x1A:
   case 0x1B:
      N_BANK1(reg & 3, value);
      break;

   case 0x1C:
      mmc_bankrom(8, 0x8000, value);
      break;

   case 0x1D:
      mmc_bankrom(8, 0xA000, value);
      break;

   case 0x1E:
      mmc_bankrom(8, 0xC000, value);
      break;

   default:
      break;
   }
}

static void map19_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper19.irqCounterLowByte = irq.counter & 0xFF;
   state->extraData.mapper19.irqCounterHighByte = irq.counter >> 8;
   state->extraData.mapper19.irqCounterEnabled = irq.enabled;
}

static void map19_setstate(SnssMapperBlock *state)
{
   irq.counter = (state->extraData.mapper19.irqCounterHighByte << 8) | state->extraData.mapper19.irqCounterLowByte;
   irq.enabled = state->extraData.mapper19.irqCounterEnabled;
}

static map_memwrite map19_memwrite[] =
    {
        {0x5000, 0x5FFF, map19_write},
        {0x8000, 0xFFFF, map19_write},
        {-1, -1, NULL}};

mapintf_t map19_intf =
    {
        19,             /* mapper number */
        "Namcot 106",   /* mapper name */
        map19_init,     /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        map19_getstate, /* get state (snss) */
        map19_setstate, /* set state (snss) */
        NULL,           /* memory read structure */
        map19_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map019.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:19:33  matt
** changed directory structure
**
** Revision 1.6  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.5  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.4  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
**
** Revision 1.3  2000/07/15 23:52:20  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/06 01:01:56  matt
** initial revision
**
*/
