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
** nes_mmc.c
**
** NES Memory Management Controller (mapper) emulation
** $Id: nes_mmc.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <string.h>

#include "../noftypes.h"
#include "../cpu/nes6502.h"
#include "nes_mmc.h"
#include "nes_ppu.h"
#include "../libsnss/libsnss.h"
#include "../log.h"
#include "mmclist.h"
#include "nes_rom.h"

#define MMC_8KROM (mmc.cart->rom_banks * 2)
#define MMC_16KROM (mmc.cart->rom_banks)
#define MMC_32KROM (mmc.cart->rom_banks / 2)
#define MMC_8KVROM (mmc.cart->vrom_banks)
#define MMC_4KVROM (mmc.cart->vrom_banks * 2)
#define MMC_2KVROM (mmc.cart->vrom_banks * 4)
#define MMC_1KVROM (mmc.cart->vrom_banks * 8)

#define MMC_LAST8KROM (MMC_8KROM - 1)
#define MMC_LAST16KROM (MMC_16KROM - 1)
#define MMC_LAST32KROM (MMC_32KROM - 1)
#define MMC_LAST8KVROM (MMC_8KVROM - 1)
#define MMC_LAST4KVROM (MMC_4KVROM - 1)
#define MMC_LAST2KVROM (MMC_2KVROM - 1)
#define MMC_LAST1KVROM (MMC_1KVROM - 1)

static mmc_t mmc;

rominfo_t *mmc_getinfo(void)
{
   return mmc.cart;
}

void mmc_setcontext(mmc_t *src_mmc)
{
   ASSERT(src_mmc);

   mmc = *src_mmc;
}

void mmc_getcontext(mmc_t *dest_mmc)
{
   *dest_mmc = mmc;
}

/* VROM bankswitching */
void mmc_bankvrom(int size, uint32 address, int bank)
{
   if (0 == mmc.cart->vrom_banks)
      return;

   switch (size)
   {
   case 1:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST1KVROM;
      ppu_setpage(1, address >> 10, &mmc.cart->vrom[(bank % MMC_1KVROM) << 10] - address);
      break;

   case 2:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST2KVROM;
      ppu_setpage(2, address >> 10, &mmc.cart->vrom[(bank % MMC_2KVROM) << 11] - address);
      break;

   case 4:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST4KVROM;
      ppu_setpage(4, address >> 10, &mmc.cart->vrom[(bank % MMC_4KVROM) << 12] - address);
      break;

   case 8:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST8KVROM;
      ppu_setpage(8, 0, &mmc.cart->vrom[(bank % MMC_8KVROM) << 13]);
      break;

   default:
      nofrendo_log_printf("invalid VROM bank size %d\n", size);
   }
}

/* ROM bankswitching */
void mmc_bankrom(int size, uint32 address, int bank)
{
   nes6502_context mmc_cpu;

   nes6502_getcontext(&mmc_cpu);

   switch (size)
   {
   case 8:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST8KROM;
      {
         int page = address >> NES6502_BANKSHIFT;
         mmc_cpu.mem_page[page] = &mmc.cart->rom[(bank % MMC_8KROM) << 13];
         mmc_cpu.mem_page[page + 1] = mmc_cpu.mem_page[page] + 0x1000;
      }

      break;

   case 16:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST16KROM;
      {
         int page = address >> NES6502_BANKSHIFT;
         mmc_cpu.mem_page[page] = &mmc.cart->rom[(bank % MMC_16KROM) << 14];
         mmc_cpu.mem_page[page + 1] = mmc_cpu.mem_page[page] + 0x1000;
         mmc_cpu.mem_page[page + 2] = mmc_cpu.mem_page[page] + 0x2000;
         mmc_cpu.mem_page[page + 3] = mmc_cpu.mem_page[page] + 0x3000;
      }
      break;

   case 32:
      if (bank == MMC_LASTBANK)
         bank = MMC_LAST32KROM;

      mmc_cpu.mem_page[8] = &mmc.cart->rom[(bank % MMC_32KROM) << 15];
      mmc_cpu.mem_page[9] = mmc_cpu.mem_page[8] + 0x1000;
      mmc_cpu.mem_page[10] = mmc_cpu.mem_page[8] + 0x2000;
      mmc_cpu.mem_page[11] = mmc_cpu.mem_page[8] + 0x3000;
      mmc_cpu.mem_page[12] = mmc_cpu.mem_page[8] + 0x4000;
      mmc_cpu.mem_page[13] = mmc_cpu.mem_page[8] + 0x5000;
      mmc_cpu.mem_page[14] = mmc_cpu.mem_page[8] + 0x6000;
      mmc_cpu.mem_page[15] = mmc_cpu.mem_page[8] + 0x7000;
      break;

   default:
      nofrendo_log_printf("invalid ROM bank size %d\n", size);
      break;
   }

   nes6502_setcontext(&mmc_cpu);
}

