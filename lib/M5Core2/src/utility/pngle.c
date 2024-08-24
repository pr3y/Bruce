#if defined (CORE2)
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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <rom/miniz.h>
#include "pngle.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef PNGLE_DEBUG
#define debug_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug_printf(...) ((void)0)
#endif

#define PNGLE_ERROR(s) \
    (pngle->error = (s), pngle->state = PNGLE_STATE_ERROR, -1)
#define PNGLE_CALLOC(a, b, name)                           \
    (debug_printf("[pngle] Allocating %zu bytes for %s\n", \
                  (size_t)(a) * (size_t)(b), (name)),      \
     calloc((size_t)(a), (size_t)(b)))

#define PNGLE_UNUSED(x) (void)(x)

typedef enum {
    PNGLE_STATE_ERROR   = -2,
    PNGLE_STATE_EOF     = -1,
    PNGLE_STATE_INITIAL = 0,

    PNGLE_STATE_FIND_CHUNK_HEADER,
    PNGLE_STATE_HANDLE_CHUNK,
    PNGLE_STATE_CRC,
} pngle_state_t;

typedef enum {
    // Supported chunks
    //   Filter chunk names by following command to (re)generate hex constants;
    //     % perl -ne 'chomp; s/.*\s*\/\/\s*//; print "\tPNGLE_CHUNK_$_ = 0x" .
    //     unpack("H*") . "UL, // $_\n";'
    PNGLE_CHUNK_IHDR = 0x49484452UL,  // IHDR
    PNGLE_CHUNK_PLTE = 0x504c5445UL,  // PLTE
    PNGLE_CHUNK_IDAT = 0x49444154UL,  // IDAT
    PNGLE_CHUNK_IEND = 0x49454e44UL,  // IEND
    PNGLE_CHUNK_tRNS = 0x74524e53UL,  // tRNS
    PNGLE_CHUNK_gAMA = 0x67414d41UL,  // gAMA
} pngle_chunk_t;

// typedef struct _pngle_t pngle_t; // declared in pngle.h
struct _pngle_t {
    pngle_ihdr_t hdr;

    uint_fast8_t channels;  // 0 indicates IHDR hasn't been processed yet

    // PLTE chunk
    size_t n_palettes;
    uint8_t *palette;

    // tRNS chunk
    size_t n_trans_palettes;
    uint8_t *trans_palette;

    // parser state (reset on every chunk header)
    pngle_state_t state;
    uint32_t chunk_type;
    uint32_t chunk_remain;
    mz_ulong crc32;

    // decompression state (reset on IHDR)
    tinfl_decompressor inflator;         // 11000 bytes
    uint8_t lz_buf[TINFL_LZ_DICT_SIZE];  // 32768 bytes
    uint8_t *next_out;  // NULL indicates IDAT hasn't been processed yet
    size_t avail_out;

    // scanline decoder (reset on every set_interlace_pass() call)
    uint8_t *scanline_ringbuf;
    size_t scanline_ringbuf_size;
    size_t scanline_ringbuf_cidx;
    int_fast8_t scanline_remain_bytes_to_render;
    int_fast8_t filter_type;
    uint32_t drawing_x;
    uint32_t drawing_y;

    // interlace
    uint_fast8_t interlace_pass;

    const char *error;

#ifndef PNGLE_NO_GAMMA_CORRECTION
    uint8_t *gamma_table;
    double display_gamma;
#endif

    pngle_init_callback_t init_callback;
    pngle_draw_callback_t draw_callback;
    pngle_done_callback_t done_callback;

    void *user_data;
};

// magic
static const uint8_t png_sig[]     = {137, 80, 78, 71, 13, 10, 26, 10};
static uint32_t interlace_off_x[8] = {0, 0, 4, 0, 2, 0, 1, 0};
static uint32_t interlace_off_y[8] = {0, 0, 0, 4, 0, 2, 0, 1};
static uint32_t interlace_div_x[8] = {1, 8, 8, 4, 4, 2, 2, 1};
static uint32_t interlace_div_y[8] = {1, 8, 8, 8, 4, 4, 2, 2};

static inline uint8_t read_uint8(const uint8_t *p) {
    return *p;
}

static inline uint32_t read_uint32(const uint8_t *p) {
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3] << 0);
}

