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
** nes.c
**
** NES hardware related routines
** $Id: nes.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../noftypes.h"
#include "../cpu/nes6502.h"
#include "../log.h"
#include "../osd.h"
#include "../gui.h"
#include "nes.h"
#include "../sndhrdw/nes_apu.h"
#include "nes_ppu.h"
#include "nes_rom.h"
#include "nes_mmc.h"
#include "../vid_drv.h"
#include "../nofrendo.h"

#define NES_CLOCK_DIVIDER 12
//#define  NES_MASTER_CLOCK     21477272.727272727272
#define NES_MASTER_CLOCK (236250000 / 11)
#define NES_SCANLINE_CYCLES (1364.0 / NES_CLOCK_DIVIDER)
#define NES_FIQ_PERIOD (NES_MASTER_CLOCK / NES_CLOCK_DIVIDER / 60)

#define NES_RAMSIZE 0x800

#define NES_SKIP_LIMIT (NES_REFRESH_RATE / 5) /* 12 or 10, depending on PAL/NTSC */

static nes_t nes;

/* find out if a file is ours */
int nes_isourfile(const char *filename)
{
   return rom_checkmagic(filename);
}

/* TODO: just asking for problems -- please remove */
nes_t *nes_getcontextptr(void)
{
   return &nes;
}

void nes_getcontext(nes_t *machine)
{
   apu_getcontext(nes.apu);
   ppu_getcontext(nes.ppu);
   nes6502_getcontext(nes.cpu);
   mmc_getcontext(nes.mmc);

   *machine = nes;
}

void nes_setcontext(nes_t *machine)
{
   ASSERT(machine);

   apu_setcontext(machine->apu);
   ppu_setcontext(machine->ppu);
   nes6502_setcontext(machine->cpu);
   mmc_setcontext(machine->mmc);

   nes = *machine;
}

static uint8 ram_read(uint32 address)
{
   return nes.cpu->mem_page[0][address & (NES_RAMSIZE - 1)];
}

static void ram_write(uint32 address, uint8 value)
{
   nes.cpu->mem_page[0][address & (NES_RAMSIZE - 1)] = value;
}

static void write_protect(uint32 address, uint8 value)
{
   /* don't allow write to go through */
   UNUSED(address);
   UNUSED(value);
}

static uint8 read_protect(uint32 address)
{
   /* don't allow read to go through */
   UNUSED(address);

   return 0xFF;
}

#define LAST_MEMORY_HANDLER \
   {                        \
      -1, -1, NULL          \
   }
/* read/write handlers for standard NES */
static nes6502_memread default_readhandler[] =
    {
        {0x0800, 0x1FFF, ram_read},
        {0x2000, 0x3FFF, ppu_read},
        {0x4000, 0x4015, apu_read},
        {0x4016, 0x4017, ppu_readhigh},
        LAST_MEMORY_HANDLER};

static nes6502_memwrite default_writehandler[] =
    {
        {0x0800, 0x1FFF, ram_write},
        {0x2000, 0x3FFF, ppu_write},
        {0x4000, 0x4013, apu_write},
        {0x4015, 0x4015, apu_write},
        {0x4014, 0x4017, ppu_writehigh},
        LAST_MEMORY_HANDLER};

