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
** map4.c
**
** mapper 4 interface
** $Id: map004.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../libsnss/libsnss.h"

static struct
{
   int counter, latch;
   bool enabled, reset;
} irq;

static uint8 reg;
static uint8 command;
static uint16 vrombase;

/* mapper 4: MMC3 */
static void map4_write(uint32 address, uint8 value)
{
   switch (address & 0xE001)
   {
   case 0x8000:
      command = value;
      vrombase = (command & 0x80) ? 0x1000 : 0x0000;

      if (reg != (value & 0x40))
      {
         if (value & 0x40)
            mmc_bankrom(8, 0x8000, (mmc_getinfo()->rom_banks * 2) - 2);
         else
            mmc_bankrom(8, 0xC000, (mmc_getinfo()->rom_banks * 2) - 2);
      }
      reg = value & 0x40;
      break;

   case 0x8001:
      switch (command & 0x07)
      {
      case 0:
         value &= 0xFE;
         mmc_bankvrom(1, vrombase ^ 0x0000, value);
         mmc_bankvrom(1, vrombase ^ 0x0400, value + 1);
         break;

      case 1:
         value &= 0xFE;
         mmc_bankvrom(1, vrombase ^ 0x0800, value);
         mmc_bankvrom(1, vrombase ^ 0x0C00, value + 1);
         break;

      case 2:
         mmc_bankvrom(1, vrombase ^ 0x1000, value);
         break;

      case 3:
         mmc_bankvrom(1, vrombase ^ 0x1400, value);
         break;

      case 4:
         mmc_bankvrom(1, vrombase ^ 0x1800, value);
         break;

      case 5:
         mmc_bankvrom(1, vrombase ^ 0x1C00, value);
         break;

      case 6:
         mmc_bankrom(8, (command & 0x40) ? 0xC000 : 0x8000, value);
         break;

      case 7:
         mmc_bankrom(8, 0xA000, value);
         break;
      }
      break;

   case 0xA000:
      /* four screen mirroring crap */
      if (0 == (mmc_getinfo()->flags & ROM_FLAG_FOURSCREEN))
      {
         if (value & 1)
            ppu_mirror(0, 0, 1, 1); /* horizontal */
         else
            ppu_mirror(0, 1, 0, 1); /* vertical */
      }
      break;

   case 0xA001:
      /* Save RAM enable / disable */
      /* Messes up Startropics I/II if implemented -- bah */
      break;

   case 0xC000:
      irq.latch = value;
      //      if (irq.reset)
      //         irq.counter = irq.latch;
      break;

   case 0xC001:
      irq.reset = true;
      irq.counter = irq.latch;
      break;

   case 0xE000:
      irq.enabled = false;
      //      if (irq.reset)
      //         irq.counter = irq.latch;
      break;

   case 0xE001:
      irq.enabled = true;
      //      if (irq.reset)
      //         irq.counter = irq.latch;
      break;

   default:
      //printf("map004: unhandled write: address=%p, value=0x%x\n", (void*)address, value);
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      break;
   }

   if (true == irq.reset)
      irq.counter = irq.latch;
}

static void map4_hblank(int vblank)
{
   if (vblank)
      return;

   if (ppu_enabled())
   {
      if (irq.counter >= 0)
      {
         irq.reset = false;
         irq.counter--;

         if (irq.counter < 0)
         {
            if (irq.enabled)
            {
               irq.reset = true;
               nes_irq();
            }
         }
      }
   }
}

static void map4_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper4.irqCounter = irq.counter;
   state->extraData.mapper4.irqLatchCounter = irq.latch;
   state->extraData.mapper4.irqCounterEnabled = irq.enabled;
   state->extraData.mapper4.last8000Write = command;
}

static void map4_setstate(SnssMapperBlock *state)
{
   irq.counter = state->extraData.mapper4.irqCounter;
   irq.latch = state->extraData.mapper4.irqLatchCounter;
   irq.enabled = state->extraData.mapper4.irqCounterEnabled;
   command = state->extraData.mapper4.last8000Write;
}

static void map4_init(void)
{
   irq.counter = irq.latch = 0;
   irq.enabled = irq.reset = false;
   reg = command = 0;
   vrombase = 0x0000;
}

static map_memwrite map4_memwrite[] =
    {
        {0x8000, 0xFFFF, map4_write},
        {-1, -1, NULL}};

mapintf_t map4_intf =
    {
        4,             /* mapper number */
        "MMC3",        /* mapper name */
        map4_init,     /* init routine */
        NULL,          /* vblank callback */
        map4_hblank,   /* hblank callback */
        map4_getstate, /* get state (snss) */
        map4_setstate, /* set state (snss) */
        NULL,          /* memory read structure */
        map4_memwrite, /* memory write structure */
        NULL           /* external sound device */
};

/*
** $Log: map004.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.2  2000/11/26 15:40:49  matt
** hey, it actually works now
**
** Revision 1.1  2000/10/24 12:19:32  matt
** changed directory structure
**
** Revision 1.12  2000/10/23 15:53:27  matt
** suppressed warnings
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
** Revision 1.7  2000/10/08 18:05:44  matt
** kept old version around, just in case....
**
** Revision 1.6  2000/07/15 23:52:19  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.5  2000/07/10 13:51:25  matt
** using generic nes_irq() routine now
**
** Revision 1.4  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.3  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.2  2000/07/05 05:04:39  matt
** minor modifications
**
** Revision 1.1  2000/07/04 23:11:45  matt
** initial revision
**
*/
