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
** nes_ppu.c
**
** NES PPU emulation
** $Id: nes_ppu.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <string.h>
#include <stdlib.h>

#include "../noftypes.h"
#include "nes_ppu.h"
#include "nes.h"
#include "../gui.h"
#include "../cpu/nes6502.h"
#include "../log.h"
#include "nes_mmc.h"

#include "../bitmap.h"
#include "../vid_drv.h"
#include "nes_pal.h"
#include "nesinput.h"

/* PPU access */
#define PPU_MEM(x) ppu.page[(x) >> 10][(x)]

/* Background (color 0) and solid sprite pixel flags */
#define BG_TRANS 0x80
#define SP_PIXEL 0x40
#define BG_CLEAR(V) ((V)&BG_TRANS)
#define BG_SOLID(V) (0 == BG_CLEAR(V))
#define SP_CLEAR(V) (0 == ((V)&SP_PIXEL))

/* Full BG color */
#define FULLBG (ppu.palette[0] | BG_TRANS)

/* the NES PPU */
static ppu_t ppu;

void ppu_displaysprites(bool display)
{
   ppu.drawsprites = display;
}

void ppu_setcontext(ppu_t *src_ppu)
{
   int nametab[4];
   ASSERT(src_ppu);
   ppu = *src_ppu;

   /* we can't just copy contexts here, because more than likely,
   ** the top 8 pages of the ppu are pointing to internal PPU memory,
   ** which means we need to recalculate the page pointers.
   ** TODO: we can either get rid of the page pointing in the code,
   ** or add more robust checks to make sure that pages 8-15 are
   ** definitely pointing to internal PPU RAM, not just something
   ** that some crazy mapper paged in.
   */
   nametab[0] = (src_ppu->page[8] - src_ppu->nametab + 0x2000) >> 10;
   nametab[1] = (src_ppu->page[9] - src_ppu->nametab + 0x2400) >> 10;
   nametab[2] = (src_ppu->page[10] - src_ppu->nametab + 0x2800) >> 10;
   nametab[3] = (src_ppu->page[11] - src_ppu->nametab + 0x2C00) >> 10;

   ppu.page[8] = ppu.nametab + (nametab[0] << 10) - 0x2000;
   ppu.page[9] = ppu.nametab + (nametab[1] << 10) - 0x2400;
   ppu.page[10] = ppu.nametab + (nametab[2] << 10) - 0x2800;
   ppu.page[11] = ppu.nametab + (nametab[3] << 10) - 0x2C00;
   ppu.page[12] = ppu.page[8] - 0x1000;
   ppu.page[13] = ppu.page[9] - 0x1000;
   ppu.page[14] = ppu.page[10] - 0x1000;
   ppu.page[15] = ppu.page[11] - 0x1000;
}

void ppu_getcontext(ppu_t *dest_ppu)
{
   int nametab[4];

   ASSERT(dest_ppu);
   *dest_ppu = ppu;

   /* we can't just copy contexts here, because more than likely,
   ** the top 8 pages of the ppu are pointing to internal PPU memory,
   ** which means we need to recalculate the page pointers.
   ** TODO: we can either get rid of the page pointing in the code,
   ** or add more robust checks to make sure that pages 8-15 are
   ** definitely pointing to internal PPU RAM, not just something
   ** that some crazy mapper paged in.
   */
   nametab[0] = (ppu.page[8] - ppu.nametab + 0x2000) >> 10;
   nametab[1] = (ppu.page[9] - ppu.nametab + 0x2400) >> 10;
   nametab[2] = (ppu.page[10] - ppu.nametab + 0x2800) >> 10;
   nametab[3] = (ppu.page[11] - ppu.nametab + 0x2C00) >> 10;

   dest_ppu->page[8] = dest_ppu->nametab + (nametab[0] << 10) - 0x2000;
   dest_ppu->page[9] = dest_ppu->nametab + (nametab[1] << 10) - 0x2400;
   dest_ppu->page[10] = dest_ppu->nametab + (nametab[2] << 10) - 0x2800;
   dest_ppu->page[11] = dest_ppu->nametab + (nametab[3] << 10) - 0x2C00;
   dest_ppu->page[12] = dest_ppu->page[8] - 0x1000;
   dest_ppu->page[13] = dest_ppu->page[9] - 0x1000;
   dest_ppu->page[14] = dest_ppu->page[10] - 0x1000;
   dest_ppu->page[15] = dest_ppu->page[11] - 0x1000;
}

ppu_t *ppu_create(void)
{
   static bool pal_generated = false;
   ppu_t *temp;

   temp = NOFRENDO_MALLOC(sizeof(ppu_t));
   if (NULL == temp)
      return NULL;

   memset(temp, 0, sizeof(ppu_t));

   temp->latchfunc = NULL;
   temp->vromswitch = NULL;
   temp->vram_present = false;
   temp->drawsprites = true;

   /* TODO: probably a better way to do this... */
   if (false == pal_generated)
   {
      pal_generate();
      pal_generated = true;
   }

   ppu_setdefaultpal(temp);

   return temp;
}

void ppu_destroy(ppu_t **src_ppu)
{
   if (*src_ppu)
   {
      NOFRENDO_FREE(*src_ppu);
      *src_ppu = NULL;
   }
}

void ppu_setpage(int size, int page_num, uint8 *location)
{
   /* deliberately fall through */
   switch (size)
   {
   case 8:
      ppu.page[page_num++] = location;
      ppu.page[page_num++] = location;
      ppu.page[page_num++] = location;
      ppu.page[page_num++] = location;
   case 4:
      ppu.page[page_num++] = location;
      ppu.page[page_num++] = location;
   case 2:
      ppu.page[page_num++] = location;
   case 1:
      ppu.page[page_num++] = location;
      break;
   }
}

