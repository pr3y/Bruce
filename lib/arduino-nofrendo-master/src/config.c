/* Nofrendo Configuration Braindead Sample Implementation
**
** $Id: config.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "noftypes.h"
#include "log.h"
#include "osd.h"
#include "nofconfig.h"
#include "version.h"

typedef struct myvar_s
{
   struct myvar_s *less, *greater;
   char *group, *key, *value;
} myvar_t;

static myvar_t *myVars = NULL;
static bool mySaveNeeded = false;

static void my_destroy(myvar_t **var)
{
   ASSERT(*var);

   if ((*var)->group)
      NOFRENDO_FREE((*var)->group);
   if ((*var)->key)
      NOFRENDO_FREE((*var)->key);
   if ((*var)->value)
      NOFRENDO_FREE((*var)->value);
   NOFRENDO_FREE(*var);
}

static myvar_t *my_create(const char *group, const char *key, const char *value)
{
   myvar_t *var;

   var = NOFRENDO_MALLOC(sizeof(*var));
   if (NULL == var)
   {
      return 0;
   }

   var->less = var->greater = NULL;
   var->group = var->key = var->value = NULL;

   if ((var->group = NOFRENDO_MALLOC(strlen(group) + 1)) && (var->key = NOFRENDO_MALLOC(strlen(key) + 1)) && (var->value = NOFRENDO_MALLOC(strlen(value) + 1)))
   {
      strcpy(var->group, group);
      strcpy(var->key, key);
      strcpy(var->value, value);
      return var;
   }

   my_destroy(&var);
   return NULL;
}

static myvar_t *my_lookup(const char *group, const char *key)
{
   int cmp;
   myvar_t *current = myVars;

   while (current && ((cmp = stricmp(group, current->group)) || (cmp = stricmp(key, current->key))))
   {
      if (cmp < 0)
         current = current->less;
      else
         current = current->greater;
   }

   return current;
}

static void my_insert(myvar_t *var)
{
   int cmp;
   myvar_t **current = &myVars;

   while (*current && ((cmp = stricmp(var->group, (*current)->group)) || (cmp = stricmp(var->key, (*current)->key))))
   {
      current = (cmp < 0) ? &(*current)->less : &(*current)->greater;
   }

   if (*current)
   {
      var->less = (*current)->less;
      var->greater = (*current)->greater;
      my_destroy(current);
   }
   else
   {
      var->less = var->greater = NULL;
   }

   *current = var;
}

static void my_save(FILE *stream, myvar_t *var, char **group)
{
   if (NULL == var)
      return;

   my_save(stream, var->less, group);

   if (stricmp(*group, var->group))
   {
      fprintf(stream, "\n[%s]\n", var->group);
      *group = var->group;
   }

   fprintf(stream, "%s=%s\n", var->key, var->value);

   my_save(stream, var->greater, group);
}

static void my_cleanup(myvar_t *var)
{
   if (NULL == var)
      return;

   my_cleanup(var->less);
   my_cleanup(var->greater);
   my_destroy(&var);
}

static char *my_getline(FILE *stream)
{
   char buf[1024];
   char *dynamic = NULL;

   do
   {
      if (NULL == (fgets(buf, sizeof(buf), stream)))
      {
         if (dynamic)
            NOFRENDO_FREE(dynamic);
         return 0;
      }

      if (NULL == dynamic)
      {
         dynamic = NOFRENDO_MALLOC(strlen(buf) + 1);
         if (NULL == dynamic)
         {
            return 0;
         }
         strcpy(dynamic, buf);
      }
      else
      {
         /* a mini-version of realloc that works with our memory manager */
         char *temp = NULL;
         temp = NOFRENDO_MALLOC(strlen(dynamic) + strlen(buf) + 1);
         if (NULL == temp)
            return 0;

         strcpy(temp, dynamic);
         NOFRENDO_FREE(dynamic);
         dynamic = temp;

         strcat(dynamic, buf);
      }

      if (feof(stream))
      {
         return dynamic;
      }
   } while (dynamic[strlen(dynamic) - 1] != '\n');

   return dynamic;
}

