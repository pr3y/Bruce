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
** gui.c
**
** GUI routines
** $Id: gui.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "noftypes.h"
#include "nes/nes_ppu.h"
#include "sndhrdw/nes_apu.h"
#include "nes/nesinput.h"
#include "nes/nes.h"
#include "log.h"
#include "osd.h"

#include "bitmap.h"

#include "gui.h"
#include "gui_elem.h"
#include "vid_drv.h"

/* TODO: oh god */
/* 8-bit GUI color table */
rgb_t gui_pal[GUI_TOTALCOLORS] =
    {
        {0x00, 0x00, 0x00}, /* black      */
        {0x3F, 0x3F, 0x3F}, /* dark gray  */
        {0x7F, 0x7F, 0x7F}, /* gray       */
        {0xBF, 0xBF, 0xBF}, /* light gray */
        {0xFF, 0xFF, 0xFF}, /* white      */
        {0xFF, 0x00, 0x00}, /* red        */
        {0x00, 0xFF, 0x00}, /* green      */
        {0x00, 0x00, 0xFF}, /* blue       */
        {0xFF, 0xFF, 0x00}, /* yellow     */
        {0xFF, 0xAF, 0x00}, /* orange     */
        {0xFF, 0x00, 0xFF}, /* purple     */
        {0x3F, 0x7F, 0x7F}, /* teal       */
        {0x00, 0x2A, 0x00}, /* dk. green  */
        {0x00, 0x00, 0x3F}  /* dark blue  */
};

/**************************************************************/
#include "pcx.h"
#include "nes/nesstate.h"
static bool option_drawsprites = true;

/* save a PCX snapshot */
void gui_savesnap(void)
{
   char filename[PATH_MAX];
   nes_t *nes = nes_getcontextptr();

   if (osd_makesnapname(filename, PATH_MAX) < 0)
      return;

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   if (pcx_write(filename, nes->vidbuf, nes->ppu->curpal))
#else /* !NOFRENDO_DOUBLE_FRAMEBUFFER */
   if (pcx_write(filename, vid_getbuffer(), nes->ppu->curpal))
#endif /* !NOFRENDO_DOUBLE_FRAMEBUFFER */

      return;

   gui_sendmsg(GUI_GREEN, "Screen saved to %s", filename);
}

/* Show/hide sprites (hiding sprites useful for making maps) */
void gui_togglesprites(void)
{
   option_drawsprites ^= true;
   ppu_displaysprites(option_drawsprites);
   gui_sendmsg(GUI_GREEN, "Sprites %s", option_drawsprites ? "displayed" : "hidden");
}

/* Set the frameskip policy */
void gui_togglefs(void)
{
   nes_t *machine = nes_getcontextptr();

   machine->autoframeskip ^= true;
   if (machine->autoframeskip)
      gui_sendmsg(GUI_YELLOW, "automatic frameskip");
   else
      gui_sendmsg(GUI_YELLOW, "unthrottled emulation");
}

/* display rom information */
void gui_displayinfo()
{
   gui_sendmsg(GUI_ORANGE, (char *)rom_getinfo(nes_getcontextptr()->rominfo));
}

void gui_toggle_chan(int chan)
{
#define FILL_CHAR 0x7C  /* ASCII 124 '|' */
#define BLANK_CHAR 0x7F /* ASCII 127   [delta] */
   static bool chan_enabled[6] = {true, true, true, true, true, true};

   chan_enabled[chan] ^= true;
   apu_setchan(chan, chan_enabled[chan]);

   gui_sendmsg(GUI_ORANGE, "%ca %cb %cc %cd %ce %cext",
               chan_enabled[0] ? FILL_CHAR : BLANK_CHAR,
               chan_enabled[1] ? FILL_CHAR : BLANK_CHAR,
               chan_enabled[2] ? FILL_CHAR : BLANK_CHAR,
               chan_enabled[3] ? FILL_CHAR : BLANK_CHAR,
               chan_enabled[4] ? FILL_CHAR : BLANK_CHAR,
               chan_enabled[5] ? FILL_CHAR : BLANK_CHAR);
}