/* make sure $3000-$3F00 mirrors $2000-$2F00 */
void ppu_mirrorhipages(void)
{
   ppu.page[12] = ppu.page[8] - 0x1000;
   ppu.page[13] = ppu.page[9] - 0x1000;
   ppu.page[14] = ppu.page[10] - 0x1000;
   ppu.page[15] = ppu.page[11] - 0x1000;
}

void ppu_mirror(int nt1, int nt2, int nt3, int nt4)
{
   ppu.page[8] = ppu.nametab + (nt1 << 10) - 0x2000;
   ppu.page[9] = ppu.nametab + (nt2 << 10) - 0x2400;
   ppu.page[10] = ppu.nametab + (nt3 << 10) - 0x2800;
   ppu.page[11] = ppu.nametab + (nt4 << 10) - 0x2C00;
   ppu.page[12] = ppu.page[8] - 0x1000;
   ppu.page[13] = ppu.page[9] - 0x1000;
   ppu.page[14] = ppu.page[10] - 0x1000;
   ppu.page[15] = ppu.page[11] - 0x1000;
}

/* bleh, for snss */
uint8 *ppu_getpage(int page)
{
   return ppu.page[page];
}

static void mem_trash(uint8 *buffer, int length)
{
   int i;

   for (i = 0; i < length; i++)
      buffer[i] = (uint8)rand();
}

/* reset state of ppu */
void ppu_reset(int reset_type)
{
   if (HARD_RESET == reset_type)
      mem_trash(ppu.oam, 256);

   ppu.ctrl0 = 0;
   ppu.ctrl1 = PPU_CTRL1F_OBJON | PPU_CTRL1F_BGON;
   ppu.stat = 0;
   ppu.flipflop = 0;
   ppu.vaddr = ppu.vaddr_latch = 0x2000;
   ppu.oam_addr = 0;
   ppu.tile_xofs = 0;

   ppu.latch = 0;
   ppu.vram_accessible = true;
}

/* we render a scanline of graphics first so we know exactly
** where the sprite 0 strike is going to occur (in terms of
** cpu cycles), using the relation that 3 pixels == 1 cpu cycle
*/
static void ppu_setstrike(int x_loc)
{
   if (false == ppu.strikeflag)
   {
      ppu.strikeflag = true;

      /* 3 pixels per cpu cycle */
      ppu.strike_cycle = nes6502_getcycles(false) + (x_loc / 3);
   }
}

static void ppu_oamdma(uint8 value)
{
   uint32 cpu_address;
   uint8 oam_loc;

   cpu_address = (uint32)(value << 8);

   /* Sprite DMA starts at the current SPRRAM address */
   oam_loc = ppu.oam_addr;
   do
   {
      ppu.oam[oam_loc++] = nes6502_getbyte(cpu_address++);
   } while (oam_loc != ppu.oam_addr);

   /* TODO: enough with houdini */
   cpu_address -= 256;
   /* Odd address in $2003 */
   if ((ppu.oam_addr >> 2) & 1)
   {
      for (oam_loc = 4; oam_loc < 8; oam_loc++)
         ppu.oam[oam_loc] = nes6502_getbyte(cpu_address++);
      cpu_address += 248;
      for (oam_loc = 0; oam_loc < 4; oam_loc++)
         ppu.oam[oam_loc] = nes6502_getbyte(cpu_address++);
   }
   /* Even address in $2003 */
   else
   {
      for (oam_loc = 0; oam_loc < 8; oam_loc++)
         ppu.oam[oam_loc] = nes6502_getbyte(cpu_address++);
   }

   /* make the CPU spin for DMA cycles */
   nes6502_burn(513);
   nes6502_release();
}

/* TODO: this isn't the PPU! */
void ppu_writehigh(uint32 address, uint8 value)
{
   switch (address)
   {
   case PPU_OAMDMA:
      ppu_oamdma(value);
      break;

   case PPU_JOY0:
      /* VS system VROM switching - bleh!*/
      if (ppu.vromswitch)
         ppu.vromswitch(value);

      /* see if we need to strobe them joypads */
      value &= 1;

      if (0 == value && ppu.strobe)
         input_strobe();

      ppu.strobe = value;
      break;

   case PPU_JOY1: /* frame IRQ control */
      nes_setfiq(value);
      break;

   default:
      break;
   }
}

/* TODO: this isn't the PPU! */
uint8 ppu_readhigh(uint32 address)
{
   uint8 value;

   switch (address)
   {
   case PPU_JOY0:
      value = input_get(INP_JOYPAD0);
      break;

   case PPU_JOY1:
      /* TODO: better input handling */
      value = input_get(INP_ZAPPER | INP_JOYPAD1
                        /*| INP_ARKANOID*/
                        /*| INP_POWERPAD*/);
      break;

   default:
      value = 0xFF;
      break;
   }

   return value;
}

/* Read from $2000-$2007 */
uint8 ppu_read(uint32 address)
{
   uint8 value;

   /* handle mirrored reads up to $3FFF */
   switch (address & 0x2007)
   {
   case PPU_STAT:
      value = (ppu.stat & 0xE0) | (ppu.latch & 0x1F);

      if (ppu.strikeflag)
      {
         if (nes6502_getcycles(false) >= ppu.strike_cycle)
            value |= PPU_STATF_STRIKE;
      }

      /* clear both vblank flag and vram address flipflop */
      ppu.stat &= ~PPU_STATF_VBLANK;
      ppu.flipflop = 0;
      break;

   case PPU_VDATA:
      /* buffered VRAM reads */
      value = ppu.latch = ppu.vdata_latch;

      /* VRAM only accessible during VBL */
      if ((ppu.bg_on || ppu.obj_on) && !ppu.vram_accessible)
      {
         ppu.vdata_latch = 0xFF;
#ifdef NOFRENDO_VRAM_DEBUG
         nofrendo_log_printf("VRAM read at $%04X, scanline %d\n",
                             ppu.vaddr, nes_getcontextptr()->scanline);
#endif /* NOFRENDO_VRAM_DEBUG */
      }
      else
      {
         uint32 addr = ppu.vaddr;
         if (addr >= 0x3000)
            addr -= 0x1000;
         ppu.vdata_latch = PPU_MEM(addr);
      }

      ppu.vaddr += ppu.vaddr_inc;
      ppu.vaddr &= 0x3FFF;
      break;

   case PPU_OAMDATA:
   case PPU_CTRL0:
   case PPU_CTRL1:
   case PPU_OAMADDR:
   case PPU_SCROLL:
   case PPU_VADDR:
   default:
      value = ppu.latch;
      break;
   }

   return value;
}

