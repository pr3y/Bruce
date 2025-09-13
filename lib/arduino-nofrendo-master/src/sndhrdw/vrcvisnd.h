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
** vrcvisnd.h
**
** VRCVI (Konami MMC) sound hardware emulation header
** $Id: vrcvisnd.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _VRCVISND_H_
#define _VRCVISND_H_

#include "nes_apu.h"

extern apuext_t vrcvi_ext;

#endif /* _VRCVISND_H_ */

/*
** $Log: vrcvisnd.h,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:20:00  matt
** changed directory structure
**
** Revision 1.10  2000/09/27 12:26:03  matt
** changed sound accumulators back to floats
**
** Revision 1.9  2000/09/15 13:38:40  matt
** changes for optimized apu core
**
** Revision 1.8  2000/07/17 01:52:31  matt
** made sure last line of all source files is a newline
**
** Revision 1.7  2000/06/20 04:06:16  matt
** migrated external sound definition to apu module
**
** Revision 1.6  2000/06/20 00:08:58  matt
** changed to driver based API
**
** Revision 1.5  2000/06/09 16:49:02  matt
** removed all floating point from sound generation
**
** Revision 1.4  2000/06/09 15:12:28  matt
** initial revision
**
*/
