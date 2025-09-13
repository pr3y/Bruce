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
** memguard.c
**
** memory allocation wrapper routines
**
** NOTE: based on code (c) 1998 the Retrocade group
** $Id: memguard.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <string.h>
#include <stdlib.h>

#include "noftypes.h"
#include "memguard.h"
#include "log.h"

/* Maximum number of allocated blocks at any one time */
#define MAX_BLOCKS 4096

/* Memory block structure */
typedef struct memblock_s
{
   void *block_addr;
   int block_size;
   char *file_name;
   int line_num;
} memblock_t;

/* debugging flag */
#ifdef NOFRENDO_MEM_DEBUG
bool mem_debug = true;
#else /* !NOFRENDO_MEM_DEBUG */
bool mem_debug = false;
#endif /* !NOFRENDO_MEM_DEBUG */

#ifdef NOFRENDO_DEBUG

static int mem_blockcount = 0; /* allocated block count */
static memblock_t *mem_record = NULL;

#define GUARD_STRING "GgUuAaRrDdSsTtRrIiNnGgBbLlOoCcKk"
#define GUARD_LENGTH 32 /* before and after allocated block */

/*
** Check the memory guard to make sure out of bounds writes have not
** occurred.
*/
static int mem_checkguardblock(void *data, int guard_size)
{
   char *check, *block;
   int i, alloc_size;

   /* get the original pointer */
   block = ((char *)data) - guard_size;

   /* get the size */
   alloc_size = *((uint32 *)block);
   block += 4;

   /* check leading guard string */
   check = GUARD_STRING;
   for (i = sizeof(uint32); i < guard_size; i++)
   {
      /* wrap */
      if ('\0' == *check)
         check = GUARD_STRING;

      if (*block++ != *check++)
         return -1;
   }

   /* check end of block */
   check = GUARD_STRING;
   block = ((char *)data) + alloc_size;
   for (i = 0; i < guard_size; i++)
   {
      /* wrap */
      if ('\0' == *check)
         check = GUARD_STRING;
      if (*block++ != *check++)
         return -1;
   }

   /* we're okay! */
   return 0;
}

/* free a guard block */
static void mem_freeguardblock(void *data, int guard_size)
{
   char *orig = ((char *)data) - guard_size;

   free(orig);
}

/* allocate memory, guarding with a guard block in front and behind */
static void *mem_guardalloc(int alloc_size, int guard_size)
{
   void *orig;
   char *block, *check;
   uint32 *ptr;
   int i;

   /* pad it up to a 32-bit multiple */
   alloc_size = (alloc_size + 3) & ~3;

   /* allocate memory */
   // orig = malloc(alloc_size + (guard_size * 2));
   orig = mem_alloc(alloc_size + (guard_size * 2), true);
   if (NULL == orig)
      return NULL;

   block = (char *)orig;

   /* get it to the pointer we will actually return */
   orig = (void *)((char *)orig + guard_size);

   /* trash it all */
   ptr = (uint32 *)orig;
   for (i = alloc_size / 4; i; i--)
      *ptr++ = 0xDEADBEEF;

   /* store the size of the newly allocated block*/
   *((uint32 *)block) = alloc_size;
   block += 4;

   /* put guard string at beginning of block */
   check = GUARD_STRING;
   for (i = sizeof(uint32); i < guard_size; i++)
   {
      /* wrap */
      if ('\0' == *check)
         check = GUARD_STRING;

      *block++ = *check++;
   }

   /* put at end of block */
   check = GUARD_STRING;
   block = (char *)orig + alloc_size;
   for (i = 0; i < guard_size; i++)
   {
      /* wrap */
      if ('\0' == *check)
         check = GUARD_STRING;

      *block++ = *check++;
   }

   return orig;
}

/* Free up the space used by the memory block manager */
void mem_cleanup(void)
{
   if (mem_record)
   {
      free(mem_record);
      mem_record = NULL;
   }
}