void gui_setfilter(int filter_type)
{
   char *types[3] = {"no", "lowpass", "weighted"};
   static int last_filter = 2;

   if (last_filter == filter_type || filter_type < 0 || filter_type > 2)
      return;

   apu_setfilter(filter_type);
   gui_sendmsg(GUI_ORANGE, "%s filter", types[filter_type]);
   last_filter = filter_type;
}
/**************************************************************/

enum
{
   GUI_WAVENONE,
   GUI_WAVELINE,
   GUI_WAVESOLID,
   GUI_NUMWAVESTYLES
};

enum
{
   BUTTON_UP,
   BUTTON_DOWN
};

/* TODO: roll options into a structure */
static message_t msg;
static bool option_showfps = false;
static bool option_showgui = false;
static int option_wavetype = GUI_WAVENONE;
static bool option_showpattern = false;
static bool option_showoam = false;
static int pattern_col = 0;

/* timimg variables */
static bool gui_fpsupdate = false;
static int gui_ticks = 0;
static int gui_fps = 0;
static int gui_refresh = 60; /* default to 60Hz */

static int mouse_x, mouse_y, mouse_button;

static bitmap_t *gui_surface;

/* Put a pixel on our bitmap- just for GUI use */
INLINE void gui_putpixel(int x_pos, int y_pos, uint8 color)
{
   gui_surface->line[y_pos][x_pos] = color;
}

/* Line drawing */
static void gui_hline(int x_pos, int y_pos, int length, uint8 color)
{
   while (length--)
      gui_putpixel(x_pos++, y_pos, color);
}

static void gui_vline(int x_pos, int y_pos, int height, uint8 color)
{
   while (height--)
      gui_putpixel(x_pos, y_pos++, color);
}

/* Rectangles */
static void gui_rect(int x_pos, int y_pos, int width, int height, uint8 color)
{
   gui_hline(x_pos, y_pos, width, color);
   gui_hline(x_pos, y_pos + height - 1, width, color);
   gui_vline(x_pos, y_pos + 1, height - 2, color);
   gui_vline(x_pos + width - 1, y_pos + 1, height - 2, color);
}

static void gui_rectfill(int x_pos, int y_pos, int width, int height, uint8 color)
{
   while (height--)
      gui_hline(x_pos, y_pos++, width, color);
}

/* Draw the outline of a button */
static void gui_buttonrect(int x_pos, int y_pos, int width, int height, bool down)
{
   uint8 color1, color2;

   if (down)
   {
      color1 = GUI_GRAY;
      color2 = GUI_WHITE;
   }
   else
   {
      color1 = GUI_WHITE;
      color2 = GUI_GRAY;
   }

   gui_hline(x_pos, y_pos, width - 1, color1);
   gui_vline(x_pos, y_pos + 1, height - 2, color1);
   gui_hline(x_pos, y_pos + height - 1, width, color2);
   gui_vline(x_pos + width - 1, y_pos, height - 1, color2);
}

/* Text blitting */
INLINE void gui_charline(char ch, int x_pos, int y_pos, uint8 color)
{
   int count = 8;
   while (count--)
   {
      if (ch & (1 << count))
         gui_putpixel(x_pos, y_pos, color);
      x_pos++;
   }
}

static void gui_putchar(uint8 *dat, int height, int x_pos, int y_pos, uint8 color)
{
   while (height--)
      gui_charline(*dat++, x_pos, y_pos++, color);
}

/* Return length of text in pixels */
static int gui_textlen(char *str, font_t *font)
{
   int pixels = 0;
   int num_chars = strlen(str);

   while (num_chars--)
      pixels += font->character[(*str++ - 32)].spacing;

   return pixels;
}