/* this big nasty boy sets up the address handlers that the CPU uses */
static void build_address_handlers(nes_t *machine)
{
   int count, num_handlers = 0;
   mapintf_t *intf;

   ASSERT(machine);
   intf = machine->mmc->intf;

   memset(machine->readhandler, 0, sizeof(machine->readhandler));
   memset(machine->writehandler, 0, sizeof(machine->writehandler));

   for (count = 0; num_handlers < MAX_MEM_HANDLERS; count++, num_handlers++)
   {
      if (NULL == default_readhandler[count].read_func)
         break;

      memcpy(&machine->readhandler[num_handlers], &default_readhandler[count],
             sizeof(nes6502_memread));
   }

   if (intf->sound_ext)
   {
      if (NULL != intf->sound_ext->mem_read)
      {
         for (count = 0; num_handlers < MAX_MEM_HANDLERS; count++, num_handlers++)
         {
            if (NULL == intf->sound_ext->mem_read[count].read_func)
               break;

            memcpy(&machine->readhandler[num_handlers], &intf->sound_ext->mem_read[count],
                   sizeof(nes6502_memread));
         }
      }
   }

   if (NULL != intf->mem_read)
   {
      for (count = 0; num_handlers < MAX_MEM_HANDLERS; count++, num_handlers++)
      {
         if (NULL == intf->mem_read[count].read_func)
            break;

         memcpy(&machine->readhandler[num_handlers], &intf->mem_read[count],
                sizeof(nes6502_memread));
      }
   }

   /* TODO: poof! numbers */
   machine->readhandler[num_handlers].min_range = 0x4018;
   machine->readhandler[num_handlers].max_range = 0x5FFF;
   machine->readhandler[num_handlers].read_func = read_protect;
   num_handlers++;
   machine->readhandler[num_handlers].min_range = -1;
   machine->readhandler[num_handlers].max_range = -1;
   machine->readhandler[num_handlers].read_func = NULL;
   num_handlers++;
   ASSERT(num_handlers <= MAX_MEM_HANDLERS);

   num_handlers = 0;

   for (count = 0; num_handlers < MAX_MEM_HANDLERS; count++, num_handlers++)
   {
      if (NULL == default_writehandler[count].write_func)
         break;

      memcpy(&machine->writehandler[num_handlers], &default_writehandler[count],
             sizeof(nes6502_memwrite));
   }

   if (intf->sound_ext)
   {
      if (NULL != intf->sound_ext->mem_write)
      {
         for (count = 0; num_handlers < MAX_MEM_HANDLERS; count++, num_handlers++)
         {
            if (NULL == intf->sound_ext->mem_write[count].write_func)
               break;

            memcpy(&machine->writehandler[num_handlers], &intf->sound_ext->mem_write[count],
                   sizeof(nes6502_memwrite));
         }
      }
   }

   if (NULL != intf->mem_write)
   {
      for (count = 0; num_handlers < MAX_MEM_HANDLERS; count++, num_handlers++)
      {
         if (NULL == intf->mem_write[count].write_func)
            break;

         memcpy(&machine->writehandler[num_handlers], &intf->mem_write[count],
                sizeof(nes6502_memwrite));
      }
   }

   /* catch-all for bad writes */
   /* TODO: poof! numbers */
   machine->writehandler[num_handlers].min_range = 0x4018;
   machine->writehandler[num_handlers].max_range = 0x5FFF;
   machine->writehandler[num_handlers].write_func = write_protect;
   num_handlers++;
   machine->writehandler[num_handlers].min_range = 0x8000;
   machine->writehandler[num_handlers].max_range = 0xFFFF;
   machine->writehandler[num_handlers].write_func = write_protect;
   num_handlers++;
   machine->writehandler[num_handlers].min_range = -1;
   machine->writehandler[num_handlers].max_range = -1;
   machine->writehandler[num_handlers].write_func = NULL;
   num_handlers++;
   ASSERT(num_handlers <= MAX_MEM_HANDLERS);
}

/* raise an IRQ */
void nes_irq(void)
{
#ifdef NOFRENDO_DEBUG
#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   if (nes.scanline <= NES_SCREEN_HEIGHT)
      memset(nes.vidbuf->line[nes.scanline - 1], GUI_RED, NES_SCREEN_WIDTH);
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */
#endif /* NOFRENDO_DEBUG */

   nes6502_irq();
}

static uint8 nes_clearfiq(void)
{
   if (nes.fiq_occurred)
   {
      nes.fiq_occurred = false;
      return 0x40;
   }

   return 0;
}

void nes_setfiq(uint8 value)
{
   nes.fiq_state = value;
   nes.fiq_cycles = (int)NES_FIQ_PERIOD;
}

static void nes_checkfiq(int cycles)
{
   nes.fiq_cycles -= cycles;
   if (nes.fiq_cycles <= 0)
   {
      nes.fiq_cycles += (int)NES_FIQ_PERIOD;
      if (0 == (nes.fiq_state & 0xC0))
      {
         nes.fiq_occurred = true;
         nes6502_irq();
      }
   }
}

void nes_nmi(void)
{
   nes6502_nmi();
}

