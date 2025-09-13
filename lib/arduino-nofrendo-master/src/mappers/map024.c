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
** map24.c
**
** mapper 24 interface
** $Id: map024.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../log.h"
#include "../sndhrdw/vrcvisnd.h"

static struct
{
   int counter, enabled;
   int latch, wait_state;
} irq;

static void map24_init(void)
{
   irq.counter = irq.enabled = 0;
   irq.latch = irq.wait_state = 0;
}

static void map24_hblank(int vblank)
{
   UNUSED(vblank);

   if (irq.enabled)
   {
      if (256 == ++irq.counter)
      {
         irq.counter = irq.latch;
         nes_irq();
         //irq.enabled = false;
         irq.enabled = irq.wait_state;
      }
   }
}

static void map24_write(uint32 address, uint8 value)
{
   switch (address & 0xF003)
   {
   case 0x8000:
      mmc_bankrom(16, 0x8000, value);
      break;

   case 0x9003:
      /* ??? */
      break;

   case 0xB003:
      switch (value & 0x0C)
      {
      case 0x00:
         ppu_mirror(0, 1, 0, 1); /* vertical */
         break;

      case 0x04:
         ppu_mirror(0, 0, 1, 1); /* horizontal */
         break;

      case 0x08:
         ppu_mirror(0, 0, 0, 0);
         break;

      case 0x0C:
         ppu_mirror(1, 1, 1, 1);
         break;

      default:
         break;
      }
      break;

   case 0xC000:
      mmc_bankrom(8, 0xC000, value);
      break;

   case 0xD000:
      mmc_bankvrom(1, 0x0000, value);
      break;

   case 0xD001:
      mmc_bankvrom(1, 0x0400, value);
      break;

   case 0xD002:
      mmc_bankvrom(1, 0x0800, value);
      break;

   case 0xD003:
      mmc_bankvrom(1, 0x0C00, value);
      break;

   case 0xE000:
      mmc_bankvrom(1, 0x1000, value);
      break;

   case 0xE001:
      mmc_bankvrom(1, 0x1400, value);
      break;

   case 0xE002:
      mmc_bankvrom(1, 0x1800, value);
      break;

   case 0xE003:
      mmc_bankvrom(1, 0x1C00, value);
      break;

   case 0xF000:
      irq.latch = value;
      break;

   case 0xF001:
      irq.enabled = (value >> 1) & 0x01;
      irq.wait_state = value & 0x01;
      if (irq.enabled)
         irq.counter = irq.latch;
      break;

   case 0xF002:
      irq.enabled = irq.wait_state;
      break;

   default:
#ifdef NOFRENDO_DEBUG
      nofrendo_log_printf("invalid VRC6 write: $%02X to $%04X", value, address);
#endif /* NOFRENDO_DEBUG */
      break;
   }
}

static void map24_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper24.irqCounter = irq.counter;
   state->extraData.mapper24.irqCounterEnabled = irq.enabled;
}

static void map24_setstate(SnssMapperBlock *state)
{
   irq.counter = state->extraData.mapper24.irqCounter;
   irq.enabled = state->extraData.mapper24.irqCounterEnabled;
}

static map_memwrite map24_memwrite[] =
    {
        {0x8000, 0xF002, map24_write},
        {-1, -1, NULL}};

mapintf_t map24_intf =
    {
        24,             /* mapper number */
        "Konami VRC6",  /* mapper name */
        map24_init,     /* init routine */
        NULL,           /* vblank callback */
        map24_hblank,   /* hblank callback */
        map24_getstate, /* get state (snss) */
        map24_setstate, /* set state (snss) */
        NULL,           /* memory read structure */
        map24_memwrite, /* memory write structure */
        &vrcvi_ext      /* external sound device */
};

/*
** $Log: map024.c,v $
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
** Revision 1.11  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.10  2000/10/22 15:03:13  matt
** simplified mirroring
**
** Revision 1.9  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.8  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
**
** Revision 1.7  2000/10/09 12:00:53  matt
** removed old code
**
** Revision 1.6  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.5  2000/07/15 23:52:19  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.4  2000/07/10 13:51:25  matt
** using generic nes_irq() routine now
**
** Revision 1.3  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/04 23:11:45  matt
** initial revision
**
*/