static inline uint32_t U32_CLAMP_ADD(uint32_t a, uint32_t b, uint32_t top) {
    uint32_t v = a + b;
    if (v < a) return top;    // uint32 overflow
    if (v > top) return top;  // clamp
    return v;
}

void pngle_reset(pngle_t *pngle) {
    if (!pngle) return;

    pngle->state = PNGLE_STATE_INITIAL;
    pngle->error = "No error";

    if (pngle->scanline_ringbuf) free(pngle->scanline_ringbuf);
    if (pngle->palette) free(pngle->palette);
    if (pngle->trans_palette) free(pngle->trans_palette);
#ifndef PNGLE_NO_GAMMA_CORRECTION
    if (pngle->gamma_table) free(pngle->gamma_table);
#endif

    pngle->scanline_ringbuf = NULL;
    pngle->palette          = NULL;
    pngle->trans_palette    = NULL;
#ifndef PNGLE_NO_GAMMA_CORRECTION
    pngle->gamma_table = NULL;
#endif

    pngle->channels = 0;     // indicates IHDR hasn't been processed yet
    pngle->next_out = NULL;  // indicates IDAT hasn't been processed yet

    // clear them just in case...
    memset(&pngle->hdr, 0, sizeof(pngle->hdr));
    pngle->n_palettes       = 0;
    pngle->n_trans_palettes = 0;

    tinfl_init(&pngle->inflator);
}

pngle_t *pngle_new() {
    pngle_t *pngle = (pngle_t *)PNGLE_CALLOC(1, sizeof(pngle_t), "pngle_t");
    if (!pngle) return NULL;

    pngle_reset(pngle);

    return pngle;
}

void pngle_destroy(pngle_t *pngle) {
    if (pngle) {
        pngle_reset(pngle);
        free(pngle);
    }
}

const char *pngle_error(pngle_t *pngle) {
    if (!pngle) return "Uninitialized";
    return pngle->error;
}

uint32_t pngle_get_width(pngle_t *pngle) {
    if (!pngle) return 0;
    return pngle->hdr.width;
}

uint32_t pngle_get_height(pngle_t *pngle) {
    if (!pngle) return 0;
    return pngle->hdr.height;
}

pngle_ihdr_t *pngle_get_ihdr(pngle_t *pngle) {
    if (!pngle) return NULL;
    if (pngle->channels == 0) return NULL;
    return &pngle->hdr;
}

static int is_trans_color(pngle_t *pngle, uint16_t *value, size_t n) {
    if (pngle->n_trans_palettes != 1) return 0;  // false (none or indexed)

    for (size_t i = 0; i < n; i++) {
        if (value[i] != (pngle->trans_palette[i * 2 + 0] * 0x100 +
                         pngle->trans_palette[i * 2 + 1]))
            return 0;  // false
    }
    return 1;  // true
}

static inline void scanline_ringbuf_push(pngle_t *pngle, uint8_t value) {
    pngle->scanline_ringbuf[pngle->scanline_ringbuf_cidx] = value;
    pngle->scanline_ringbuf_cidx =
        (pngle->scanline_ringbuf_cidx + 1) % pngle->scanline_ringbuf_size;
}

static inline uint16_t get_value(pngle_t *pngle, size_t *ridx, int *bitcount,
                                 int depth) {
    uint16_t v;

    switch (depth) {
        case 1:
        case 2:
        case 4:
            if (*bitcount >= 8) {
                *bitcount = 0;
                *ridx     = (*ridx + 1) % pngle->scanline_ringbuf_size;
            }
            *bitcount += depth;
            uint8_t mask  = ((1UL << depth) - 1);
            uint8_t shift = (8 - *bitcount);
            return (pngle->scanline_ringbuf[*ridx] >> shift) & mask;

        case 8:
            v     = pngle->scanline_ringbuf[*ridx];
            *ridx = (*ridx + 1) % pngle->scanline_ringbuf_size;
            return v;

        case 16:
            v     = pngle->scanline_ringbuf[*ridx];
            *ridx = (*ridx + 1) % pngle->scanline_ringbuf_size;

            v     = v * 0x100 + pngle->scanline_ringbuf[*ridx];
            *ridx = (*ridx + 1) % pngle->scanline_ringbuf_size;
            return v;
    }

    return 0;
}