static void nes_renderframe(bool draw_flag)
{
   int elapsed_cycles;
   mapintf_t *mapintf = nes.mmc->intf;
   int in_vblank = 0;

   while (262 != nes.scanline)
   {
#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
      ppu_scanline(nes.vidbuf, nes.scanline, draw_flag);
#else  /* !NOFRENDO_DOUBLE_FRAMEBUFFER */
      ppu_scanline(vid_getbuffer(), nes.scanline, draw_flag);
#endif /* !NOFRENDO_DOUBLE_FRAMEBUFFER */

      if (241 == nes.scanline)
      {
         /* 7-9 cycle delay between when VINT flag goes up and NMI is taken */
         elapsed_cycles = nes6502_execute(7);
         nes.scanline_cycles -= elapsed_cycles;
         nes_checkfiq(elapsed_cycles);

         ppu_checknmi();

         if (mapintf->vblank)
            mapintf->vblank();
         in_vblank = 1;
      }

      if (mapintf->hblank)
         mapintf->hblank(in_vblank);

      nes.scanline_cycles += (float)NES_SCANLINE_CYCLES;
      elapsed_cycles = nes6502_execute((int)nes.scanline_cycles);
      nes.scanline_cycles -= (float)elapsed_cycles;
      nes_checkfiq(elapsed_cycles);

      ppu_endscanline(nes.scanline);
      nes.scanline++;
   }

   nes.scanline = 0;
}

static void system_video(bool draw)
{
   /* TODO: hack */
   if (false == draw)
   {
      gui_frame(false);
      return;
   }

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   /* blit the NES screen to our video surface */
   vid_blit(nes.vidbuf, 0, (NES_SCREEN_HEIGHT - NES_VISIBLE_HEIGHT) / 2,
            0, 0, NES_SCREEN_WIDTH, NES_VISIBLE_HEIGHT);
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   /* overlay our GUI on top of it */
   gui_frame(true);

   /* blit to screen */
   vid_flush();

   /* grab input */
   osd_getinput();
}

/* main emulation loop */
void nes_emulate(void)
{
   int last_ticks, frames_to_render;

   osd_setsound(nes.apu->process);

   last_ticks = nofrendo_ticks;
   frames_to_render = 0;
   nes.scanline_cycles = 0;
   nes.fiq_cycles = (int)NES_FIQ_PERIOD;

   while (false == nes.poweroff)
   {
      if (nofrendo_ticks != last_ticks)
      {
         int tick_diff = nofrendo_ticks - last_ticks;

         frames_to_render += tick_diff;
         gui_tick(tick_diff);
         last_ticks = nofrendo_ticks;
      }

      if (true == nes.pause)
      {
         /* TODO: dim the screen, and pause/silence the apu */
         system_video(true);
         frames_to_render = 0;
      }
      else if (frames_to_render > 1)
      {
         frames_to_render--;
         nes_renderframe(false);
         system_video(false);
      }
      else if ((1 == frames_to_render && true == nes.autoframeskip) || false == nes.autoframeskip)
      {
         frames_to_render = 0;
         nes_renderframe(true);
         system_video(true);
      }
   }
}

static void mem_trash(uint8 *buffer, int length)
{
   int i;

   for (i = 0; i < length; i++)
      buffer[i] = (uint8)rand();
}

/* Reset NES hardware */
void nes_reset(int reset_type)
{
   if (HARD_RESET == reset_type)
   {
      memset(nes.cpu->mem_page[0], 0, NES_RAMSIZE);
      if (nes.rominfo->vram)
         mem_trash(nes.rominfo->vram, 0x2000 * nes.rominfo->vram_banks);
   }

   apu_reset();
   ppu_reset(reset_type);
   mmc_reset();
   nes6502_reset();

   nes.scanline = 241;

   gui_sendmsg(GUI_GREEN, "NES %s",
               (HARD_RESET == reset_type) ? "powered on" : "reset");
}

void nes_destroy(nes_t **machine)
{
   if (*machine)
   {
      rom_free(&(*machine)->rominfo);
      mmc_destroy(&(*machine)->mmc);
      ppu_destroy(&(*machine)->ppu);
      apu_destroy(&(*machine)->apu);

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
      bmp_destroy(&(*machine)->vidbuf);
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

      if ((*machine)->cpu)
      {
         if ((*machine)->cpu->mem_page[0])
            NOFRENDO_FREE((*machine)->cpu->mem_page[0]);
         NOFRENDO_FREE((*machine)->cpu);
      }

      NOFRENDO_FREE(*machine);
      *machine = NULL;
   }
}

void nes_poweroff(void)
{
   nes.poweroff = true;
}

void nes_togglepause(void)
{
   nes.pause ^= true;
}

