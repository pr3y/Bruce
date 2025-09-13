/* Nofrendo Configuration API 
**
** This file is in the public domain.
**
** $Id: nofconfig.h,v 1.1 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NOFCONFIG_H_
#define _NOFCONFIG_H_

#include <stdbool.h>

#ifndef CONFIG_FILE
#define CONFIG_FILE "nofrendo.cfg"
#endif

typedef struct config_s
{
   /* open loads from the disk the saved configuration.
   **
   ** open must be the first config function called.
   **
   ** open returns true on success, false otherwise.
   */
   bool (*open)(void);

   /* close saves the current configuration to disk.
   **
   ** close must be the last config function called.
   */
   void (*close)(void);

   /* read_int loads an integer from the configuration into "value"
   **
   ** If the specified "key" does not exist, the "def"ault is returned
   */
   int (*read_int)(const char *group, const char *key, int def);

   /* read_string copies a string from the configuration into "value"
   **
   ** If the specified "key" does not exist, the "def"ault is returned
   */
   const char *(*read_string)(const char *group, const char *key, const char *def);

   void (*write_int)(const char *group, const char *key, int value);
   void (*write_string)(const char *group, const char *key, const char *value);
   char *filename;
} config_t;

extern config_t config;

#endif /* _NOFCONFIG_H_ */

/*
** $Log: nofconfig.h,v $
** Revision 1.1  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.5  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.4  2000/07/19 15:58:55  neil
** config file now configurable (ha)
**
** Revision 1.3  2000/07/11 14:59:27  matt
** minor cosmetics.. =)
**
** Revision 1.2  2000/07/11 13:35:38  bsittler
** Changed the config API, implemented config file "nofrendo.cfg". The
** GGI drivers use the group [GGI]. Visual= and Mode= keys are understood.
**
** Revision 1.1  2000/07/11 07:46:11  neil
** Initial commit
**
**
*/
