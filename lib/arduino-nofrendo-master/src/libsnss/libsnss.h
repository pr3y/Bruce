/**************************************************************************/
/*
      libsnss.h

      (C) 2000 The SNSS Group
      See README.TXT file for license and terms of use.

      $Id: libsnss.h,v 1.1 2001/04/27 12:54:40 neil Exp $
*/
/**************************************************************************/

#ifndef _LIBSNSS_H_
#define _LIBSNSS_H_

#include <stdio.h>

#include "../memguard.h"

/**************************************************************************/
/* endian customization */
/**************************************************************************/
/* 
   Endian-ness quick reference:
   the number is:
      $12345678
   the little-endian representation (e.g.: 6502, Intel x86) is:
      78 56 34 12
   the big-endian representation (e.g.: Motorola 68000) is:
      12 34 56 78
   the SNSS file format uses big-endian representation
*/

/* comment/uncomment depending on your processor architecture */
/* commenting this out implies BIG ENDIAN */
#define USE_LITTLE_ENDIAN

/**************************************************************************/
/* SNSS constants */
/**************************************************************************/

typedef enum _SNSS_OPEN_MODES
{
   SNSS_OPEN_READ,
   SNSS_OPEN_WRITE
} SNSS_OPEN_MODE;

/* block types */
typedef enum _SNSS_BLOCK_TYPES
{
   SNSS_BASR,
   SNSS_VRAM,
   SNSS_SRAM,
   SNSS_MPRD,
   SNSS_CNTR,
   SNSS_SOUN,
   SNSS_UNKNOWN_BLOCK
} SNSS_BLOCK_TYPE;

/* function return types */
typedef enum _SNSS_RETURN_CODES
{
   SNSS_OK,
   SNSS_BAD_FILE_TAG,
   SNSS_OPEN_FAILED,
   SNSS_CLOSE_FAILED,
   SNSS_READ_FAILED,
   SNSS_WRITE_FAILED,
   SNSS_OUT_OF_MEMORY,
   SNSS_UNSUPPORTED_BLOCK
} SNSS_RETURN_CODE;

#define TAG_LENGTH 4
#define SNSS_BLOCK_VERSION 1

/**************************************************************************/
/* SNSS data structures */
/**************************************************************************/

struct mapper1Data
{
   unsigned char registers[4];
   unsigned char latch;
   unsigned char numberOfBits;
};

struct mapper4Data
{
   unsigned char irqCounter;
   unsigned char irqLatchCounter;
   unsigned char irqCounterEnabled;
   unsigned char last8000Write;
};

struct mapper5Data
{
   unsigned char dummy; /* needed for some compilers; remove if any members are added */
};

struct mapper6Data
{
   unsigned char irqCounter;
   unsigned char irqLatchCounter;
   unsigned char irqCounterEnabled;
   unsigned char last43FEWrite;
   unsigned char last4500Write;
};

struct mapper9Data
{
   unsigned char latch[2];
   unsigned char lastB000Write;
   unsigned char lastC000Write;
   unsigned char lastD000Write;
   unsigned char lastE000Write;
};

struct mapper10Data
{
   unsigned char latch[2];
   unsigned char lastB000Write;
   unsigned char lastC000Write;
   unsigned char lastD000Write;
   unsigned char lastE000Write;
};

struct mapper16Data
{
   unsigned char irqCounterLowByte;
   unsigned char irqCounterHighByte;
   unsigned char irqCounterEnabled;
};

struct mapper17Data
{
   unsigned char irqCounterLowByte;
   unsigned char irqCounterHighByte;
   unsigned char irqCounterEnabled;
};

struct mapper18Data
{
   unsigned char irqCounterLowByte;
   unsigned char irqCounterHighByte;
   unsigned char irqCounterEnabled;
};

struct mapper19Data
{
   unsigned char irqCounterLowByte;
   unsigned char irqCounterHighByte;
   unsigned char irqCounterEnabled;
};

struct mapper21Data
{
   unsigned char irqCounter;
   unsigned char irqCounterEnabled;
};

struct mapper24Data
{
   unsigned char irqCounter;
   unsigned char irqCounterEnabled;
};

struct mapper40Data
{
   unsigned char irqCounter;
   unsigned char irqCounterEnabled;
};

struct mapper69Data
{
   unsigned char irqCounterLowByte;
   unsigned char irqCounterHighByte;
   unsigned char irqCounterEnabled;
};

struct mapper90Data
{
   unsigned char irqCounter;
   unsigned char irqLatchCounter;
   unsigned char irqCounterEnabled;
};

struct mapper224Data
{
   unsigned char chrRamWriteable;
};

struct mapper225Data
{
   unsigned char prgSize;
   unsigned char registers[4];
};

struct mapper226Data
{
   unsigned char chrRamWriteable;
};

struct mapper228Data
{
   unsigned char prgChipSelected;
};

struct mapper230Data
{
   unsigned char numberOfResets;
};

typedef struct _SnssFileHeader
{
   char tag[TAG_LENGTH + 1];
   unsigned int numberOfBlocks;
} SnssFileHeader;

/* this block appears before every block in the SNSS file */
typedef struct _SnssBlockHeader
{
   char tag[TAG_LENGTH + 1];
   unsigned int blockVersion;
   unsigned int blockLength;
} SnssBlockHeader;

