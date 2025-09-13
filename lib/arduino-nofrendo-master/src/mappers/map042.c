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
** map042.c
**
** Mapper #42 (Baby Mario bootleg)
** Implementation by Firebug
** Mapper information courtesy of Kevin Horton
** $Id: map042.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../libsnss/libsnss.h"
#include "../log.h"

static struct
{
  bool enabled;
  uint32 counter;
} irq;

/********************************/
/* Mapper #42 IRQ reset routine */
/********************************/
static void map42_irq_reset(void)
{
  /* Turn off IRQs */
  irq.enabled = false;
  irq.counter = 0x0000;

  /* Done */
  return;
}

/********************************************/
/* Mapper #42: Baby Mario bootleg cartridge */
/********************************************/
static void map42_init(void)
{
  /* Set the hardwired pages */
  mmc_bankrom(8, 0x8000, 0x0C);
  mmc_bankrom(8, 0xA000, 0x0D);
  mmc_bankrom(8, 0xC000, 0x0E);
  mmc_bankrom(8, 0xE000, 0x0F);

  /* Reset the IRQ counter */
  map42_irq_reset();

  /* Done */
  return;
}

/****************************************/
/* Mapper #42 callback for IRQ handling */
/****************************************/
static void map42_hblank(int vblank)
{
  /* Counter is M2 based so it doesn't matter whether */
  /* the PPU is in its VBlank period or not           */
  UNUSED(vblank);

  /* Increment the counter if it is enabled and check for strike */
  if (irq.enabled)
  {
    /* Is there a constant for cycles per scanline? */
    /* If so, someone ought to substitute it here   */
    irq.counter = irq.counter + 114;

    /* IRQ is triggered after 24576 M2 cycles */
    if (irq.counter >= 0x6000)
    {
      /* Trigger the IRQ */
      nes_irq();

      /* Reset the counter */
      map42_irq_reset();
    }
  }
}

/******************************************/
/* Mapper #42 write handler ($E000-$FFFF) */
/******************************************/
static void map42_write(uint32 address, uint8 value)
{
  switch (address & 0x03)
  {
  /* Register 0: Select ROM page at $6000-$7FFF */
  case 0x00:
    mmc_bankrom(8, 0x6000, value & 0x0F);
    break;

  /* Register 1: mirroring */
  case 0x01:
    if (value & 0x08)
      ppu_mirror(0, 0, 1, 1); /* horizontal */
    else
      ppu_mirror(0, 1, 0, 1); /* vertical   */
    break;

  /* Register 2: IRQ */
  case 0x02:
    if (value & 0x02)
      irq.enabled = true;
    else
      map42_irq_reset();
    break;

  /* Register 3: unused */
  default:
    break;
  }

  /* Done */
  return;
}

/****************************************************/
/* Shove extra mapper information into a SNSS block */
/****************************************************/
static void map42_setstate(SnssMapperBlock *state)
{
  /* TODO: Store SNSS information */
  UNUSED(state);

  /* Done */
  return;
}

/*****************************************************/
/* Pull extra mapper information out of a SNSS block */
/*****************************************************/
static void map42_getstate(SnssMapperBlock *state)
{
  /* TODO: Retrieve SNSS information */
  UNUSED(state);

  /* Done */
  return;
}

static map_memwrite map42_memwrite[] =
    {
        {0xE000, 0xFFFF, map42_write},
        {-1, -1, NULL}};

mapintf_t map42_intf =
    {
        42,                     /* Mapper number */
        "Baby Mario (bootleg)", /* Mapper name */
        map42_init,             /* Initialization routine */
        NULL,                   /* VBlank callback */
        map42_hblank,           /* HBlank callback */
        map42_getstate,         /* Get state (SNSS) */
        map42_setstate,         /* Set state (SNSS) */
        NULL,                   /* Memory read structure */
        map42_memwrite,         /* Memory write structure */
        NULL                    /* External sound device */
};

/*
** $Log: map042.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/27 19:23:30  firebug
** initial revision
**
**
*/
