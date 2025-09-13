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
** nes.h
**
** NES hardware related definitions / prototypes
** $Id: nes.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NES_H_
#define _NES_H_

#include "../noftypes.h"
#include "../sndhrdw/nes_apu.h"
#include "nes_mmc.h"
#include "nes_ppu.h"
#include "nes_rom.h"
#include "../cpu/nes6502.h"
#include "../bitmap.h"

#define NES_SCREEN_WIDTH 256
#define NES_SCREEN_HEIGHT 240

/* NTSC = 60Hz, PAL = 50Hz */
#ifdef PAL
#define NES_REFRESH_RATE 50
#else /* !PAL */
#define NES_REFRESH_RATE 60
#endif /* !PAL */

#define MAX_MEM_HANDLERS 32

enum
{
   SOFT_RESET,
   HARD_RESET
};

typedef struct nes_s
{
   /* hardware things */
   nes6502_context *cpu;
   nes6502_memread readhandler[MAX_MEM_HANDLERS];
   nes6502_memwrite writehandler[MAX_MEM_HANDLERS];

   ppu_t *ppu;
   apu_t *apu;
   mmc_t *mmc;
   rominfo_t *rominfo;

   /* video buffer */
#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   bitmap_t *vidbuf;
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   bool fiq_occurred;
   uint8 fiq_state;
   int fiq_cycles;

   int scanline;

   /* Timing stuff */
   float scanline_cycles;
   bool autoframeskip;

   /* control */
   bool poweroff;
   bool pause;

} nes_t;

extern int nes_isourfile(const char *filename);

/* temp hack */
extern nes_t *nes_getcontextptr(void);

/* Function prototypes */
extern void nes_getcontext(nes_t *machine);
extern void nes_setcontext(nes_t *machine);

extern nes_t *nes_create(void);
extern void nes_destroy(nes_t **machine);
extern int nes_insertcart(const char *filename, nes_t *machine);

extern void nes_setfiq(uint8 state);
extern void nes_nmi(void);
extern void nes_irq(void);
extern void nes_emulate(void);

extern void nes_reset(int reset_type);

extern void nes_poweroff(void);
extern void nes_togglepause(void);

#endif /* _NES_H_ */

/*
** $Log: nes.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.8  2000/11/26 15:51:13  matt
** frame IRQ emulation
**
** Revision 1.7  2000/11/25 20:30:39  matt
** scanline emulation simplifications/timing fixes
**
** Revision 1.6  2000/11/25 01:52:17  matt
** bool stinks sometimes
**
** Revision 1.5  2000/11/09 14:07:28  matt
** state load fixed, state save mostly fixed
**
** Revision 1.4  2000/10/29 14:36:45  matt
** nes_clearframeirq is static
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
** Revision 1.26  2000/10/23 17:51:10  matt
** adding fds support
**
** Revision 1.25  2000/10/23 15:53:08  matt
** better system handling
**
** Revision 1.24  2000/10/22 19:16:15  matt
** more sane timer ISR / autoframeskip
**
** Revision 1.23  2000/10/21 19:26:59  matt
** many more cleanups
**
** Revision 1.22  2000/10/10 13:58:15  matt
** stroustrup squeezing his way in the door
**
** Revision 1.21  2000/10/08 17:53:36  matt
** minor accuracy changes
**
** Revision 1.20  2000/09/15 04:58:07  matt
** simplifying and optimizing APU core
**
** Revision 1.19  2000/09/08 11:57:29  matt
** no more nes_fiq
**
** Revision 1.18  2000/08/11 02:43:50  matt
** moved frame irq stuff out of APU into here
**
** Revision 1.17  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.16  2000/07/30 04:32:32  matt
** emulation of the NES frame IRQ
**
** Revision 1.15  2000/07/27 01:17:09  matt
** nes_insertrom -> nes_insertcart
**
** Revision 1.14  2000/07/26 21:36:16  neil
** Big honkin' change -- see the mailing list
**
** Revision 1.13  2000/07/25 02:25:53  matt
** safer xxx_destroy calls
**
** Revision 1.12  2000/07/23 15:13:13  matt
** autoframeskip is now a member variable of nes_t
**
** Revision 1.11  2000/07/17 05:12:55  matt
** nes_ppu.c is no longer a scary place to be-- cleaner & faster
**
** Revision 1.10  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.9  2000/07/16 09:48:58  neil
** Make visible height compile-time configurable in the Makefile
**
** Revision 1.8  2000/07/11 04:31:55  matt
** less magic number nastiness for screen dimensions
**
** Revision 1.7  2000/07/11 02:40:36  matt
** forgot to remove framecounter
**
** Revision 1.6  2000/07/11 02:38:25  matt
** encapsulated memory address handlers into nes/nsf
**
** Revision 1.5  2000/07/10 13:50:50  matt
** added function nes_irq()
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