static int pngle_draw_pixels(pngle_t *pngle, size_t scanline_ringbuf_xidx) {
    uint16_t v[4];  // MAX_CHANNELS
    int bitcount        = 0;
    uint8_t pixel_depth = (pngle->hdr.color_type & 1) ? 8 : pngle->hdr.depth;
    uint16_t maxval     = (1UL << pixel_depth) - 1;

    int n_pixels = pngle->hdr.depth == 16 ? 1 : (8 / pngle->hdr.depth);

    for (; n_pixels-- > 0 && pngle->drawing_x < pngle->hdr.width;
         pngle->drawing_x = U32_CLAMP_ADD(
             pngle->drawing_x, interlace_div_x[pngle->interlace_pass],
             pngle->hdr.width)) {
        for (uint_fast8_t c = 0; c < pngle->channels; c++) {
            v[c] = get_value(pngle, &scanline_ringbuf_xidx, &bitcount,
                             pngle->hdr.depth);
        }

        // color type: 0000 0111
        //                     ^-- indexed color (palette)
        //                    ^--- Color
        //                   ^---- Alpha channel

        if (pngle->hdr.color_type & 2) {
            // color
            if (pngle->hdr.color_type & 1) {
                // indexed color: type 3

                // lookup palette info
                uint16_t pidx = v[0];
                if (pidx >= pngle->n_palettes)
                    return PNGLE_ERROR("Color index is out of range");

                v[0] = pngle->palette[pidx * 3 + 0];
                v[1] = pngle->palette[pidx * 3 + 1];
                v[2] = pngle->palette[pidx * 3 + 2];

                // tRNS as an indexed alpha value table (for color type 3)
                v[3] = pidx < pngle->n_trans_palettes
                           ? pngle->trans_palette[pidx]
                           : maxval;
            } else {
                // true color: 2, and 6
                v[3] = (pngle->hdr.color_type & 4)   ? v[3]
                       : is_trans_color(pngle, v, 3) ? 0
                                                     : maxval;
            }
        } else {
            // alpha, tRNS, or opaque
            v[3] = (pngle->hdr.color_type & 4)   ? v[1]
                   : is_trans_color(pngle, v, 1) ? 0
                                                 : maxval;

            // monochrome
            v[1] = v[2] = v[0];
        }

        if (pngle->draw_callback) {
            uint8_t rgba[4] = {(v[0] * 255 + maxval / 2) / maxval,
                               (v[1] * 255 + maxval / 2) / maxval,
                               (v[2] * 255 + maxval / 2) / maxval,
                               (v[3] * 255 + maxval / 2) / maxval};

#ifndef PNGLE_NO_GAMMA_CORRECTION
            if (pngle->gamma_table) {
                for (int i = 0; i < 3; i++) {
                    rgba[i] = pngle->gamma_table[v[i]];
                }
            }
#endif

            pngle->draw_callback(pngle, pngle->drawing_x, pngle->drawing_y,
                                 MIN(interlace_div_x[pngle->interlace_pass] -
                                         interlace_off_x[pngle->interlace_pass],
                                     pngle->hdr.width - pngle->drawing_x),
                                 MIN(interlace_div_y[pngle->interlace_pass] -
                                         interlace_off_y[pngle->interlace_pass],
                                     pngle->hdr.height - pngle->drawing_y),
                                 rgba);
        }
    }

    return 0;
}

static inline int paeth(int a, int b, int c) {
    int p  = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);

    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

static int set_interlace_pass(pngle_t *pngle, uint_fast8_t pass) {
    pngle->interlace_pass = pass;

    uint_fast8_t bytes_per_pixel =
        (pngle->channels * pngle->hdr.depth + 7) / 8;  // 1 if depth <= 8
    size_t scanline_pixels =
        (pngle->hdr.width - interlace_off_x[pngle->interlace_pass] +
         interlace_div_x[pngle->interlace_pass] - 1) /
        interlace_div_x[pngle->interlace_pass];
    size_t scanline_stride =
        (scanline_pixels * pngle->channels * pngle->hdr.depth + 7) / 8;

    pngle->scanline_ringbuf_size =
        scanline_stride + bytes_per_pixel * 2;  // 2 rooms for c/x and a

    if (pngle->scanline_ringbuf) free(pngle->scanline_ringbuf);
    if ((pngle->scanline_ringbuf = PNGLE_CALLOC(pngle->scanline_ringbuf_size, 1,
                                                "scanline ringbuf")) == NULL)
        return PNGLE_ERROR("Insufficient memory");

    pngle->drawing_x   = interlace_off_x[pngle->interlace_pass];
    pngle->drawing_y   = interlace_off_y[pngle->interlace_pass];
    pngle->filter_type = -1;

    pngle->scanline_ringbuf_cidx           = 0;
    pngle->scanline_remain_bytes_to_render = -1;

    return 0;
}

