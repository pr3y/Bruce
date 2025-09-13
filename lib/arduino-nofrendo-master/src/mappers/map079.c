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
** map78.c
**
** mapper 78 interface
** $Id: map079.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"

/* mapper 79: NINA-03/06 */
static void map79_write(uint32 address, uint8 value)
{
   if ((address & 0x5100) == 0x4100)
   {
      mmc_bankrom(32, 0x8000, (value >> 3) & 1);
      mmc_bankvrom(8, 0x0000, value & 7);
   }
}

static void map79_init(void)
{
   mmc_bankrom(32, 0x8000, 0);
   mmc_bankvrom(8, 0x0000, 0);
}

static map_memwrite map79_memwrite[] =
    {
        {0x4100, 0x5FFF, map79_write}, /* ????? incorrect range ??? */
        {-1, -1, NULL}};

mapintf_t map79_intf =
    {
        79,             /* mapper number */
        "NINA-03/06",   /* mapper name */
        map79_init,     /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map79_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map079.c,v $
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
** Revision 1.4  2000/10/22 19:17:47  matt
** mapper cleanups galore
**
** Revision 1.3  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/06 01:01:56  matt
** initial revision
**
*/
