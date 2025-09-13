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
** osd.h
**
** O/S dependent routine defintions (must be customized)
** $Id: osd.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _OSD_H_
#define _OSD_H_

#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 512
#endif /* PATH_MAX */

#ifdef __GNUC__
#define __PACKED__ __attribute__((packed))

#ifdef __DJGPP__
#define PATH_SEP '\\'
#else /* !__DJGPP__ */
#define PATH_SEP '/'
#endif /* !__DJGPP__ */

#elif defined(WIN32)
#define __PACKED__
#define PATH_SEP '\\'
#else /* !defined(WIN32) && !__GNUC__ */
#define __PACKED__
#define PATH_SEP ':'
#endif /* !defined(WIN32) && !__GNUC__ */

#if !defined(WIN32) && !defined(__DJGPP__)
#define stricmp strcasecmp
#endif /* !WIN32 && !__DJGPP__ */

/* memory allocation */
extern void *mem_alloc(int size, bool prefer_fast_memory);

/* audio */
extern void osd_setsound(void (*playfunc)(void *buffer, int size));

#ifndef NSF_PLAYER
#include "noftypes.h"
#include "vid_drv.h"

typedef struct vidinfo_s
{
   int default_width, default_height;
   viddriver_t *driver;
} vidinfo_t;

typedef struct sndinfo_s
{
   int sample_rate;
   int bps;
} sndinfo_t;

/* get info */
extern void osd_getvideoinfo(vidinfo_t *info);
extern void osd_getsoundinfo(sndinfo_t *info);

/* init / shutdown */
extern int osd_init(void);
extern void osd_shutdown(void);
extern int osd_main(int argc, char *argv[]);

extern int osd_installtimer(int frequency, void *func, int funcsize,
                            void *counter, int countersize);

/* input */
extern void osd_getinput(void);
extern void osd_getmouse(int *x, int *y, int *button);

/* filename manipulation */
extern void osd_fullname(char *fullname, const char *shortname);
extern char *osd_newextension(char *string, char *ext);

/* build a filename for a snapshot, return -ve for error */
extern int osd_makesnapname(char *filename, int len);

#endif /* !NSF_PLAYER */

#endif /* _OSD_H_ */

/*
** $Log: osd.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.30  2000/11/05 22:53:13  matt
** only one video driver per system, please
**
** Revision 1.29  2000/11/05 06:23:41  matt
** thinlib spawns changes
**
** Revision 1.28  2000/10/22 19:15:39  matt
** more sane timer ISR / autoframeskip
**
** Revision 1.27  2000/10/21 19:25:59  matt
** many more cleanups
**
** Revision 1.26  2000/10/10 13:03:53  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.25  2000/10/02 15:01:12  matt
** frag size has been removed
**
** Revision 1.24  2000/08/31 02:40:18  matt
** fixed some crap
**
** Revision 1.23  2000/08/16 02:57:14  matt
** changed video interface a wee bit
**
** Revision 1.22  2000/08/11 01:46:30  matt
** new OSD sound information interface
**
** Revision 1.21  2000/08/04 15:01:32  neil
** BeOS cleanups
**
** Revision 1.20  2000/08/04 14:36:14  neil
** BeOS is working.. kinda
**
** Revision 1.19  2000/07/26 21:36:13  neil
** Big honkin' change -- see the mailing list
**
** Revision 1.18  2000/07/23 16:21:35  neil
** neither stricmp nor strcasecmp works everywhere
**
** Revision 1.17  2000/07/23 15:17:40  matt
** osd_getfragsize
**
** Revision 1.16  2000/07/21 13:37:20  neil
** snap filenames are OS-dependent
**
** Revision 1.15  2000/07/21 04:58:37  matt
** new osd_main structure
**
** Revision 1.14  2000/07/21 04:26:15  matt
** added some nasty externs
**
** Revision 1.13  2000/07/21 02:42:45  matt
** merged osd_getinput and osd_gethostinput
**
** Revision 1.12  2000/07/19 13:10:35  neil
** PATH_MAX
**
** Revision 1.11  2000/07/10 03:04:15  matt
** removed scanlines, backbuffer from custom blit
**
** Revision 1.10  2000/07/06 16:48:25  matt
** new video driver
**
** Revision 1.9  2000/07/05 17:26:16  neil
** Moved the externs in nofrendo.c to osd.h
**
** Revision 1.8  2000/07/04 23:07:06  matt
** djgpp path separator bugfix
**
** Revision 1.7  2000/07/04 04:45:33  matt
** moved INLINE define into types.h
**
** Revision 1.6  2000/06/29 16:06:18  neil
** Wrapped DOS-specific headers in an ifdef
**
** Revision 1.5  2000/06/09 15:12:25  matt
** initial revision
**
*/