/* load_config loads from the disk the saved configuration. */
static int load_config(char *filename)
{
   FILE *config_file;

   if ((config_file = fopen(filename, "r")))
   {
      char *line;
      char *group = NULL, *key = NULL, *value = NULL;

      mySaveNeeded = true;
      while ((line = my_getline(config_file)))
      {
         char *s;

         if ('\n' == line[strlen(line) - 1])
            line[strlen(line) - 1] = '\0';

         s = line;

         do
         {
            /* eat up whitespace */
            while (isspace(*s))
               s++;

            switch (*s)
            {
            case ';':
            case '#':
            case '\0':
               *s = '\0';
               break;

            case '[':
               if (group)
                  NOFRENDO_FREE(group);

               group = ++s;

               s = strchr(s, ']');
               if (NULL == s)
               {
                  nofrendo_log_printf("load_config: missing ']' after group\n");
                  s = group + strlen(group);
               }
               else
               {
                  *s++ = '\0';
               }

               if ((value = NOFRENDO_MALLOC(strlen(group) + 1)))
               {
                  strcpy(value, group);
               }
               group = value;
               break;

            default:
               key = s;
               s = strchr(s, '=');
               if (NULL == s)
               {
                  nofrendo_log_printf("load_config: missing '=' after key\n");
                  s = key + strlen(key);
               }
               else
               {
                  *s++ = '\0';
               }

               while (strlen(key) && isspace(key[strlen(key) - 1]))
                  key[strlen(key) - 1] = '\0';

               while (isspace(*s))
                  s++;

               while (strlen(s) && isspace(s[strlen(s) - 1]))
                  s[strlen(s) - 1] = '\0';

               {
                  myvar_t *var = my_create(group ? group : "", key, s);
                  if (NULL == var)
                  {
                     nofrendo_log_printf("load_config: my_create failed\n");
                     return -1;
                  }

                  my_insert(var);
               }
               s += strlen(s);
            }
         } while (*s);

         NOFRENDO_FREE(line);
      }

      if (group)
         NOFRENDO_FREE(group);

      fclose(config_file);
   }

   return 0;
}

/* save_config saves the current configuration to disk.*/
static int save_config(char *filename)
{
   FILE *config_file;
   char *group = "";

   config_file = fopen(filename, "w");
   if (NULL == config_file)
   {
      nofrendo_log_printf("save_config failed\n");
      return -1;
   }

   fprintf(config_file, ";; " APP_STRING " " APP_VERSION "\n");
   fprintf(config_file, ";; NOTE: comments are not preserved.\n");
   my_save(config_file, myVars, &group);
   fclose(config_file);

   return 0;
}

static bool open_config(void)
{
   return load_config(config.filename);
}

static void close_config(void)
{
   if (true == mySaveNeeded)
   {
      save_config(config.filename);
   }

   my_cleanup(myVars);
}

static void write_int(const char *group, const char *key, int value)
{
   char buf[24];
   static myvar_t *var;

   sprintf(buf, "%d", value);
   buf[sizeof(buf) - 1] = '\0';

   var = my_create(group, key, buf);
   if (NULL == var)
   {
      nofrendo_log_printf("write_int failed\n");
      return;
   }

   my_insert(var);
   mySaveNeeded = true;
}

/* read_int loads an integer from the configuration into "value"
**
** If the specified "key" does not exist, the "def"ault is returned
*/
static int read_int(const char *group, const char *key, int def)
{
   static myvar_t *var;

   var = my_lookup(group, key);
   if (NULL == var)
   {
      write_int(group, key, def);

      return def;
   }

   return strtoul(var->value, 0, 0);
}

static void write_string(const char *group, const char *key, const char *value)
{
   static myvar_t *var;

   var = my_create(group, key, value);
   if (NULL == var)
   {
      nofrendo_log_printf("write_string failed\n");
      return;
   }

   my_insert(var);
   mySaveNeeded = true;
}

/* read_string copies a string from the configuration into "value"
**
** If the specified "key" does not exist, the "def"ault is returned
*/
static const char *read_string(const char *group, const char *key, const char *def)
{
   static myvar_t *var;

   var = my_lookup(group, key);
   if (NULL == var)
   {
      if (def != NULL)
         write_string(group, key, def);

      return def;
   }

   return var->value;
}

/* interface */
config_t config =
    {
        open_config,
        close_config,
        read_int,
        read_string,
        write_int,
        write_string,
        CONFIG_FILE};

/*
** $Log: config.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.14  2000/11/05 06:23:10  matt
** realloc was incompatible with memguard
**
** Revision 1.13  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.12  2000/09/20 01:13:28  matt
** damn tabs
**
** Revision 1.11  2000/08/04 12:41:04  neil
** current not a bug
**
** Revision 1.10  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.9  2000/07/24 04:30:42  matt
** slight cleanup
**
** Revision 1.8  2000/07/23 15:16:08  matt
** changed strcasecmp to stricmp
**
** Revision 1.7  2000/07/19 15:58:55  neil
** config file now configurable (ha)
**
** Revision 1.6  2000/07/18 03:28:32  matt
** help me!  I'm a complete mess!
**
** Revision 1.5  2000/07/12 11:03:08  neil
** Always write a config, even if no defaults are changed
**
** Revision 1.4  2000/07/11 15:09:30  matt
** suppressed all warnings
**
** Revision 1.3  2000/07/11 14:59:27  matt
** minor cosmetics.. =)
**
** Revision 1.2  2000/07/11 13:35:38  bsittler
** Changed the config API, implemented config file "nofrendo.cfg". The
** GGI drivers use the group [GGI]. Visual= and Mode= keys are understood.
**
** Revision 1.1  2000/07/11 09:21:10  bsittler
** This is a skeletal configuration system.
**
*/
