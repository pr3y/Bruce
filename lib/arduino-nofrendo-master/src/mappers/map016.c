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
** map16.c
**
** mapper 16 interface
** $Id: map016.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"
#include "../nes/nes.h"

static struct
{
   int counter;
   bool enabled;
} irq;

/* mapper 16: Bandai */

static void map16_init(void)
{
   mmc_bankrom(16, 0x8000, 0);
   mmc_bankrom(16, 0xC000, MMC_LASTBANK);
   irq.counter = 0;
   irq.enabled = false;
}

static void map16_write(uint32 address, uint8 value)
{
   int reg = address & 0xF;

   if (reg < 8)
   {
      mmc_bankvrom(1, reg << 10, value);
   }
   else
   {
      switch (address & 0x000F)
      {
      case 0x8:
         mmc_bankrom(16, 0x8000, value);
         break;

      case 0x9:
         switch (value & 3)
         {
         case 0:
            ppu_mirror(0, 0, 1, 1); /* horizontal */
            break;

         case 1:
            ppu_mirror(0, 1, 0, 1); /* vertical */
            break;

         case 2:
            ppu_mirror(0, 0, 0, 0);
            break;

         case 3:
            ppu_mirror(1, 1, 1, 1);
            break;
         }
         break;

      case 0xA:
         irq.enabled = (value & 1) ? true : false;
         break;

      case 0xB:
         irq.counter = (irq.counter & 0xFF00) | value;
         break;

      case 0xC:
         irq.counter = (value << 8) | (irq.counter & 0xFF);
         break;

      case 0xD:
         /* eeprom I/O port? */
         break;
      }
   }
}

static void map16_hblank(int vblank)
{
   UNUSED(vblank);

   if (irq.enabled)
   {
      if (irq.counter)
      {
         if (0 == --irq.counter)
            nes_irq();
      }
   }
}

static void map16_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper16.irqCounterLowByte = irq.counter & 0xFF;
   state->extraData.mapper16.irqCounterHighByte = irq.counter >> 8;
   state->extraData.mapper16.irqCounterEnabled = irq.enabled;
}

static void map16_setstate(SnssMapperBlock *state)
{
   irq.counter = (state->extraData.mapper16.irqCounterHighByte << 8) | state->extraData.mapper16.irqCounterLowByte;
   irq.enabled = state->extraData.mapper16.irqCounterEnabled;
}

static map_memwrite map16_memwrite[] =
    {
        {0x6000, 0x600D, map16_write},
        {0x7FF0, 0x7FFD, map16_write},
        {0x8000, 0x800D, map16_write},
        {-1, -1, NULL}};

mapintf_t map16_intf =
    {
        16,             /* mapper number */
        "Bandai",       /* mapper name */
        map16_init,     /* init routine */
        NULL,           /* vblank callback */
        map16_hblank,   /* hblank callback */
        map16_getstate, /* get state (snss) */
        map16_setstate, /* set state (snss) */
        NULL,           /* memory read structure */
        map16_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map016.c,v $
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
** Revision 1.8  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.7  2000/10/22 15:03:13  matt
** simplified mirroring
**
** Revision 1.6  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.5  2000/10/10 13:58:16  matt
** stroustrup squeezing his way in the door
**
** Revision 1.4  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.3  2000/07/15 23:52:20  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.2  2000/07/11 05:03:49  matt
** value masking isn't necessary for the banking routines
**
** Revision 1.1  2000/07/11 03:14:18  melanson
** Initial commit for mappers 16, 34, and 231
**
**
*/