/* Write to $2000-$2007 */
void ppu_write(uint32 address, uint8 value)
{
   /* write goes into ppu latch... */
   ppu.latch = value;

   switch (address & 0x2007)
   {
   case PPU_CTRL0:
      ppu.ctrl0 = value;

      ppu.obj_height = (value & PPU_CTRL0F_OBJ16) ? 16 : 8;
      ppu.bg_base = (value & PPU_CTRL0F_BGADDR) ? 0x1000 : 0;
      ppu.obj_base = (value & PPU_CTRL0F_OBJADDR) ? 0x1000 : 0;
      ppu.vaddr_inc = (value & PPU_CTRL0F_ADDRINC) ? 32 : 1;
      ppu.tile_nametab = value & PPU_CTRL0F_NAMETAB;

      /* Mask out bits 10 & 11 in the ppu latch */
      ppu.vaddr_latch &= ~0x0C00;
      ppu.vaddr_latch |= ((value & 3) << 10);
      break;

   case PPU_CTRL1:
      ppu.ctrl1 = value;

      ppu.obj_on = (value & PPU_CTRL1F_OBJON) ? true : false;
      ppu.bg_on = (value & PPU_CTRL1F_BGON) ? true : false;
      ppu.obj_mask = (value & PPU_CTRL1F_OBJMASK) ? false : true;
      ppu.bg_mask = (value & PPU_CTRL1F_BGMASK) ? false : true;
      break;

   case PPU_OAMADDR:
      ppu.oam_addr = value;
      break;

   case PPU_OAMDATA:
      ppu.oam[ppu.oam_addr++] = value;
      break;

   case PPU_SCROLL:
      if (0 == ppu.flipflop)
      {
         /* Mask out bits 4 - 0 in the ppu latch */
         ppu.vaddr_latch &= ~0x001F;
         ppu.vaddr_latch |= (value >> 3); /* Tile number */
         ppu.tile_xofs = (value & 7);     /* Tile offset (0-7 pix) */
      }
      else
      {
         /* Mask out bits 14-12 and 9-5 in the ppu latch */
         ppu.vaddr_latch &= ~0x73E0;
         ppu.vaddr_latch |= ((value & 0xF8) << 2); /* Tile number */
         ppu.vaddr_latch |= ((value & 7) << 12);   /* Tile offset (0-7 pix) */
      }

      ppu.flipflop ^= 1;

      break;

   case PPU_VADDR:
      if (0 == ppu.flipflop)
      {
         /* Mask out bits 15-8 in ppu latch */
         ppu.vaddr_latch &= ~0xFF00;
         ppu.vaddr_latch |= ((value & 0x3F) << 8);
      }
      else
      {
         /* Mask out bits 7-0 in ppu latch */
         ppu.vaddr_latch &= ~0x00FF;
         ppu.vaddr_latch |= value;
         ppu.vaddr = ppu.vaddr_latch;
      }

      ppu.flipflop ^= 1;

      break;

   case PPU_VDATA:
      if (ppu.vaddr < 0x3F00)
      {
         /* VRAM only accessible during scanlines 241-260 */
         if ((ppu.bg_on || ppu.obj_on) && !ppu.vram_accessible)
         {
            nofrendo_log_printf("VRAM write to $%04X, scanline %d\n",
                                ppu.vaddr, nes_getcontextptr()->scanline);
            PPU_MEM(ppu.vaddr) = 0xFF; /* corrupt */
         }
         else
         {
            uint32 addr = ppu.vaddr;

            if (false == ppu.vram_present && addr >= 0x3000)
               ppu.vaddr -= 0x1000;

            PPU_MEM(addr) = value;
         }
      }
      else
      {
         if (0 == (ppu.vaddr & 0x0F))
         {
            int i;

            for (i = 0; i < 8; i++)
               ppu.palette[i << 2] = (value & 0x3F) | BG_TRANS;
         }
         else if (ppu.vaddr & 3)
         {
            ppu.palette[ppu.vaddr & 0x1F] = value & 0x3F;
         }
      }

      ppu.vaddr += ppu.vaddr_inc;
      ppu.vaddr &= 0x3FFF;
      break;

   default:
      break;
   }
}

/* Builds a 256 color 8-bit palette based on a 64-color NES palette
** Note that we set it up 3 times so that we flip bits on the primary
** NES buffer for priorities
*/
static void ppu_buildpalette(ppu_t *src_ppu, rgb_t *pal)
{
   int i;

   /* Set it up 3 times, for sprite priority/BG transparency trickery */
   for (i = 0; i < 64; i++)
   {
      src_ppu->curpal[i].r = src_ppu->curpal[i + 64].r = src_ppu->curpal[i + 128].r = pal[i].r;
      src_ppu->curpal[i].g = src_ppu->curpal[i + 64].g = src_ppu->curpal[i + 128].g = pal[i].g;
      src_ppu->curpal[i].b = src_ppu->curpal[i + 64].b = src_ppu->curpal[i + 128].b = pal[i].b;
   }

   for (i = 0; i < GUI_TOTALCOLORS; i++)
   {
      src_ppu->curpal[i + 192].r = gui_pal[i].r;
      src_ppu->curpal[i + 192].g = gui_pal[i].g;
      src_ppu->curpal[i + 192].b = gui_pal[i].b;
   }
}

