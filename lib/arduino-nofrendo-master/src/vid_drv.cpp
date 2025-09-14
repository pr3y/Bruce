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
** vid_drv.c
**
** Video driver
** $Id: vid_drv.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <string.h>

#include "noftypes.h"
#include "log.h"
#include "bitmap.h"
#include "vid_drv.h"
#include "gui.h"
#include "osd.h"

/* hardware surface */
static bitmap_t *screen = NULL;

/* primary / backbuffer surfaces */
#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
static bitmap_t *primary_buffer = NULL, *back_buffer = NULL;
#else /* !NOFRENDO_DOUBLE_FRAMEBUFFER */
static bitmap_t *primary_buffer = NULL;
#endif /* !NOFRENDO_DOUBLE_FRAMEBUFFER */

static viddriver_t *driver = NULL;

/* fast automagic loop unrolling */
#define DUFFS_DEVICE(transfer, count)   \
   {                                    \
      register int n = (count + 7) / 8; \
      switch (count % 8)                \
      {                                 \
      case 0:                           \
         do                             \
         {                              \
            {                           \
               transfer;                \
            }                           \
         case 7:                        \
         {                              \
            transfer;                   \
         }                              \
         case 6:                        \
         {                              \
            transfer;                   \
         }                              \
         case 5:                        \
         {                              \
            transfer;                   \
         }                              \
         case 4:                        \
         {                              \
            transfer;                   \
         }                              \
         case 3:                        \
         {                              \
            transfer;                   \
         }                              \
         case 2:                        \
         {                              \
            transfer;                   \
         }                              \
         case 1:                        \
         {                              \
            transfer;                   \
         }                              \
         } while (--n > 0);             \
      }                                 \
   }

/* some system dependent replacement routines (for speed) */
INLINE int vid_memcmp(const void *p1, const void *p2, int len)
{
   /* check for 32-bit aligned data */
   if (0 == (((uint32)p1 & 3) | ((uint32)p2 & 3)))
   {
      uint32 *dw1 = (uint32 *)p1;
      uint32 *dw2 = (uint32 *)p2;

      len >>= 2;

      DUFFS_DEVICE(if (*dw1++ != *dw2++) return -1, len);
   }
   else
   /* fall back to 8-bit compares */
   {
      uint8 *b1 = (uint8 *)p1;
      uint8 *b2 = (uint8 *)p2;

      DUFFS_DEVICE(if (*b1++ != *b2++) return -1, len);
   }

   return 0;
}

/* super-dooper assembly memcpy (thanks, SDL!) */
#if defined(__GNUC__) && defined(i386)
#define vid_memcpy(dest, src, len)                        \
   {                                                      \
      int u0, u1, u2;                                     \
      __asm__ __volatile__(                               \
          "  cld            \n"                           \
          "  rep            \n"                           \
          "  movsl          \n"                           \
          "  testb $2,%b4   \n"                           \
          "  je    1f       \n"                           \
          "  movsw          \n"                           \
          "1:               \n"                           \
          "  testb $1,%b4   \n"                           \
          "  je 2f          \n"                           \
          "  movsb          \n"                           \
          "2:               \n"                           \
          : "=&c"(u0), "=&D"(u1), "=&S"(u2)               \
          : "0"((len) / 4), "q"(len), "1"(dest), "2"(src) \
          : "memory");                                    \
   }
#else  /* !(defined(__GNUC__) && defined(i386)) */
INLINE void vid_memcpy(void *dest, const void *src, int len)
{
   uint32 *s = (uint32 *)src;
   uint32 *d = (uint32 *)dest;

   ASSERT(0 == ((len & 3) | ((uint32)src & 3) | ((uint32)dest & 3)));
   len >>= 2;

   DUFFS_DEVICE(*d++ = *s++, len);
}
#endif /* !(defined(__GNUC__) && defined(i386)) */

/* TODO: any way to remove this filth (GUI needs it)? */
bitmap_t *vid_getbuffer(void)
{
   return primary_buffer;
}