static int setup_gamma_table(pngle_t *pngle, uint32_t png_gamma) {
#ifndef PNGLE_NO_GAMMA_CORRECTION
    if (pngle->gamma_table) free(pngle->gamma_table);

    if (pngle->display_gamma <= 0) return 0;  // disable gamma correction
    if (png_gamma == 0) return 0;

    uint8_t pixel_depth = (pngle->hdr.color_type & 1) ? 8 : pngle->hdr.depth;
    uint16_t maxval     = (1UL << pixel_depth) - 1;

    pngle->gamma_table = PNGLE_CALLOC(1, maxval + 1, "gamma table");
    if (!pngle->gamma_table) return PNGLE_ERROR("Insufficient memory");

    for (int i = 0; i < maxval + 1; i++) {
        pngle->gamma_table[i] =
            (uint8_t)floor(pow(i / (double)maxval,
                               100000.0 / png_gamma / pngle->display_gamma) *
                               255.0 +
                           0.5);
    }
    debug_printf("[pngle] gamma value = %d\n", png_gamma);
#else
    PNGLE_UNUSED(pngle);
    PNGLE_UNUSED(png_gamma);
#endif
    return 0;
}

static int pngle_on_data(pngle_t *pngle, const uint8_t *p, int len) {
    const uint8_t *ep = p + len;

    uint_fast8_t bytes_per_pixel =
        (pngle->channels * pngle->hdr.depth + 7) / 8;  // 1 if depth <= 8

    while (p < ep) {
        if (pngle->drawing_x >= pngle->hdr.width) {
            // New row
            pngle->drawing_x = interlace_off_x[pngle->interlace_pass];
            pngle->drawing_y = U32_CLAMP_ADD(
                pngle->drawing_y, interlace_div_y[pngle->interlace_pass],
                pngle->hdr.height);
            pngle->filter_type = -1;  // Indicate new line
        }

        if (pngle->drawing_x >= pngle->hdr.width ||
            pngle->drawing_y >= pngle->hdr.height) {
            if (pngle->interlace_pass == 0 || pngle->interlace_pass >= 7)
                return len;  // Do nothing further

            // Interlace: Next pass
            if (set_interlace_pass(pngle, pngle->interlace_pass + 1) < 0)
                return -1;
            debug_printf("[pngle] interlace pass changed to: %d\n",
                         pngle->interlace_pass);

            continue;  // This is required because "No filter type bytes are
                       // present in an empty pass".
        }

        if (pngle->filter_type < 0) {
            if (*p > 4) {
                debug_printf("[pngle] Invalid filter type is found; 0x%02x\n",
                             *p);
                return PNGLE_ERROR("Invalid filter type is found");
            }

            pngle->filter_type = (int_fast8_t)*p++;  // 0 - 4

            // push sentinel bytes for new line
            for (uint_fast8_t i = 0; i < bytes_per_pixel; i++) {
                scanline_ringbuf_push(pngle, 0);
            }

            continue;
        }

        size_t cidx = pngle->scanline_ringbuf_cidx;
        size_t bidx = (pngle->scanline_ringbuf_cidx + bytes_per_pixel) %
                      pngle->scanline_ringbuf_size;
        size_t aidx = (pngle->scanline_ringbuf_cidx +
                       pngle->scanline_ringbuf_size - bytes_per_pixel) %
                      pngle->scanline_ringbuf_size;
        // debug_printf("[pngle] cidx = %zd, bidx = %zd, aidx = %zd\n", cidx,
        // bidx, aidx);

        uint8_t c = pngle->scanline_ringbuf[cidx];  // left-up
        uint8_t b = pngle->scanline_ringbuf[bidx];  // up
        uint8_t a = pngle->scanline_ringbuf[aidx];  // left
        uint8_t x = *p++;                           // target
        // debug_printf("[pngle] c = 0x%02x, b = 0x%02x, a = 0x%02x, x =
        // 0x%02x\n", c, b, a, x);

        // Reverse the filter
        switch (pngle->filter_type) {
            case 0:
                break;  // None
            case 1:
                x += a;
                break;  // Sub
            case 2:
                x += b;
                break;  // Up
            case 3:
                x += (a + b) / 2;
                break;  // Average
            case 4:
                x += paeth(a, b, c);
                break;  // Paeth
        }

        scanline_ringbuf_push(pngle, x);  // updates scanline_ringbuf_cidx

        if (pngle->scanline_remain_bytes_to_render < 0)
            pngle->scanline_remain_bytes_to_render = bytes_per_pixel;
        if (--pngle->scanline_remain_bytes_to_render == 0) {
            size_t xidx = (pngle->scanline_ringbuf_cidx +
                           pngle->scanline_ringbuf_size - bytes_per_pixel) %
                          pngle->scanline_ringbuf_size;

            if (pngle_draw_pixels(pngle, xidx) < 0) return -1;

            pngle->scanline_remain_bytes_to_render = -1;  // reset
        }
    }

    return len;
}

