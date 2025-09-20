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
** nes_apu.h
**
** NES APU emulation header file
** $Id: nes_apu.h,v 1.1 2001/04/27 12:54:40 neil Exp $
*/

#ifndef _NES_APU_H_
#define _NES_APU_H_

/* define this for realtime generated noise */
#define REALTIME_NOISE

#define APU_WRA0 0x4000
#define APU_WRA1 0x4001
#define APU_WRA2 0x4002
#define APU_WRA3 0x4003
#define APU_WRB0 0x4004
#define APU_WRB1 0x4005
#define APU_WRB2 0x4006
#define APU_WRB3 0x4007
#define APU_WRC0 0x4008
#define APU_WRC2 0x400A
#define APU_WRC3 0x400B
#define APU_WRD0 0x400C
#define APU_WRD2 0x400E
#define APU_WRD3 0x400F
#define APU_WRE0 0x4010
#define APU_WRE1 0x4011
#define APU_WRE2 0x4012
#define APU_WRE3 0x4013

#define APU_SMASK 0x4015

/* length of generated noise */
#define APU_NOISE_32K 0x7FFF
#define APU_NOISE_93 93

#define APU_BASEFREQ 1789772.7272727272727272

/* channel structures */
/* As much data as possible is precalculated,
** to keep the sample processing as lean as possible
*/

typedef struct rectangle_s
{
   uint8 regs[4];

   bool enabled;

   float accum;
   int32 freq;
   int32 output_vol;
   bool fixed_envelope;
   bool holdnote;
   uint8 volume;

   int32 sweep_phase;
   int32 sweep_delay;
   bool sweep_on;
   uint8 sweep_shifts;
   uint8 sweep_length;
   bool sweep_inc;

   /* this may not be necessary in the future */
   int32 freq_limit;
   int32 env_phase;
   int32 env_delay;
   uint8 env_vol;

   int vbl_length;
   uint8 adder;
   int duty_flip;
} rectangle_t;

typedef struct triangle_s
{
   uint8 regs[3];

   bool enabled;

   float accum;
   int32 freq;
   int32 output_vol;

   uint8 adder;

   bool holdnote;
   bool counter_started;
   /* quasi-hack */
   int write_latency;

   int vbl_length;
   int linear_length;
} triangle_t;

typedef struct noise_s
{
   uint8 regs[3];

   bool enabled;

   float accum;
   int32 freq;
   int32 output_vol;

   int32 env_phase;
   int32 env_delay;
   uint8 env_vol;
   bool fixed_envelope;
   bool holdnote;

   uint8 volume;

   int vbl_length;

#ifdef REALTIME_NOISE
   uint8 xor_tap;
#else
   bool short_sample;
   int cur_pos;
#endif /* REALTIME_NOISE */
} noise_t;

typedef struct dmc_s
{
   uint8 regs[4];

   /* bodge for timestamp queue */
   bool enabled;

   float accum;
   int32 freq;
   int32 output_vol;

   uint32 address;
   uint32 cached_addr;
   int dma_length;
   int cached_dmalength;
   uint8 cur_byte;

   bool looping;
   bool irq_gen;
   bool irq_occurred;

} dmc_t;

enum
{
   APU_FILTER_NONE,
   APU_FILTER_LOWPASS,
   APU_FILTER_WEIGHTED
};

typedef struct
{
   uint32 min_range, max_range;
   uint8 (*read_func)(uint32 address);
} apu_memread;

typedef struct
{
   uint32 min_range, max_range;
   void (*write_func)(uint32 address, uint8 value);
} apu_memwrite;

/* external sound chip stuff */
typedef struct apuext_s
{
   int (*init)(void);
   void (*shutdown)(void);
   void (*reset)(void);
   int32 (*process)(void);
   apu_memread *mem_read;
   apu_memwrite *mem_write;
} apuext_t;

