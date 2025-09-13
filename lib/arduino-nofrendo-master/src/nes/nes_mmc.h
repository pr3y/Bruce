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
** nes_mmc.h
**
** NES Memory Management Controller (mapper) defines / prototypes
** $Id: nes_mmc.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NES_MMC_H_
#define _NES_MMC_H_

#include "../libsnss/libsnss.h"
#include "../sndhrdw/nes_apu.h"

#define MMC_LASTBANK -1

typedef struct
{
   uint32 min_range, max_range;
   uint8 (*read_func)(uint32 address);
} map_memread;

typedef struct
{
   uint32 min_range, max_range;
   void (*write_func)(uint32 address, uint8 value);
} map_memwrite;

typedef struct mapintf_s
{
   int number;
   char *name;
   void (*init)(void);
   void (*vblank)(void);
   void (*hblank)(int vblank);
   void (*get_state)(SnssMapperBlock *state);
   void (*set_state)(SnssMapperBlock *state);
   map_memread *mem_read;
   map_memwrite *mem_write;
   apuext_t *sound_ext;
} mapintf_t;

#include "nes_rom.h"
typedef struct mmc_s
{
   mapintf_t *intf;
   rominfo_t *cart; /* link it back to the cart */
} mmc_t;

extern rominfo_t *mmc_getinfo(void);

extern void mmc_bankvrom(int size, uint32 address, int bank);
extern void mmc_bankrom(int size, uint32 address, int bank);

/* Prototypes */
extern mmc_t *mmc_create(rominfo_t *rominfo);
extern void mmc_destroy(mmc_t **nes_mmc);

extern void mmc_getcontext(mmc_t *dest_mmc);
extern void mmc_setcontext(mmc_t *src_mmc);

extern bool mmc_peek(int map_num);

extern void mmc_reset(void);

#endif /* _NES_MMC_H_ */

/*
** $Log: nes_mmc.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.2  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.18  2000/10/22 19:17:24  matt
** mapper cleanups galore
**
** Revision 1.17  2000/10/21 19:26:59  matt
** many more cleanups
**
** Revision 1.16  2000/10/17 03:22:58  matt
** cleaning up rom module
**
** Revision 1.15  2000/10/10 13:58:15  matt
** stroustrup squeezing his way in the door
**
** Revision 1.14  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.13  2000/07/25 02:25:53  matt
** safer xxx_destroy calls
**
** Revision 1.12  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.11  2000/07/15 23:50:03  matt
** migrated state get/set from nes_mmc.c to state.c
**
** Revision 1.10  2000/07/11 02:38:01  matt
** added setcontext() routine
**
** Revision 1.9  2000/07/10 05:27:41  matt
** cleaned up mapper-specific callbacks
**
** Revision 1.8  2000/07/04 23:12:58  matt
** brand spankin' new mapper interface implemented
**
** Revision 1.7  2000/07/04 04:56:36  matt
** modifications for new SNSS
**
** Revision 1.6  2000/06/29 14:17:18  matt
** uses snsslib now
**
** Revision 1.5  2000/06/29 03:09:24  matt
** modified to support new snss code
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
