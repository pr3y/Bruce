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
** mmc5_snd.c
**
** Nintendo MMC5 sound emulation
** $Id: mmc5_snd.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <string.h>

#include "../noftypes.h"
#include "mmc5_snd.h"
#include "nes_apu.h"

/* TODO: encapsulate apu/mmc5 rectangle */

#define APU_OVERSAMPLE
#define APU_VOLUME_DECAY(x) ((x) -= ((x) >> 7))

/* look up table madness */
static int32 decay_lut[16];
static int vbl_lut[32];

/* various sound constants for sound emulation */
/* vblank length table used for rectangles, triangle, noise */
static const uint8 vbl_length[32] =
    {
        5, 127, 10, 1, 19, 2, 40, 3, 80, 4, 30, 5, 7, 6, 13, 7,
        6, 8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15};

/* ratios of pos/neg pulse for rectangle waves
** 2/16 = 12.5%, 4/16 = 25%, 8/16 = 50%, 12/16 = 75% 
** (4-bit adder in rectangles, hence the 16)
*/
static const int duty_lut[4] =
    {
        2, 4, 8, 12};

#define MMC5_WRA0 0x5000
#define MMC5_WRA1 0x5001
#define MMC5_WRA2 0x5002
#define MMC5_WRA3 0x5003
#define MMC5_WRB0 0x5004
#define MMC5_WRB1 0x5005
#define MMC5_WRB2 0x5006
#define MMC5_WRB3 0x5007
#define MMC5_SMASK 0x5015

typedef struct mmc5rectangle_s
{
   uint8 regs[4];

   bool enabled;

   float accum;
   int32 freq;
   int32 output_vol;
   bool fixed_envelope;
   bool holdnote;
   uint8 volume;

   int32 env_phase;
   int32 env_delay;
   uint8 env_vol;

   int vbl_length;
   uint8 adder;
   int duty_flip;
} mmc5rectangle_t;

typedef struct mmc5dac_s
{
   int32 output;
   bool enabled;
} mmc5dac_t;

static struct
{
   float incsize;
   uint8 mul[2];
   mmc5rectangle_t rect[2];
   mmc5dac_t dac;
} mmc5;

#define MMC5_RECTANGLE_OUTPUT chan->output_vol
static int32 mmc5_rectangle(mmc5rectangle_t *chan)
{
   int32 output;

#ifdef APU_OVERSAMPLE
   int num_times;
   int32 total;
#endif /* APU_OVERSAMPLE */

   /* reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle
   ** reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on
   ** reg2: 8 bits of freq
   ** reg3: 0-2=high freq, 7-4=vbl length counter
   */

   APU_VOLUME_DECAY(chan->output_vol);

   if (false == chan->enabled || 0 == chan->vbl_length)
      return MMC5_RECTANGLE_OUTPUT;

   /* vbl length counter */
   if (false == chan->holdnote)
      chan->vbl_length--;

   /* envelope decay at a rate of (env_delay + 1) / 240 secs */
   chan->env_phase -= 4; /* 240/60 */
   while (chan->env_phase < 0)
   {
      chan->env_phase += chan->env_delay;

      if (chan->holdnote)
         chan->env_vol = (chan->env_vol + 1) & 0x0F;
      else if (chan->env_vol < 0x0F)
         chan->env_vol++;
   }

   if (chan->freq < 4)
      return MMC5_RECTANGLE_OUTPUT;

   chan->accum -= mmc5.incsize; /* # of cycles per sample */
   if (chan->accum >= 0)
      return MMC5_RECTANGLE_OUTPUT;

#ifdef APU_OVERSAMPLE
   num_times = total = 0;

   if (chan->fixed_envelope)
      output = chan->volume << 8; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F) << 8;
#endif

   while (chan->accum < 0)
   {
      chan->accum += chan->freq;
      chan->adder = (chan->adder + 1) & 0x0F;

#ifdef APU_OVERSAMPLE
      if (chan->adder < chan->duty_flip)
         total += output;
      else
         total -= output;

      num_times++;
#endif
   }

#ifdef APU_OVERSAMPLE
   chan->output_vol = total / num_times;
#else
   if (chan->fixed_envelope)
      output = chan->volume << 8; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F) << 8;

   if (0 == chan->adder)
      chan->output_vol = output;
   else if (chan->adder == chan->duty_flip)
      chan->output_vol = -output;
#endif

   return MMC5_RECTANGLE_OUTPUT;
}

static uint8 mmc5_read(uint32 address)
{
   uint32 retval;

   retval = (uint32)(mmc5.mul[0] * mmc5.mul[1]);

   switch (address)
   {
   case 0x5205:
      return (uint8)retval;

   case 0x5206:
      return (uint8)(retval >> 8);

   default:
      return 0xFF;
   }
}

/* mix vrcvi sound channels together */
static int32 mmc5_process(void)
{
   int32 accum;

   accum = mmc5_rectangle(&mmc5.rect[0]);
   accum += mmc5_rectangle(&mmc5.rect[1]);
   if (mmc5.dac.enabled)
      accum += mmc5.dac.output;

   return accum;
}

