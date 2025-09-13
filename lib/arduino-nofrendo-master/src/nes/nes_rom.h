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
** nes_rom.h
**
** NES ROM loading/saving related defines / prototypes
** $Id: nes_rom.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NES_ROM_H_
#define _NES_ROM_H_

#include <unistd.h>
#include "nes_ppu.h"
#include "../osd.h"

typedef enum
{
   MIRROR_HORIZ = 0,
   MIRROR_VERT = 1
} mirror_t;

#define ROM_FLAG_BATTERY 0x01
#define ROM_FLAG_TRAINER 0x02
#define ROM_FLAG_FOURSCREEN 0x04
#define ROM_FLAG_VERSUS 0x08

typedef struct rominfo_s
{
   /* pointers to ROM and VROM */
   uint8 *rom, *vrom;

   /* pointers to SRAM and VRAM */
   uint8 *sram, *vram;

   /* number of banks */
   int rom_banks, vrom_banks;
   int sram_banks, vram_banks;

   int mapper_number;
   mirror_t mirror;

   uint8 flags;

   char filename[PATH_MAX + 1];
} rominfo_t;

extern int rom_checkmagic(const char *filename);
extern rominfo_t *rom_load(const char *filename, ppu_t *ppu);
extern void rom_free(rominfo_t **rominfo);
extern char *rom_getinfo(rominfo_t *rominfo);

#endif /* _NES_ROM_H_ */

/*
** $Log: nes_rom.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.2  2000/10/25 01:23:08  matt
** basic system autodetection
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.11  2000/10/22 15:01:29  matt
** irrelevant mirroring modes removed
**
** Revision 1.10  2000/10/17 03:22:38  matt
** cleaning up rom module
**
** Revision 1.9  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.8  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.7  2000/07/25 02:20:58  matt
** cleanups
**
** Revision 1.6  2000/07/19 15:59:39  neil
** PATH_MAX, strncpy, snprintf, and strncat are our friends
**
** Revision 1.5  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
