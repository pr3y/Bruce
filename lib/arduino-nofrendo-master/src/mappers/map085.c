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
** map85.c
**
** mapper 85 interface
** $Id: map085.c,v 1.3 2001/05/06 01:42:03 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../log.h"

static struct
{
   int counter, latch;
   int wait_state;
   bool enabled;
} irq;

/* mapper 85: Konami VRC7 */
static void map85_write(uint32 address, uint8 value)
{
   uint8 bank = address >> 12;
   uint8 reg = (address & 0x10) | ((address & 0x08) << 1);

   switch (bank)
   {
   case 0x08:
      if (0x10 == reg)
         mmc_bankrom(8, 0xA000, value);
      else
         mmc_bankrom(8, 0x8000, value);
      break;

   case 0x09:
      /* 0x10 & 0x30 should be trapped by sound emulation */
      mmc_bankrom(8, 0xC000, value);
      break;

   case 0x0A:
      if (0x10 == reg)
         mmc_bankvrom(1, 0x0400, value);
      else
         mmc_bankvrom(1, 0x0000, value);
      break;

   case 0x0B:
      if (0x10 == reg)
         mmc_bankvrom(1, 0x0C00, value);
      else
         mmc_bankvrom(1, 0x0800, value);
      break;

   case 0x0C:
      if (0x10 == reg)
         mmc_bankvrom(1, 0x1400, value);
      else
         mmc_bankvrom(1, 0x1000, value);
      break;

   case 0x0D:
      if (0x10 == reg)
         mmc_bankvrom(1, 0x1C00, value);
      else
         mmc_bankvrom(1, 0x1800, value);
      break;

   case 0x0E:
      if (0x10 == reg)
      {
         irq.latch = value;
      }
      else
      {
         switch (value & 3)
         {
         case 0:
            ppu_mirror(0, 1, 0, 1); /* vertical */
            break;

         case 1:
            ppu_mirror(0, 0, 1, 1); /* horizontal */
            break;

         case 2:
            ppu_mirror(0, 0, 0, 0);
            break;

         case 3:
            ppu_mirror(1, 1, 1, 1);
            break;
         }
      }
      break;

   case 0x0F:
      if (0x10 == reg)
      {
         irq.enabled = irq.wait_state;
      }
      else
      {
         irq.wait_state = value & 0x01;
         irq.enabled = (value & 0x02) ? true : false;
         if (true == irq.enabled)
            irq.counter = irq.latch;
      }
      break;

   default:
#ifdef NOFRENDO_DEBUG
      nofrendo_log_printf("unhandled vrc7 write: $%02X to $%04X\n", value, address);
#endif /* NOFRENDO_DEBUG */
      break;
   }
}

static void map85_hblank(int vblank)
{
   UNUSED(vblank);

   if (irq.enabled)
   {
      if (++irq.counter > 0xFF)
      {
         irq.counter = irq.latch;
         nes_irq();

         //return;
      }
      //irq.counter++;
   }
}

static map_memwrite map85_memwrite[] =
    {
        {0x8000, 0xFFFF, map85_write},
        {-1, -1, NULL}};

static void map85_init(void)
{
   mmc_bankrom(16, 0x8000, 0);
   mmc_bankrom(16, 0xC000, MMC_LASTBANK);

   mmc_bankvrom(8, 0x0000, 0);

   irq.counter = irq.latch = 0;
   irq.wait_state = 0;
   irq.enabled = false;
}

mapintf_t map85_intf =
    {
        85,             /* mapper number */
        "Konami VRC7",  /* mapper name */
        map85_init,     /* init routine */
        NULL,           /* vblank callback */
        map85_hblank,   /* hblank callback */
        NULL,           /* get state (snss) */
        NULL,           /* set state (snss) */
        NULL,           /* memory read structure */
        map85_memwrite, /* memory write structure */
        NULL};

/*
** $Log: map085.c,v $
** Revision 1.3  2001/05/06 01:42:03  neil
** boooo
**
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
** Revision 1.10  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.9  2000/10/22 15:03:14  matt
** simplified mirroring
**
** Revision 1.8  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.7  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
**
** Revision 1.6  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.5  2000/07/23 14:37:21  matt
** added a break statement
**
** Revision 1.4  2000/07/15 23:52:19  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.3  2000/07/10 13:51:25  matt
** using generic nes_irq() routine now
**
** Revision 1.2  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.1  2000/07/06 02:47:47  matt
** initial revision
**
*/