/* build the emulator specific palette based on a 64-entry palette
** input palette can be either nes_palette or a 64-entry RGB palette
** read in from disk (i.e. for VS games)
*/
void ppu_setpal(ppu_t *src_ppu, rgb_t *pal)
{
   ppu_buildpalette(src_ppu, pal);
   vid_setpalette(src_ppu->curpal);
}

void ppu_setdefaultpal(ppu_t *src_ppu)
{
   ppu_setpal(src_ppu, nes_palette);
}

void ppu_setlatchfunc(ppulatchfunc_t func)
{
   ppu.latchfunc = func;
}

void ppu_setvromswitch(ppuvromswitch_t func)
{
   ppu.vromswitch = func;
}

/* rendering routines */
INLINE void draw_bgtile(uint8 *surface, uint8 pat1, uint8 pat2,
                        const uint8 *colors)
{
   uint32 pattern = ((pat2 & 0xAA) << 8) | ((pat2 & 0x55) << 1) | ((pat1 & 0xAA) << 7) | (pat1 & 0x55);

   *surface++ = colors[(pattern >> 14) & 3];
   *surface++ = colors[(pattern >> 6) & 3];
   *surface++ = colors[(pattern >> 12) & 3];
   *surface++ = colors[(pattern >> 4) & 3];
   *surface++ = colors[(pattern >> 10) & 3];
   *surface++ = colors[(pattern >> 2) & 3];
   *surface++ = colors[(pattern >> 8) & 3];
   *surface = colors[pattern & 3];
}

INLINE int draw_oamtile(uint8 *surface, uint8 attrib, uint8 pat1,
                        uint8 pat2, const uint8 *col_tbl, bool check_strike)
{
   int strike_pixel = -1;
   uint32 color = ((pat2 & 0xAA) << 8) | ((pat2 & 0x55) << 1) | ((pat1 & 0xAA) << 7) | (pat1 & 0x55);

   /* sprite is not 100% transparent */
   if (color)
   {
      uint8 colors[8];

      /* swap pixels around if our tile is flipped */
      if (0 == (attrib & OAMF_HFLIP))
      {
         colors[0] = (color >> 14) & 3;
         colors[1] = (color >> 6) & 3;
         colors[2] = (color >> 12) & 3;
         colors[3] = (color >> 4) & 3;
         colors[4] = (color >> 10) & 3;
         colors[5] = (color >> 2) & 3;
         colors[6] = (color >> 8) & 3;
         colors[7] = color & 3;
      }
      else
      {
         colors[7] = (color >> 14) & 3;
         colors[6] = (color >> 6) & 3;
         colors[5] = (color >> 12) & 3;
         colors[4] = (color >> 4) & 3;
         colors[3] = (color >> 10) & 3;
         colors[2] = (color >> 2) & 3;
         colors[1] = (color >> 8) & 3;
         colors[0] = color & 3;
      }

      /* check for solid sprite pixel overlapping solid bg pixel */
      if (check_strike)
      {
         if (colors[0] && BG_SOLID(surface[0]))
            strike_pixel = 0;
         else if (colors[1] && BG_SOLID(surface[1]))
            strike_pixel = 1;
         else if (colors[2] && BG_SOLID(surface[2]))
            strike_pixel = 2;
         else if (colors[3] && BG_SOLID(surface[3]))
            strike_pixel = 3;
         else if (colors[4] && BG_SOLID(surface[4]))
            strike_pixel = 4;
         else if (colors[5] && BG_SOLID(surface[5]))
            strike_pixel = 5;
         else if (colors[6] && BG_SOLID(surface[6]))
            strike_pixel = 6;
         else if (colors[7] && BG_SOLID(surface[7]))
            strike_pixel = 7;
      }

      /* draw the character */
      if (attrib & OAMF_BEHIND)
      {
         if (colors[0])
            surface[0] = SP_PIXEL | (BG_CLEAR(surface[0]) ? col_tbl[colors[0]] : surface[0]);
         if (colors[1])
            surface[1] = SP_PIXEL | (BG_CLEAR(surface[1]) ? col_tbl[colors[1]] : surface[1]);
         if (colors[2])
            surface[2] = SP_PIXEL | (BG_CLEAR(surface[2]) ? col_tbl[colors[2]] : surface[2]);
         if (colors[3])
            surface[3] = SP_PIXEL | (BG_CLEAR(surface[3]) ? col_tbl[colors[3]] : surface[3]);
         if (colors[4])
            surface[4] = SP_PIXEL | (BG_CLEAR(surface[4]) ? col_tbl[colors[4]] : surface[4]);
         if (colors[5])
            surface[5] = SP_PIXEL | (BG_CLEAR(surface[5]) ? col_tbl[colors[5]] : surface[5]);
         if (colors[6])
            surface[6] = SP_PIXEL | (BG_CLEAR(surface[6]) ? col_tbl[colors[6]] : surface[6]);
         if (colors[7])
            surface[7] = SP_PIXEL | (BG_CLEAR(surface[7]) ? col_tbl[colors[7]] : surface[7]);
      }
      else
      {
         if (colors[0] && SP_CLEAR(surface[0]))
            surface[0] = SP_PIXEL | col_tbl[colors[0]];
         if (colors[1] && SP_CLEAR(surface[1]))
            surface[1] = SP_PIXEL | col_tbl[colors[1]];
         if (colors[2] && SP_CLEAR(surface[2]))
            surface[2] = SP_PIXEL | col_tbl[colors[2]];
         if (colors[3] && SP_CLEAR(surface[3]))
            surface[3] = SP_PIXEL | col_tbl[colors[3]];
         if (colors[4] && SP_CLEAR(surface[4]))
            surface[4] = SP_PIXEL | col_tbl[colors[4]];
         if (colors[5] && SP_CLEAR(surface[5]))
            surface[5] = SP_PIXEL | col_tbl[colors[5]];
         if (colors[6] && SP_CLEAR(surface[6]))
            surface[6] = SP_PIXEL | col_tbl[colors[6]];
         if (colors[7] && SP_CLEAR(surface[7]))
            surface[7] = SP_PIXEL | col_tbl[colors[7]];
      }
   }

   return strike_pixel;
}

