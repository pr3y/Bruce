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
** map70.c
**
** mapper 70 interface
** $Id: map070.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"

/* mapper 70: Arkanoid II, Kamen Rider Club, etc. */
/* ($8000-$FFFF) D6-D4 = switch $8000-$BFFF */
/* ($8000-$FFFF) D3-D0 = switch PPU $0000-$1FFF */
/* ($8000-$FFFF) D7 = switch mirroring */
static void map70_write(uint32 address, uint8 value)
{
   UNUSED(address);

   mmc_bankrom(16, 0x8000, (value >> 4) & 0x07);
   mmc_bankvrom(8, 0x0000, value & 0x0F);

   /* Argh! FanWen used the 4-screen bit to determine
   ** whether the game uses D7 to switch between
   ** horizontal and vertical mirroring, or between
   ** one-screen mirroring from $2000 or $2400.
   */
   if (mmc_getinfo()->flags & ROM_FLAG_FOURSCREEN)
   {
      if (value & 0x80)
         ppu_mirror(0, 0, 1, 1); /* horiz */
      else
         ppu_mirror(0, 1, 0, 1); /* vert */
   }
   else
   {
      int mirror = (value & 0x80) >> 7;
      ppu_mirror(mirror, mirror, mirror, mirror);
   }
}

static map_memwrite map70_memwrite[] =
    {
        {0x8000, 0xFFFF, map70_write},
        {-1, -1, NULL}};

mapintf_t map70_intf =
    {
        70,             /* mapper number */
        "Mapper 70",    /* mapper name */
        NULL,           /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map70_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map070.c,v $
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
** Revision 1.7  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.6  2000/10/22 15:03:13  matt
** simplified mirroring
**
** Revision 1.5  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.4  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.3  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/06 01:01:56  matt
** initial revision
**
*/
