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
** map15.c
**
** mapper 15 interface
** $Id: map015.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"

/* mapper 15: Contra 100-in-1 */
static void map15_write(uint32 address, uint8 value)
{
   int bank = value & 0x3F;
   uint8 swap = (value & 0x80) >> 7;

   switch (address & 0x3)
   {
   case 0:
      mmc_bankrom(8, 0x8000, (bank << 1) + swap);
      mmc_bankrom(8, 0xA000, (bank << 1) + (swap ^ 1));
      mmc_bankrom(8, 0xC000, ((bank + 1) << 1) + swap);
      mmc_bankrom(8, 0xE000, ((bank + 1) << 1) + (swap ^ 1));

      if (value & 0x40)
         ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
         ppu_mirror(0, 1, 0, 1); /* vertical */
      break;

   case 1:
      mmc_bankrom(8, 0xC000, (bank << 1) + swap);
      mmc_bankrom(8, 0xE000, (bank << 1) + (swap ^ 1));
      break;

   case 2:
      if (swap)
      {
         mmc_bankrom(8, 0x8000, (bank << 1) + 1);
         mmc_bankrom(8, 0xA000, (bank << 1) + 1);
         mmc_bankrom(8, 0xC000, (bank << 1) + 1);
         mmc_bankrom(8, 0xE000, (bank << 1) + 1);
      }
      else
      {
         mmc_bankrom(8, 0x8000, (bank << 1));
         mmc_bankrom(8, 0xA000, (bank << 1));
         mmc_bankrom(8, 0xC000, (bank << 1));
         mmc_bankrom(8, 0xE000, (bank << 1));
      }
      break;

   case 3:
      mmc_bankrom(8, 0xC000, (bank << 1) + swap);
      mmc_bankrom(8, 0xE000, (bank << 1) + (swap ^ 1));

      if (value & 0x40)
         ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
         ppu_mirror(0, 1, 0, 1); /* vertical */
      break;

   default:
      break;
   }
}

static void map15_init(void)
{
   mmc_bankrom(32, 0x8000, 0);
}

static map_memwrite map15_memwrite[] =
    {
        {0x8000, 0xFFFF, map15_write},
        {-1, -1, NULL}};

mapintf_t map15_intf =
    {
        15,                /* mapper number */
        "Contra 100-in-1", /* mapper name */
        map15_init,        /* init routine */
        NULL,              /* vblank callback */
        NULL,              /* hblank callback */
        NULL,              /* get state (snss) */
        NULL,              /* set state (snss) */
        NULL,              /* memory read structure */
        map15_memwrite,    /* memory write structure */
        NULL               /* external sound device */
};

/*
** $Log: map015.c,v $
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
** Revision 1.5  2000/10/22 15:03:13  matt
** simplified mirroring
**
** Revision 1.4  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.3  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/05 05:05:18  matt
** initial revision
**
*/