static void ppu_renderbg(uint8 *vidbuf)
{
   uint8 *bmp_ptr, *data_ptr, *tile_ptr, *attrib_ptr;
   uint32 refresh_vaddr, bg_offset, attrib_base;
   int tile_count;
   uint8 tile_index, x_tile, y_tile;
   uint8 col_high, attrib, attrib_shift;

   /* draw a line of transparent background color if bg is disabled */
   if (false == ppu.bg_on)
   {
      memset(vidbuf, FULLBG, NES_SCREEN_WIDTH);
      return;
   }

   bmp_ptr = vidbuf - ppu.tile_xofs;              /* scroll x */
   refresh_vaddr = 0x2000 + (ppu.vaddr & 0x0FE0); /* mask out x tile */
   x_tile = ppu.vaddr & 0x1F;
   y_tile = (ppu.vaddr >> 5) & 0x1F;                  /* to simplify calculations */
   bg_offset = ((ppu.vaddr >> 12) & 7) + ppu.bg_base; /* offset in y tile */

   /* calculate initial values */
   tile_ptr = &PPU_MEM(refresh_vaddr + x_tile); /* pointer to tile index */
   attrib_base = (refresh_vaddr & 0x2C00) + 0x3C0 + ((y_tile & 0x1C) << 1);
   attrib_ptr = &PPU_MEM(attrib_base + (x_tile >> 2));
   attrib = *attrib_ptr++;
   attrib_shift = (x_tile & 2) + ((y_tile & 2) << 1);
   col_high = ((attrib >> attrib_shift) & 3) << 2;

   /* ppu fetches 33 tiles */
   tile_count = 33;
   while (tile_count--)
   {
      /* Tile number from nametable */
      tile_index = *tile_ptr++;
      data_ptr = &PPU_MEM(bg_offset + (tile_index << 4));

      /* Handle $FD/$FE tile VROM switching (PunchOut) */
      if (ppu.latchfunc)
         ppu.latchfunc(ppu.bg_base, tile_index);

      draw_bgtile(bmp_ptr, data_ptr[0], data_ptr[8], ppu.palette + col_high);
      bmp_ptr += 8;

      x_tile++;

      if (0 == (x_tile & 1)) /* check every 2 tiles */
      {
         if (0 == (x_tile & 3)) /* check every 4 tiles */
         {
            if (32 == x_tile) /* check every 32 tiles */
            {
               x_tile = 0;
               refresh_vaddr ^= (1 << 10); /* switch nametable */
               attrib_base ^= (1 << 10);

               /* recalculate pointers */
               tile_ptr = &PPU_MEM(refresh_vaddr);
               attrib_ptr = &PPU_MEM(attrib_base);
            }

            /* Get the attribute byte */
            attrib = *attrib_ptr++;
         }

         attrib_shift ^= 2;
         col_high = ((attrib >> attrib_shift) & 3) << 2;
      }
   }

   /* Blank left hand column if need be */
   if (ppu.bg_mask)
   {
      uint32 *buf_ptr = (uint32 *)vidbuf;
      uint32 bg_clear = FULLBG | FULLBG << 8 | FULLBG << 16 | FULLBG << 24;

      ((uint32 *)buf_ptr)[0] = bg_clear;
      ((uint32 *)buf_ptr)[1] = bg_clear;
   }
}

/* OAM entry */
typedef struct obj_s
{
   uint8 y_loc;
   uint8 tile;
   uint8 atr;
   uint8 x_loc;
} obj_t;

