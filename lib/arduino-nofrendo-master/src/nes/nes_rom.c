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
** nes_rom.c
**
** NES ROM loading/saving related functions
** $Id: nes_rom.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

/* TODO: make this a generic ROM loading routine */

#include <stdio.h>
#include <string.h>

#include "../noftypes.h"
#include "nes_rom.h"
#include "../intro.h"
#include "nes_mmc.h"
#include "nes_ppu.h"
#include "nes.h"
#include "../gui.h"
#include "../log.h"
#include "../osd.h"

/* Max length for displayed filename */
#define ROM_DISP_MAXLEN 20

#ifdef ZLIB
#include <zlib.h>
#define _fopen gzopen
#define _fclose gzclose
#define _fread(B, N, L, F) gzread((F), (B), (L) * (N))
#else
#define _fopen fopen
#define _fclose fclose
#define _fread(B, N, L, F) fread((B), (N), (L), (F))
#endif

#define ROM_FOURSCREEN 0x08
#define ROM_TRAINER 0x04
#define ROM_BATTERY 0x02
#define ROM_MIRRORTYPE 0x01
#define ROM_INES_MAGIC "NES\x1A"

typedef struct inesheader_s
{
   uint8 ines_magic[4];
   uint8 rom_banks;
   uint8 vrom_banks;
   uint8 rom_type;
   uint8 mapper_hinybble;
   uint8 reserved[8];
} __attribute__((packed)) inesheader_t;

#define TRAINER_OFFSET 0x1000
#define TRAINER_LENGTH 0x200
#define VRAM_LENGTH 0x2000

#define ROM_BANK_LENGTH 0x4000
#define VROM_BANK_LENGTH 0x2000

#define SRAM_BANK_LENGTH 0x0400
#define VRAM_BANK_LENGTH 0x2000

/* Save battery-backed RAM */
static void rom_savesram(rominfo_t *rominfo)
{
   FILE *fp;
   char fn[PATH_MAX + 1];

   ASSERT(rominfo);

   if (rominfo->flags & ROM_FLAG_BATTERY)
   {
      strncpy(fn, rominfo->filename, PATH_MAX);
      osd_newextension(fn, ".sav");

      fp = fopen(fn, "wb");
      if (NULL != fp)
      {
         fwrite(rominfo->sram, SRAM_BANK_LENGTH, rominfo->sram_banks, fp);
         fclose(fp);
         nofrendo_log_printf("Wrote battery RAM to %s.\n", fn);
      }
   }
}

/* Load battery-backed RAM from disk */
static void rom_loadsram(rominfo_t *rominfo)
{
   FILE *fp;
   char fn[PATH_MAX + 1];

   ASSERT(rominfo);

   if (rominfo->flags & ROM_FLAG_BATTERY)
   {
      strncpy(fn, rominfo->filename, PATH_MAX);
      osd_newextension(fn, ".sav");

      fp = fopen(fn, "rb");
      if (NULL != fp)
      {
         fread(rominfo->sram, SRAM_BANK_LENGTH, rominfo->sram_banks, fp);
         fclose(fp);
         nofrendo_log_printf("Read battery RAM from %s.\n", fn);
      }
   }
}

/* Allocate space for SRAM */
static int rom_allocsram(rominfo_t *rominfo)
{
   /* Load up SRAM */
   rominfo->sram = NOFRENDO_MALLOC(SRAM_BANK_LENGTH * rominfo->sram_banks);
   if (NULL == rominfo->sram)
   {
      gui_sendmsg(GUI_RED, "Could not allocate space for battery RAM");
      return -1;
   }

   /* make damn sure SRAM is clear */
   memset(rominfo->sram, 0, SRAM_BANK_LENGTH * rominfo->sram_banks);
   return 0;
}

/* If there's a trainer, load it in at $7000 */
static void rom_loadtrainer(FILE *fp, rominfo_t *rominfo)
{
   ASSERT(fp);
   ASSERT(rominfo);

   if (rominfo->flags & ROM_FLAG_TRAINER)
   {
      fread(rominfo->sram + TRAINER_OFFSET, TRAINER_LENGTH, 1, fp);
      nofrendo_log_printf("Read in trainer at $7000\n");
   }
}

