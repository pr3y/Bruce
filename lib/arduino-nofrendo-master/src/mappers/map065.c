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
** map65.c
**
** mapper 65 interface
** $Id: map065.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"

static struct
{
   int counter;
   bool enabled;
   int cycles;
   uint8 low, high;
} irq;

static void map65_init(void)
{
   irq.counter = 0;
   irq.enabled = false;
   irq.low = irq.high = 0;
   irq.cycles = 0;
}

/* TODO: shouldn't there be some kind of HBlank callback??? */

/* mapper 65: Irem H-3001*/
static void map65_write(uint32 address, uint8 value)
{
   int range = address & 0xF000;
   int reg = address & 7;

   switch (range)
   {
   case 0x8000:
   case 0xA000:
   case 0xC000:
      mmc_bankrom(8, range, value);
      break;

   case 0xB000:
      mmc_bankvrom(1, reg << 10, value);
      break;

   case 0x9000:
      switch (reg)
      {
      case 4:
         irq.enabled = (value & 0x01) ? false : true;
         break;

      case 5:
         irq.high = value;
         irq.cycles = (irq.high << 8) | irq.low;
         irq.counter = (uint8)(irq.cycles / 128);
         break;

      case 6:
         irq.low = value;
         irq.cycles = (irq.high << 8) | irq.low;
         irq.counter = (uint8)(irq.cycles / 128);
         break;

      default:
         break;
      }
      break;

   default:
      break;
   }
}

static map_memwrite map65_memwrite[] =
    {
        {0x8000, 0xFFFF, map65_write},
        {-1, -1, NULL}};

mapintf_t map65_intf =
    {
        65,             /* mapper number */
        "Irem H-3001",  /* mapper name */
        map65_init,     /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map65_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map065.c,v $
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
** Revision 1.5  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.4  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.3  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/06 01:01:56  matt
** initial revision
**
*/
