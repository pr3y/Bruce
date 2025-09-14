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
** types.h
**
** Data type definitions
** $Id: noftypes.h,v 1.1 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NOFTYPES_H_
#define _NOFTYPES_H_

#include <stdbool.h>

#define NOFRENDO_DEBUG
#define NOFRENDO_MEM_DEBUG
// #define NOFRENDO_VRAM_DEBUG
// #define NOFRENDO_LOG_TO_FILE
/* For the ESP32, it costs too much memory to render to a separate buffer and blit that to the main buffer.
   Instead, the code has been modified to directly grab the primary buffer from the video subsystem and render
   there, saving us about 64K of memory. */
// #define NOFRENDO_DOUBLE_FRAMEBUFFER

/* Define this if running on little-endian (x86) systems */
#define HOST_LITTLE_ENDIAN

#ifdef __GNUC__
#define INLINE static inline
#define ZERO_LENGTH 0
#elif defined(WIN32)
#define INLINE static __inline
#define ZERO_LENGTH 0
#else /* crapintosh? */
#define INLINE static
#define ZERO_LENGTH 1
#endif

/* quell stupid compiler warnings */
#define UNUSED(x) ((x) = (x))

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#include "memguard.h"
#include "log.h"

#ifdef NOFRENDO_DEBUG

#define ASSERT(expr) nofrendo_log_assert((int)(expr), __LINE__, __FILE__, NULL)
#define ASSERT_MSG(msg) nofrendo_log_assert(false, __LINE__, __FILE__, (msg))

#else /* !NOFRENDO_DEBUG */

#define ASSERT(expr)
#define ASSERT_MSG(msg)

#endif /* !NOFRENDO_DEBUG */

#endif /* _NOFTYPES_H_ */

/*
** $Log: noftypes.h,v $
** Revision 1.1  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.15  2000/11/05 16:37:18  matt
** rolled rgb.h into bitmap.h
**
** Revision 1.14  2000/10/17 03:22:16  matt
** safe UNUSED
**
** Revision 1.13  2000/10/10 13:58:14  matt
** stroustrup squeezing his way in the door
**
** Revision 1.12  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.11  2000/08/11 01:44:05  matt
** cosmeses
**
** Revision 1.10  2000/07/31 04:28:47  matt
** one million cleanups
**
** Revision 1.9  2000/07/24 04:30:17  matt
** ASSERTs should have been calling log_shutdown
**
** Revision 1.8  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.7  2000/07/04 04:46:44  matt
** moved INLINE define from osd.h
**
** Revision 1.6  2000/06/09 15:12:25  matt
** initial revision
**
*/