/* write to registers */
static void mmc5_write(uint32 address, uint8 value)
{
   int chan;

   switch (address)
   {
   /* rectangles */
   case MMC5_WRA0:
   case MMC5_WRB0:
      chan = (address & 4) ? 1 : 0;
      mmc5.rect[chan].regs[0] = value;

      mmc5.rect[chan].volume = value & 0x0F;
      mmc5.rect[chan].env_delay = decay_lut[value & 0x0F];
      mmc5.rect[chan].holdnote = (value & 0x20) ? true : false;
      mmc5.rect[chan].fixed_envelope = (value & 0x10) ? true : false;
      mmc5.rect[chan].duty_flip = duty_lut[value >> 6];
      break;

   case MMC5_WRA1:
   case MMC5_WRB1:
      break;

   case MMC5_WRA2:
   case MMC5_WRB2:
      chan = (address & 4) ? 1 : 0;
      mmc5.rect[chan].regs[2] = value;
      if (mmc5.rect[chan].enabled)
         mmc5.rect[chan].freq = (((mmc5.rect[chan].regs[3] & 7) << 8) + value) + 1;
      break;

   case MMC5_WRA3:
   case MMC5_WRB3:
      chan = (address & 4) ? 1 : 0;
      mmc5.rect[chan].regs[3] = value;

      if (mmc5.rect[chan].enabled)
      {
         mmc5.rect[chan].vbl_length = vbl_lut[value >> 3];
         mmc5.rect[chan].env_vol = 0;
         mmc5.rect[chan].freq = (((value & 7) << 8) + mmc5.rect[chan].regs[2]) + 1;
         mmc5.rect[chan].adder = 0;
      }
      break;

   case MMC5_SMASK:
      if (value & 0x01)
      {
         mmc5.rect[0].enabled = true;
      }
      else
      {
         mmc5.rect[0].enabled = false;
         mmc5.rect[0].vbl_length = 0;
      }

      if (value & 0x02)
      {
         mmc5.rect[1].enabled = true;
      }
      else
      {
         mmc5.rect[1].enabled = false;
         mmc5.rect[1].vbl_length = 0;
      }

      break;

   case 0x5010:
      if (value & 0x01)
         mmc5.dac.enabled = true;
      else
         mmc5.dac.enabled = false;
      break;

   case 0x5011:
      mmc5.dac.output = (value ^ 0x80) << 8;
      break;

   case 0x5205:
      mmc5.mul[0] = value;
      break;

   case 0x5206:
      mmc5.mul[1] = value;
      break;

   case 0x5114:
   case 0x5115:
      /* ???? */
      break;

   default:
      break;
   }
}

/* reset state of vrcvi sound channels */
static void mmc5_reset(void)
{
   int i;
   apu_t apu;

   /* get the phase period from the apu */
   apu_getcontext(&apu);
   mmc5.incsize = apu.cycle_rate;

   for (i = 0x5000; i < 0x5008; i++)
      mmc5_write(i, 0);

   mmc5_write(0x5010, 0);
   mmc5_write(0x5011, 0);
}

static int mmc5_init(void)
{
   int i, num_samples;
   apu_t apu;

   apu_getcontext(&apu);
   num_samples = apu.num_samples;

   /* lut used for enveloping and frequency sweeps */
   for (i = 0; i < 16; i++)
      decay_lut[i] = num_samples * (i + 1);

   /* used for note length, based on vblanks and size of audio buffer */
   for (i = 0; i < 32; i++)
      vbl_lut[i] = vbl_length[i] * num_samples;

   return 0;
}

static apu_memread mmc5_memread[] =
    {
        {0x5205, 0x5206, mmc5_read},
        {-1, -1, NULL}};

static apu_memwrite mmc5_memwrite[] =
    {
        {0x5000, 0x5015, mmc5_write},
        {0x5114, 0x5115, mmc5_write},
        {0x5205, 0x5206, mmc5_write},
        {-1, -1, NULL}};

apuext_t mmc5_ext =
    {
        mmc5_init,
        NULL, /* no shutdown */
        mmc5_reset,
        mmc5_process,
        mmc5_memread,
        mmc5_memwrite};

/*
** $Log: mmc5_snd.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.2  2000/11/13 00:57:08  matt
** doesn't look as nasty now
**
** Revision 1.1  2000/10/24 12:19:59  matt
** changed directory structure
**
** Revision 1.16  2000/10/17 11:56:42  matt
** selectable apu base frequency
**
** Revision 1.15  2000/10/10 14:04:29  matt
** make way for bjarne
**
** Revision 1.14  2000/10/10 13:58:18  matt
** stroustrup squeezing his way in the door
**
** Revision 1.13  2000/10/08 17:50:18  matt
** appears $5114/$5115 do something
**
** Revision 1.12  2000/10/03 11:56:20  matt
** better support for optional sound ext routines
**
** Revision 1.11  2000/09/27 12:26:03  matt
** changed sound accumulators back to floats
**
** Revision 1.10  2000/09/15 13:38:40  matt
** changes for optimized apu core
**
** Revision 1.9  2000/09/15 04:58:07  matt
** simplifying and optimizing APU core
**
** Revision 1.8  2000/07/30 04:32:59  matt
** no more apu_getcyclerate hack
**
** Revision 1.7  2000/07/17 01:52:31  matt
** made sure last line of all source files is a newline
**
** Revision 1.6  2000/07/04 04:51:41  matt
** cleanups
**
** Revision 1.5  2000/07/03 02:18:53  matt
** much better external module exporting
**
** Revision 1.4  2000/06/28 22:03:51  matt
** fixed stupid oversight
**
** Revision 1.3  2000/06/20 20:46:58  matt
** minor cleanups
**
** Revision 1.2  2000/06/20 04:06:16  matt
** migrated external sound definition to apu module
**
** Revision 1.1  2000/06/20 00:06:47  matt
** initial revision
**
*/
