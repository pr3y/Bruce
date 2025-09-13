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
** map66.c
**
** mapper 66 interface
** $Id: map066.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"

/* mapper 66: GNROM */
static void map66_write(uint32 address, uint8 value)
{
   UNUSED(address);

   mmc_bankrom(32, 0x8000, (value >> 4) & 3);
   mmc_bankvrom(8, 0x0000, value & 3);
}

static void map66_init(void)
{
   mmc_bankrom(32, 0x8000, 0);
   mmc_bankvrom(8, 0x0000, 0);
}

static map_memwrite map66_memwrite[] =
    {
        {0x8000, 0xFFFF, map66_write},
        {-1, -1, NULL}};

mapintf_t map66_intf =
    {
        66,             /* mapper number */
        "GNROM",        /* mapper name */
        map66_init,     /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map66_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map066.c,v $
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
** Revision 1.3  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/05 05:05:18  matt
** initial revision
**
*/
