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
** pcx.c
**
** PCX format screen-saving routines
** $Id: pcx.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <stdio.h>
#include <string.h>

#include "noftypes.h"
#include "bitmap.h"
#include "pcx.h"

/* Save a PCX snapshot from a given NES bitmap */
int pcx_write(char *filename, bitmap_t *bmp, rgb_t *pal)
{
   FILE *fp;
   pcxheader_t header;
   int i, line;
   int width, height, x_min, y_min;

   ASSERT(bmp);

   width = bmp->width;
   height = bmp->height;
   x_min = 0;
   y_min = 0;

   fp = fopen(filename, "wb");
   if (NULL == fp)
      return -1;

   /* Fill in the header nonsense */
   memset(&header, 0, sizeof(header));

   header.Manufacturer = 10;
   header.Version = 5;
   header.Encoding = 1;
   header.BitsPerPixel = 8;
   header.Xmin = x_min;
   header.Ymin = y_min;
   header.Xmax = width - 1;
   header.Ymax = height - 1;
   header.NPlanes = 1;
   header.BytesPerLine = width;
   header.PaletteInfo = 1;
   header.HscreenSize = width - 1;
   header.VscreenSize = height - 1;

   fwrite(&header, 1, sizeof(header), fp);

   /* RLE encoding */
   for (line = 0; line < height; line++)
   {
      uint8 last, *mem;
      int xpos = 0;

      mem = bmp->line[line + y_min] + x_min;

      while (xpos < width)
      {
         int rle_count = 0;

         do
         {
            last = *mem++;
            xpos++;
            rle_count++;
         } while (*mem == last && xpos < width && rle_count < 0x3F);

         if (rle_count > 1 || 0xC0 == (last & 0xC0))
         {
            fputc(0xC0 | rle_count, fp);
            fputc(last, fp);
         }
         else
         {
            fputc(last, fp);
         }
      }
   }

   /* Write palette */
   fputc(0x0C, fp); /* $0C signifies 256 color palette */
   for (i = 0; i < 256; i++)
   {
      fputc(pal[i].r, fp);
      fputc(pal[i].g, fp);
      fputc(pal[i].b, fp);
   }

   /* We're done! */
   fclose(fp);
   return 0;
}

/*
** $Log: pcx.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.8  2000/11/06 05:17:48  matt
** better!
**
** Revision 1.7  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.6  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.5  2000/06/26 04:55:13  matt
** changed routine name, big whoop
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
