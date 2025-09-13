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
** nes_pal.h
**
** NES palette definition
** $Id: nes_pal.h,v 1.1.1.1 2001/04/27 07:03:54 neil Exp $
*/

#ifndef _NESPAL_H_
#define _NESPAL_H_

extern rgb_t nes_palette[];
extern rgb_t shady_palette[];

extern void pal_generate(void);

/* TODO: these are temporary hacks */
extern void pal_dechue(void);
extern void pal_inchue(void);
extern void pal_dectint(void);
extern void pal_inctint(void);

#endif /* _NESPAL_H_ */

/*
** $Log: nes_pal.h,v $
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.8  2000/07/31 04:27:59  matt
** one million cleanups
**
** Revision 1.7  2000/07/21 04:20:35  matt
** added some nasty externs
**
** Revision 1.6  2000/07/10 13:49:32  matt
** renamed my palette and extern'ed it
**
** Revision 1.5  2000/07/05 17:14:34  neil
** Linux: Act Two, Scene One
**
** Revision 1.4  2000/06/09 15:12:26  matt
** initial revision
**
*/
