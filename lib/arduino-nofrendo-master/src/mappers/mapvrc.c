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
** map_vrc.c
**
** VRC mapper interface
** $Id: mapvrc.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include "../noftypes.h"
#include "../nes/nes_mmc.h"
#include "../nes/nes.h"
#include "../log.h"

#define VRC_VBANK(bank, value, high)                                                  \
   {                                                                                  \
      if ((high))                                                                     \
         highnybbles[(bank)] = (value)&0x0F;                                          \
      else                                                                            \
         lownybbles[(bank)] = (value)&0x0F;                                           \
      mmc_bankvrom(1, (bank) << 10, (highnybbles[(bank)] << 4) + lownybbles[(bank)]); \
   }

static struct
{
   int counter, enabled;
   int latch, wait_state;
} irq;

static int select_c000 = 0;
static uint8 lownybbles[8];
static uint8 highnybbles[8];

static void vrc_init(void)
{
   irq.counter = irq.enabled = 0;
   irq.latch = irq.wait_state = 0;
}

static void map21_write(uint32 address, uint8 value)
{
   switch (address)
   {
   case 0x8000:
      if (select_c000)
         mmc_bankrom(8, 0xC000, value);
      else
         mmc_bankrom(8, 0x8000, value);
      break;

   case 0x9000:
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

      default:
         break;
      }
      break;
   case 0x9002:
      select_c000 = (value & 0x02) >> 1;
      break;
   case 0xA000:
      mmc_bankrom(8, 0xA000, value);
      break;

   case 0xB000:
      VRC_VBANK(0, value, 0);
      break;
   case 0xB002:
   case 0xB040:
      VRC_VBANK(0, value, 1);
      break;
   case 0xB001:
   case 0xB004:
   case 0xB080:
      VRC_VBANK(1, value, 0);
      break;
   case 0xB003:
   case 0xB006:
   case 0xB0C0:
      VRC_VBANK(1, value, 1);
      break;
   case 0xC000:
      VRC_VBANK(2, value, 0);
      break;
   case 0xC002:
   case 0xC040:
      VRC_VBANK(2, value, 1);
      break;
   case 0xC001:
   case 0xC004:
   case 0xC080:
      VRC_VBANK(3, value, 0);
      break;
   case 0xC003:
   case 0xC006:
   case 0xC0C0:
      VRC_VBANK(3, value, 1);
      break;
   case 0xD000:
      VRC_VBANK(4, value, 0);
      break;
   case 0xD002:
   case 0xD040:
      VRC_VBANK(4, value, 1);
      break;
   case 0xD001:
   case 0xD004:
   case 0xD080:
      VRC_VBANK(5, value, 0);
      break;
   case 0xD003:
   case 0xD006:
   case 0xD0C0:
      VRC_VBANK(5, value, 1);
      break;
   case 0xE000:
      VRC_VBANK(6, value, 0);
      break;
   case 0xE002:
   case 0xE040:
      VRC_VBANK(6, value, 1);
      break;
   case 0xE001:
   case 0xE004:
   case 0xE080:
      VRC_VBANK(7, value, 0);
      break;
   case 0xE003:
   case 0xE006:
   case 0xE0C0:
      VRC_VBANK(7, value, 1);
      break;

   case 0xF000:
      irq.latch &= 0xF0;
      irq.latch |= (value & 0x0F);
      break;
   case 0xF002:
   case 0xF040:
      irq.latch &= 0x0F;
      irq.latch |= ((value & 0x0F) << 4);
      break;
   case 0xF004:
   case 0xF001:
   case 0xF080:
      irq.enabled = (value >> 1) & 0x01;
      irq.wait_state = value & 0x01;
      irq.counter = irq.latch;
      break;
   case 0xF006:
   case 0xF003:
   case 0xF0C0:
      irq.enabled = irq.wait_state;
      break;

   default:
#ifdef NOFRENDO_DEBUG
      nofrendo_log_printf("wrote $%02X to $%04X", value, address);
#endif /* !NOFRENDO_DEBUG */
      break;
   }
}

static void map22_write(uint32 address, uint8 value)
{
   int reg = address >> 12;

   switch (reg)
   {
   case 0x8:
      mmc_bankrom(8, 0x8000, value);
      break;

   case 0xA:
      mmc_bankrom(8, 0xA000, value);
      break;

   case 0x9:
      switch (value & 3)
      {
      case 0:
         ppu_mirror(0, 1, 0, 1); /* vertical */
         break;

      case 1:
         ppu_mirror(0, 0, 1, 1); /* horizontal */
         break;

      case 2:
         ppu_mirror(1, 1, 1, 1);
         break;

      case 3:
         ppu_mirror(0, 0, 0, 0);
         break;
      }
      break;

   case 0xB:
   case 0xC:
   case 0xD:
   case 0xE:
   {
      int loc = (((reg - 0xB) << 1) + (address & 1)) << 10;
      mmc_bankvrom(1, loc, value >> 1);
   }
   break;

   default:
      break;
   }
}