static int pngle_handle_chunk(pngle_t *pngle, const uint8_t *buf, size_t len) {
    size_t consume = 0;

    switch (pngle->chunk_type) {
        case PNGLE_CHUNK_IHDR:
            // parse IHDR
            consume = 13;
            if (len < consume) return 0;

            debug_printf("[pngle]   Parse IHDR\n");

            pngle->hdr.width       = read_uint32(buf + 0);
            pngle->hdr.height      = read_uint32(buf + 4);
            pngle->hdr.depth       = read_uint8(buf + 8);
            pngle->hdr.color_type  = read_uint8(buf + 9);
            pngle->hdr.compression = read_uint8(buf + 10);
            pngle->hdr.filter      = read_uint8(buf + 11);
            pngle->hdr.interlace   = read_uint8(buf + 12);

            debug_printf("[pngle]     width      : %d\n", pngle->hdr.width);
            debug_printf("[pngle]     height     : %d\n", pngle->hdr.height);
            debug_printf("[pngle]     depth      : %d\n", pngle->hdr.depth);
            debug_printf("[pngle]     color_type : %d\n",
                         pngle->hdr.color_type);
            debug_printf("[pngle]     compression: %d\n",
                         pngle->hdr.compression);
            debug_printf("[pngle]     filter     : %d\n", pngle->hdr.filter);
            debug_printf("[pngle]     interlace  : %d\n", pngle->hdr.interlace);

            /*
                  Color    Allowed    Interpretation channels Type    Bit Depths

                  0       1,2,4,8,16  Each pixel is a grayscale sample. 1
               channels (Brightness)

                  2       8,16        Each pixel is an R,G,B triple. 3 channels
               (R, G, B)

                  3       1,2,4,8     Each pixel is a palette index; 1 channels
               (palette info) a PLTE chunk must appear.

                  4       8,16        Each pixel is a grayscale sample, 2
               channels (Brightness, Alpha) followed by an alpha sample.

                  6       8,16        Each pixel is an R,G,B triple, 4 channels
               (R, G, B, Alpha) followed by an alpha sample.
              */
            //  111
            //    ^-- indexed color (palette)
            //   ^--- Color
            //  ^---- Alpha channel

            switch (pngle->hdr.color_type) {
                case 0:
                    pngle->channels = 1;
                    if (pngle->hdr.depth != 1 && pngle->hdr.depth != 2 &&
                        pngle->hdr.depth != 4 && pngle->hdr.depth != 8 &&
                        pngle->hdr.depth != 16)
                        return PNGLE_ERROR("Invalid bit depth");
                    break;  // grayscale
                case 2:
                    pngle->channels = 3;
                    if (pngle->hdr.depth != 8 && pngle->hdr.depth != 16)
                        return PNGLE_ERROR("Invalid bit depth");
                    break;  // truecolor
                case 3:
                    pngle->channels = 1;
                    if (pngle->hdr.depth != 1 && pngle->hdr.depth != 2 &&
                        pngle->hdr.depth != 4 && pngle->hdr.depth != 8)
                        return PNGLE_ERROR("Invalid bit depth");
                    break;  // indexed color
                case 4:
                    pngle->channels = 2;
                    if (pngle->hdr.depth != 8 && pngle->hdr.depth != 16)
                        return PNGLE_ERROR("Invalid bit depth");
                    break;  // grayscale + alpha
                case 6:
                    pngle->channels = 4;
                    if (pngle->hdr.depth != 8 && pngle->hdr.depth != 16)
                        return PNGLE_ERROR("Invalid bit depth");
                    break;  // truecolor + alpha
                default:
                    return PNGLE_ERROR("Incorrect IHDR info");
            }

            if (pngle->hdr.compression != 0)
                return PNGLE_ERROR("Unsupported compression type in IHDR");
            if (pngle->hdr.filter != 0)
                return PNGLE_ERROR("Unsupported filter type in IHDR");

            // interlace
            if (set_interlace_pass(pngle, pngle->hdr.interlace ? 1 : 0) < 0)
                return -1;

            // callback
            if (pngle->init_callback)
                pngle->init_callback(pngle, pngle->hdr.width,
                                     pngle->hdr.height);

            break;

        case PNGLE_CHUNK_IDAT:
            // parse & decode IDAT chunk
            if (len < 1) return 0;

            debug_printf("[pngle]   Reading IDAT (len %zd / chunk remain %u)\n",
                         len, pngle->chunk_remain);

            size_t in_bytes  = len;
            size_t out_bytes = pngle->avail_out;

            // debug_printf("[pngle]     in_bytes %zd, out_bytes %zd, next_out
            // %p\n", in_bytes, out_bytes, pngle->next_out);

            // XXX: tinfl_decompress always requires (next_out - lz_buf +
            // avail_out) == TINFL_LZ_DICT_SIZE
            tinfl_status status = tinfl_decompress(
                &pngle->inflator, (const mz_uint8 *)buf, &in_bytes,
                pngle->lz_buf, (mz_uint8 *)pngle->next_out, &out_bytes,
                TINFL_FLAG_HAS_MORE_INPUT | TINFL_FLAG_PARSE_ZLIB_HEADER);

            // debug_printf("[pngle]       tinfl_decompress\n");
            // debug_printf("[pngle]       => in_bytes %zd, out_bytes %zd,
            // next_out %p, status %d\n", in_bytes, out_bytes, pngle->next_out,
            // status);

            if (status < TINFL_STATUS_DONE) {
                // Decompression failed.
                debug_printf(
                    "[pngle] tinfl_decompress() failed with status %d!\n",
                    status);
                return PNGLE_ERROR("Failed to decompress the IDAT stream");
            }

            pngle->next_out += out_bytes;
            pngle->avail_out -= out_bytes;

            // debug_printf("[pngle]         => avail_out %zd, next_out %p\n",
            // pngle->avail_out, pngle->next_out);

            if (status == TINFL_STATUS_DONE || pngle->avail_out == 0) {
                // Output buffer is full, or decompression is done, so write
                // buffer to output file.
                // XXX: This is the only chance to process the buffer.
                uint8_t *read_ptr = pngle->lz_buf;
                size_t n = TINFL_LZ_DICT_SIZE - (size_t)pngle->avail_out;

                // pngle_on_data() usually returns n, otherwise -1 on error
                if (pngle_on_data(pngle, read_ptr, n) < 0) return -1;

                // XXX: tinfl_decompress always requires (next_out - lz_buf +
                // avail_out) == TINFL_LZ_DICT_SIZE
                pngle->next_out  = pngle->lz_buf;
                pngle->avail_out = TINFL_LZ_DICT_SIZE;
            }

            consume = in_bytes;
            break;

        case PNGLE_CHUNK_PLTE:
            consume = 3;
            if (len < consume) return 0;

            memcpy(pngle->palette + pngle->n_palettes * 3, buf, 3);

            debug_printf("[pngle] PLTE[%zd]: (%d, %d, %d)\n", pngle->n_palettes,
                         pngle->palette[pngle->n_palettes * 3 + 0],
                         pngle->palette[pngle->n_palettes * 3 + 1],
                         pngle->palette[pngle->n_palettes * 3 + 2]);

            pngle->n_palettes++;

            break;

        case PNGLE_CHUNK_IEND:
            consume = 0;
            break;

        case PNGLE_CHUNK_tRNS:
            switch (pngle->hdr.color_type) {
                case 3:
                    consume = 1;
                    break;
                case 0:
                    consume = 2 * 1;
                    break;
                case 2:
                    consume = 2 * 3;
                    break;
                default:
                    return PNGLE_ERROR(
                        "tRNS chunk is prohibited on the color type");
            }
            if (len < consume) return 0;

            memcpy(pngle->trans_palette + pngle->n_trans_palettes, buf,
                   consume);

            pngle->n_trans_palettes++;

            break;

        case PNGLE_CHUNK_gAMA:
            consume = 4;
            if (len < consume) return 0;

            if (setup_gamma_table(pngle, read_uint32(buf)) < 0) return -1;

            break;

        default:
            // unknown chunk
            consume = len;

            debug_printf("[pngle] Unknown chunk; %zd bytes discarded\n",
                         consume);
            break;
    }

    return consume;
}