/* Simple textout() type function */
static int gui_textout(char *str, int x_pos, int y_pos, font_t *font, uint8 color)
{
   int x_new;
   int num_chars = strlen(str);
   int code;

   x_new = x_pos;

   while (num_chars--)
   {
      /* Turn ASCII code into letter */
      code = *str++;
      if (code > 0x7F)
         code = 0x7F;
      code -= 32; /* normalize */
      gui_putchar(font->character[code].lines, font->height, x_new, y_pos, color);
      x_new += font->character[code].spacing;
   }

   /* Return the length in pixels */
   return (x_new - x_pos);
}

/* Draw bar-/button-type text */
static int gui_textbar(char *str, int x_pos, int y_pos, font_t *font,
                       uint8 color, uint8 bgcolor, bool buttonstate)
{
   int width = gui_textlen(str, &small);

   /* Fill the 'button' */
   gui_buttonrect(x_pos, y_pos, width + 3, font->height + 3, buttonstate);
   gui_rectfill(x_pos + 1, y_pos + 1, width + 1, font->height + 1, bgcolor);

   /* Print the text */
   return gui_textout(str, x_pos + 2, y_pos + 2, font, color);
}

/* Draw the mouse pointer */
static void gui_drawmouse(void)
{
   int ythresh, xthresh;
   int i, j, color;

   ythresh = gui_surface->height - mouse_y - 1;
   for (j = 0; j < CURSOR_HEIGHT; j++)
   {
      if (ythresh < 0)
         continue;

      xthresh = gui_surface->width - mouse_x - 1;
      for (i = 0; i < CURSOR_WIDTH; i++)
      {
         if (xthresh < 0)
            continue;

         color = cursor[(j * CURSOR_WIDTH) + i];

         if (color)
            gui_putpixel(mouse_x + i, mouse_y + j, cursor_color[color]);
         xthresh--;
      }
      ythresh--;
   }
}

void gui_tick(int ticks)
{

   static int fps_counter = 0;

   gui_ticks += ticks;
   fps_counter += ticks;

   if (fps_counter >= gui_refresh)
   {
      fps_counter -= gui_refresh;
      gui_fpsupdate = true;
   }
}

/* updated in sync with the timer interrupt */
static void gui_tickdec(void)
{
#ifdef NOFRENDO_DEBUG
   static int hertz_ticks = 0;
#endif /* NOFRENDO_DEBUG */
   int ticks = gui_ticks;

   if (0 == ticks)
      return;

   gui_ticks = 0;

#ifdef NOFRENDO_DEBUG
   /* Check for corrupt memory block every 10 seconds */
   hertz_ticks += ticks;
   if (hertz_ticks >= (10 * gui_refresh))
   {
      hertz_ticks -= (10 * gui_refresh);
      mem_checkblocks();
   }
#endif /* NOFRENDO_DEBUG */

   /* TODO: bleh */
   if (msg.ttl > 0)
   {
      msg.ttl -= ticks;
      if (msg.ttl < 0)
         msg.ttl = 0;
   }
}

/* Update the FPS display */
static void gui_updatefps(void)
{
   static char fpsbuf[20];

   /* Check to see if we need to do an sprintf or not */
   if (true == gui_fpsupdate)
   {
      sprintf(fpsbuf, "%4d FPS /%4d%%", gui_fps, (gui_fps * 100) / gui_refresh);
      gui_fps = 0;
      gui_fpsupdate = false;
   }

   gui_textout(fpsbuf, gui_surface->width - 1 - 90, 1, &small, GUI_GREEN);
}

/* Turn FPS on/off */
void gui_togglefps(void)
{
   option_showfps ^= true;
}

/* Turn GUI on/off */
void gui_togglegui(void)
{
   option_showgui ^= true;
}

void gui_togglewave(void)
{
   option_wavetype = (option_wavetype + 1) % GUI_NUMWAVESTYLES;
}

void gui_toggleoam(void)
{
   option_showoam ^= true;
}

/* TODO: hack! */
void gui_togglepattern(void)
{
   option_showpattern ^= true;
}

