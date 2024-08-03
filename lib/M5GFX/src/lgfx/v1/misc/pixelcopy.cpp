/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [BSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/

#include "pixelcopy.hpp"

namespace lgfx
{
  inline namespace v1
  {
//----------------------------------------------------------------------------

    pixelcopy_t::pixelcopy_t( const void* src_data_
               , color_depth_t dst_depth_
               , color_depth_t src_depth_
               , bool dst_palette_
               , const void* src_palette_
               , uint32_t src_transp_
               )
    : transp    ( src_transp_ )
    , src_depth ( src_depth_ )
    , dst_depth ( dst_depth_ )
    , src_data  ( src_data_   )
    , palette   ( src_palette_)
    , src_mask  ( (1 << (src_depth_ & color_depth_t::bit_mask)) - 1 )
    , dst_mask  ( (1 << (dst_depth_ & color_depth_t::bit_mask)) - 1 )
    , no_convert( src_depth_ == dst_depth_ )
    {
      if (dst_palette_ || dst_bits < 8) {
        if (src_palette_ && (dst_bits == 8) && (src_bits == 8)) {
          fp_copy = pixelcopy_t::copy_rgb_affine<rgb332_t, rgb332_t>;
          fp_skip = pixelcopy_t::skip_rgb_affine<rgb332_t>;
        } else {
          fp_copy = pixelcopy_t::copy_bit_affine;
          fp_skip = pixelcopy_t::skip_bit_affine;
        }
      } else
      if (src_palette_ || src_bits < 8) {
        fp_copy = pixelcopy_t::get_fp_copy_palette_affine<bgr888_t>(dst_depth);
        fp_skip = pixelcopy_t::skip_bit_affine;
      } else {
        if (src_bits > 16) {
          fp_skip = pixelcopy_t::skip_rgb_affine<bgr888_t>;
          if (src_depth == rgb888_3Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<bgr888_t>(dst_depth);
          } else if (src_depth == rgb666_3Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<bgr666_t>(dst_depth);
          }
        } else {
          if (src_depth == rgb565_2Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<swap565_t>(dst_depth);
            fp_skip = pixelcopy_t::skip_rgb_affine<swap565_t>;
          } else if (src_depth == rgb332_1Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<rgb332_t >(dst_depth);
            fp_skip = pixelcopy_t::skip_rgb_affine<rgb332_t>;
          } else { // src_depth == grayscale_8bit:
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<grayscale_t >(dst_depth);
            fp_skip = pixelcopy_t::skip_rgb_affine<grayscale_t>;
          }
        }
      }
    }

    uint32_t pixelcopy_t::copy_bit_fast(void* __restrict dst, uint32_t index, uint32_t last, pixelcopy_t* __restrict param)
    {
      auto dst_bits = param->dst_bits;
      auto shift = ((~index) * dst_bits) & 7;
      auto s = static_cast<const uint8_t*>(param->src_data);
      auto d = &(static_cast<uint8_t*>(dst)[(index * dst_bits) >> 3]);

      uint32_t i = param->positions[0] * param->src_bits;
      param->positions[0] += last - index;
      do {
        uint32_t raw = s[i >> 3];
        i += param->src_bits;
        raw = (raw >> (-(int32_t)i & 7)) & param->src_mask;
        *d = (*d & ~(param->dst_mask << shift)) | ((param->dst_mask & raw) << shift);
        if (!shift) ++d;
        shift = (shift - dst_bits) & 7;
      } while (++index != last);
      return last;
    }

    uint32_t pixelcopy_t::copy_bit_affine(void* __restrict dst, uint32_t index, uint32_t last, pixelcopy_t* __restrict param)
    {
      auto s = static_cast<const uint8_t*>(param->src_data);
      auto d = static_cast<uint8_t*>(dst);

      do {
        uint32_t i = (param->src_x + param->src_y * param->src_bitwidth) * param->src_bits;
        param->src_x32 += param->src_x32_add;
        param->src_y32 += param->src_y32_add;
        uint32_t raw = (pgm_read_byte(&s[i >> 3]) >> (-((int32_t)i + param->src_bits) & 7)) & param->src_mask;
        if (raw != param->transp) {
          auto dstidx = index * param->dst_bits;
          auto shift = (-(int32_t)(dstidx + param->dst_bits)) & 7;
          auto tmp = &d[dstidx >> 3];
          *tmp = (*tmp & ~(param->dst_mask << shift)) | ((param->dst_mask & raw) << shift);
        }
      } while (++index != last);
      return index;
    }

    uint32_t pixelcopy_t::copy_alpha_affine(void* __restrict dst, uint32_t index, uint32_t last, pixelcopy_t* __restrict param)
    {
      auto s = static_cast<const uint8_t*>(param->src_data);
      auto d = static_cast<argb8888_t*>(dst);
      auto src_bitwidth = param->src_bitwidth;
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
      auto src_x32 = param->src_x32;
      auto src_y32 = param->src_y32;
      auto src_bits = param->src_bits;
      uint32_t k = (src_bits == 1) ? 0xFF
                 : (src_bits == 2) ? 0x55
                 : (src_bits == 4) ? 0x11
                 :                   0x01
                 ;
      uint32_t color = param->fore_rgb888 & 0xFFFFFF;
      do
      {
        uint32_t alp = 0;
        uint32_t x = src_x32 >> FP_SCALE;
        if (x < static_cast<uint32_t>(param->src_width))
        {
          uint32_t y = src_y32 >> FP_SCALE;
          if (y < static_cast<uint32_t>(param->src_height))
          {
            uint32_t i = (x + y * src_bitwidth) * src_bits;
            alp = k * ((pgm_read_byte(&s[i >> 3]) >> (-((int32_t)i + src_bits) & 7)) & param->src_mask);
          }
        }
        d[index].set(color + (alp << 24));
        src_x32 += src_x32_add;
        src_y32 += src_y32_add;
      } while (++index != last);
      param->src_x32 = src_x32;
      param->src_y32 = src_y32;
      return index;
    }

    uint32_t pixelcopy_t::blend_palette_fast(void* __restrict dst, uint32_t index, uint32_t last, pixelcopy_t* __restrict param)
    {
      auto dst_bits = param->dst_bits;
      auto dst_mask = param->dst_mask;
      uint32_t k = (dst_bits == 1) ? 0xFF
                 : (dst_bits == 2) ? 0x55
                 : (dst_bits == 4) ? 0x11
                                   : 0x01
                                   ;
      auto shift = ((~index) * dst_bits) & 7;
      auto d = &(static_cast<uint8_t*>(dst)[(index * dst_bits) >> 3]);
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
/*
      if (src_y32_add == 0 && src_x32_add == (1<<FP_SCALE))
      {
        auto s = &(static_cast<const argb8888_t*>(param->src_data)[param->src_x + param->src_y * param->src_bitwidth - index]);
        do {
          uint_fast16_t a = s[index].a;
          if (a)
          {
            uint32_t raw = (s[index].R8() + (s[index].G8()<<1) + s[index].B8()) >> 2;
            if (a != 255)
            {
              uint_fast16_t inv = (256 - a) * k;
              raw = (((*d >> shift) & dst_mask) * inv + raw * ++a) >> 8;
            }
            *d = (*d & ~(dst_mask << shift)) | (dst_mask & (raw >> (8 - dst_bits))) << shift;
          }
          if (!shift) ++d;
          shift = (shift - dst_bits) & 7;
        } while (++index != last);
        return last;
      }
//*/
      auto s = static_cast<const argb8888_t*>(param->src_data);
      do {
        uint32_t i = param->src_x + param->src_y * param->src_bitwidth;
        uint_fast16_t a = s[i].a;
        if (a)
        {
          uint32_t raw = (s[i].R8() + (s[i].G8()<<1) + s[i].B8()) >> 2;
          if (a != 255)
          {
            uint_fast16_t inv = (256 - a) * k;
            raw = (((*d >> shift) & dst_mask) * inv + raw * ++a) >> 8;
          }
          *d = (*d & ~(dst_mask << shift)) | (dst_mask & (raw >> (8 - dst_bits))) << shift;
        }
        if (!shift) ++d;
        shift = (shift - dst_bits) & 7;
        param->src_x32 += src_x32_add;
        param->src_y32 += src_y32_add;
      } while (++index != last);
      return last;
    }

    uint32_t pixelcopy_t::skip_bit_affine(uint32_t index, uint32_t last, pixelcopy_t* param)
    {
      auto s = static_cast<const uint8_t*>(param->src_data);
      auto src_x32     = param->src_x32;
      auto src_y32     = param->src_y32;
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
      auto src_bitwidth= param->src_bitwidth;
      auto transp      = param->transp;
      auto src_bits    = param->src_bits;
      auto src_mask    = param->src_mask;
      do {
        uint32_t i = ((src_x32 >> FP_SCALE) + (src_y32 >> FP_SCALE) * src_bitwidth) * src_bits;
        uint32_t raw = (pgm_read_byte(&s[i >> 3]) >> (-(int32_t)(i + src_bits) & 7)) & src_mask;
        if (raw != transp) break;
        src_x32 += src_x32_add;
        src_y32 += src_y32_add;
      } while (++index != last);
      param->src_x32 = src_x32;
      param->src_y32 = src_y32;
      return index;
    }

    uint32_t pixelcopy_t::compare_bit_affine(void* __restrict dst, uint32_t index, uint32_t last, pixelcopy_t* __restrict param)
    {
      auto s = static_cast<const uint8_t*>(param->src_data);
      auto d = static_cast<bool*>(dst);
      auto src_x32     = param->src_x32;
      auto src_y32     = param->src_y32;
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
      auto src_bitwidth= param->src_bitwidth;
      auto transp      = param->transp;
      auto src_bits    = param->src_bits;
      auto src_mask    = param->src_mask;
      do {
        uint32_t i = ((src_x32 >> FP_SCALE) + (src_y32 >> FP_SCALE) * src_bitwidth) * src_bits;
        uint32_t raw = (pgm_read_byte(&s[i >> 3]) >> (-(int32_t)(i + src_bits) & 7)) & src_mask;
        d[index] = (raw == transp);
        src_x32 += src_x32_add;
        src_y32 += src_y32_add;
      } while (++index != last);
      param->src_x32 = src_x32;
      param->src_y32 = src_y32;
      return index;
    }

//----------------------------------------------------------------------------
  }
}