/* Check to see if this mapper is supported */
bool mmc_peek(int map_num)
{
   mapintf_t **map_ptr = mappers;

   while (NULL != *map_ptr)
   {
      if ((*map_ptr)->number == map_num)
         return true;
      map_ptr++;
   }

   return false;
}

static void mmc_setpages(void)
{
   nofrendo_log_printf("setting up mapper %d\n", mmc.intf->number);

   /* Switch ROM into CPU space, set VROM/VRAM (done for ALL ROMs) */
   mmc_bankrom(16, 0x8000, 0);
   mmc_bankrom(16, 0xC000, MMC_LASTBANK);
   mmc_bankvrom(8, 0x0000, 0);

   if (mmc.cart->flags & ROM_FLAG_FOURSCREEN)
   {
      ppu_mirror(0, 1, 2, 3);
   }
   else
   {
      if (MIRROR_VERT == mmc.cart->mirror)
         ppu_mirror(0, 1, 0, 1);
      else
         ppu_mirror(0, 0, 1, 1);
   }

   /* if we have no VROM, switch in VRAM */
   /* TODO: fix this hack implementation */
   if (0 == mmc.cart->vrom_banks)
   {
      ASSERT(mmc.cart->vram);

      ppu_setpage(8, 0, mmc.cart->vram);
      ppu_mirrorhipages();
   }
}

/* Mapper initialization routine */
void mmc_reset(void)
{
   mmc_setpages();

   ppu_setlatchfunc(NULL);
   ppu_setvromswitch(NULL);

   if (mmc.intf->init)
      mmc.intf->init();

   nofrendo_log_printf("reset memory mapper\n");
}

void mmc_destroy(mmc_t **nes_mmc)
{
   if (*nes_mmc)
      NOFRENDO_FREE(*nes_mmc);
}

mmc_t *mmc_create(rominfo_t *rominfo)
{
   mmc_t *temp;
   mapintf_t **map_ptr;

   for (map_ptr = mappers; (*map_ptr)->number != rominfo->mapper_number; map_ptr++)
   {
      if (NULL == *map_ptr)
         return NULL; /* Should *never* happen */
   }

   temp = NOFRENDO_MALLOC(sizeof(mmc_t));
   if (NULL == temp)
      return NULL;

   memset(temp, 0, sizeof(mmc_t));

   temp->intf = *map_ptr;
   temp->cart = rominfo;

   mmc_setcontext(temp);

   nofrendo_log_printf("created memory mapper: %s\n", (*map_ptr)->name);

   return temp;
}

/*
** $Log: nes_mmc.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.4  2000/11/21 13:28:40  matt
** take care to zero allocated mem
**
** Revision 1.3  2000/10/27 12:55:58  matt
** nes6502 now uses 4kB banks across the boards
**
** Revision 1.2  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.28  2000/10/22 19:17:24  matt
** mapper cleanups galore
**
** Revision 1.27  2000/10/22 15:02:32  matt
** simplified mirroring
**
** Revision 1.26  2000/10/21 19:38:56  matt
** that two year old crap code *was* flushed
**
** Revision 1.25  2000/10/21 19:26:59  matt
** many more cleanups
**
** Revision 1.24  2000/10/17 03:22:57  matt
** cleaning up rom module
**
** Revision 1.23  2000/10/10 13:58:15  matt
** stroustrup squeezing his way in the door
**
** Revision 1.22  2000/08/16 02:51:55  matt
** random cleanups
**
** Revision 1.21  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.20  2000/07/25 02:25:53  matt
** safer xxx_destroy calls
**
** Revision 1.19  2000/07/23 15:11:45  matt
** removed unused variables
**
** Revision 1.18  2000/07/15 23:50:03  matt
** migrated state get/set from nes_mmc.c to state.c
**
** Revision 1.17  2000/07/11 03:15:09  melanson
** Added support for mappers 16, 34, and 231
**
** Revision 1.16  2000/07/10 05:27:41  matt
** cleaned up mapper-specific callbacks
**
** Revision 1.15  2000/07/10 03:02:49  matt
** minor change on loading state
**
** Revision 1.14  2000/07/06 17:38:49  matt
** replaced missing string.h include
**
** Revision 1.13  2000/07/06 02:47:11  matt
** mapper addition madness
**
** Revision 1.12  2000/07/05 05:04:15  matt
** added more mappers
**
** Revision 1.11  2000/07/04 23:12:58  matt
** brand spankin' new mapper interface implemented
**
** Revision 1.10  2000/07/04 04:56:36  matt
** modifications for new SNSS
**
** Revision 1.9  2000/06/29 14:17:18  matt
** uses snsslib now
**
** Revision 1.8  2000/06/29 03:09:24  matt
** modified to support new snss code
**
** Revision 1.7  2000/06/26 04:57:54  matt
** bugfix - irqs/mmcstate not cleared on reset
**
** Revision 1.6  2000/06/23 11:01:10  matt
** updated for new external sound interface
**
** Revision 1.5  2000/06/20 04:04:57  matt
** hacked to use new external soundchip struct
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
