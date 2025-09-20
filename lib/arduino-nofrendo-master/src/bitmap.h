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
** bitmap.h
**
** Bitmap object defines / prototypes
** $Id: bitmap.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "noftypes.h"

/* a bitmap rectangle */
typedef struct rect_s
{
   int16 x, y;
   uint16 w, h;
} rect_t;

typedef struct rgb_s
{
   int r, g, b;
} rgb_t;

typedef struct bitmap_s
{
   int width, height, pitch;
   bool hardware;            /* is data a hardware region? */
   uint8 *data;              /* protected */
   uint8 *line[ZERO_LENGTH]; /* will hold line pointers */
} bitmap_t;

extern void bmp_clear(const bitmap_t *bitmap, uint8 color);
extern bitmap_t *bmp_create(int width, int height, int overdraw);
extern bitmap_t *bmp_createhw(uint8 *addr, int width, int height, int pitch);
extern void bmp_destroy(bitmap_t **bitmap);

#endif /* _BITMAP_H_ */

/*
** $Log: bitmap.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.12  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.11  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.10  2000/09/18 02:06:48  matt
** -pedantic is your friend
**
** Revision 1.9  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.8  2000/07/24 04:31:43  matt
** pitch/data area on non-hw bitmaps get padded to 32-bit boundaries
**
** Revision 1.7  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.6  2000/07/09 14:43:01  matt
** pitch is now configurable for bmp_createhw()
**
** Revision 1.5  2000/07/06 16:46:57  matt
** added bmp_clear() routine
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
