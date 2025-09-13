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
** intro.h
**
** Nofrendo intro -- 6502 code
** $Id: intro.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _INTRO_H_
#define _INTRO_H_

#include "nes/nes_rom.h"

extern void intro_get_header(rominfo_t *rominfo);
extern int intro_get_rom(rominfo_t *rominfo);

#endif /* !_INTRO_H_ */

/*
** $Log: intro.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.3  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.2  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.1  2000/07/30 04:29:11  matt
** initial revision
**
*/