/* TODO: hack! */
void gui_decpatterncol(void)
{
   if (pattern_col && option_showpattern)
      pattern_col--;
}

/* TODO: hack! */
void gui_incpatterncol(void)
{
   if ((pattern_col < 7) && option_showpattern)
      pattern_col++;
}

/* Downward-scrolling message display */
static void gui_updatemsg(void)
{
   if (msg.ttl)
      gui_textbar(msg.text, 2, gui_surface->height - 10, &small, msg.color, GUI_DKGRAY, BUTTON_UP);
}

/* Little thing to display the waveform */
static void gui_updatewave(int wave_type)
{
#define WAVEDISP_WIDTH 128
   int loop, xofs, yofs;
   int difference, offset;
   float scale;
   uint8 val, oldval;
   int vis_length = 0;
   void *vis_buffer = NULL;
   int vis_bps;
   apu_t apu;

   apu_getcontext(&apu);
   vis_buffer = apu.buffer;
   vis_length = apu.num_samples;
   vis_bps = apu.sample_bits;

   xofs = (NES_SCREEN_WIDTH - WAVEDISP_WIDTH);
   yofs = 1;
   scale = (float)(vis_length / (float)WAVEDISP_WIDTH);

   if (NULL == vis_buffer)
   {
      /* draw centerline */
      gui_hline(xofs, yofs + 0x20, WAVEDISP_WIDTH, GUI_GRAY);
      gui_textbar("no sound", xofs + 40, yofs + 0x20 - 4, &small, GUI_RED, GUI_DKGRAY, BUTTON_UP);
   }
   else if (GUI_WAVELINE == wave_type)
   {
      /* draw centerline */
      gui_hline(xofs, yofs + 0x20, WAVEDISP_WIDTH, GUI_GRAY);

      /* initial old value */
      if (16 == vis_bps)
         oldval = 0x40 - (((((uint16 *)vis_buffer)[0] >> 8) ^ 0x80) >> 2);
      else
         oldval = 0x40 - (((uint8 *)vis_buffer)[0] >> 2);

      for (loop = 1; loop < WAVEDISP_WIDTH; loop++)
      {
         //val = 0x40 - (vis_buffer[(uint32) (loop * scale)] >> 2);
         if (16 == vis_bps)
            val = 0x40 - (((((uint16 *)vis_buffer)[(uint32)(loop * scale)] >> 8) ^ 0x80) >> 2);
         else
            val = 0x40 - (((uint8 *)vis_buffer)[(uint32)(loop * scale)] >> 2);
         if (oldval < val)
         {
            offset = oldval;
            difference = (val - oldval) + 1;
         }
         else
         {
            offset = val;
            difference = (oldval - val) + 1;
         }

         gui_vline(xofs + loop, yofs + offset, difference, GUI_GREEN);
         oldval = val;
      }
   }
   /* solid wave */
   else if (GUI_WAVESOLID == wave_type)
   {
      for (loop = 0; loop < WAVEDISP_WIDTH; loop++)
      {
         //val = vis_buffer[(uint32) (loop * scale)] >> 2;
         if (16 == vis_bps)
            val = ((((uint16 *)vis_buffer)[(uint32)(loop * scale)] >> 8) ^ 0x80) >> 2;
         else
            val = ((uint8 *)vis_buffer)[(uint32)(loop * scale)] >> 2;
         if (val == 0x20)
            gui_putpixel(xofs + loop, yofs + 0x20, GUI_GREEN);
         else if (val < 0x20)
            gui_vline(xofs + loop, yofs + 0x20, 0x20 - val, GUI_GREEN);
         else
            gui_vline(xofs + loop, yofs + 0x20 - (val - 0x20), val - 0x20,
                      GUI_GREEN);
      }
   }

   gui_rect(xofs, yofs - 1, WAVEDISP_WIDTH, 66, GUI_DKGRAY);
}

