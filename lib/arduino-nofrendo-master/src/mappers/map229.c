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
** map229.c
**
** Mapper #229 (31 in 1)
** Implementation by Firebug
** Mapper information courtesy of Mark Knibbs
** $Id: map229.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../libsnss/libsnss.h"
#include "../log.h"

/************************/
/* Mapper #229: 31 in 1 */
/************************/
static void map229_init(void)
{
  /* On reset, PRG is set to first 32K and CHR to first 8K */
  mmc_bankrom(32, 0x8000, 0x00);
  mmc_bankvrom(8, 0x0000, 0x00);

  /* Done */
  return;
}

/*******************************************/
/* Mapper #229 write handler ($8000-$FFFF) */
/*******************************************/
static void map229_write(uint32 address, uint8 value)
{
  /* Value written is irrelevant */
  UNUSED(value);

  /* A4-A0 sets 8K CHR page */
  mmc_bankvrom(8, 0x0000, (uint8)(address & 0x1F));

  /* If A4-A1 are all low then select the first 32K,     */
  /* otherwise select a 16K bank at both $8000 and $C000 */
  if ((address & 0x1E) == 0x00)
  {
    mmc_bankrom(32, 0x8000, 0x00);
  }
  else
  {
    mmc_bankrom(16, 0x8000, (uint8)(address & 0x1F));
    mmc_bankrom(16, 0xC000, (uint8)(address & 0x1F));
  }

  /* A5: mirroring (low = vertical, high = horizontal) */
  if (address & 0x20)
    ppu_mirror(0, 0, 1, 1);
  else
    ppu_mirror(0, 1, 0, 1);

  /* Done */
  return;
}

static map_memwrite map229_memwrite[] =
    {
        {0x8000, 0xFFFF, map229_write},
        {-1, -1, NULL}};

mapintf_t map229_intf =
    {
        229,                 /* Mapper number */
        "31 in 1 (bootleg)", /* Mapper name */
        map229_init,         /* Initialization routine */
        NULL,                /* VBlank callback */
        NULL,                /* HBlank callback */
        NULL,                /* Get state (SNSS) */
        NULL,                /* Set state (SNSS) */
        NULL,                /* Memory read structure */
        map229_memwrite,     /* Memory write structure */
        NULL                 /* External sound device */
};

/*
** $Log: map229.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/30 06:34:31  firebug
** Initial revision
**
**
*/
