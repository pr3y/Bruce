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
** nofrendo.h (c) 1998-2000 Matthew Conte (matt@conte.com)
**            (c) 2000 Neil Stevens (multivac@fcmail.com)
**
** Note: all architectures should call these functions
**
** $Id: nofrendo.h,v 1.2 2001/04/27 11:10:08 neil Exp $
*/

#ifndef _NOFRENDO_H_
#define _NOFRENDO_H_

typedef enum
{
   system_unknown,
   system_autodetect,
   system_nes,
   NUM_SUPPORTED_SYSTEMS
} system_t;

int nofrendo_main(int argc, char *argv[]);

extern volatile int nofrendo_ticks; /* system timer ticks */

/* osd_main should end with a call to main_loop().
** Pass filename = NULL if you want to start with the demo rom 
*/
extern int main_loop(const char *filename, system_t type);

/* These should not be called directly. Use the event interface */
extern void main_insert(const char *filename, system_t type);
extern void main_eject(void);
extern void main_quit(void);

#endif /* !_NOFRENDO_H_ */

/*
** $Log: nofrendo.h,v $
** Revision 1.2  2001/04/27 11:10:08  neil
** compile
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.9  2000/11/25 20:26:05  matt
** removed fds "system"
**
** Revision 1.8  2000/11/20 13:22:12  matt
** standardized timer ISR, added nofrendo_ticks
**
** Revision 1.7  2000/11/01 14:15:35  matt
** multi-system event system, or whatever
**
** Revision 1.6  2000/10/25 13:42:02  matt
** strdup - giddyap!
**
** Revision 1.5  2000/10/25 01:23:08  matt
** basic system autodetection
**
** Revision 1.4  2000/10/23 15:52:04  matt
** better system handling
**
** Revision 1.3  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.2  2000/07/27 01:16:36  matt
** sorted out the video problems
**
** Revision 1.1  2000/07/26 21:36:13  neil
** Big honkin' change -- see the mailing list
**
**
*/
