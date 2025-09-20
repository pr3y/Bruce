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
** nes_ppu.h
**
** NES Picture Processing Unit (PPU) emulation header file
** $Id: nes_ppu.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NES_PPU_H_
#define _NES_PPU_H_

#include "../bitmap.h"

/* PPU register defines */
#define PPU_CTRL0 0x2000
#define PPU_CTRL1 0x2001
#define PPU_STAT 0x2002
#define PPU_OAMADDR 0x2003
#define PPU_OAMDATA 0x2004
#define PPU_SCROLL 0x2005
#define PPU_VADDR 0x2006
#define PPU_VDATA 0x2007

#define PPU_OAMDMA 0x4014
#define PPU_JOY0 0x4016
#define PPU_JOY1 0x4017

/* $2000 */
#define PPU_CTRL0F_NMI 0x80
#define PPU_CTRL0F_OBJ16 0x20
#define PPU_CTRL0F_BGADDR 0x10
#define PPU_CTRL0F_OBJADDR 0x08
#define PPU_CTRL0F_ADDRINC 0x04
#define PPU_CTRL0F_NAMETAB 0x03

/* $2001 */
#define PPU_CTRL1F_OBJON 0x10
#define PPU_CTRL1F_BGON 0x08
#define PPU_CTRL1F_OBJMASK 0x04
#define PPU_CTRL1F_BGMASK 0x02

/* $2002 */
#define PPU_STATF_VBLANK 0x80
#define PPU_STATF_STRIKE 0x40
#define PPU_STATF_MAXSPRITE 0x20

/* Sprite attribute byte bitmasks */
#define OAMF_VFLIP 0x80
#define OAMF_HFLIP 0x40
#define OAMF_BEHIND 0x20

/* Maximum number of sprites per horizontal scanline */
#define PPU_MAXSPRITE 8

/* some mappers do *dumb* things */
typedef void (*ppulatchfunc_t)(uint32 address, uint8 value);
typedef void (*ppuvromswitch_t)(uint8 value);

typedef struct ppu_s
{
   /* big nasty memory chunks */
   uint8 nametab[0x1000];
   uint8 oam[256];
   uint8 palette[32];
   uint8 *page[16];

   /* hardware registers */
   uint8 ctrl0, ctrl1, stat, oam_addr;
   uint32 vaddr, vaddr_latch;
   int tile_xofs, flipflop;
   int vaddr_inc;
   uint32 tile_nametab;

   uint8 obj_height;
   uint32 obj_base, bg_base;

   bool bg_on, obj_on;
   bool obj_mask, bg_mask;

   uint8 latch, vdata_latch;
   uint8 strobe;

   bool strikeflag;
   uint32 strike_cycle;

   /* callbacks for naughty mappers */
   ppulatchfunc_t latchfunc;
   ppuvromswitch_t vromswitch;

   /* copy of our current palette */
   rgb_t curpal[256];

   bool vram_accessible;

   bool vram_present;
   bool drawsprites;
} ppu_t;

/* TODO: should use this pointers */
extern void ppu_setlatchfunc(ppulatchfunc_t func);
extern void ppu_setvromswitch(ppuvromswitch_t func);

extern void ppu_getcontext(ppu_t *dest_ppu);
extern void ppu_setcontext(ppu_t *src_ppu);

/* Mirroring */
/* TODO: this is only used bloody once */
extern void ppu_mirrorhipages(void);

extern void ppu_mirror(int nt1, int nt2, int nt3, int nt4);

extern void ppu_setpage(int size, int page_num, uint8 *location);
extern uint8 *ppu_getpage(int page);

/* control */
extern void ppu_reset(int reset_type);
extern bool ppu_enabled(void);
extern void ppu_scanline(bitmap_t *bmp, int scanline, bool draw_flag);
extern void ppu_endscanline(int scanline);
extern void ppu_checknmi();

extern ppu_t *ppu_create(void);
extern void ppu_destroy(ppu_t **ppu);

/* IO */
extern uint8 ppu_read(uint32 address);
extern void ppu_write(uint32 address, uint8 value);
extern uint8 ppu_readhigh(uint32 address);
extern void ppu_writehigh(uint32 address, uint8 value);

/* rendering */
extern void ppu_setpal(ppu_t *src_ppu, rgb_t *pal);
extern void ppu_setdefaultpal(ppu_t *src_ppu);

/* bleh */
extern void ppu_dumppattern(bitmap_t *bmp, int table_num, int x_loc, int y_loc, int col);
extern void ppu_dumpoam(bitmap_t *bmp, int x_loc, int y_loc);
extern void ppu_displaysprites(bool display);

#endif /* _NES_PPU_H_ */

/*
** $Log: nes_ppu.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.8  2000/11/29 12:58:23  matt
** timing/fiq fixes
**
** Revision 1.7  2000/11/27 19:36:16  matt
** more timing fixes
**
** Revision 1.6  2000/11/26 15:51:13  matt
** frame IRQ emulation
**
** Revision 1.5  2000/11/25 20:30:39  matt
** scanline emulation simplifications/timing fixes
**
** Revision 1.4  2000/11/19 13:40:19  matt
** more accurate ppu behavior
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
** Revision 1.19  2000/10/22 15:02:32  matt
** simplified mirroring
**
** Revision 1.18  2000/10/21 21:36:04  matt
** ppu cleanups / fixes
**
** Revision 1.17  2000/10/21 19:26:59  matt
** many more cleanups
**
** Revision 1.16  2000/10/10 13:58:15  matt
** stroustrup squeezing his way in the door
**
** Revision 1.15  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.14  2000/07/30 04:32:33  matt
** emulation of the NES frame IRQ
**
** Revision 1.13  2000/07/25 02:25:53  matt
** safer xxx_destroy calls
**
** Revision 1.12  2000/07/17 05:12:56  matt
** nes_ppu.c is no longer a scary place to be-- cleaner & faster
**
** Revision 1.11  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.10  2000/07/10 05:28:30  matt
** moved joypad/oam dma from apu to ppu
**
** Revision 1.9  2000/07/10 03:03:16  matt
** added ppu_getcontext() routine
**
** Revision 1.8  2000/07/06 16:42:40  matt
** better palette setting interface
**
** Revision 1.7  2000/07/04 23:13:26  matt
** added an irq line drawing debug feature hack
**
** Revision 1.6  2000/06/26 04:58:08  matt
** accuracy changes
**
** Revision 1.5  2000/06/20 00:04:35  matt
** removed STATQUIRK macro
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
