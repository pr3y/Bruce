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
** gui.h
**
** GUI defines / prototypes
** $Id: gui.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _GUI_H_
#define _GUI_H_

/* GUI colors - the last 64 of a 256-color palette */

#define GUI_FIRSTENTRY 192

enum
{
   GUI_BLACK = GUI_FIRSTENTRY,
   GUI_DKGRAY,
   GUI_GRAY,
   GUI_LTGRAY,
   GUI_WHITE,
   GUI_RED,
   GUI_GREEN,
   GUI_BLUE,
   GUI_YELLOW,
   GUI_ORANGE,
   GUI_PURPLE,
   GUI_TEAL,
   GUI_DKGREEN,
   GUI_DKBLUE,
   GUI_LASTENTRY
};

#define GUI_TOTALCOLORS (GUI_LASTENTRY - GUI_FIRSTENTRY)

/* TODO: bleh */
#include "bitmap.h"
extern rgb_t gui_pal[GUI_TOTALCOLORS];

#define MAX_MSG_LENGTH 256

typedef struct message_s
{
   int ttl;
   char text[MAX_MSG_LENGTH];
   uint8 color;
} message_t;

extern void gui_tick(int ticks);
extern void gui_setrefresh(int frequency);

extern void gui_sendmsg(int color, char *format, ...);

extern int gui_init(void);
extern void gui_shutdown(void);

extern void gui_frame(bool draw);

extern void gui_togglefps(void);
extern void gui_togglegui(void);
extern void gui_togglewave(void);
extern void gui_togglepattern(void);
extern void gui_toggleoam(void);

extern void gui_decpatterncol(void);
extern void gui_incpatterncol(void);

extern void gui_savesnap(void);
extern void gui_togglesprites(void);
extern void gui_togglefs(void);
extern void gui_displayinfo();
extern void gui_toggle_chan(int chan);
extern void gui_setfilter(int filter_type);

#endif /* _GUI_H_ */

/*
** $Log: gui.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.17  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.16  2000/10/27 12:57:49  matt
** fixed pcx snapshots
**
** Revision 1.15  2000/10/23 17:50:47  matt
** adding fds support
**
** Revision 1.14  2000/10/23 15:52:04  matt
** better system handling
**
** Revision 1.13  2000/10/22 19:15:39  matt
** more sane timer ISR / autoframeskip
**
** Revision 1.12  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.11  2000/10/10 13:03:53  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.10  2000/10/08 17:59:12  matt
** gui_ticks is volatile
**
** Revision 1.9  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.8  2000/07/25 02:20:47  matt
** moved gui palette filth here, for the time being
**
** Revision 1.7  2000/07/23 15:16:25  matt
** moved non-osd code here
**
** Revision 1.6  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.5  2000/07/09 03:39:33  matt
** small gui_frame cleanup
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