typedef struct apu_s
{
   rectangle_t rectangle[2];
   triangle_t triangle;
   noise_t noise;
   dmc_t dmc;
   uint8 enable_reg;

   void *buffer; /* pointer to output buffer */
   int num_samples;

   uint8 mix_enable;
   int filter_type;

   double base_freq;
   float cycle_rate;

   int sample_rate;
   int sample_bits;
   int refresh_rate;

   void (*process)(void *buffer, int num_samples);
   void (*irq_callback)(void);
   uint8 (*irqclear_callback)(void);

   /* external sound chip */
   apuext_t *ext;
} apu_t;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

   /* Function prototypes */
   extern void apu_setcontext(apu_t *src_apu);
   extern void apu_getcontext(apu_t *dest_apu);

   extern void apu_setparams(double base_freq, int sample_rate, int refresh_rate, int sample_bits);
   extern apu_t *apu_create(double base_freq, int sample_rate, int refresh_rate, int sample_bits);
   extern void apu_destroy(apu_t **apu);

   extern void apu_process(void *buffer, int num_samples);
   extern void apu_reset(void);

   extern void apu_setext(apu_t *apu, apuext_t *ext);
   extern void apu_setfilter(int filter_type);
   extern void apu_setchan(int chan, bool enabled);

   extern uint8 apu_read(uint32 address);
   extern void apu_write(uint32 address, uint8 value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NES_APU_H_ */

/*
** $Log: nes_apu.h,v $
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.3  2000/12/08 02:36:14  matt
** bye bye apu queue (for now)
**
** Revision 1.2  2000/10/28 15:20:59  matt
** irq callbacks in nes_apu
**
** Revision 1.1  2000/10/24 12:19:59  matt
** changed directory structure
**
** Revision 1.28  2000/10/17 11:56:42  matt
** selectable apu base frequency
**
** Revision 1.27  2000/10/10 13:58:18  matt
** stroustrup squeezing his way in the door
**
** Revision 1.26  2000/09/28 23:20:58  matt
** bye bye, fixed point
**
** Revision 1.25  2000/09/27 12:26:03  matt
** changed sound accumulators back to floats
**
** Revision 1.24  2000/09/18 02:12:55  matt
** more optimizations
**
** Revision 1.23  2000/09/15 13:38:40  matt
** changes for optimized apu core
**
** Revision 1.22  2000/09/15 04:58:07  matt
** simplifying and optimizing APU core
**
** Revision 1.21  2000/08/11 02:27:21  matt
** general cleanups, plus apu_setparams routine
**
** Revision 1.20  2000/07/30 04:32:59  matt
** no more apu_getcyclerate hack
**
** Revision 1.19  2000/07/27 02:49:50  matt
** eccentricity in sweeping hardware now emulated correctly
**
** Revision 1.18  2000/07/25 02:25:15  matt
** safer apu_destroy
**
** Revision 1.17  2000/07/23 15:10:54  matt
** hacks for win32
**
** Revision 1.16  2000/07/23 00:48:15  neil
** Win32 SDL
**
** Revision 1.15  2000/07/17 01:52:31  matt
** made sure last line of all source files is a newline
**
** Revision 1.14  2000/07/11 02:39:26  matt
** added setcontext() routine
**
** Revision 1.13  2000/07/10 05:29:34  matt
** moved joypad/oam dma from apu to ppu
**
** Revision 1.12  2000/07/04 04:54:48  matt
** minor changes that helped with MAME
**
** Revision 1.11  2000/07/03 02:18:53  matt
** much better external module exporting
**
** Revision 1.10  2000/06/26 05:00:37  matt
** cleanups
**
** Revision 1.9  2000/06/23 03:29:28  matt
** cleaned up external sound inteface
**
** Revision 1.8  2000/06/20 04:06:16  matt
** migrated external sound definition to apu module
**
** Revision 1.7  2000/06/20 00:07:35  matt
** added convenience members to apu_t struct
**
** Revision 1.6  2000/06/09 16:49:02  matt
** removed all floating point from sound generation
**
** Revision 1.5  2000/06/09 15:12:28  matt
** initial revision
**
*/
