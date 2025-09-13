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
** map3.c
**
** mapper 3 interface
** $Id: map003.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"

/* mapper 3: CNROM */
static void map3_write(uint32 address, uint8 value)
{
   UNUSED(address);

   mmc_bankvrom(8, 0x0000, value);
}

static map_memwrite map3_memwrite[] =
    {
        {0x8000, 0xFFFF, map3_write},
        {-1, -1, NULL}};

mapintf_t map3_intf =
    {
        3,             /* mapper number */
        "CNROM",       /* mapper name */
        NULL,          /* init routine */
        NULL,          /* vblank callback */
        NULL,          /* hblank callback */
        NULL,          /* get state (snss) */
        NULL,          /* set state (snss) */
        NULL,          /* memory read structure */
        map3_memwrite, /* memory write structure */
        NULL           /* external sound device */
};

/*
** $Log: map003.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:19:32  matt
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
** Revision 1.1  2000/07/04 23:11:45  matt
** initial revision
**
*/