static int pngle_feed_internal(pngle_t *pngle, const uint8_t *buf, size_t len) {
    if (!pngle) return -1;

    switch (pngle->state) {
        case PNGLE_STATE_ERROR:
            return -1;

        case PNGLE_STATE_EOF:
            return len;

        case PNGLE_STATE_INITIAL:
            // find PNG header
            if (len < sizeof(png_sig)) return 0;

            if (memcmp(png_sig, buf, sizeof(png_sig)))
                return PNGLE_ERROR("Incorrect PNG signature");

            debug_printf("[pngle] PNG signature found\n");

            pngle->state = PNGLE_STATE_FIND_CHUNK_HEADER;
            return sizeof(png_sig);

        case PNGLE_STATE_FIND_CHUNK_HEADER:
            if (len < 8) return 0;

            pngle->chunk_remain = read_uint32(buf);
            pngle->chunk_type   = read_uint32(buf + 4);

            pngle->crc32 =
                mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)(buf + 4), 4);

            debug_printf("[pngle] Chunk '%.4s' len %u\n", buf + 4,
                         pngle->chunk_remain);

            pngle->state = PNGLE_STATE_HANDLE_CHUNK;

            // initialize & sanity check
            switch (pngle->chunk_type) {
                case PNGLE_CHUNK_IHDR:
                    if (pngle->chunk_remain != 13)
                        return PNGLE_ERROR("Invalid IHDR chunk size");
                    if (pngle->channels != 0)
                        return PNGLE_ERROR(
                            "Multiple IHDR chunks are not allowed");
                    break;

                case PNGLE_CHUNK_IDAT:
                    if (pngle->chunk_remain <= 0)
                        return PNGLE_ERROR("Invalid IDAT chunk size");
                    if (pngle->channels == 0)
                        return PNGLE_ERROR("No IHDR chunk is found");
                    if (pngle->hdr.color_type == 3 && pngle->palette == NULL)
                        return PNGLE_ERROR("No PLTE chunk is found");

                    if (pngle->next_out == NULL) {
                        // Very first IDAT
                        pngle->next_out  = pngle->lz_buf;
                        pngle->avail_out = TINFL_LZ_DICT_SIZE;
                    }
                    break;

                case PNGLE_CHUNK_PLTE:
                    if (pngle->chunk_remain <= 0)
                        return PNGLE_ERROR("Invalid PLTE chunk size");
                    if (pngle->channels == 0)
                        return PNGLE_ERROR("No IHDR chunk is found");
                    if (pngle->palette)
                        return PNGLE_ERROR("Too many PLTE chunk");

                    switch (pngle->hdr.color_type) {
                        case 3:  // indexed color
                            break;
                        case 2:  // truecolor
                        case 6:  // truecolor + alpha
                            // suggested palettes
                            break;
                        default:
                            return PNGLE_ERROR(
                                "PLTE chunk is prohibited on the color type");
                    }

                    if (pngle->chunk_remain % 3)
                        return PNGLE_ERROR("Invalid PLTE chunk size");
                    if (pngle->chunk_remain / 3 >
                        MIN(256, (1UL << pngle->hdr.depth)))
                        return PNGLE_ERROR("Too many palettes in PLTE");
                    if ((pngle->palette = PNGLE_CALLOC(pngle->chunk_remain / 3,
                                                       3, "palette")) == NULL)
                        return PNGLE_ERROR("Insufficient memory");
                    pngle->n_palettes = 0;
                    break;

                case PNGLE_CHUNK_IEND:
                    if (pngle->next_out == NULL)
                        return PNGLE_ERROR("No IDAT chunk is found");
                    if (pngle->chunk_remain > 0)
                        return PNGLE_ERROR("Invalid IEND chunk size");
                    break;

                case PNGLE_CHUNK_tRNS:
                    if (pngle->chunk_remain <= 0)
                        return PNGLE_ERROR("Invalid tRNS chunk size");
                    if (pngle->channels == 0)
                        return PNGLE_ERROR("No IHDR chunk is found");
                    if (pngle->trans_palette)
                        return PNGLE_ERROR("Too many tRNS chunk");

                    switch (pngle->hdr.color_type) {
                        case 3:  // indexed color
                            if (pngle->chunk_remain > (1UL << pngle->hdr.depth))
                                return PNGLE_ERROR("Too many palettes in tRNS");
                            break;
                        case 0:  // grayscale
                            if (pngle->chunk_remain != 2)
                                return PNGLE_ERROR("Invalid tRNS chunk size");
                            break;
                        case 2:  // truecolor
                            if (pngle->chunk_remain != 6)
                                return PNGLE_ERROR("Invalid tRNS chunk size");
                            break;

                        default:
                            return PNGLE_ERROR(
                                "tRNS chunk is prohibited on the color type");
                    }
                    if ((pngle->trans_palette = PNGLE_CALLOC(
                             pngle->chunk_remain, 1, "trans palette")) == NULL)
                        return PNGLE_ERROR("Insufficient memory");
                    pngle->n_trans_palettes = 0;
                    break;

                default:
                    break;
            }

            return 8;

        case PNGLE_STATE_HANDLE_CHUNK:
            len = MIN(len, pngle->chunk_remain);

            int consumed = pngle_handle_chunk(pngle, buf, len);

            if (consumed > 0) {
                if (pngle->chunk_remain < (uint32_t)consumed)
                    return PNGLE_ERROR("Chunk data has been consumed too much");

                pngle->chunk_remain -= consumed;
                pngle->crc32 =
                    mz_crc32(pngle->crc32, (const mz_uint8 *)buf, consumed);
            }
            if (pngle->chunk_remain <= 0) pngle->state = PNGLE_STATE_CRC;

            return consumed;

        case PNGLE_STATE_CRC:
            if (len < 4) return 0;

            uint32_t crc32 = read_uint32(buf);

            if (crc32 != pngle->crc32) {
                debug_printf("[pngle] CRC: %08x vs %08x => NG\n", crc32,
                             (uint32_t)pngle->crc32);
                return PNGLE_ERROR("CRC mismatch");
            }

            debug_printf("[pngle] CRC: %08x vs %08x => OK\n", crc32,
                         (uint32_t)pngle->crc32);
            pngle->state = PNGLE_STATE_FIND_CHUNK_HEADER;

            // XXX:
            if (pngle->chunk_type == PNGLE_CHUNK_IEND) {
                pngle->state = PNGLE_STATE_EOF;
                if (pngle->done_callback) pngle->done_callback(pngle);
                debug_printf("[pngle] DONE\n");
            }

            return 4;

        default:
            break;
    }

    return PNGLE_ERROR("Invalid state");
}

