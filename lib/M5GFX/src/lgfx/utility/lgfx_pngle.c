/*-
 * MIT License
 *
 * Copyright (c) 2019 kikuchan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*----------------------------------------------------------------------------/
/ original source is here : https://github.com/kikuchan/pngle/
/
/ Modified for LGFX  by lovyan03, 2020-2022
/ tweak for 32bit processor
/----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include "lgfx_miniz.h"
#include "lgfx_pngle.h"

#include "pgmspace.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef PNGLE_DEBUG
#define debug_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug_printf(...) ((void)0)
#endif

typedef enum {
  PNGLE_STATE_ERROR = -2,
  PNGLE_STATE_OK = 0,
} pngle_state_t;

#define PNGLE_ERROR(s) ( debug_printf(s), PNGLE_STATE_ERROR)
#define PNGLE_MALLOC(a, b, name) (debug_printf("[pngle] Allocating %zu bytes for %s\n", (size_t)(a) * (size_t)(b), (name)), malloc((size_t)(a) * (size_t)(b)))

#define LGFX_PNGLE_OUTBUF_LEN 64 /// At least 32 required
#define LGFX_PNGLE_READBUF_LEN 512 /// At least 256 required

#define LGFX_PNGLE_NON_TRANS_COLOR 0x01000000

typedef enum {
// Supported chunks
  PNGLE_CHUNK_IDAT = 0x49444154UL, // IDAT
  PNGLE_CHUNK_IEND = 0x49454e44UL, // IEND
  PNGLE_CHUNK_PLTE = 0x504c5445UL, // PLTE
  PNGLE_CHUNK_tRNS = 0x74524e53UL, // tRNS
} pngle_chunk_t;

// typedef struct _pngle_t pngle_t; // declared in pngle.h
struct _pngle_t {
  // PLTE chunk
  uint8_t *scanline_buf;
  uint8_t *palette;

  size_t n_palettes;
  uint32_t trans_color;

  // scanline decoder (reset on every set_interlace_pass() call)
  size_t scanline_pixels;
  size_t scanline_stride;
  size_t scanline_remain_bytes_to_render;

  lgfx_pngle_read_callback_t read_callback;
  lgfx_pngle_draw_callback_t draw_callback;
  void *user_data;

  uint32_t drawing_y;

  // for grayscale
  uint32_t magni;

  pngle_ihdr_t hdr;

  uint8_t filter_type;
  // interlace
  uint8_t interlace_pass;

  // 0 indicates IHDR hasn't been processed yet
  uint8_t channels;

  // decompression state (reset on IHDR)
  uint8_t *next_out; // NULL indicates IDAT hasn't been processed yet
  size_t  avail_out;
  uint32_t out_buf[LGFX_PNGLE_OUTBUF_LEN >> 2]; // out_buf + read_buf (Do not change the order)
  uint8_t read_buf[LGFX_PNGLE_READBUF_LEN];
  lgfx_tinfl_decompressor inflator; // 11000 bytes
  uint8_t lz_buf[TINFL_LZ_DICT_SIZE]; // 32768 bytes
};

// magic
static const uint8_t png_header[] PROGMEM = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52 };
static const uint8_t interlace_div_y[9] PROGMEM = { 8, 8, 8, 4, 4, 2, 2, 1, 1 };
static const uint8_t interlace_off_y[9] PROGMEM = { 0, 0, 4, 0, 2, 0, 1, 0, 0 };
static const uint8_t* interlace_off_x = &interlace_off_y[1];
static const uint8_t* interlace_div_x = &interlace_div_y[1];

static inline uint32_t swap32(uint32_t c) { c = (c >> 16) + (c << 16); return ((c >> 8) & 0xFF00FF) + ((c & 0xFF00FF) << 8); }

static inline uint32_t read_uint32(const uint8_t *p)
{
  return (p[0] << 24)
       + (p[1] << 16)
       + (p[2] <<  8)
       + (p[3] <<  0)
  ;
}

pngle_t *lgfx_pngle_new()
{
  pngle_t* res = (pngle_t *)PNGLE_MALLOC(1, sizeof(pngle_t), "pngle_t");
  if (res)
  {
    res->palette       = NULL;
    res->scanline_buf  = NULL;
  }
  return res;
}

void lgfx_pngle_destroy(pngle_t *pngle)
{
  if (pngle) {
    if (pngle->scanline_buf ) { free(pngle->scanline_buf ); }
    if (pngle->palette      ) { free(pngle->palette      ); }
    free(pngle);
  }
}

uint32_t lgfx_pngle_get_width(pngle_t *pngle)
{
  if (!pngle) return 0;
  return pngle->hdr.width;
}

uint32_t lgfx_pngle_get_height(pngle_t *pngle)
{
  if (!pngle) return 0;
  return pngle->hdr.height;
}

pngle_ihdr_t *lgfx_pngle_get_ihdr(pngle_t *pngle)
{
  if (!pngle) return NULL;
  if (pngle->channels == 0) return NULL;
  return &pngle->hdr;
}

static void make_pixels(pngle_t *pngle, const uint8_t* buf, uint32_t* rgbbuf, size_t len)
{
  size_t depth = pngle->hdr.depth;
  uint32_t* argb32 = rgbbuf - 1;
  uint32_t* last = &argb32[len];
  if (depth >= 8)
  {
    size_t add = depth >> 3;
    switch (pngle->channels)
    {
    case 1: // grayscale or palette
      do
      {
        *++argb32 = buf[0];
        buf += add;
      } while (argb32 != last);
      break;

    case 2: // grayscale with alpha
      do
      {
        /// grayscale to color (* 0x010101) + alpha channel
        *++argb32 = buf[0]*0x01010100 + buf[add];
        buf += add * 2;
      } while (argb32 != last);
      break;

    case 3: // color
      do
      {
        *++argb32 = 0xFF
                  + (buf[    0] <<  8)
                  + (buf[add  ] << 16)
                  + (buf[add*2] << 24);
        buf += add * 3;
      } while (argb32 != last);
      break;

    default: //4 : color + alpha
      do
      {
        *++argb32 = (buf[    0] <<  8)
                  + (buf[add  ] << 16)
                  + (buf[add*2] << 24)
                  + (buf[add*3]      );
        buf += add * 4;
      } while (argb32 != last);
      break;
    }
  }
  else
  {
    size_t mask = ((1 << depth) - 1);
    size_t shift = 8;
    size_t b = buf[0];
    do
    {
      shift -= depth;
      *++argb32 = (b >> shift) & mask;
      if (shift == 0) {
        shift = 8;
        b = *++buf;
      }
    } while (argb32 != last);
  }

  argb32 = rgbbuf - 1;
  // color type: 0000 0111
  //                     ^-- indexed color (palette)
  //                    ^--- Color
  //                   ^---- Alpha channel
  switch (pngle->hdr.color_type)
  {
  case 0: // grayscale
    {
      uint32_t tp = pngle->trans_color;
      uint32_t magni = pngle->magni;
      do {
        uint32_t tmp = *++argb32;
        *argb32 = ((tmp == tp) ? 0 : 0xFF) + (tmp * magni);
      } while (argb32 != last);
    }
    break;

  case 2: // true color
    if (pngle->trans_color != LGFX_PNGLE_NON_TRANS_COLOR)
    {
      uint32_t tp = pngle->trans_color;
      do
      {
        if (*++argb32 == tp) { ((uint8_t*)argb32)[0] = 0; }
      } while (argb32 != last);
    }
    break;

  case 3: // indexed color
    {
      size_t n_palettes = pngle->n_palettes;
      uint32_t* palette = ((uint32_t*)(pngle->palette));
      do
      {  // lookup palette info
        size_t pidx = *++argb32;
        *argb32 = pidx < n_palettes ? palette[pidx] : ~0u;
      } while (argb32 != last);
    }
    break;

//case 4: // grayscale with alpha ...nothing to do.
//case 6: // truecolor with alpha ...nothing to do.
  default:
    break;
  }
}

static inline int paeth(int a, int b, int c)
{
  int pa = b - c;
  int pb = a - c;
  int pc = abs(pa + pb);
  pa = abs(pa);
  pb = abs(pb);
  if (pb < pa) { pa = pb; a = b; }
  return (pc < pa) ? c : a;
}

static void set_interlace_pass(pngle_t *pngle, uint_fast8_t pass)
{
  while (pngle->hdr.width <= pgm_read_byte(&interlace_off_x[pass]) || pngle->hdr.height <= pgm_read_byte(&interlace_off_y[pass]))
  {
    ++pass;
  }
  pngle->interlace_pass = pass;
  pngle->drawing_y = pgm_read_byte(&interlace_off_y[pass]);
  size_t div_x = pgm_read_byte(&interlace_div_x[pass]);
  size_t scanline_pixels = (pngle->hdr.width - pgm_read_byte(&interlace_off_x[pass]) + div_x - 1) / div_x;
  pngle->scanline_pixels = scanline_pixels;
  size_t scanline_stride = (scanline_pixels * pngle->channels * pngle->hdr.depth + 7) >> 3;
  pngle->scanline_stride = scanline_stride;
  pngle->scanline_remain_bytes_to_render = scanline_stride;
}

static int pngle_on_data(pngle_t *pngle, uint8_t *lzbuf, size_t len, size_t outbuf_len)
{
  uint_fast8_t bytes_per_pixel = (pngle->channels * pngle->hdr.depth + 7) >> 3; // 1 if depth <= 8
  size_t filter_type = pngle->filter_type;
  size_t remain_bytes = pngle->scanline_remain_bytes_to_render;


  const uint8_t* p = lzbuf;
  uint8_t* scanline = &(pngle->scanline_buf[bytes_per_pixel]);
  while (len)
  {
    if (filter_type > 4)
    {
      filter_type = *p++; // 0 - 4
      if (filter_type >= 4)
      {
        if (filter_type > 4)
        {
          debug_printf("[pngle] Invalid filter type is found; 0x%02x\n", *p);
          return PNGLE_ERROR("Invalid filter type is found");
        }
        // Only when using the paeth filter, shift the previous result to the right by bytes_per_pixel. (To refer to the result of the upper left pixel).
        memmove(scanline, scanline - bytes_per_pixel, bytes_per_pixel + pngle->scanline_stride);
      }
      if (--len == 0) { break; }
    }
    size_t cidx = pngle->scanline_stride - remain_bytes;

    size_t l = (len < remain_bytes) ? len : remain_bytes;
    remain_bytes -= l;
    len -= l;

    const uint8_t* newdata = p - cidx;
    p += l;
    size_t last = cidx + l;

    switch (filter_type) {
    default: memcpy(&scanline[cidx], &newdata[cidx], l); cidx = last; break;
    case 1: do { scanline[cidx]  = newdata[cidx] + scanline[cidx - bytes_per_pixel];                                                          } while (++cidx != last); break;
    case 2: do { scanline[cidx] += newdata[cidx];                                                                                             } while (++cidx != last); break;
    case 3: do { scanline[cidx]  = newdata[cidx] + ((scanline[cidx - bytes_per_pixel] + scanline[cidx]) >> 1);                                } while (++cidx != last); break;
    case 4: do { scanline[cidx]  = newdata[cidx] + paeth(scanline[cidx - bytes_per_pixel], scanline[cidx + bytes_per_pixel], scanline[cidx]); } while (++cidx != last); break;
    }
    if (remain_bytes) { break; }

    remain_bytes = pngle->scanline_stride; // reset
    filter_type = ~0;

    uint32_t draw_x = pgm_read_byte(&interlace_off_x[pngle->interlace_pass]);
    uint32_t div_x  = pgm_read_byte(&interlace_div_x[pngle->interlace_pass]);
    size_t scanline_pixels = pngle->scanline_pixels;
    size_t out_pos = 0;
    size_t out_len = ((((scanline_pixels + 7) & ~7) - 1) % outbuf_len) + 1;

    do
    {
      if (out_len > scanline_pixels - out_pos) { out_len = scanline_pixels - out_pos; }
      make_pixels(pngle, &scanline[(out_pos * pngle->channels * pngle->hdr.depth) >> 3], pngle->out_buf, out_len);
      pngle->draw_callback(pngle->user_data, draw_x + out_pos * div_x, pngle->drawing_y, div_x, out_len, (const uint8_t*)pngle->out_buf);

      out_pos += out_len;
      out_len = outbuf_len;
    } while (out_pos < scanline_pixels);

    pngle->drawing_y += pgm_read_byte(&interlace_div_y[pngle->interlace_pass]);
    if (pngle->drawing_y >= pngle->hdr.height) {
      if (pngle->interlace_pass >= 6) { return 0; } // Do nothing further

      // Interlace: Next pass
      set_interlace_pass(pngle, pngle->interlace_pass + 1);
      debug_printf("[pngle] interlace pass changed to: %d\n", pngle->interlace_pass);
      remain_bytes = pngle->scanline_stride; // reset
      memset(scanline, 0, remain_bytes);
    }
  }
  pngle->scanline_remain_bytes_to_render = remain_bytes;
  pngle->filter_type = filter_type;

  return 0;
}

int lgfx_pngle_prepare(pngle_t *pngle, lgfx_pngle_read_callback_t read_cb, void* user_data)
{
  if (pngle == NULL || read_cb == NULL) { return PNGLE_STATE_ERROR; }
  if (pngle->palette      ) { free(pngle->palette      ); pngle->palette      = NULL; }
  if (pngle->scanline_buf ) { free(pngle->scanline_buf ); pngle->scanline_buf = NULL; }

  pngle->read_callback = read_cb;
  pngle->user_data = user_data;
  pngle->n_palettes = 0;
  pngle->next_out = pngle->lz_buf;
  pngle->avail_out = TINFL_LZ_DICT_SIZE;
  pngle->filter_type = ~0;
  pngle->trans_color = LGFX_PNGLE_NON_TRANS_COLOR;
  lgfx_tinfl_init(&pngle->inflator);

  if (pngle->read_callback(user_data, pngle->read_buf, 29) != 29
   || memcmp_P(pngle->read_buf, png_header, sizeof(png_header))) return PNGLE_ERROR("Incorrect PNG signature");

  memcpy(&(pngle->hdr), &pngle->read_buf[16], sizeof(pngle_ihdr_t));
  pngle->hdr.width  = swap32(pngle->hdr.width );
  pngle->hdr.height = swap32(pngle->hdr.height);

  debug_printf("[pngle]     width      : %d\n", pngle->hdr.width      );
  debug_printf("[pngle]     height     : %d\n", pngle->hdr.height     );
  debug_printf("[pngle]     depth      : %d\n", pngle->hdr.depth      );
  debug_printf("[pngle]     color_type : %d\n", pngle->hdr.color_type );
  debug_printf("[pngle]     compression: %d\n", pngle->hdr.compression);
  debug_printf("[pngle]     filter     : %d\n", pngle->hdr.filter     );
  debug_printf("[pngle]     interlace  : %d\n", pngle->hdr.interlace  );

  if (pngle->hdr.compression != 0) return PNGLE_ERROR("Unsupported compression type in IHDR");
  if (pngle->hdr.filter      != 0) return PNGLE_ERROR("Unsupported filter type in IHDR");

  /*
          Color    Allowed    Interpretation                            channels
          Type    Bit Depths

          0       1,2,4,8,16  Each pixel is a grayscale sample.         1 channels (Brightness)

          2       8,16        Each pixel is an R,G,B triple.            3 channels (R, G, B)

          3       1,2,4,8     Each pixel is a palette index;            1 channels (palette info)
                              a PLTE chunk must appear.

          4       8,16        Each pixel is a grayscale sample,         2 channels (Brightness, Alpha)
                              followed by an alpha sample.

          6       8,16        Each pixel is an R,G,B triple,            4 channels (R, G, B, Alpha)
                              followed by an alpha sample.
  */
  //  111
  //    ^-- indexed color (palette)
  //   ^--- Color
  //  ^---- Alpha channel
  {
    static const uint8_t table[] PROGMEM =
    { 1 << 5 | 0b00000          // type 0 : 1 channels (Brightness)
    ,          0b11111
    , 3 << 5 | 0b00111          // type 2 : 3 channels (R, G, B)
    , 1 << 5 | 0b10000          // type 3 : 1 channels (palette info)
    , 2 << 5 | 0b00111          // type 4 : 2 channels (Brightness, Alpha)
    ,          0b11111
    , 4 << 5 | 0b00111          // type 6 : 4 channels (R, G, B, Alpha)
    ,          0b11111
    };
    uint_fast8_t ch_mask = pgm_read_byte(&table[pngle->hdr.color_type & 7]);
    pngle->channels = ch_mask >> 5;
    int depth = pngle->hdr.depth;
    if (pngle->hdr.color_type > 7 || pngle->channels == 0
     || depth == 0 || (depth & ch_mask) || (depth != (depth & (-depth)))) return PNGLE_ERROR("Incorrect IHDR info");

    pngle->magni = 0x01010100
                 * ( (depth == 1) ? 0xFF
                   : (depth == 2) ? 0x55
                   : (depth == 4) ? 0x11
                                  : 0x01
                   );

    uint_fast8_t bytes_per_pixel = (pngle->channels * pngle->hdr.depth + 7) >> 3;
    size_t memlen = ((pngle->hdr.width * pngle->channels * pngle->hdr.depth + 7) >> 3) + (2 * bytes_per_pixel);
    if ((pngle->scanline_buf = (uint8_t*)PNGLE_MALLOC(memlen, 1, "scanline flipbuf")) == NULL) return PNGLE_ERROR("Insufficient memory");
    memset(pngle->scanline_buf, 0, memlen);
  }
  // interlace
  set_interlace_pass(pngle, pngle->hdr.interlace ? 0 : 7);

  return 0;
}