/* Allocate a bunch of memory to keep track of all memory blocks */
static void mem_init(void)
{
   mem_cleanup();

   mem_blockcount = 0;

   // mem_record = malloc(MAX_BLOCKS * sizeof(memblock_t));
   mem_record = mem_alloc(MAX_BLOCKS * sizeof(memblock_t), false);

   ASSERT(mem_record);
   memset(mem_record, 0, MAX_BLOCKS * sizeof(memblock_t));
}

/* add a block of memory to the master record */
static void mem_addblock(void *data, int block_size, char *file, int line)
{
   int i;

   for (i = 0; i < MAX_BLOCKS; i++)
   {
      if (NULL == mem_record[i].block_addr)
      {
         mem_record[i].block_addr = data;
         mem_record[i].block_size = block_size;
         mem_record[i].file_name = file;
         mem_record[i].line_num = line;
         return;
      }
   }

   ASSERT_MSG("out of memory blocks.");
}

/* find an entry in the block record and delete it */
static void mem_deleteblock(void *data, char *file, int line)
{
   int i;
   char fail[256];

   for (i = 0; i < MAX_BLOCKS; i++)
   {
      if (data == mem_record[i].block_addr)
      {
         if (mem_checkguardblock(mem_record[i].block_addr, GUARD_LENGTH))
         {
            sprintf(fail, "mem_deleteblock 0x%08X at line %d of %s -- block corrupt",
                    (uint32)data, line, file);
            ASSERT_MSG(fail);
         }

         memset(&mem_record[i], 0, sizeof(memblock_t));
         return;
      }
   }

   sprintf(fail, "mem_deleteblock 0x%08X at line %d of %s -- block not found",
           (uint32)data, line, file);
   ASSERT_MSG(fail);
}
#endif /* NOFRENDO_DEBUG */

/* debugger-friendly versions of calls */
#ifdef NOFRENDO_DEBUG

/* allocates memory and clears it */
void *_my_malloc(int size, char *file, int line)
{
   void *temp;
   char fail[256];

   if (NULL == mem_record && false != mem_debug)
      mem_init();

   if (false != mem_debug)
      temp = mem_guardalloc(size, GUARD_LENGTH);
   else
      // temp = malloc(size);
      temp = mem_alloc(size, true);

   nofrendo_log_printf("_my_malloc: %d at %s:%d\n", size, file, line);
   if (NULL == temp)
   {
      sprintf(fail, "malloc: out of memory at line %d of %s.  block size: %d\n",
              line, file, size);
      ASSERT_MSG(fail);
   }

   if (false != mem_debug)
      mem_addblock(temp, size, file, line);

   mem_blockcount++;

   return temp;
}

/* free a pointer allocated with my_malloc */
void _my_free(void **data, char *file, int line)
{
   char fail[256];

   if (NULL == data || NULL == *data)
   {
      sprintf(fail, "free: attempted to free NULL pointer at line %d of %s\n",
              line, file);
      ASSERT_MSG(fail);
   }

   /* if this is true, we are in REAL trouble */
   if (0 == mem_blockcount)
   {
      ASSERT_MSG("free: attempted to free memory when no blocks available");
   }

   mem_blockcount--; /* dec our block count */

   if (false != mem_debug)
   {
      mem_deleteblock(*data, file, line);
      mem_freeguardblock(*data, GUARD_LENGTH);
   }
   else
   {
      free(*data);
   }

   *data = NULL; /* NULL our source */
}

char *_my_strdup(const char *string, char *file, int line)
{
   char *temp;

   if (NULL == string)
      return NULL;

   temp = (char *)_my_malloc(strlen(string) + 1, file, line);
   if (NULL == temp)
      return NULL;

   strcpy(temp, string);

   return temp;
}

#else /* !NOFRENDO_DEBUG */

/* allocates memory and clears it */
void *_my_malloc(int size)
{
   void *temp;
   char fail[256];

   // temp = malloc(size);
   temp = mem_alloc(size, true);

   if (NULL == temp)
   {
      sprintf(fail, "malloc: out of memory.  block size: %d\n", size);
      ASSERT_MSG(fail);
   }

   return temp;
}