/* insert a cart into the NES */
int nes_insertcart(const char *filename, nes_t *machine)
{
   nes6502_setcontext(machine->cpu);

   /* rom file */
   machine->rominfo = rom_load(filename, machine->ppu);
   if (NULL == machine->rominfo)
      goto _fail;

   /* map cart's SRAM to CPU $6000-$7FFF */
   if (machine->rominfo->sram)
   {
      machine->cpu->mem_page[6] = machine->rominfo->sram;
      machine->cpu->mem_page[7] = machine->rominfo->sram + 0x1000;
   }

   /* mapper */
   machine->mmc = mmc_create(machine->rominfo);
   if (NULL == machine->mmc)
      goto _fail;

   /* if there's VRAM, let the PPU know */
   if (NULL != machine->rominfo->vram)
      machine->ppu->vram_present = true;

   apu_setext(machine->apu, machine->mmc->intf->sound_ext);

   build_address_handlers(machine);

   nes_setcontext(machine);

   nes_reset(HARD_RESET);
   return 0;

_fail:
   nes_destroy(&machine);
   return -1;
}

/* Initialize NES CPU, hardware, etc. */
nes_t *nes_create(void)
{
   nes_t *machine;
   sndinfo_t osd_sound;
   int i;

   machine = NOFRENDO_MALLOC(sizeof(nes_t));
   if (NULL == machine)
      return NULL;

   memset(machine, 0, sizeof(nes_t));

   /* bitmap */
   /* 8 pixel overdraw */
#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   machine->vidbuf = bmp_create(NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, 8);
   if (NULL == machine->vidbuf)
      goto _fail;
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   machine->autoframeskip = true;

   /* cpu */
   machine->cpu = NOFRENDO_MALLOC(sizeof(nes6502_context));
   if (NULL == machine->cpu)
      goto _fail;

   memset(machine->cpu, 0, sizeof(nes6502_context));

   /* allocate 2kB RAM */
   machine->cpu->mem_page[0] = NOFRENDO_MALLOC(NES_RAMSIZE);
   if (NULL == machine->cpu->mem_page[0])
      goto _fail;

   /* point all pages at NULL for now */
   for (i = 1; i < NES6502_NUMBANKS; i++)
      machine->cpu->mem_page[i] = NULL;

   machine->cpu->read_handler = machine->readhandler;
   machine->cpu->write_handler = machine->writehandler;

   /* apu */
   osd_getsoundinfo(&osd_sound);
   machine->apu = apu_create(0, osd_sound.sample_rate, NES_REFRESH_RATE, osd_sound.bps);

   if (NULL == machine->apu)
      goto _fail;

   /* set the IRQ routines */
   machine->apu->irq_callback = nes_irq;
   machine->apu->irqclear_callback = nes_clearfiq;

   /* ppu */
   machine->ppu = ppu_create();
   if (NULL == machine->ppu)
      goto _fail;

   machine->poweroff = false;
   machine->pause = false;

   return machine;

_fail:
   nes_destroy(&machine);
   return NULL;
}