int lgfx_pngle_decomp(pngle_t *pngle, lgfx_pngle_draw_callback_t draw_cb)
{
  if (pngle == NULL || draw_cb == NULL) { return PNGLE_STATE_ERROR; }
  pngle->draw_callback = draw_cb;

  uint8_t* read_buf = pngle->read_buf;
  for (;;)
  {
    if (pngle->read_callback(pngle->user_data, read_buf, 12) != 12) { return PNGLE_ERROR("Insufficient data"); }

    uint32_t chunk_remain = read_uint32(&read_buf[4]);
    uint32_t chunk_type   = read_uint32(&read_buf[8]);

    debug_printf("[pngle] Chunk '%.4s' len %u\n", read_buf + 8, chunk_remain);

    // initialize & sanity check
    switch (chunk_type) {
    default:
      if (pngle->read_callback(pngle->user_data, NULL, chunk_remain) != chunk_remain) { return PNGLE_ERROR("Insufficient data"); }
      break;

    case PNGLE_CHUNK_IDAT:
      if (chunk_remain <= 0) return PNGLE_ERROR("Invalid IDAT chunk size");

      do
      {
        size_t len = pngle->read_callback(pngle->user_data, read_buf, (chunk_remain < LGFX_PNGLE_READBUF_LEN) ? chunk_remain : LGFX_PNGLE_READBUF_LEN);
        if (len == 0) { return PNGLE_ERROR("Insufficient data"); }
        chunk_remain -= len;

        debug_printf("[pngle]   Reading IDAT (len %zd / chunk remain %u)\n", len, chunk_remain);

      //debug_printf("[pngle]     in_bytes %zd, out_bytes %zd, next_out %p\n", in_bytes, out_bytes, pngle->next_out);
        size_t in_pos = 0;
        do
        {
          size_t in_bytes = len;
          size_t out_bytes = pngle->avail_out;

          // XXX: lgfx_tinfl_decompress always requires (next_out - lz_buf + avail_out) == TINFL_LZ_DICT_SIZE
          lgfx_tinfl_status status = lgfx_tinfl_decompress(&pngle->inflator, (const lgfx_mz_uint8*)&read_buf[in_pos], &in_bytes, pngle->lz_buf, (lgfx_mz_uint8*)pngle->next_out, &out_bytes, TINFL_FLAG_HAS_MORE_INPUT | TINFL_FLAG_PARSE_ZLIB_HEADER);
          if (status < TINFL_STATUS_DONE)
          {
            // Decompression failed.
            debug_printf("[pngle] lgfx_tinfl_decompress() failed with status %d!\n", status);
            return PNGLE_ERROR("Failed to decompress the IDAT stream");
          }

          len -= in_bytes;
          in_pos += in_bytes;

        //debug_printf("[pngle]       lgfx_tinfl_decompress\n");
        //debug_printf("[pngle]       => in_bytes %zd, out_bytes %zd, next_out %p, status %d\n", in_bytes, out_bytes, pngle->next_out, status);

          if (out_bytes)
          {
            if (pngle_on_data(pngle, pngle->next_out, out_bytes, (LGFX_PNGLE_OUTBUF_LEN >> 2) + (len ? in_pos >> 2 : (LGFX_PNGLE_READBUF_LEN >> 2))) < 0) return -1;
          }
          pngle->next_out += out_bytes;
          pngle->avail_out -= out_bytes;
      // debug_printf("[pngle]         => avail_out %zd, next_out %p\n", pngle->avail_out, pngle->next_out);
          if (pngle->avail_out == 0 || status == TINFL_STATUS_DONE)
          { // Output buffer is full, or decompression is done, so write buffer to output file.
            pngle->avail_out = TINFL_LZ_DICT_SIZE;
            pngle->next_out = pngle->lz_buf;
          }
        } while (len);
      } while (chunk_remain);
      break;

    case PNGLE_CHUNK_IEND:
      pngle->read_callback(pngle->user_data, NULL, 4); // skip crc
      return 0;

    case PNGLE_CHUNK_PLTE:
    {
      // Allow only 2, 3, 6. (2=truecolor / 3=indexed color / 6=truecolor+alpha)
      if (((1 << pngle->hdr.color_type) & 0b1001100) == 0) { return PNGLE_ERROR("PLTE chunk is prohibited on the color type"); }

      uint32_t chunk_remain_3 = chunk_remain / 3;
      if (chunk_remain_3 > MIN(256, (1UL << pngle->hdr.depth))) return PNGLE_ERROR("Too many palettes in PLTE");
      if (chunk_remain_3 <= 0 || chunk_remain != chunk_remain_3 * 3) return PNGLE_ERROR("Invalid PLTE chunk size");
      if (pngle->palette) return PNGLE_ERROR("Too many PLTE chunk");
      uint8_t* plt = (uint8_t*)PNGLE_MALLOC(chunk_remain_3, 4, "palette");
      if (plt == NULL) return PNGLE_ERROR("Insufficient memory");
      pngle->palette = plt;
      pngle->n_palettes = chunk_remain_3;

      if (chunk_remain != pngle->read_callback(pngle->user_data, plt, chunk_remain)) { return PNGLE_ERROR("Insufficient data"); }
      while (chunk_remain_3--)
      {
        ((uint32_t*)(plt))[chunk_remain_3] = ( plt[chunk_remain_3 * 3 + 0] <<  8 )
                                           + ( plt[chunk_remain_3 * 3 + 1] << 16 )
                                           + ( plt[chunk_remain_3 * 3 + 2] << 24 )
                                           + 0xFF;
      }
      break;
    }

    case PNGLE_CHUNK_tRNS:
      if (chunk_remain <= 0 || chunk_remain > 256) return PNGLE_ERROR("Invalid tRNS chunk size");
      if (chunk_remain != pngle->read_callback(pngle->user_data, read_buf, chunk_remain)) { return PNGLE_ERROR("Insufficient data"); }
      switch (pngle->hdr.color_type) {
      case 3: // indexed color
        if (chunk_remain > pngle->n_palettes) return PNGLE_ERROR("Too many palettes in tRNS");
        for (size_t i = 0; i < chunk_remain; ++i)
        {
          pngle->palette[i * 4] = read_buf[i];
        }
        break;

      case 0: // grayscale
        if (chunk_remain != 2) return PNGLE_ERROR("Invalid tRNS chunk size");
        pngle->trans_color = read_buf[1];
        break;

      case 2: // truecolor
        if (chunk_remain != 6) return PNGLE_ERROR("Invalid tRNS chunk size");
        pngle->trans_color = (read_buf[1] <<  8)
                            + (read_buf[3] << 16)
                            + (read_buf[5] << 24)
                            + 0xFF;
        break;

      default:
        return PNGLE_ERROR("tRNS chunk is prohibited on the color type");
      }
      break;
    }
  }
  return 0;
}
