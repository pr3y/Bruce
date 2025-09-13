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
** map087.c
**
** Mapper #87 (16K VROM switch)
** Implementation by Firebug
** $Id: map087.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../libsnss/libsnss.h"
#include "../log.h"

/******************************************/
/* Mapper #87 write handler ($6000-$7FFF) */
/******************************************/
static void map87_write(uint32 address, uint8 value)
{
   /* Within range, address written to is irrelevant */
   UNUSED(address);

   /* Very simple: 8K CHR page is selected by D1 */
   if (value & 0x02)
      mmc_bankvrom(8, 0x0000, 0x01);
   else
      mmc_bankvrom(8, 0x0000, 0x00);

   /* Done */
   return;
}

static map_memwrite map87_memwrite[] =
    {
        {0x6000, 0x7FFF, map87_write},
        {-1, -1, NULL}};

mapintf_t map87_intf =
    {
        87,                /* Mapper number */
        "16K VROM switch", /* Mapper name */
        NULL,              /* Initialization routine */
        NULL,              /* VBlank callback */
        NULL,              /* HBlank callback */
        NULL,              /* Get state (SNSS) */
        NULL,              /* Set state (SNSS) */
        NULL,              /* Memory read structure */
        map87_memwrite,    /* Memory write structure */
        NULL               /* External sound device */
};

/*
** $Log: map087.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/30 06:34:44  firebug
** Initial revision
**
**
*/
