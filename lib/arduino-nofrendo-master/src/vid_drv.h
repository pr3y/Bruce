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
** vid_drv.h
**
** Video driver
** $Id: vid_drv.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _VID_DRV_H_
#define _VID_DRV_H_

#include "bitmap.h"

typedef struct viddriver_s
{
   /* name of driver */
   const char *name;
   /* init function - return 0 on success, nonzero on failure */
   int (*init)(int width, int height);
   /* clean up after driver (can be NULL) */
   void (*shutdown)(void);
   /* set a video mode - return 0 on success, nonzero on failure */
   int (*set_mode)(int width, int height);
   /* set up a palette */
   void (*set_palette)(rgb_t *palette);
   /* custom bitmap clear (can be NULL) */
   void (*clear)(uint8 color);
   /* lock surface for writing (required) */
   bitmap_t *(*lock_write)(void);
   /* free a locked surface (can be NULL) */
   void (*free_write)(int num_dirties, rect_t *dirty_rects);
   /* custom blitter - num_dirties == -1 if full blit required */
   void (*custom_blit)(bitmap_t *primary, int num_dirties,
                       rect_t *dirty_rects);
   /* immediately invalidate the buffer, i.e. full redraw */
   bool invalidate;
} viddriver_t;

/* TODO: filth */
extern bitmap_t *vid_getbuffer(void);

extern int vid_init(int width, int height, viddriver_t *osd_driver);
extern void vid_shutdown(void);

extern int vid_setmode(int width, int height);
extern void vid_setpalette(rgb_t *pal);

extern void vid_blit(bitmap_t *bitmap, int src_x, int src_y, int dest_x,
                     int dest_y, int blit_width, int blit_height);
extern void vid_flush(void);

#endif /* _VID_DRV_H_ */

/*
** $Log: vid_drv.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.22  2000/11/05 22:53:13  matt
** only one video driver per system, please
**
** Revision 1.21  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.20  2000/11/05 06:23:41  matt
** thinlib spawns changes
**
** Revision 1.19  2000/10/10 13:58:14  matt
** stroustrup squeezing his way in the door
**
** Revision 1.18  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.17  2000/08/16 02:53:04  matt
** changed init() interface a wee bit
**
** Revision 1.16  2000/07/31 04:28:47  matt
** one million cleanups
**
** Revision 1.15  2000/07/28 07:25:49  neil
** Video driver has an invalidate flag, telling vid_drv not to calculate dirties for the next frame
**
** Revision 1.14  2000/07/27 23:49:52  matt
** no more getmode
**
** Revision 1.13  2000/07/27 04:30:37  matt
** change to get_mode api
**
** Revision 1.12  2000/07/27 04:08:04  matt
** char * -> const char *
**
** Revision 1.11  2000/07/27 04:05:58  matt
** changed place where name goes
**
** Revision 1.10  2000/07/27 01:16:22  matt
** api changes for new main and dirty rects
**
** Revision 1.9  2000/07/26 21:36:14  neil
** Big honkin' change -- see the mailing list
**
** Revision 1.8  2000/07/24 04:33:57  matt
** skeleton of dirty rectangle code in place
**
** Revision 1.7  2000/07/10 19:07:57  matt
** added custom clear() member call to video driver
**
** Revision 1.6  2000/07/10 03:04:15  matt
** removed scanlines, backbuffer from custom blit
**
** Revision 1.5  2000/07/10 01:03:20  neil
** New video scheme allows for custom blitters to be determined by the driver at runtime
**
** Revision 1.4  2000/07/09 03:34:47  matt
** temporary cleanup
**
** Revision 1.3  2000/07/07 20:17:35  matt
** better custom blitting support
**
** Revision 1.2  2000/07/07 18:11:38  neil
** Generalizing the video driver
**
** Revision 1.1  2000/07/06 16:48:47  matt
** initial revision
**
*/