static int rom_loadrom(FILE *fp, rominfo_t *rominfo)
{
   ASSERT(fp);
   ASSERT(rominfo);

   /* Allocate ROM space, and load it up! */
   // rominfo->rom = malloc(rominfo->rom_banks * ROM_BANK_LENGTH);
   rominfo->rom = mem_alloc(rominfo->rom_banks * ROM_BANK_LENGTH, false);
   if (NULL == rominfo->rom)
   {
      gui_sendmsg(GUI_RED, "Could not allocate space for ROM image");
      return -1;
   }
   _fread(rominfo->rom, ROM_BANK_LENGTH, rominfo->rom_banks, fp);

   /* If there's VROM, allocate and stuff it in */
   if (rominfo->vrom_banks)
   {
      // rominfo->vrom = malloc((rominfo->vrom_banks * VROM_BANK_LENGTH));
      rominfo->vrom = mem_alloc(rominfo->vrom_banks * VROM_BANK_LENGTH, false);
      if (NULL == rominfo->vrom)
      {
         gui_sendmsg(GUI_RED, "Could not allocate space for VROM");
         return -1;
      }
      _fread(rominfo->vrom, VROM_BANK_LENGTH, rominfo->vrom_banks, fp);
   }
   else
   {
      rominfo->vram = NOFRENDO_MALLOC(VRAM_LENGTH);
      if (NULL == rominfo->vram)
      {
         gui_sendmsg(GUI_RED, "Could not allocate space for VRAM");
         return -1;
      }
      memset(rominfo->vram, 0, VRAM_LENGTH);
   }

   return 0;
}

/* If we've got a VS. system game, load in the palette, as well */
static void rom_checkforpal(rominfo_t *rominfo, ppu_t *ppu)
{
   FILE *fp;
   rgb_t vs_pal[64];
   char filename[PATH_MAX + 1];
   int i;

   ASSERT(rominfo);

   strncpy(filename, rominfo->filename, PATH_MAX);
   osd_newextension(filename, ".pal");

   fp = fopen(filename, "rb");
   if (NULL == fp)
      return; /* no palette found  */

   for (i = 0; i < 64; i++)
   {
      vs_pal[i].r = fgetc(fp);
      vs_pal[i].g = fgetc(fp);
      vs_pal[i].b = fgetc(fp);
   }

   fclose(fp);
   /* TODO: this should really be a *SYSTEM* flag */
   rominfo->flags |= ROM_FLAG_VERSUS;
   /* TODO: bad, BAD idea, calling nes_getcontextptr... */
   // ppu_setpal(nes_getcontextptr()->ppu, vs_pal);
   ppu_setpal(ppu, vs_pal);
   nofrendo_log_printf("Game specific palette found -- assuming VS. UniSystem\n");
}

static FILE *rom_findrom(const char *filename, rominfo_t *rominfo)
{
   FILE *fp;

   ASSERT(rominfo);

   if (NULL == filename)
      return NULL;

   /* Make a copy of the name so we can extend it */
   osd_fullname(rominfo->filename, filename);

   fp = _fopen(rominfo->filename, "rb");
   if (NULL == fp)
   {
      /* Didn't find the file?  Maybe the .NES extension was omitted */
      if (NULL == strrchr(rominfo->filename, '.'))
         strncat(rominfo->filename, ".nes", PATH_MAX - strlen(rominfo->filename));

      /* this will either return NULL or a valid file pointer */
      fp = _fopen(rominfo->filename, "rb");
   }

   return fp;
}

/* Add ROM name to a list with dirty headers */
static int rom_adddirty(char *filename)
{
#ifdef NOFRENDO_DEBUG
#define MAX_BUFFER_LENGTH 255
   char buffer[MAX_BUFFER_LENGTH + 1];
   bool found = false;

   FILE *fp = fopen("dirtyrom.txt", "rt");
   if (NULL == fp)
      return -1;

   while (fgets(buffer, MAX_BUFFER_LENGTH, fp))
   {
      if (0 == strncmp(filename, buffer, strlen(filename)))
      {
         found = true;
         break;
      }
   }

   if (false == found)
   {
      /* close up the file, open it back up for writing */
      fclose(fp);
      fp = fopen("dirtyrom.txt", "at");
      fprintf(fp, "%s -- dirty header\n", filename);
   }

   fclose(fp);
#endif /* NOFRENDO_DEBUG */

   return 0;
}

/* return 0 if this *is* an iNES file */
int rom_checkmagic(const char *filename)
{
   inesheader_t head;
   rominfo_t rominfo;
   FILE *fp;

   fp = rom_findrom(filename, &rominfo);
   if (NULL == fp)
      return -1;

   _fread(&head, 1, sizeof(head), fp);

   _fclose(fp);

   if (0 == memcmp(head.ines_magic, ROM_INES_MAGIC, 4))
      /* not an iNES file */
      return 0;

   return -1;
}

