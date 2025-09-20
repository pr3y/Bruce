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
** nesstate.h
**
** state saving header
** $Id: nesstate.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NESSTATE_H_
#define _NESSTATE_H_

#include "nes.h"

extern void state_setslot(int slot);
extern int state_load();
extern int state_save();

#endif /* _NESSTATE_H_ */

/*
** $Log: nesstate.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.2  2000/10/25 00:23:16  matt
** makefiles updated for new directory structure
**
** Revision 1.1  2000/10/24 12:20:28  matt
** changed directory structure
**
** Revision 1.4  2000/10/23 17:50:47  matt
** adding fds support
**
** Revision 1.3  2000/07/25 02:21:23  matt
** changed routine names to reduce confusion with SNSS routines
**
** Revision 1.2  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.1  2000/06/29 03:08:18  matt
** initial revision
**
*/