/* free a pointer allocated with my_malloc */
void _my_free(void **data)
{
   char fail[256];

   if (NULL == data || NULL == *data)
   {
      sprintf(fail, "free: attempted to free NULL pointer.\n");
      ASSERT_MSG(fail);
   }

   free(*data);
   *data = NULL; /* NULL our source */
}

char *_my_strdup(const char *string)
{
   char *temp;

   if (NULL == string)
      return NULL;

   /* will ASSERT for us */
   temp = (char *)_my_malloc(strlen(string) + 1);
   if (NULL == temp)
      return NULL;

   strcpy(temp, string);

   return temp;
}

#endif /* !NOFRENDO_DEBUG */

/* check for orphaned memory handles */
void mem_checkleaks(void)
{
#ifdef NOFRENDO_DEBUG
   int i;

   if (false == mem_debug || NULL == mem_record)
      return;

   if (mem_blockcount)
   {
      nofrendo_log_printf("memory leak - %d unfreed block%s\n\n", mem_blockcount,
                          mem_blockcount == 1 ? "" : "s");

      for (i = 0; i < MAX_BLOCKS; i++)
      {
         if (mem_record[i].block_addr)
         {
            nofrendo_log_printf("addr: 0x%08X, size: %d, line %d of %s%s\n",
                                (uint32)mem_record[i].block_addr,
                                mem_record[i].block_size,
                                mem_record[i].line_num,
                                mem_record[i].file_name,
                                (mem_checkguardblock(mem_record[i].block_addr, GUARD_LENGTH))
                                    ? " -- block corrupt"
                                    : "");
         }
      }
   }
   else
      nofrendo_log_printf("no memory leaks\n");
#endif /* NOFRENDO_DEBUG */
}

void mem_checkblocks(void)
{
#ifdef NOFRENDO_DEBUG
   int i;

   if (false == mem_debug || NULL == mem_record)
      return;

   for (i = 0; i < MAX_BLOCKS; i++)
   {
      if (mem_record[i].block_addr)
      {
         if (mem_checkguardblock(mem_record[i].block_addr, GUARD_LENGTH))
         {
            nofrendo_log_printf("addr: 0x%08X, size: %d, line %d of %s -- block corrupt\n",
                                (uint32)mem_record[i].block_addr,
                                mem_record[i].block_size,
                                mem_record[i].line_num,
                                mem_record[i].file_name);
         }
      }
   }
#endif /* NOFRENDO_DEBUG */
}

/*
** $Log: memguard.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.23  2000/11/24 21:42:48  matt
** vc complaints
**
** Revision 1.22  2000/11/21 13:27:30  matt
** trash all newly allocated memory
**
** Revision 1.21  2000/11/21 13:22:30  matt
** memory guard shouldn't zero memory for us
**
** Revision 1.20  2000/10/28 14:01:53  matt
** memguard.h was being included in the wrong place
**
** Revision 1.19  2000/10/26 22:48:33  matt
** strdup'ing a NULL ptr returns NULL
**
** Revision 1.18  2000/10/25 13:41:29  matt
** added strdup
**
** Revision 1.17  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.16  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.15  2000/09/18 02:06:48  matt
** -pedantic is your friend
**
** Revision 1.14  2000/08/11 01:45:48  matt
** hearing about no corrupt blocks every 10 seconds really was annoying
**
** Revision 1.13  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.12  2000/07/24 04:31:07  matt
** mem_checkblocks now gives feedback
**
** Revision 1.11  2000/07/06 17:20:52  matt
** block manager space itself wasn't being freed - d'oh!
**
** Revision 1.10  2000/07/06 17:15:43  matt
** false isn't NULL, Neil... =)
**
** Revision 1.9  2000/07/05 23:10:01  neil
** It's a shame if the memguard segfaults
**
** Revision 1.8  2000/06/26 04:54:48  matt
** simplified and made more robust
**
** Revision 1.7  2000/06/12 01:11:41  matt
** cleaned up some error output for win32
**
** Revision 1.6  2000/06/09 15:12:25  matt
** initial revision
**
*/