static void map23_write(uint32 address, uint8 value)
{
   switch (address)
   {
   case 0x8000:
   case 0x8FFF:
      mmc_bankrom(8, 0x8000, value);
      break;

   case 0xA000:
   case 0xAFFF:
      mmc_bankrom(8, 0xA000, value);
      break;

   case 0x9000:
   case 0x9004:
   case 0x9008:
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
      break;

   case 0xB000:
      VRC_VBANK(0, value, 0);
      break;
   case 0xB001:
   case 0xB004:
      VRC_VBANK(0, value, 1);
      break;
   case 0xB002:
   case 0xB008:
      VRC_VBANK(1, value, 0);
      break;
   case 0xB003:
   case 0xB00C:
      VRC_VBANK(1, value, 1);
      break;
   case 0xC000:
      VRC_VBANK(2, value, 0);
      break;
   case 0xC001:
   case 0xC004:
      VRC_VBANK(2, value, 1);
      break;
   case 0xC002:
   case 0xC008:
      VRC_VBANK(3, value, 0);
      break;
   case 0xC003:
   case 0xC00C:
      VRC_VBANK(3, value, 1);
      break;
   case 0xD000:
      VRC_VBANK(4, value, 0);
      break;
   case 0xD001:
   case 0xD004:
      VRC_VBANK(4, value, 1);
      break;
   case 0xD002:
   case 0xD008:
      VRC_VBANK(5, value, 0);
      break;
   case 0xD003:
   case 0xD00C:
      VRC_VBANK(5, value, 1);
      break;
   case 0xE000:
      VRC_VBANK(6, value, 0);
      break;
   case 0xE001:
   case 0xE004:
      VRC_VBANK(6, value, 1);
      break;
   case 0xE002:
   case 0xE008:
      VRC_VBANK(7, value, 0);
      break;
   case 0xE003:
   case 0xE00C:
      VRC_VBANK(7, value, 1);
      break;

   case 0xF000:
      irq.latch &= 0xF0;
      irq.latch |= (value & 0x0F);
      break;

   case 0xF004:
      irq.latch &= 0x0F;
      irq.latch |= ((value & 0x0F) << 4);
      break;

   case 0xF008:
      irq.enabled = (value >> 1) & 0x01;
      irq.wait_state = value & 0x01;
      irq.counter = irq.latch;
      break;

   case 0xF00C:
      irq.enabled = irq.wait_state;
      break;

   default:
#ifdef NOFRENDO_DEBUG
      nofrendo_log_printf("wrote $%02X to $%04X", value, address);
#endif /* !NOFRENDO_DEBUG */
      break;
   }
}

static void vrc_hblank(int vblank)
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

static map_memwrite map21_memwrite[] =
    {
        {0x8000, 0xFFFF, map21_write},
        {-1, -1, NULL}};

static map_memwrite map22_memwrite[] =
    {
        {0x8000, 0xFFFF, map22_write},
        {-1, -1, NULL}};

static map_memwrite map23_memwrite[] =
    {
        {0x8000, 0xFFFF, map23_write},
        {-1, -1, NULL}};

static void map21_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper21.irqCounter = irq.counter;
   state->extraData.mapper21.irqCounterEnabled = irq.enabled;
}

static void map21_setstate(SnssMapperBlock *state)
{
   irq.counter = state->extraData.mapper21.irqCounter;
   irq.enabled = state->extraData.mapper21.irqCounterEnabled;
}

mapintf_t map21_intf =
    {
        21,              /* mapper number */
        "Konami VRC4 A", /* mapper name */
        vrc_init,        /* init routine */
        NULL,            /* vblank callback */
        vrc_hblank,      /* hblank callback */
        map21_getstate,  /* get state (snss) */
        map21_setstate,  /* set state (snss) */
        NULL,            /* memory read structure */
        map21_memwrite,  /* memory write structure */
        NULL             /* external sound device */
};

mapintf_t map22_intf =
    {
        22,              /* mapper number */
        "Konami VRC2 A", /* mapper name */
        vrc_init,        /* init routine */
        NULL,            /* vblank callback */
        NULL,            /* hblank callback */
        NULL,            /* get state (snss) */
        NULL,            /* set state (snss) */
        NULL,            /* memory read structure */
        map22_memwrite,  /* memory write structure */
        NULL             /* external sound device */
};

mapintf_t map23_intf =
    {
        23,              /* mapper number */
        "Konami VRC2 B", /* mapper name */
        vrc_init,        /* init routine */
        NULL,            /* vblank callback */
        vrc_hblank,      /* hblank callback */
        NULL,            /* get state (snss) */
        NULL,            /* set state (snss) */
        NULL,            /* memory read structure */
        map23_memwrite,  /* memory write structure */
        NULL             /* external sound device */
};

mapintf_t map25_intf =
    {
        25,              /* mapper number */
        "Konami VRC4 B", /* mapper name */
        NULL,            /* init routine */
        NULL,            /* vblank callback */
        vrc_hblank,      /* hblank callback */
        NULL,            /* get state (snss) */
        NULL,            /* set state (snss) */
        NULL,            /* memory read structure */
        map21_memwrite,  /* memory write structure */
        NULL             /* external sound device */
};

/*
** $Log: mapvrc.c,v $
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
** Revision 1.5  2000/07/15 23:52:20  matt
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
** Revision 1.1  2000/07/06 01:01:56  matt
** initial revision
**
*/