/* TODO: fetch valid OAM a scanline before, like the Real Thing */
static void ppu_renderoam(uint8 *vidbuf, int scanline)
{
   uint8 *buf_ptr;
   uint32 vram_offset, savecol[2] = {0};
   int sprite_num, spritecount;
   obj_t *sprite_ptr;
   uint8 sprite_height;

   if (false == ppu.obj_on)
      return;

   /* Get our buffer pointer */
   buf_ptr = vidbuf;

   /* Save left hand column? */
   if (ppu.obj_mask)
   {
      savecol[0] = ((uint32 *)buf_ptr)[0];
      savecol[1] = ((uint32 *)buf_ptr)[1];
   }

   sprite_height = ppu.obj_height;
   vram_offset = ppu.obj_base;
   spritecount = 0;

   sprite_ptr = (obj_t *)ppu.oam;

   for (sprite_num = 0; sprite_num < 64; sprite_num++, sprite_ptr++)
   {
      uint8 *data_ptr, *bmp_ptr;
      uint32 vram_adr;
      int y_offset;
      uint8 tile_index, attrib, col_high;
      uint8 sprite_y, sprite_x;
      bool check_strike;
      int strike_pixel;

      sprite_y = sprite_ptr->y_loc + 1;

      /* Check to see if sprite is out of range */
      if ((sprite_y > scanline) || (sprite_y <= (scanline - sprite_height)) || (0 == sprite_y) || (sprite_y >= 240))
         continue;

      sprite_x = sprite_ptr->x_loc;
      tile_index = sprite_ptr->tile;
      attrib = sprite_ptr->atr;

      bmp_ptr = buf_ptr + sprite_x;

      /* Handle $FD/$FE tile VROM switching (PunchOut) */
      if (ppu.latchfunc)
         ppu.latchfunc(vram_offset, tile_index);

      /* Get upper two bits of color */
      col_high = ((attrib & 3) << 2);

      /* 8x16 even sprites use $0000, odd use $1000 */
      if (16 == ppu.obj_height)
         vram_adr = ((tile_index & 1) << 12) | ((tile_index & 0xFE) << 4);
      else
         vram_adr = vram_offset + (tile_index << 4);

      /* Get the address of the tile */
      data_ptr = &PPU_MEM(vram_adr);

      /* Calculate offset (line within the sprite) */
      y_offset = scanline - sprite_y;
      if (y_offset > 7)
         y_offset += 8;

      /* Account for vertical flippage */
      if (attrib & OAMF_VFLIP)
      {
         if (16 == ppu.obj_height)
            y_offset -= 23;
         else
            y_offset -= 7;

         data_ptr -= y_offset;
      }
      else
      {
         data_ptr += y_offset;
      }

      /* if we're on sprite 0 and sprite 0 strike flag isn't set,
      ** check for a strike 
      */
      check_strike = (0 == sprite_num) && (false == ppu.strikeflag);
      strike_pixel = draw_oamtile(bmp_ptr, attrib, data_ptr[0], data_ptr[8], ppu.palette + 16 + col_high, check_strike);
      if (strike_pixel >= 0)
         ppu_setstrike(strike_pixel);

      /* maximum of 8 sprites per scanline */
      if (++spritecount == PPU_MAXSPRITE)
      {
         ppu.stat |= PPU_STATF_MAXSPRITE;
         break;
      }
   }

   /* Restore lefthand column */
   if (ppu.obj_mask)
   {
      ((uint32 *)buf_ptr)[0] = savecol[0];
      ((uint32 *)buf_ptr)[1] = savecol[1];
   }
}

/* Fake rendering a line */
/* This is needed for sprite 0 hits when we're skipping drawing a frame */
static void ppu_fakeoam(int scanline)
{
   uint8 *data_ptr;
   obj_t *sprite_ptr;
   uint32 vram_adr, color;
   int y_offset;
   uint8 pat1, pat2;
   uint8 tile_index, attrib;
   uint8 sprite_height, sprite_y, sprite_x;

   /* we don't need to be here if strike flag is set */

   if (false == ppu.obj_on || ppu.strikeflag)
      return;

   sprite_height = ppu.obj_height;
   sprite_ptr = (obj_t *)ppu.oam;
   sprite_y = sprite_ptr->y_loc + 1;

   /* Check to see if sprite is out of range */
   if ((sprite_y > scanline) || (sprite_y <= (scanline - sprite_height)) || (0 == sprite_y) || (sprite_y > 240))
      return;

   sprite_x = sprite_ptr->x_loc;
   tile_index = sprite_ptr->tile;
   attrib = sprite_ptr->atr;

   /* 8x16 even sprites use $0000, odd use $1000 */
   if (16 == ppu.obj_height)
      vram_adr = ((tile_index & 1) << 12) | ((tile_index & 0xFE) << 4);
   else
      vram_adr = ppu.obj_base + (tile_index << 4);

   data_ptr = &PPU_MEM(vram_adr);

   /* Calculate offset (line within the sprite) */
   y_offset = scanline - sprite_y;
   if (y_offset > 7)
      y_offset += 8;

   /* Account for vertical flippage */
   if (attrib & OAMF_VFLIP)
   {
      if (16 == ppu.obj_height)
         y_offset -= 23;
      else
         y_offset -= 7;
      data_ptr -= y_offset;
   }
   else
   {
      data_ptr += y_offset;
   }

   /* check for a solid sprite 0 pixel */
   pat1 = data_ptr[0];
   pat2 = data_ptr[8];
   color = ((pat2 & 0xAA) << 8) | ((pat2 & 0x55) << 1) | ((pat1 & 0xAA) << 7) | (pat1 & 0x55);

   if (color)
   {
      uint8 colors[8];

      /* buckle up, it's going to get ugly... */
      if (0 == (attrib & OAMF_HFLIP))
      {
         colors[0] = (color >> 14) & 3;
         colors[1] = (color >> 6) & 3;
         colors[2] = (color >> 12) & 3;
         colors[3] = (color >> 4) & 3;
         colors[4] = (color >> 10) & 3;
         colors[5] = (color >> 2) & 3;
         colors[6] = (color >> 8) & 3;
         colors[7] = color & 3;
      }
      else
      {
         colors[7] = (color >> 14) & 3;
         colors[6] = (color >> 6) & 3;
         colors[5] = (color >> 12) & 3;
         colors[4] = (color >> 4) & 3;
         colors[3] = (color >> 10) & 3;
         colors[2] = (color >> 2) & 3;
         colors[1] = (color >> 8) & 3;
         colors[0] = color & 3;
      }

      if (colors[0])
         ppu_setstrike(sprite_x + 0);
      else if (colors[1])
         ppu_setstrike(sprite_x + 1);
      else if (colors[2])
         ppu_setstrike(sprite_x + 2);
      else if (colors[3])
         ppu_setstrike(sprite_x + 3);
      else if (colors[4])
         ppu_setstrike(sprite_x + 4);
      else if (colors[5])
         ppu_setstrike(sprite_x + 5);
      else if (colors[6])
         ppu_setstrike(sprite_x + 6);
      else if (colors[7])
         ppu_setstrike(sprite_x + 7);
   }
}