void vid_setpalette(rgb_t *p)
{
   ASSERT(driver);
   ASSERT(p);

   driver->set_palette(p);
}

/* blits a bitmap onto primary buffer */
void vid_blit(bitmap_t *bitmap, int src_x, int src_y, int dest_x, int dest_y,
              int width, int height)
{
   int bitmap_pitch, primary_pitch;
   uint8 *dest_ptr, *src_ptr;

   ASSERT(bitmap);

   /* clip to source */
   if (src_y >= bitmap->height)
      return;
   if (src_y + height > bitmap->height)
      height = bitmap->height - src_y;

   if (src_x >= bitmap->width)
      return;
   if (src_x + width > bitmap->width)
      width = bitmap->width - src_x;

   /* clip to dest */
   if (dest_y + height <= 0)
   {
      return;
   }
   else if (dest_y < 0)
   {
      height += dest_y;
      src_y -= dest_y;
      dest_y = 0;
   }

   if (dest_y >= primary_buffer->height)
      return;
   if (dest_y + height > primary_buffer->height)
      height = primary_buffer->height - dest_y;

   if (dest_x + width <= 0)
   {
      return;
   }
   else if (dest_x < 0)
   {
      width += dest_x;
      src_x -= dest_x;
      dest_x = 0;
   }

   if (dest_x >= primary_buffer->width)
      return;
   if (dest_x + width > primary_buffer->width)
      width = primary_buffer->width - dest_x;

   src_ptr = bitmap->line[src_y] + src_x;
   dest_ptr = primary_buffer->line[dest_y] + dest_x;

   /* Avoid doing unnecessary indexed lookups */
   bitmap_pitch = bitmap->pitch;
   primary_pitch = primary_buffer->pitch;

   /* do the copy */
   while (height--)
   {
      vid_memcpy(dest_ptr, src_ptr, width);
      src_ptr += bitmap_pitch;
      dest_ptr += primary_pitch;
   }
}

static void vid_blitscreen(int num_dirties, rect_t *dirty_rects)
{
   int src_x, src_y, dest_x, dest_y;
   int blit_width, blit_height;

   screen = driver->lock_write();

   /* center in y direction */
   if (primary_buffer->height <= screen->height)
   {
      src_y = 0;
      blit_height = primary_buffer->height;
      dest_y = (screen->height - blit_height) >> 1;
   }
   else
   {
      src_y = (primary_buffer->height - screen->height) >> 1;
      blit_height = screen->height;
      dest_y = 0;
   }

   /* and in x */
   if (primary_buffer->width <= screen->width)
   {
      src_x = 0;
      blit_width = primary_buffer->width;
      dest_x = (screen->width - blit_width) >> 1;
   }
   else
   {
      src_x = (primary_buffer->width - screen->width) >> 1;
      blit_width = screen->width;
      dest_x = 0;
   }

   /* should we just copy the entire screen? */
   if (-1 == num_dirties)
   {
      uint8 *dest, *src;

      src = primary_buffer->line[src_y] + src_x;
      dest = screen->line[dest_y] + dest_x;

      while (blit_height--)
      {
         vid_memcpy(dest, src, primary_buffer->width);
         src += primary_buffer->pitch;
         dest += screen->pitch;
      }
   }
   else
   {
      /* we need to blit just a bunch of dirties */
      int i, j, height;
      rect_t *rects = dirty_rects;

      for (i = 0; i < num_dirties && blit_height; i++)
      {
         height = rects->h;
         if (blit_height < height)
            height = blit_height;

         j = 0;
         DUFFS_DEVICE(
             {
                vid_memcpy(screen->line[dest_y + rects->y + j] + rects->x + dest_x,
                           primary_buffer->line[src_y + rects->y + j] + rects->x + src_x,
                           rects->w);
                j++;
                blit_height--;
             },
             height);

         rects++;
      }
   }

   if (driver->free_write)
      driver->free_write(num_dirties, dirty_rects);
}

/* TODO: this code is sickly */