static int rom_getheader(FILE *fp, rominfo_t *rominfo)
{
#define RESERVED_LENGTH 8
   inesheader_t head;
   uint8 reserved[RESERVED_LENGTH];
   bool header_dirty;

   ASSERT(fp);
   ASSERT(rominfo);

   /* Read in the header */
   _fread(&head, sizeof(head), 1, fp);

   if (memcmp(head.ines_magic, ROM_INES_MAGIC, 4))
   {
      gui_sendmsg(GUI_RED, "%s is not a valid ROM image", rominfo->filename);
      return -1;
   }

   rominfo->rom_banks = head.rom_banks;
   rominfo->vrom_banks = head.vrom_banks;
   /* iNES assumptions */
   rominfo->sram_banks = 8; /* 1kB banks, so 8KB */
   rominfo->vram_banks = 1; /* 8kB banks, so 8KB */
   rominfo->mirror = (head.rom_type & ROM_MIRRORTYPE) ? MIRROR_VERT : MIRROR_HORIZ;
   rominfo->flags = 0;
   if (head.rom_type & ROM_BATTERY)
      rominfo->flags |= ROM_FLAG_BATTERY;
   if (head.rom_type & ROM_TRAINER)
      rominfo->flags |= ROM_FLAG_TRAINER;
   if (head.rom_type & ROM_FOURSCREEN)
      rominfo->flags |= ROM_FLAG_FOURSCREEN;
   /* TODO: fourscreen a mirroring type? */
   rominfo->mapper_number = head.rom_type >> 4;

   /* Do a compare - see if we've got a clean extended header */
   memset(reserved, 0, RESERVED_LENGTH);
   if (0 == memcmp(head.reserved, reserved, RESERVED_LENGTH))
   {
      /* We were clean */
      header_dirty = false;
      rominfo->mapper_number |= (head.mapper_hinybble & 0xF0);
   }
   else
   {
      header_dirty = true;

      /* @!?#@! DiskDude. */
      if (('D' == head.mapper_hinybble) && (0 == memcmp(head.reserved, "iskDude!", 8)))
         nofrendo_log_printf("`DiskDude!' found in ROM header, ignoring high mapper nybble\n");
      else
      {
         nofrendo_log_printf("ROM header dirty, possible problem\n");
         rominfo->mapper_number |= (head.mapper_hinybble & 0xF0);
      }

      rom_adddirty(rominfo->filename);
   }

   /* TODO: this is an ugly hack, but necessary, I guess */
   /* Check for VS unisystem mapper */
   if (99 == rominfo->mapper_number)
      rominfo->flags |= ROM_FLAG_VERSUS;

   return 0;
}

/* Build the info string for ROM display */
char *rom_getinfo(rominfo_t *rominfo)
{
   static char info[PATH_MAX + 1];
   char romname[PATH_MAX + 1], temp[PATH_MAX + 1];

   /* Look to see if we were given a path along with filename */
   /* TODO: strip extensions */
   if (strrchr(rominfo->filename, PATH_SEP))
      strncpy(romname, strrchr(rominfo->filename, PATH_SEP) + 1, PATH_MAX);
   else
      strncpy(romname, rominfo->filename, PATH_MAX);

   /* If our filename is too long, truncate our displayed filename */
   if (strlen(romname) > ROM_DISP_MAXLEN)
   {
      strncpy(info, romname, ROM_DISP_MAXLEN - 3);
      strcpy(info + (ROM_DISP_MAXLEN - 3), "...");
   }
   else
   {
      strcpy(info, romname);
   }

   sprintf(temp, " [%d] %dk/%dk %c", rominfo->mapper_number,
           rominfo->rom_banks * 16, rominfo->vrom_banks * 8,
           (rominfo->mirror == MIRROR_VERT) ? 'V' : 'H');

   /* Stick it on there! */
   strncat(info, temp, PATH_MAX - strlen(info));

   if (rominfo->flags & ROM_FLAG_BATTERY)
      strncat(info, "B", PATH_MAX - strlen(info));
   if (rominfo->flags & ROM_FLAG_TRAINER)
      strncat(info, "T", PATH_MAX - strlen(info));
   if (rominfo->flags & ROM_FLAG_FOURSCREEN)
      strncat(info, "4", PATH_MAX - strlen(info));

   return info;
}