#define BASE_BLOCK_LENGTH 0x1931
typedef struct _SnssBaseBlock
{
   unsigned char regA;
   unsigned char regX;
   unsigned char regY;
   unsigned char regFlags;
   unsigned char regStack;
   unsigned short regPc;
   unsigned char reg2000;
   unsigned char reg2001;
   unsigned char cpuRam[0x800];
   unsigned char spriteRam[0x100];
   unsigned char ppuRam[0x1000];
   unsigned char palette[0x20];
   unsigned char mirrorState[4];
   unsigned short vramAddress;
   unsigned char spriteRamAddress;
   unsigned char tileXOffset;
} SnssBaseBlock;

#define VRAM_8K 0x2000
#define VRAM_16K 0x4000
typedef struct _SnssVramBlock
{
   unsigned short vramSize;
   unsigned char vram[VRAM_16K];
} SnssVramBlock;

#define SRAM_1K 0x0400
#define SRAM_2K 0x0800
#define SRAM_3K 0x0C00
#define SRAM_4K 0x1000
#define SRAM_5K 0x1400
#define SRAM_6K 0x1800
#define SRAM_7K 0x1C00
#define SRAM_8K 0x2000
typedef struct _SnssSramBlock
{
   unsigned short sramSize;
   unsigned char sramEnabled;
   unsigned char sram[SRAM_8K];
} SnssSramBlock;

#define MAPPER_BLOCK_LENGTH 0x98
typedef struct _SnssMapperBlock
{
   unsigned short prgPages[4];
   unsigned short chrPages[8];

   union _extraData
   {
      unsigned char mapperData[128];
      struct mapper1Data mapper1;
      struct mapper4Data mapper4;
      struct mapper5Data mapper5;
      struct mapper6Data mapper6;
      struct mapper9Data mapper9;
      struct mapper10Data mapper10;
      struct mapper16Data mapper16;
      struct mapper17Data mapper17;
      struct mapper18Data mapper18;
      struct mapper19Data mapper19;
      struct mapper21Data mapper21;
      struct mapper24Data mapper24;
      struct mapper40Data mapper40;
      struct mapper69Data mapper69;
      struct mapper90Data mapper90;
      struct mapper224Data mapper224;
      struct mapper225Data mapper225;
      struct mapper226Data mapper226;
      struct mapper228Data mapper228;
      struct mapper230Data mapper230;
   } extraData;
} SnssMapperBlock;

typedef struct _SnssControllersBlock
{
   unsigned char dummy; /* needed for some compilers; remove if any members are added */
} SnssControllersBlock;

#define SOUND_BLOCK_LENGTH 0x16
typedef struct _SnssSoundBlock
{
   unsigned char soundRegisters[0x16];
} SnssSoundBlock;

/**************************************************************************/
/* SNSS file manipulation functions */
/**************************************************************************/

typedef struct _SNSS_FILE
{
   FILE *fp;
   SNSS_OPEN_MODE mode;
   SnssFileHeader headerBlock;
   SnssBaseBlock baseBlock;
   SnssVramBlock vramBlock;
   SnssSramBlock sramBlock;
   SnssMapperBlock mapperBlock;
   SnssControllersBlock contBlock;
   SnssSoundBlock soundBlock;
} SNSS_FILE;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

   /* general file manipulation routines */
   SNSS_RETURN_CODE SNSS_OpenFile(SNSS_FILE **snssFile, const char *filename,
                                  SNSS_OPEN_MODE mode);
   SNSS_RETURN_CODE SNSS_CloseFile(SNSS_FILE **snssFile);

   /* block traversal */
   SNSS_RETURN_CODE SNSS_GetNextBlockType(SNSS_BLOCK_TYPE *blockType,
                                          SNSS_FILE *snssFile);
   SNSS_RETURN_CODE SNSS_SkipNextBlock(SNSS_FILE *snssFile);

   /* functions to read/write SNSS blocks */
   SNSS_RETURN_CODE SNSS_ReadBlock(SNSS_FILE *snssFile, SNSS_BLOCK_TYPE blockType);
   SNSS_RETURN_CODE SNSS_WriteBlock(SNSS_FILE *snssFile, SNSS_BLOCK_TYPE blockType);

   /* support functions */
   const char *SNSS_GetErrorString(SNSS_RETURN_CODE code);

   static SNSS_FILE *snssFileBlock;
   static char *blockBytes;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LIBSNSS_H_ */

/*
** $Log: libsnss.h,v $
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.2  2000/11/09 14:07:41  matt
** minor update
**
** Revision 1.1  2000/10/24 12:19:01  matt
** changed directory structure
**
** Revision 1.9  2000/09/18 02:06:48  matt
** -pedantic is your friend
**
** Revision 1.8  2000/07/17 01:52:27  matt
** made sure last line of all source files is a newline
**
** Revision 1.7  2000/07/15 23:49:14  matt
** fixed some typos in the mapper-specific data
**
** Revision 1.6  2000/07/09 15:37:21  matt
** all block read/write calls now pass through a common handler
**
** Revision 1.5  2000/07/09 03:39:06  matt
** minor modifications
**
** Revision 1.4  2000/07/08 16:01:39  matt
** added bald's changes, made error checking more robust
**
** Revision 1.3  2000/07/05 22:46:52  matt
** cleaned up header
**
** Revision 1.2  2000/07/04 04:46:06  matt
** simplified handling of SNSS states
**
** Revision 1.1  2000/06/29 14:13:28  matt
** initial revision
**
*/