#define CHUNK_WIDTH 256
#define CHUNK_HEIGHT 16
#define MAX_DIRTIES ((256 / CHUNK_WIDTH) * (240 / CHUNK_HEIGHT))
#define DIRTY_CUTOFF ((3 * MAX_DIRTIES) / 4)

#if 0
INLINE int calc_dirties(rect_t *list)
{
   bool dirty;
   int num_dirties = 0;
   int i = 0, j, line_offset = 0;
   int iterations = primary_buffer->height / CHUNK_HEIGHT;

   for (i = 0; i < iterations; i++)
   {
      dirty = false;

      j = line_offset;
      DUFFS_DEVICE(
      { 
         if (vid_memcmp(back_buffer->line[j], primary_buffer->line[j], 
                        CHUNK_WIDTH))
         { 
            dirty = true; 
            break; 
         } 

         j++; 
      }, CHUNK_HEIGHT);

      if (true == dirty)
      {
         list->h = CHUNK_HEIGHT;
         list->w = CHUNK_WIDTH;
         list->x = 0;
         list->y = line_offset;
         list++;

         /* totally arbitrary at this point */
         if (++num_dirties > DIRTY_CUTOFF)
            return -1;
      }

      line_offset += CHUNK_HEIGHT;
   }

   return num_dirties;
}
#endif

void vid_flush(void)
{
   bitmap_t *temp;
   int num_dirties;
   rect_t dirty_rects[MAX_DIRTIES];

   ASSERT(driver);

   if (true == driver->invalidate)
   {
      driver->invalidate = false;
      num_dirties = -1;
   }
   else
   {
      //num_dirties = calc_dirties(dirty_rects);
      num_dirties = -1;
   }

   if (driver->custom_blit)
      driver->custom_blit(primary_buffer, num_dirties, dirty_rects);
   else
      vid_blitscreen(num_dirties, dirty_rects);

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   /* Swap pointers to the main/back buffers */
   temp = back_buffer;
   back_buffer = primary_buffer;
   primary_buffer = temp;
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */
}

/* emulated machine tells us which resolution it wants */
int vid_setmode(int width, int height)
{
   if (NULL != primary_buffer)
      bmp_destroy(&primary_buffer);
#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   if (NULL != back_buffer)
      bmp_destroy(&back_buffer);
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   primary_buffer = bmp_create(width, height, 0); /* no overdraw */
   if (NULL == primary_buffer)
      return -1;

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   /* Create our backbuffer */
   back_buffer = bmp_create(width, height, 0); /* no overdraw */
   if (NULL == back_buffer)
   {
      bmp_destroy(&primary_buffer);
      return -1;
   }
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   bmp_clear(primary_buffer, GUI_BLACK);

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   bmp_clear(back_buffer, GUI_BLACK);
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   return 0;
}

static int vid_findmode(int width, int height, viddriver_t *osd_driver)
{
   if (osd_driver->init(width, height))
   {
      driver = NULL;
      return -1; /* mode not available! */
   }

   /* we got our driver */
   driver = osd_driver;

   /* re-assert dimensions, clear the surface */
   screen = driver->lock_write();

   /* use custom pageclear, if necessary */
   if (driver->clear)
      driver->clear(GUI_BLACK);
   else
      bmp_clear(screen, GUI_BLACK);

   /* release surface */
   if (driver->free_write)
      driver->free_write(-1, NULL);

   nofrendo_log_printf("video driver: %s at %dx%d\n", driver->name,
                       screen->width, screen->height);

   return 0;
}

/* This is the interface to the drivers, used in nofrendo.c */
int vid_init(int width, int height, viddriver_t *osd_driver)
{
   if (vid_findmode(width, height, osd_driver))
   {
      nofrendo_log_printf("video initialization failed for %s at %dx%d\n",
                          osd_driver->name, width, height);
      return -1;
   }

   return 0;
}

void vid_shutdown(void)
{
   if (NULL == driver)
      return;

   if (NULL != primary_buffer)
      bmp_destroy(&primary_buffer);

#ifdef NOFRENDO_DOUBLE_FRAMEBUFFER
   if (NULL != back_buffer)
      bmp_destroy(&back_buffer);
#endif /* NOFRENDO_DOUBLE_FRAMEBUFFER */

   if (driver && driver->shutdown)
      driver->shutdown();
}

