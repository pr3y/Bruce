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
** map93.c
**
** mapper 93 interface
** $Id: map093.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes_ppu.h"

static void map93_write(uint32 address, uint8 value)
{
   UNUSED(address);

   /* ($8000-$FFFF) D7-D4 = switch $8000-$BFFF D0: mirror */
   mmc_bankrom(16, 0x8000, value >> 4);

   if (value & 1)
      ppu_mirror(0, 1, 0, 1); /* vert */
   else
      ppu_mirror(0, 0, 1, 1); /* horiz */
}

static map_memwrite map93_memwrite[] =
    {
        {0x8000, 0xFFFF, map93_write},
        {-1, -1, NULL}};

mapintf_t map93_intf =
    {
        93,             /* mapper number */
        "Mapper 93",    /* mapper name */
        NULL,           /* init routine */
        NULL,           /* vblank callback */
        NULL,           /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map93_memwrite, /* memory write structure */
        NULL            /* external sound device */
};

/*
** $Log: map093.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/12/11 12:33:48  matt
** initial revision
**
*/
