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
** map99.c
**
** mapper 99 interface
** $Id: map099.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"

/* Switch VROM for VS games */
static void map99_vromswitch(uint8 value)
{
   int bank = (value & 0x04) >> 2;
   mmc_bankvrom(8, 0x0000, bank);
}

/* mapper 99: VS. System */
static void map99_init(void)
{
   ppu_mirror(0, 1, 2, 3);
   ppu_setvromswitch(map99_vromswitch);
}

mapintf_t map99_intf =
    {
        99,           /* mapper number */
        "VS. System", /* mapper name */
        map99_init,   /* init routine */
        NULL,         /* vblank callback */
        NULL,         /* hblank callback */
        NULL,         /* get state (snss) */
        NULL,         /* set state (snss) */
        NULL,         /* memory read structure */
        NULL,         /* memory write structure */
        NULL          /* external sound device */
};

/*
** $Log: map099.c,v $
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
** Revision 1.6  2000/10/22 19:17:47  matt
** mapper cleanups galore
**
** Revision 1.5  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.4  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
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