/*
** $Log: vid_drv.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.40  2000/11/25 20:26:42  matt
** not much
**
** Revision 1.39  2000/11/16 14:27:27  matt
** even more crash-proofness
**
** Revision 1.38  2000/11/16 14:11:18  neil
** Better *not* to crash in case of catastrophic failure in the driver
**
** Revision 1.37  2000/11/13 00:55:16  matt
** no dirties seems to be faster (!?!?)
**
** Revision 1.36  2000/11/06 05:16:18  matt
** minor clipping bug
**
** Revision 1.35  2000/11/06 02:16:26  matt
** cleanups
**
** Revision 1.34  2000/11/05 22:53:13  matt
** only one video driver per system, please
**
** Revision 1.33  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.32  2000/11/05 06:23:41  matt
** thinlib spawns changes
**
** Revision 1.31  2000/10/10 13:58:14  matt
** stroustrup squeezing his way in the door
**
** Revision 1.30  2000/10/10 13:03:53  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.29  2000/10/08 17:58:23  matt
** lock_read() should not allow us to clear the bitmap
**
** Revision 1.28  2000/09/18 02:06:48  matt
** -pedantic is your friend
**
** Revision 1.27  2000/08/16 02:53:05  matt
** changed init() interface a wee bit
**
** Revision 1.26  2000/08/14 02:45:59  matt
** fixed nasty bug in vid_blitscreen
**
** Revision 1.24  2000/08/11 01:44:37  matt
** clipping bugfix
**
** Revision 1.23  2000/07/31 04:28:47  matt
** one million cleanups
**
** Revision 1.22  2000/07/28 07:25:49  neil
** Video driver has an invalidate flag, telling vid_drv not to calculate dirties for the next frame
**
** Revision 1.21  2000/07/28 03:51:45  matt
** lock_read used instead of lock_write in some places
**
** Revision 1.20  2000/07/28 01:24:05  matt
** dirty rectangle support
**
** Revision 1.19  2000/07/27 23:49:52  matt
** no more getmode
**
** Revision 1.18  2000/07/27 04:30:37  matt
** change to get_mode api
**
** Revision 1.17  2000/07/27 04:05:58  matt
** changed place where name goes
**
** Revision 1.16  2000/07/27 01:16:22  matt
** api changes for new main and dirty rects
**
** Revision 1.15  2000/07/26 21:36:13  neil
** Big honkin' change -- see the mailing list
**
** Revision 1.14  2000/07/24 04:33:57  matt
** skeleton of dirty rectangle code in place
**
** Revision 1.13  2000/07/23 14:35:39  matt
** cleanups
**
** Revision 1.12  2000/07/18 19:41:26  neil
** use screen->pitch in blitscreen, not screen_width
**
** Revision 1.11  2000/07/11 04:30:16  matt
** overdraw unnecessary!
**
** Revision 1.10  2000/07/10 19:07:57  matt
** added custom clear() member call to video driver
**
** Revision 1.9  2000/07/10 03:06:49  matt
** my dependency file is broken... *snif*
**
** Revision 1.8  2000/07/10 03:04:15  matt
** removed scanlines, backbuffer from custom blit
**
** Revision 1.7  2000/07/10 01:03:20  neil
** New video scheme allows for custom blitters to be determined by the driver at runtime
**
** Revision 1.6  2000/07/09 03:34:46  matt
** temporary cleanup
**
** Revision 1.5  2000/07/08 23:48:29  neil
** Another assumption GGI kills: pitch == width for hardware bitmaps
**
** Revision 1.4  2000/07/07 20:18:03  matt
** added overdraw, fixed some bugs in blitters
**
** Revision 1.3  2000/07/07 18:33:55  neil
** no need to lock for reading just to get the dimensions
**
** Revision 1.2  2000/07/07 18:11:37  neil
** Generalizing the video driver
**
** Revision 1.1  2000/07/06 16:48:47  matt
** initial revision
**
*/
