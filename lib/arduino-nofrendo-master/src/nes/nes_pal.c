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
** nes_pal.c
**
** NES RGB palette
** $Id: nes_pal.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <math.h>
#include "../noftypes.h"
#include "../bitmap.h"
#include "nes_pal.h"

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

/* my NES palette, converted to RGB */
rgb_t shady_palette[] =
    {
        {0x80, 0x80, 0x80}, {0x00, 0x00, 0xBB}, {0x37, 0x00, 0xBF}, {0x84, 0x00, 0xA6}, {0xBB, 0x00, 0x6A}, {0xB7, 0x00, 0x1E}, {0xB3, 0x00, 0x00}, {0x91, 0x26, 0x00}, {0x7B, 0x2B, 0x00}, {0x00, 0x3E, 0x00}, {0x00, 0x48, 0x0D}, {0x00, 0x3C, 0x22}, {0x00, 0x2F, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05}, {0x05, 0x05, 0x05},

        {0xC8, 0xC8, 0xC8},
        {0x00, 0x59, 0xFF},
        {0x44, 0x3C, 0xFF},
        {0xB7, 0x33, 0xCC},
        {0xFF, 0x33, 0xAA},
        {0xFF, 0x37, 0x5E},
        {0xFF, 0x37, 0x1A},
        {0xD5, 0x4B, 0x00},
        {0xC4, 0x62, 0x00},
        {0x3C, 0x7B, 0x00},
        {0x1E, 0x84, 0x15},
        {0x00, 0x95, 0x66},
        {0x00, 0x84, 0xC4},
        {0x11, 0x11, 0x11},
        {0x09, 0x09, 0x09},
        {0x09, 0x09, 0x09},

        {0xFF, 0xFF, 0xFF},
        {0x00, 0x95, 0xFF},
        {0x6F, 0x84, 0xFF},
        {0xD5, 0x6F, 0xFF},
        {0xFF, 0x77, 0xCC},
        {0xFF, 0x6F, 0x99},
        {0xFF, 0x7B, 0x59},
        {0xFF, 0x91, 0x5F},
        {0xFF, 0xA2, 0x33},
        {0xA6, 0xBF, 0x00},
        {0x51, 0xD9, 0x6A},
        {0x4D, 0xD5, 0xAE},
        {0x00, 0xD9, 0xFF},
        {0x66, 0x66, 0x66},
        {0x0D, 0x0D, 0x0D},
        {0x0D, 0x0D, 0x0D},

        {0xFF, 0xFF, 0xFF},
        {0x84, 0xBF, 0xFF},
        {0xBB, 0xBB, 0xFF},
        {0xD0, 0xBB, 0xFF},
        {0xFF, 0xBF, 0xEA},
        {0xFF, 0xBF, 0xCC},
        {0xFF, 0xC4, 0xB7},
        {0xFF, 0xCC, 0xAE},
        {0xFF, 0xD9, 0xA2},
        {0xCC, 0xE1, 0x99},
        {0xAE, 0xEE, 0xB7},
        {0xAA, 0xF7, 0xEE},
        {0xB3, 0xEE, 0xFF},
        {0xDD, 0xDD, 0xDD},
        {0x11, 0x11, 0x11},
        {0x11, 0x11, 0x11}};

/* dynamic palette building routines,
** care of Kevin Horton (khorton@iquest.net)
*/

/* our global palette */
rgb_t nes_palette[64];

static float hue = 334.0f;
static float tint = 0.4f;

#include "../gui.h"

void pal_dechue(void)
{
   hue -= 0.5f;
   gui_sendmsg(GUI_GREEN, "hue: %.02f", hue);
   pal_generate();
}
void pal_inchue(void)
{
   hue += 0.5f;
   gui_sendmsg(GUI_GREEN, "hue: %.02f", hue);
   pal_generate();
}
void pal_dectint(void)
{
   tint -= 0.01f;
   gui_sendmsg(GUI_GREEN, "tint: %.02f", tint);
   pal_generate();
}
void pal_inctint(void)
{
   tint += 0.01f;
   gui_sendmsg(GUI_GREEN, "tint: %.02f", tint);
   pal_generate();
}

static const float brightness[4][4] =
    {
        {0.50f, 0.75f, 1.00f, 1.00f},
        {0.29f, 0.45f, 0.73f, 0.90f},
        {0.00f, 0.24f, 0.47f, 0.77f},
        {0.02f, 0.04f, 0.05f, 0.07f}};

static const int col_angles[16] =
    {
        0, 240, 210, 180, 150, 120, 90, 60, 30, 0, 330, 300, 270, 0, 0, 0};

void pal_generate(void)
{
   int x, z;
   float s, y, theta;
   int r, g, b;

   for (x = 0; x < 4; x++)
   {
      for (z = 0; z < 16; z++)
      {
         switch (z)
         {
         case 0:
            /* is color $x0?  If so, get luma */
            s = 0;
            y = brightness[0][x];
            break;

         case 13:
            /* is color $xD?  If so, get luma */
            s = 0;
            y = brightness[2][x];
            break;

         case 14:
         case 15:
            /* is color $xE/F?  If so, set to black */
            s = 0;
            y = brightness[3][x];

            break;

         default:
            s = tint;             /* grab tint */
            y = brightness[1][x]; /* grab default luminance */
            break;
         }

         theta = (float)(PI * ((col_angles[z] + hue) / 180.0));

         r = (int)(256.0 * (y + s * sin(theta)));
         g = (int)(256.0 * (y - ((27 / 53.0) * s * sin(theta)) + ((10 / 53.0) * s * cos(theta))));
         b = (int)(256.0 * (y - (s * cos(theta))));

         if (r > 255)
            r = 255;
         else if (r < 0)
            r = 0;

         if (g > 255)
            g = 255;
         else if (g < 0)
            g = 0;

         if (b > 255)
            b = 255;
         else if (b < 0)
            b = 0;

         nes_palette[(x << 4) + z].r = r;
         nes_palette[(x << 4) + z].g = g;
         nes_palette[(x << 4) + z].b = b;
      }
   }
}

/*
** $Log: nes_pal.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.3  2000/11/06 02:17:18  matt
** no more double->float warnings
**
** Revision 1.2  2000/11/05 16:35:41  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.9  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.8  2000/07/10 13:49:31  matt
** renamed my palette and extern'ed it
**
** Revision 1.7  2000/06/26 04:59:13  matt
** selectable tint/hue hack (just for the time being)
**
** Revision 1.6  2000/06/21 21:48:19  matt
** changed range multiplier from 255.0 to 256.0
**
** Revision 1.5  2000/06/20 20:42:47  matt
** accuracy changes
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