bool ppu_enabled(void)
{
   return (ppu.bg_on || ppu.obj_on);
}

static void ppu_renderscanline(bitmap_t *bmp, int scanline, bool draw_flag)
{
   uint8 *buf = bmp->line[scanline];

   /* start scanline - transfer ppu latch into vaddr */
   if (ppu.bg_on || ppu.obj_on)
   {
      if (0 == scanline)
      {
         ppu.vaddr = ppu.vaddr_latch;
      }
      else
      {
         ppu.vaddr &= ~0x041F;
         ppu.vaddr |= (ppu.vaddr_latch & 0x041F);
      }
   }

   if (draw_flag)
      ppu_renderbg(buf);

   /* TODO: fetch obj data 1 scanline before */
   if (true == ppu.drawsprites && true == draw_flag)
      ppu_renderoam(buf, scanline);
   else
      ppu_fakeoam(scanline);
}

void ppu_endscanline(int scanline)
{
   /* modify vram address at end of scanline */
   if (scanline < 240 && (ppu.bg_on || ppu.obj_on))
   {
      int ytile;

      /* check for max 3 bit y tile offset */
      if (7 == (ppu.vaddr >> 12))
      {
         ppu.vaddr &= ~0x7000; /* clear y tile offset */
         ytile = (ppu.vaddr >> 5) & 0x1F;

         if (29 == ytile)
         {
            ppu.vaddr &= ~0x03E0; /* clear y tile */
            ppu.vaddr ^= 0x0800;  /* toggle nametable */
         }
         else if (31 == ytile)
         {
            ppu.vaddr &= ~0x03E0; /* clear y tile */
         }
         else
         {
            ppu.vaddr += 0x20; /* increment y tile */
         }
      }
      else
      {
         ppu.vaddr += 0x1000; /* increment tile y offset */
      }
   }
}

void ppu_checknmi(void)
{
   if (ppu.ctrl0 & PPU_CTRL0F_NMI)
      nes_nmi();
}

void ppu_scanline(bitmap_t *bmp, int scanline, bool draw_flag)
{
   if (scanline < NES_SCREEN_HEIGHT)
   {
      /* Lower the Max Sprite per scanline flag */
      ppu.stat &= ~PPU_STATF_MAXSPRITE;
      ppu_renderscanline(bmp, scanline, draw_flag);
   }
   else if (241 == scanline)
   {
      ppu.stat |= PPU_STATF_VBLANK;
      ppu.vram_accessible = true;
   }
   else if (261 == scanline)
   {
      ppu.stat &= ~PPU_STATF_VBLANK;
      ppu.strikeflag = false;
      ppu.strike_cycle = (uint32)-1;

      ppu.vram_accessible = false;
   }
}

/*
bool ppu_checkzapperhit(bitmap_t *bmp, int x, int y)
{
   uint8 pixel = bmp->line[y][x] & 0x3F;

   if (0x20 == pixel || 0x30 == pixel)
      return true;

   return false;
}
*/

/*************************************************/
/* TODO: all this stuff should go somewhere else */
/*************************************************/
INLINE void draw_box(bitmap_t *bmp, int x, int y, int height)
{
   int i;
   uint8 *vid;

   vid = bmp->line[y] + x;

   for (i = 0; i < 10; i++)
      *vid++ = GUI_GRAY;
   vid += (bmp->pitch - 10);
   for (i = 0; i < height; i++)
   {
      vid[0] = vid[9] = GUI_GRAY;
      vid += bmp->pitch;
   }
   for (i = 0; i < 10; i++)
      *vid++ = GUI_GRAY;
}

INLINE void draw_deadsprite(bitmap_t *bmp, int x, int y, int height)
{
   int i, j, index;
   uint8 *vid;
   uint8 colbuf[8] = {GUI_BLACK, GUI_BLACK, GUI_BLACK, GUI_BLACK,
                      GUI_BLACK, GUI_BLACK, GUI_BLACK, GUI_DKGRAY};

   vid = bmp->line[y] + x;

   for (i = 0; i < height; i++)
   {
      index = i;

      if (height == 16)
         index >>= 1;

      for (j = 0; j < 8; j++)
      {
         *(vid + j) = colbuf[index++];
         index &= 7;
      }

      vid += bmp->pitch;
   }
}

/* Stuff for the OAM viewer */
static void draw_sprite(bitmap_t *bmp, int x, int y, uint8 tile_num, uint8 attrib)
{
   int line, height;
   int col_high, vram_adr;
   uint8 *vid, *data_ptr;

   vid = bmp->line[y] + x;

   /* Get upper two bits of color */
   col_high = ((attrib & 3) << 2);

   /* 8x16 even sprites use $0000, odd use $1000 */
   height = ppu.obj_height;
   if (16 == height)
      vram_adr = ((tile_num & 1) << 12) | ((tile_num & 0xFE) << 4);
   /* else just use the offset from $2000 */
   else
      vram_adr = ppu.obj_base + (tile_num << 4);

   data_ptr = &PPU_MEM(vram_adr);

   for (line = 0; line < height; line++)
   {
      if (line == 8)
         data_ptr += 8;

      draw_bgtile(vid, data_ptr[0], data_ptr[8], ppu.palette + 16 + col_high);
      //draw_oamtile(vid, attrib, data_ptr[0], data_ptr[8], ppu.palette + 16 + col_high);

      data_ptr++;
      vid += bmp->pitch;
   }
}

