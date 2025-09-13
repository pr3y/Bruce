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
** memguard.h
**
** memory allocation wrapper routines
** $Id: memguard.h,v 1.1.1.1 2001/04/27 07:03:54 neil Exp $
*/

#ifndef _MEMGUARD_H_
#define _MEMGUARD_H_

#include <stdbool.h>

#include "noftypes.h"

#ifdef NOFRENDO_DEBUG

#define NOFRENDO_MALLOC(s) _my_malloc((s), __FILE__, __LINE__)
#define NOFRENDO_FREE(d) _my_free((void **)&(d), __FILE__, __LINE__)
#define NOFRENDO_STRDUP(s) _my_strdup((s), __FILE__, __LINE__)

extern void *_my_malloc(int size, char *file, int line);
extern void _my_free(void **data, char *file, int line);
extern char *_my_strdup(const char *string, char *file, int line);

#else /* !NOFRENDO_DEBUG */

/* Non-debugging versions of calls */
#define NOFRENDO_MALLOC(s) _my_malloc((s))
#define NOFRENDO_FREE(d) _my_free((void **)&(d))
#define NOFRENDO_STRDUP(s) _my_strdup((s))

extern void *_my_malloc(int size);
extern void _my_free(void **data);
extern char *_my_strdup(const char *string);

#endif /* !NOFRENDO_DEBUG */

extern void mem_cleanup(void);
extern void mem_checkblocks(void);
extern void mem_checkleaks(void);

extern bool mem_debug;

#endif /* _MEMGUARD_H_ */

/*
** $Log: memguard.h,v $
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.11  2000/10/28 03:55:46  neil
** strdup redefined, previous definition here
**
** Revision 1.10  2000/10/25 13:41:29  matt
** added strdup
**
** Revision 1.9  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.8  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.7  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.6  2000/07/06 17:20:52  matt
** block manager space itself wasn't being freed - d'oh!
**
** Revision 1.5  2000/06/26 04:54:48  matt
** simplified and made more robust
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