/*
** $Log: nes.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.18  2000/11/29 12:58:23  matt
** timing/fiq fixes
**
** Revision 1.17  2000/11/27 19:36:15  matt
** more timing fixes
**
** Revision 1.16  2000/11/26 16:13:13  matt
** slight fix (?) to nes_fiq
**
** Revision 1.15  2000/11/26 15:51:13  matt
** frame IRQ emulation
**
** Revision 1.14  2000/11/25 20:30:39  matt
** scanline emulation simplifications/timing fixes
**
** Revision 1.13  2000/11/25 01:53:42  matt
** bool stinks sometimes
**
** Revision 1.12  2000/11/21 13:28:40  matt
** take care to zero allocated mem
**
** Revision 1.11  2000/11/20 13:23:32  matt
** nofrendo.c now handles timer
**
** Revision 1.10  2000/11/09 14:07:27  matt
** state load fixed, state save mostly fixed
**
** Revision 1.9  2000/11/05 22:19:37  matt
** pause buglet fixed
**
** Revision 1.8  2000/11/05 06:27:09  matt
** thinlib spawns changes
**
** Revision 1.7  2000/10/29 14:36:45  matt
** nes_clearframeirq is static
**
** Revision 1.6  2000/10/28 15:20:41  matt
** irq callbacks in nes_apu
**
** Revision 1.5  2000/10/27 12:55:58  matt
** nes6502 now uses 4kB banks across the boards
**
** Revision 1.4  2000/10/25 13:44:02  matt
** no more silly define names
**
** Revision 1.3  2000/10/25 01:23:08  matt
** basic system autodetection
**
** Revision 1.2  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.50  2000/10/23 17:51:09  matt
** adding fds support
**
** Revision 1.49  2000/10/23 15:53:08  matt
** better system handling
**
** Revision 1.48  2000/10/22 20:02:29  matt
** autoframeskip bugfix
**
** Revision 1.47  2000/10/22 19:16:15  matt
** more sane timer ISR / autoframeskip
**
** Revision 1.46  2000/10/21 19:26:59  matt
** many more cleanups
**
** Revision 1.45  2000/10/17 12:00:56  matt
** selectable apu base frequency
**
** Revision 1.44  2000/10/10 13:58:14  matt
** stroustrup squeezing his way in the door
**
** Revision 1.43  2000/10/10 13:05:30  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.42  2000/10/08 17:53:37  matt
** minor accuracy changes
**
** Revision 1.41  2000/09/18 02:09:12  matt
** -pedantic is your friend
**
** Revision 1.40  2000/09/15 13:38:39  matt
** changes for optimized apu core
**
** Revision 1.39  2000/09/15 04:58:07  matt
** simplifying and optimizing APU core
**
** Revision 1.38  2000/09/08 11:57:29  matt
** no more nes_fiq
**
** Revision 1.37  2000/08/31 02:39:01  matt
** moved dos stuff in here (temp)
**
** Revision 1.36  2000/08/16 02:51:55  matt
** random cleanups
**
** Revision 1.35  2000/08/11 02:43:50  matt
** moved frame irq stuff out of APU into here
**
** Revision 1.34  2000/08/11 01:42:43  matt
** change to OSD sound info interface
**
** Revision 1.33  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.32  2000/07/30 04:32:32  matt
** emulation of the NES frame IRQ
**
** Revision 1.31  2000/07/27 04:07:14  matt
** cleaned up the neighborhood lawns
**
** Revision 1.30  2000/07/27 03:59:52  neil
** pausing tweaks, during fullscreen toggles
**
** Revision 1.29  2000/07/27 03:19:22  matt
** just a little cleaner, that's all
**
** Revision 1.28  2000/07/27 02:55:23  matt
** nes_emulate went through detox
**
** Revision 1.27  2000/07/27 02:49:18  matt
** cleaner flow in nes_emulate
**
** Revision 1.26  2000/07/27 01:17:09  matt
** nes_insertrom -> nes_insertcart
**
** Revision 1.25  2000/07/26 21:36:14  neil
** Big honkin' change -- see the mailing list
**
** Revision 1.24  2000/07/25 02:25:53  matt
** safer xxx_destroy calls
**
** Revision 1.23  2000/07/24 04:32:40  matt
** autoframeskip bugfix
**
** Revision 1.22  2000/07/23 15:13:48  matt
** apu API change, autoframeskip part of nes_t struct
**
** Revision 1.21  2000/07/21 02:44:41  matt
** merged osd_getinput and osd_gethostinput
**
** Revision 1.20  2000/07/17 05:12:55  matt
** nes_ppu.c is no longer a scary place to be-- cleaner & faster
**
** Revision 1.19  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.18  2000/07/15 23:51:23  matt
** hack for certain filthy NES titles
**
** Revision 1.17  2000/07/11 04:31:54  matt
** less magic number nastiness for screen dimensions
**
** Revision 1.16  2000/07/11 02:38:25  matt
** encapsulated memory address handlers into nes/nsf
**
** Revision 1.15  2000/07/10 13:50:49  matt
** added function nes_irq()
**
** Revision 1.14  2000/07/10 05:27:55  matt
** cleaned up mapper-specific callbacks
**
** Revision 1.13  2000/07/09 03:43:26  matt
** minor changes to gui handling
**
** Revision 1.12  2000/07/06 16:42:23  matt
** updated for new video driver
**
** Revision 1.11  2000/07/05 19:57:36  neil
** __GNUC -> __DJGPP in nes.c
**
** Revision 1.10  2000/07/05 12:23:03  matt
** removed unnecessary references
**
** Revision 1.9  2000/07/04 23:12:34  matt
** memory protection handlers
**
** Revision 1.8  2000/07/04 04:58:29  matt
** dynamic memory range handlers
**
** Revision 1.7  2000/06/26 04:58:51  matt
** minor bugfix
**
** Revision 1.6  2000/06/20 20:42:12  matt
** fixed some NULL pointer problems
**
** Revision 1.5  2000/06/09 15:12:26  matt
** initial revision
**
*/
