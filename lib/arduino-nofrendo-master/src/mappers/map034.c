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
** map34.c
**
** mapper 34 interface
** $Id: map034.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"

static void map34_init(void)
{
   mmc_bankrom(32, 0x8000, MMC_LASTBANK);
}

static void map34_write(uint32 address, uint8 value)
{
   if ((address & 0x8000) || (0x7FFD == address))
   {
      mmc_bankrom(32, 0x8000, value);
   }
   else if (0x7FFE == address)
   {
      mmc_bankvrom(4, 0x0000, value);
   }
   else if (0x7FFF == address)
   {
      mmc_bankvrom(4, 0x1000, value);
   }
}

static map_memwrite map34_memwrite[] =
    {
        {0x7FFD, 0xFFFF, map34_write},
        {-1, -1, NULL}};

mapintf_t map34_intf =
    {
        34,             /* mapper number */
        "Nina-1",       /* mapper name */
        map34_init,     /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map34_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map034.c,v $
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
** Revision 1.3  2000/07/11 05:03:49  matt
** value masking isn't necessary for the banking routines
**
** Revision 1.2  2000/07/11 03:35:08  bsittler
** Fixes to make mikes new mappers compile.
**
** Revision 1.1  2000/07/11 03:14:18  melanson
** Initial commit for mappers 16, 34, and 231
**
**
*/