void ppu_dumpoam(bitmap_t *bmp, int x_loc, int y_loc)
{
   int sprite, x_pos, y_pos, height;
   obj_t *spr_ptr;

   spr_ptr = (obj_t *)ppu.oam;
   height = ppu.obj_height;

   for (sprite = 0; sprite < 64; sprite++)
   {
      x_pos = ((sprite & 0x0F) << 3) + (sprite & 0x0F) + x_loc;
      if (height == 16)
         y_pos = (sprite & 0xF0) + (sprite >> 4) + y_loc;
      else
         y_pos = ((sprite & 0xF0) >> 1) + (sprite >> 4) + y_loc;

      draw_box(bmp, x_pos, y_pos, height);

      if (spr_ptr->y_loc && spr_ptr->y_loc < 240)
         draw_sprite(bmp, x_pos + 1, y_pos + 1, spr_ptr->tile, spr_ptr->atr);
      else
         draw_deadsprite(bmp, x_pos + 1, y_pos + 1, height);

      spr_ptr++;
   }
}

/* More of a debugging thing than anything else */
void ppu_dumppattern(bitmap_t *bmp, int table_num, int x_loc, int y_loc, int col)
{
   int x_tile, y_tile;
   uint8 *bmp_ptr, *data_ptr, *ptr;
   int tile_num, line;
   uint8 col_high;

   tile_num = 0;
   col_high = col << 2;

   for (y_tile = 0; y_tile < 16; y_tile++)
   {
      /* Get our pointer to the bitmap */
      bmp_ptr = bmp->line[y_loc] + x_loc;

      for (x_tile = 0; x_tile < 16; x_tile++)
      {
         data_ptr = &PPU_MEM((table_num << 12) + (tile_num << 4));
         ptr = bmp_ptr;

         for (line = 0; line < 8; line++)
         {
            draw_bgtile(ptr, data_ptr[0], data_ptr[8], ppu.palette + col_high);
            data_ptr++;
            ptr += bmp->pitch;
         }

         bmp_ptr += 8;
         tile_num++;
      }
      y_loc += 8;
   }
}

/*
** $Log: nes_ppu.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.14  2000/11/29 12:58:23  matt
** timing/fiq fixes
**
** Revision 1.13  2000/11/27 19:36:15  matt
** more timing fixes
**
** Revision 1.12  2000/11/26 15:51:13  matt
** frame IRQ emulation
**
** Revision 1.11  2000/11/25 20:30:39  matt
** scanline emulation simplifications/timing fixes
**
** Revision 1.10  2000/11/24 14:56:02  matt
** fixed a long-standing sprite 0 strike bug
**
** Revision 1.9  2000/11/20 13:23:17  matt
** PPU fixes
**
** Revision 1.8  2000/11/19 13:47:30  matt
** problem with frame irqs fixed
**
** Revision 1.7  2000/11/19 13:40:19  matt
** more accurate ppu behavior
**
** Revision 1.6  2000/11/14 12:09:37  matt
** only generate the palette once, please
**
** Revision 1.5  2000/11/11 14:51:43  matt
** context get/set fixed
**
** Revision 1.4  2000/11/09 12:35:50  matt
** fixed timing problem with VRAM reads/writes
**
** Revision 1.3  2000/11/05 16:35:41  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.2  2000/10/27 12:55:03  matt
** palette generating functions now take *this pointers
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.33  2000/10/23 15:53:08  matt
** better system handling
**
** Revision 1.32  2000/10/22 15:02:32  matt
** simplified mirroring
**
** Revision 1.31  2000/10/21 21:36:04  matt
** ppu cleanups / fixes
**
** Revision 1.30  2000/10/21 19:26:59  matt
** many more cleanups
**
** Revision 1.29  2000/10/10 13:58:15  matt
** stroustrup squeezing his way in the door
**
** Revision 1.28  2000/10/08 17:54:32  matt
** reject VRAM access out of VINT period
**
** Revision 1.27  2000/09/15 04:58:07  matt
** simplifying and optimizing APU core
**
** Revision 1.26  2000/09/08 11:57:29  matt
** no more nes_fiq
**
** Revision 1.25  2000/09/07 21:57:31  matt
** api change
**
** Revision 1.24  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.23  2000/07/30 06:13:12  matt
** default to no FIQs on startup
**
** Revision 1.22  2000/07/30 04:32:32  matt
** emulation of the NES frame IRQ
**
** Revision 1.21  2000/07/25 02:25:53  matt
** safer xxx_destroy calls
**
** Revision 1.20  2000/07/23 15:12:43  matt
** removed unused variables, changed INLINE
**
** Revision 1.19  2000/07/21 04:50:39  matt
** moved palette calls out of nofrendo.c and into ppu_create
**
** Revision 1.18  2000/07/17 05:12:55  matt
** nes_ppu.c is no longer a scary place to be-- cleaner & faster
**
** Revision 1.17  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.16  2000/07/11 04:42:39  matt
** updated for new screen dimension defines
**
** Revision 1.15  2000/07/10 19:10:16  matt
** should bomb out now if a game tries to write to VROM
**
** Revision 1.14  2000/07/10 05:28:30  matt
** moved joypad/oam dma from apu to ppu
**
** Revision 1.13  2000/07/10 03:03:16  matt
** added ppu_getcontext() routine
**
** Revision 1.12  2000/07/09 03:46:05  matt
** using pitch instead of width...
**
** Revision 1.11  2000/07/06 16:42:40  matt
** better palette setting interface
**
** Revision 1.10  2000/07/05 22:49:25  matt
** changed mmc2 (punchout) tile-access switching
**
** Revision 1.9  2000/07/04 23:13:26  matt
** added an irq line drawing debug feature hack
**
** Revision 1.8  2000/06/26 04:58:08  matt
** accuracy changes
**
** Revision 1.7  2000/06/22 02:13:49  matt
** more accurate emulation of $2002
**
** Revision 1.6  2000/06/20 20:42:47  matt
** accuracy changes
**
** Revision 1.5  2000/06/20 00:05:12  matt
** tested and verified STAT quirk, added code
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