static void gui_updatepattern(void)
{
   /* Pretty it up a bit */
   gui_textbar("Pattern Table 0", 0, 0, &small, GUI_GREEN, GUI_DKGRAY, BUTTON_UP);
   gui_textbar("Pattern Table 1", 128, 0, &small, GUI_GREEN, GUI_DKGRAY, BUTTON_UP);
   gui_hline(0, 9, 256, GUI_DKGRAY);
   gui_hline(0, 138, 256, GUI_DKGRAY);

   /* Dump the actual tables */
   ppu_dumppattern(gui_surface, 0, 0, 10, pattern_col);
   ppu_dumppattern(gui_surface, 1, 128, 10, pattern_col);
}

static void gui_updateoam(void)
{
   int y;

   y = option_showpattern ? 140 : 0;
   gui_textbar("Current OAM", 0, y, &small, GUI_GREEN, GUI_DKGRAY, BUTTON_UP);
   ppu_dumpoam(gui_surface, 0, y + 9);
}

/* The GUI overlay */
void gui_frame(bool draw)
{
   gui_fps++;
   if (false == draw)
      return;

   gui_surface = vid_getbuffer();

   ASSERT(gui_surface);

   gui_tickdec();

   if (option_showfps)
      gui_updatefps();

   if (option_wavetype != GUI_WAVENONE)
      gui_updatewave(option_wavetype);

   if (option_showpattern)
      gui_updatepattern();

   if (option_showoam)
      gui_updateoam();

   if (msg.ttl)
      gui_updatemsg();

   if (option_showgui)
   {
      osd_getmouse(&mouse_x, &mouse_y, &mouse_button);
      gui_drawmouse();
   }
}

void gui_sendmsg(int color, char *format, ...)
{
   va_list arg;
   va_start(arg, format);
   vsprintf(msg.text, format, arg);

#ifdef NOFRENDO_DEBUG
   nofrendo_log_print("GUI: ");
   nofrendo_log_print(msg.text);
   nofrendo_log_print("\n");
#endif /* NOFRENDO_DEBUG */

   va_end(arg);

   msg.ttl = gui_refresh * 2; /* 2 second delay */
   msg.color = color;
}

void gui_setrefresh(int frequency)
{
   gui_refresh = frequency;
}

int gui_init(void)
{
   gui_refresh = 60;
   memset(&msg, 0, sizeof(message_t));

   return 0; /* can't fail */
}

void gui_shutdown(void)
{
}

/*
** $Log: gui.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.26  2000/11/25 20:26:05  matt
** removed fds "system"
**
** Revision 1.25  2000/11/09 14:05:43  matt
** state load fixed, state save mostly fixed
**
** Revision 1.24  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.23  2000/10/27 12:57:49  matt
** fixed pcx snapshots
**
** Revision 1.22  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.21  2000/10/23 17:50:47  matt
** adding fds support
**
** Revision 1.20  2000/10/23 15:52:04  matt
** better system handling
**
** Revision 1.19  2000/10/22 19:15:39  matt
** more sane timer ISR / autoframeskip
**
** Revision 1.18  2000/10/17 03:22:37  matt
** cleaning up rom module
**
** Revision 1.17  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.16  2000/10/10 13:03:53  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.15  2000/10/08 17:59:12  matt
** gui_ticks is volatile
**
** Revision 1.14  2000/09/15 04:58:06  matt
** simplifying and optimizing APU core
**
** Revision 1.13  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.12  2000/07/30 04:29:59  matt
** no more apu_getpcmdata hack
**
** Revision 1.11  2000/07/25 02:20:47  matt
** moved gui palette filth here, for the time being
**
** Revision 1.10  2000/07/24 04:32:05  matt
** bugfix on message delay
**
** Revision 1.9  2000/07/23 15:16:25  matt
** moved non-osd code here
**
** Revision 1.8  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.7  2000/07/11 04:40:23  matt
** updated for new screen dimension defines
**
** Revision 1.6  2000/07/09 03:39:33  matt
** small gui_frame cleanup
**
** Revision 1.5  2000/07/06 16:47:18  matt
** new video driver interface
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