/* Load a ROM image into memory */
rominfo_t *rom_load(const char *filename, ppu_t *ppu)
{
   FILE *fp;
   rominfo_t *rominfo;

   rominfo = NOFRENDO_MALLOC(sizeof(rominfo_t));
   if (NULL == rominfo)
      return NULL;

   memset(rominfo, 0, sizeof(rominfo_t));

   fp = rom_findrom(filename, rominfo);

   if (NULL == fp)
      gui_sendmsg(GUI_RED, "%s not found, will use default ROM", filename);

   /* Get the header and stick it into rominfo struct */
   if (NULL == fp)
      intro_get_header(rominfo);
   else if (rom_getheader(fp, rominfo))
      goto _fail;

   /* Make sure we really support the mapper */
   if (false == mmc_peek(rominfo->mapper_number))
   {
      gui_sendmsg(GUI_RED, "Mapper %d not yet implemented", rominfo->mapper_number);
      goto _fail;
   }

   /* iNES format doesn't tell us if we need SRAM, so
   ** we have to always allocate it -- bleh!
   ** UNIF, TAKE ME AWAY!  AAAAAAAAAA!!!
   */
   if (rom_allocsram(rominfo))
      goto _fail;

   if (NULL != fp)
      rom_loadtrainer(fp, rominfo);

   if (NULL == fp)
   {
      if (intro_get_rom(rominfo))
         goto _fail;
   }
   else if (rom_loadrom(fp, rominfo))
      goto _fail;

   /* Close the file */
   if (NULL != fp)
      _fclose(fp);

   rom_loadsram(rominfo);

   /* See if there's a palette we can load up */
   rom_checkforpal(rominfo, ppu);

   gui_sendmsg(GUI_GREEN, "ROM loaded: %s", rom_getinfo(rominfo));

   return rominfo;

_fail:
   if (NULL != fp)
      _fclose(fp);
   rom_free(&rominfo);
   return NULL;
}

/* Free a ROM */
void rom_free(rominfo_t **rominfo)
{
   if (NULL == *rominfo)
   {
      gui_sendmsg(GUI_GREEN, "ROM not loaded");
      return;
   }

   /* Restore palette if we loaded in a VS jobber */
   if ((*rominfo)->flags & ROM_FLAG_VERSUS)
   {
      /* TODO: bad idea calling nes_getcontextptr... */
      ppu_setdefaultpal(nes_getcontextptr()->ppu);
      nofrendo_log_printf("Default NES palette restored\n");
   }

   rom_savesram(*rominfo);

   if ((*rominfo)->sram)
      NOFRENDO_FREE((*rominfo)->sram);
   if ((*rominfo)->rom)
      NOFRENDO_FREE((*rominfo)->rom);
   if ((*rominfo)->vrom)
      NOFRENDO_FREE((*rominfo)->vrom);
   if ((*rominfo)->vram)
      NOFRENDO_FREE((*rominfo)->vram);

   NOFRENDO_FREE(*rominfo);

   gui_sendmsg(GUI_GREEN, "ROM freed");
}

/*
** $Log: nes_rom.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.8  2000/11/21 13:28:40  matt
** take care to zero allocated mem
**
** Revision 1.7  2000/11/09 14:07:28  matt
** state load fixed, state save mostly fixed
**
** Revision 1.6  2000/10/28 14:24:54  matt
** where did I put that underscore?
**
** Revision 1.5  2000/10/27 12:56:35  matt
** api change for ppu palette functions
**
** Revision 1.4  2000/10/26 22:51:44  matt
** correct NULL filename handling
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
** Revision 1.19  2000/10/21 14:35:58  matt
** typo
**
** Revision 1.18  2000/10/17 03:22:37  matt
** cleaning up rom module
**
** Revision 1.17  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.16  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.15  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.14  2000/07/30 04:31:26  matt
** automagic loading of the nofrendo intro
**
** Revision 1.13  2000/07/25 02:20:58  matt
** cleanups
**
** Revision 1.12  2000/07/20 01:53:27  matt
** snprintf() ain't no standard function, eh?
**
** Revision 1.11  2000/07/19 16:06:54  neil
** little error fixed (tempinfo vs rominfo->info)
**
** Revision 1.10  2000/07/19 15:59:39  neil
** PATH_MAX, strncpy, snprintf, and strncat are our friends
**
** Revision 1.9  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.8  2000/07/06 16:47:50  matt
** new ppu palette setting calls
**
** Revision 1.7  2000/07/05 23:21:54  neil
** fclose(fp) should not be done if fp == NULL
**
** Revision 1.6  2000/07/04 04:45:14  matt
** changed include
**
** Revision 1.5  2000/06/26 04:56:10  matt
** minor cleanup
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