int pngle_feed(pngle_t *pngle, const void *buf, size_t len) {
    size_t pos               = 0;
    pngle_state_t last_state = pngle->state;

    while (pos < len) {
        int r =
            pngle_feed_internal(pngle, (const uint8_t *)buf + pos, len - pos);
        if (r < 0) return r;  // error

        if (r == 0 && last_state == pngle->state) break;
        last_state = pngle->state;

        pos += r;
    }

    return pos;
}

void pngle_set_display_gamma(pngle_t *pngle, double display_gamma) {
    if (!pngle) return;
#ifndef PNGLE_NO_GAMMA_CORRECTION
    pngle->display_gamma = display_gamma;
#else
    PNGLE_UNUSED(display_gamma);
#endif
}

void pngle_set_init_callback(pngle_t *pngle, pngle_init_callback_t callback) {
    if (!pngle) return;
    pngle->init_callback = callback;
}

void pngle_set_draw_callback(pngle_t *pngle, pngle_draw_callback_t callback) {
    if (!pngle) return;
    pngle->draw_callback = callback;
}

void pngle_set_done_callback(pngle_t *pngle, pngle_done_callback_t callback) {
    if (!pngle) return;
    pngle->done_callback = callback;
}

void pngle_set_user_data(pngle_t *pngle, void *user_data) {
    if (!pngle) return;
    pngle->user_data = user_data;
}

void *pngle_get_user_data(pngle_t *pngle) {
    if (!pngle) return NULL;
    return pngle->user_data;
}

/* vim: set ts=4 sw=4 noexpandtab: */
#endif