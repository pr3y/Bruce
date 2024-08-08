#include "lgfx_fonts.hpp"

#include "platforms/common.hpp"
#include "misc/pixelcopy.hpp"
#include "LGFXBase.hpp"

#include "../Fonts/IPA/lgfx_font_japan.h"
#include "../Fonts/efont/lgfx_efont_cn.h"
#include "../Fonts/efont/lgfx_efont_ja.h"
#include "../Fonts/efont/lgfx_efont_kr.h"
#include "../Fonts/efont/lgfx_efont_tw.h"

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "../internal/algorithm.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace lgfx
{
 inline namespace v1
 {
  struct glcd_fontinfo_t
  {
    uint8_t start;
    uint8_t end;
    uint8_t datawidth;
  };

  size_t IFont::drawCharDummy(LGFXBase* gfx, int32_t x, int32_t y, int32_t w, int32_t h, const TextStyle* style, int32_t& filled_x) const
  {
    int32_t sx = 65536 * style->size_x;
    w = (w * sx) >> 16;
    int32_t sy = 65536 * style->size_y;
    h = (h * sy) >> 16;
    gfx->startWrite();
    if (style->fore_rgb888 != style->back_rgb888)
    {
      gfx->fillRect(x, y, w, h, style->back_rgb888);
      filled_x = x + w;
    }
    if (2 < w && 2 < h)
    {
      gfx->drawRect(x+1, y+1, w-2, h-2, style->fore_rgb888);
    }
    gfx->endWrite();
    return w;
  }

  void BaseFont::getDefaultMetric(FontMetrics *metrics) const
  {
    metrics->width    = width;
    metrics->x_advance = width;
    metrics->x_offset  = 0;
    metrics->height    = height;
    metrics->y_advance = height;
    metrics->y_offset  = 0;
    metrics->baseline  = baseline;
  }
  void BDFfont::getDefaultMetric(FontMetrics *metrics) const
  {
    BaseFont::getDefaultMetric(metrics);
    metrics->y_advance = y_advance;
  }

  bool GLCDfont::updateFontMetric(FontMetrics*, uint16_t uniCode) const {
    auto info = reinterpret_cast<const glcd_fontinfo_t*>(widthtbl);
    return info->start <= uniCode && uniCode <= info->end;
  }

  size_t GLCDfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t c, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  {
    (void)metrics;
    auto info = reinterpret_cast<const glcd_fontinfo_t*>(widthtbl);
    if (c < pgm_read_byte(&info->start) || pgm_read_byte(&info->end) < c)
    {
      return drawCharDummy(gfx, x, y, this->width, this->height, style, filled_x);
    }
    if (!style->cp437 && (c >= 176))
    {
      c++; // Handle 'classic' charset behavior
    }

    c -= pgm_read_byte(&info->start);

    const int32_t fontWidth  = this->width;
    const int32_t fontHeight = this->height;

    const auto datawidth = pgm_read_byte(&info->datawidth);
    auto font_addr = this->chartbl + (c * datawidth);
    auto cc = gfx->getColorConverter();
    bool fillbg = (style->back_rgb888 != style->fore_rgb888);
    uint32_t colortbl[2] = { cc->convert(style->back_rgb888), cc->convert(style->fore_rgb888) };

    const int32_t sy = 65536 * style->size_y;
    const int32_t sx = 65536 * style->size_x;

    gfx->startWrite();

    uint32_t x1 = 0;
    int_fast8_t i = 0;
    do
    {
      uint_fast8_t line = pgm_read_byte(&font_addr[i]);
      uint_fast8_t flg = (line & 0x01);
      int_fast8_t j = 1;
      uint32_t y1 = 0;
      uint32_t x0 = x1;
      x1 = (++i * sx) >> 16;
      uint32_t w = x1 - x0;
      do
      {
        while (flg == ((line >> j) & 0x01) && ++j < fontHeight);
        uint32_t y0 = y1;
        y1 = (j * sy) >> 16;
        if (flg || fillbg)
        {
          gfx->setRawColor(colortbl[flg]);
          gfx->writeFillRect(x, y + y0, w, y1 - y0);
        }
        flg = !flg;
      } while (j < fontHeight);
      x += w;
    } while (i < datawidth);

    uint32_t x2 = (fontWidth * sx) >> 16;
    if (fillbg && datawidth < fontWidth)
    {
      gfx->setRawColor(colortbl[0]);
      gfx->writeFillRect(x, y, x2 - x1, (fontHeight * sy) >> 16);
    }
    gfx->endWrite();

    return x2;
  }

  static size_t draw_char_bmp(LGFXBase* gfx, int32_t x, int32_t y, const TextStyle* style, const uint8_t* font_addr, int_fast8_t fontWidth, int_fast8_t fontHeight, int_fast8_t w, int_fast8_t margin)
  {
    uint32_t colortbl[2] = {gfx->getColorConverter()->convert(style->back_rgb888), gfx->getColorConverter()->convert(style->fore_rgb888)};
    bool fillbg = (style->back_rgb888 != style->fore_rgb888);

    //int32_t clip_left   = gfx->_clip_l;
    //int32_t clip_right  = gfx->_clip_r;
    //int32_t clip_top    = gfx->_clip_t;
    //int32_t clip_bottom = gfx->_clip_b;

    int32_t sx = 65536 * style->size_x;
    int32_t sy = 65536 * style->size_y;

    //if ((x <= clip_right) && (clip_left < (x + fontWidth * sx ))
    // && (y <= clip_bottom) && (clip_top < (y + sh )))
    {
      gfx->startWrite();
      if (fillbg && margin)
      {
        int32_t x0 = ((fontWidth - margin) * sx) >> 16;
        int32_t x1 = ((fontWidth         ) * sx) >> 16;
        if (x0 < x1) {
          gfx->setRawColor(colortbl[0]);
          gfx->writeFillRect(x + x0, y, x1 - x0, (fontHeight * sy) >> 16);
        }
      }
      int32_t i = 0;
      int32_t y1 = 0;
      int32_t y0 = - 1;
      int32_t height = (sy * fontHeight) >> 16;
      do {
        bool fill = y0 != y1;
        y0 = y1;
        y1 = (++i * sy) >> 16;
        int32_t h = (y1 < height && y0 == y1) ? 1 : (y1 - y0);
        uint8_t line = pgm_read_byte(&font_addr[0]);
        bool flg = line & 0x80;
        int_fast8_t j = 1;
        int_fast8_t je = fontWidth - margin;
        int32_t x0 = 0;
        do {
          do {
            if (0 == (j & 7)) line = pgm_read_byte(&font_addr[j >> 3]);
          } while (flg == (bool)(line & (0x80) >> (j&7)) && ++j < je);
          int32_t x1 = (j * sx) >> 16;
          if (flg || (fillbg && fill)) {
            gfx->setRawColor(colortbl[flg]);
            if (flg && x1 == x0) ++x1;
            gfx->writeFillRect(x + x0, y + y0, x1 - x0, h);
          }
          x0 = x1;
          flg = !flg;
        } while (j < je);
        font_addr += w;
      } while (i < fontHeight);
      gfx->endWrite();
    }

    return fontWidth * sx >> 16;
  }


  bool FixedBMPfont::updateFontMetric(FontMetrics *metrics, uint16_t uniCode) const {
    metrics->x_advance = metrics->width = this->width;
    auto info = reinterpret_cast<const glcd_fontinfo_t*>(widthtbl);
    return info->start <= uniCode && uniCode <= info->end;
  }

  bool BMPfont::updateFontMetric(FontMetrics *metrics, uint16_t uniCode) const {
    bool res = ((uniCode -= 0x20u) < 0x60u);
    if (!res) uniCode = 0;
    metrics->x_advance = metrics->width = pgm_read_byte(&this->widthtbl[uniCode]);
    return res;
  }

  bool BDFfont::updateFontMetric(FontMetrics *metrics, uint16_t uniCode) const {
    metrics->x_advance = metrics->width = (uniCode < 0x0100) ? halfwidth : width;
    return true;
  }

  size_t FixedBMPfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t uniCode, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  { // BMP font
    (void)metrics;
    const int_fast16_t fontHeight = this->height;

    auto info = reinterpret_cast<const glcd_fontinfo_t*>(widthtbl);
    if (info->start > uniCode || uniCode > info->end) { return drawCharDummy(gfx, x, y, width, fontHeight, style, filled_x); }

    if (!style->cp437 && (uniCode >= 176))
    {
      uniCode++; // Handle 'classic' charset behavior
    }
    uniCode -= info->start;
    int_fast8_t w = (width + 7) >> 3;
    auto font_addr = (const uint8_t*) &chartbl[uniCode * w * fontHeight];
    return draw_char_bmp(gfx, x, y, style, font_addr, width, fontHeight, (width + 7) >> 3, 0);
  }

  size_t BMPfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t uniCode, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  { // BMP font
    (void)metrics;
    if ((uniCode -= 0x20u) >= 0x60u) return drawCharDummy(gfx, x, y, this->widthtbl[0], this->height, style, filled_x);
    const int_fast8_t fontWidth = pgm_read_byte(&this->widthtbl[uniCode]);
    const int_fast8_t fontHeight = this->height;

    auto font_addr = ((const uint8_t**)this->chartbl)[uniCode];
    return draw_char_bmp(gfx, x, y, style, font_addr, fontWidth, fontHeight, (fontWidth + 6) >> 3, 1);
  }

  size_t BDFfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t c, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  {
    (void)metrics;
    const int_fast8_t bytesize = (this->width + 7) >> 3;
    const int_fast8_t fontHeight = this->height;
    const int_fast8_t fontWidth = (c < 0x0100) ? this->halfwidth : this->width;
    auto it = std::lower_bound(this->indextbl, &this->indextbl[this->indexsize], c);
    if (*it != c) return drawCharDummy(gfx, x, y, fontWidth, fontHeight, style, filled_x);

    const uint8_t* font_addr = &this->chartbl[std::distance(this->indextbl, it) * fontHeight * bytesize];
    return draw_char_bmp(gfx, x, y, style, font_addr, fontWidth, fontHeight, bytesize, 0);
  }

  size_t RLEfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t code, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  { // RLE font
    (void)metrics;
    if ((code -= 0x20u) >= 0x60u) return drawCharDummy(gfx, x, y, this->widthtbl[0], this->height, style, filled_x);

    const uint_fast16_t fontWidth = pgm_read_byte(&this->widthtbl[code]);
    const uint_fast16_t fontHeight = this->height;

    auto font_addr = ((const uint8_t**)this->chartbl)[code];

    uint32_t colortbl[2] = {gfx->getColorConverter()->convert(style->back_rgb888), gfx->getColorConverter()->convert(style->fore_rgb888)};
    bool fillbg = (style->back_rgb888 != style->fore_rgb888);

    //int32_t clip_left   = gfx->_clip_l;
    //int32_t clip_right  = gfx->_clip_r;
    //int32_t clip_top    = gfx->_clip_t;
    //int32_t clip_bottom = gfx->_clip_b;

    int32_t sx = 65536 * style->size_x;
    int32_t sy = 65536 * style->size_y;

    //if ((x <= clip_right) && (clip_left < (x + fontWidth  * sx ))
    // && (y <= clip_bottom) && (clip_top < (y + fontHeight * sy )))
    {
      bool flg = false;
      uint_fast8_t line = 0, i = 1, j = 0;
      int32_t len;
      int32_t y0 = 0;
      int32_t y1 = sy >> 16;
      int32_t x0 = 0;
      gfx->startWrite();
      do {
        line = pgm_read_byte(font_addr++);
        flg = line & 0x80;
        line = (line & 0x7F)+1;
        do {
          len = (line > fontWidth - j) ? fontWidth - j : line;
          line -= len;
          j += len;
          int32_t x1 = (j * sx) >> 16;
          if (fillbg || flg) {
            gfx->setRawColor(colortbl[flg]);
            gfx->writeFillRect( x + x0, y + y0, x1 - x0, y1 - y0);
          }
          x0 = x1;
          if (j == fontWidth)
          {
            j = 0;
            x0 = 0;
            y0 = y1;
            y1 = (++i * sy) >> 16;
          }
        } while (line);
      } while (i <= fontHeight);
      gfx->endWrite();
    }

    return fontWidth * sx >> 16;
  }


//----------------------------------------------------------------------------

  bool GFXfont::updateFontMetric(lgfx::FontMetrics *metrics, uint16_t uniCode) const
  {
    auto glyph_ = getGlyph(uniCode);
    bool res = glyph_;
    if (!res)
    {
      glyph_ = getGlyph(0x20);
      if (!glyph_)
      {
        metrics->x_offset = 0;
        metrics->width = metrics->x_advance = pgm_read_byte(&this->yAdvance) >> 1;
        return false;
      }
    }
    metrics->x_offset  = (int8_t)pgm_read_byte(&glyph_->xOffset);
    metrics->width     = pgm_read_byte(&glyph_->width);
    metrics->x_advance = pgm_read_byte(&glyph_->xAdvance);
    return res;
  }

  GFXglyph* GFXfont::getGlyph(uint16_t uniCode) const
  {
    auto f = pgm_read_word(&first);
    if (uniCode > pgm_read_word(&last)
    ||  uniCode < f) return nullptr;
    uint_fast16_t custom_range_num = pgm_read_word_unaligned(&range_num);
    if (custom_range_num == 0) {
      uniCode -= f;
      return &(((GFXglyph*)pgm_read_ptr( &glyph ))[uniCode]);
    }
    auto range_pst = range;
    size_t i = 0;
    while ((uniCode > pgm_read_word(&range_pst[i].end))
        || (uniCode < pgm_read_word(&range_pst[i].start))) {
      if (++i == custom_range_num) return nullptr;
    }
    uniCode -= pgm_read_word(&range_pst[i].start) - pgm_read_word(&range_pst[i].base);
    return &(((GFXglyph*)pgm_read_ptr( &glyph ))[uniCode]);
  }

  void GFXfont::getDefaultMetric(lgfx::FontMetrics *metrics) const
  {
    int_fast8_t glyph_ab = 0;   // glyph delta Y (height) above baseline
    int_fast8_t glyph_bb = 0;   // glyph delta Y (height) below baseline
    size_t numChars = pgm_read_word(&last) - pgm_read_word(&first);

    size_t custom_range_num = pgm_read_word_unaligned(&range_num);
    if (custom_range_num != 0) {
      EncodeRange *range_pst = range;
      size_t i = 0;
      numChars = custom_range_num;
      do {
        numChars += pgm_read_word(& range_pst[i].end) - pgm_read_word(& range_pst[i].start);
      } while (++i < custom_range_num);
    }

    // Find the biggest above and below baseline offsets
    size_t c = 0;
    do
    {
      GFXglyph* glyph1 = &(((GFXglyph*)pgm_read_ptr( &glyph ))[c]);
      int_fast8_t ab = - (int8_t)(pgm_read_byte(& (glyph1->yOffset)));
      if (ab > glyph_ab) glyph_ab = ab;
      int_fast8_t bb = pgm_read_byte(& glyph1->height) - ab;
      if (bb > glyph_bb) glyph_bb = bb;
    } while ( ++c < numChars );

    metrics->baseline = glyph_ab;
    metrics->y_offset = - glyph_ab;
    metrics->height   = glyph_bb + glyph_ab;
    metrics->y_advance = pgm_read_byte(& yAdvance);
  }

  size_t GFXfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t uniCode, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  {
    int32_t sy = 65536 * style->size_y;
    y += (metrics->y_offset * sy) >> 16;
    auto glyph_ = this->getGlyph(uniCode);
    if (!glyph_)
    {
      glyph_ = this->getGlyph(0x20);
      if (glyph_) return drawCharDummy(gfx, x, y, pgm_read_byte(&glyph_->xAdvance), metrics->height, style, filled_x);
      return 0;
    }

    int32_t w = pgm_read_byte(&glyph_->width);
    int32_t h = pgm_read_byte(&glyph_->height);

    int32_t sx = 65536 * style->size_x;

    int32_t xAdvance = sx * pgm_read_byte(&glyph_->xAdvance) >> 16;
    int32_t xoffset  = sx * ((int8_t)pgm_read_byte(&glyph_->xOffset)) >> 16;

    uint32_t colortbl[2] = {gfx->getColorConverter()->convert(style->back_rgb888), gfx->getColorConverter()->convert(style->fore_rgb888)};
    bool fillbg = (style->back_rgb888 != style->fore_rgb888);
    int32_t left  = 0;
    int32_t right = 0;
    if (fillbg) {
      left  = std::max<int>(filled_x, x + (xoffset < 0 ? xoffset : 0));
      right = x + std::max<int>((w * sx >> 16) + xoffset, xAdvance);
      filled_x = right;
      gfx->setRawColor(colortbl[0]);
    }

    x += xoffset;
    int32_t yoffset = (- metrics->y_offset) + (int8_t)pgm_read_byte(&glyph_->yOffset);

    gfx->startWrite();

    if (left < right) {
      if (yoffset > 0) {
        gfx->writeFillRect(left, y, right - left, (yoffset * sy) >> 16);
      }
      int32_t y0 = ((yoffset + h)   * sy) >> 16;
      int32_t y1 = (metrics->height * sy) >> 16;
      if (y0 < y1) {
        gfx->writeFillRect(left, y + y0, right - left, y1 - y0);
      }
    }

    if (h)
    {
      uint8_t *bitmap_ = &this->bitmap[pgm_read_dword(&glyph_->bitmapOffset)];
      uint_fast8_t mask = 0x80;
      int32_t btmp = pgm_read_byte(bitmap_); /// btmpの最上位ビット (符号ビット) をフラグとして扱うため敢えて uintにしない ;
      if (btmp & mask) { btmp = ~btmp; }
      uint32_t bitlen = 0;

      gfx->setRawColor(colortbl[1]);
      uint32_t limit_width = ( w            * sx) >> 16;
      int32_t limit_height = ((h + yoffset) * sy) >> 16;
      int32_t y1 = (yoffset * sy) >> 16;
      int32_t y0 = y1 - 1;
      int32_t i = 0;
      do {
        bool fill = y0 != y1;
        y0 = y1;
        y1 = ((++i + yoffset) * sy) >> 16;
        int32_t fh = (y1 < limit_height && y1 == y0) ? 1 : (y1 - y0);

        if (left < right && fill) {
          gfx->setRawColor(colortbl[0]);
          gfx->writeFillRect(left, y + y0, right - left, fh);
          gfx->setRawColor(colortbl[1]);
        }
        uint32_t j = 0;
        uint32_t x0 = 0;
        uint32_t remain = w;
        do
        {
          if (bitlen == 0)
          {
            btmp = ~btmp;
            do
            { /// ビット連続数を取得するループ;
              do
              {
                ++bitlen;

                /// 1Byteぶん走査できたら次のデータを取得する。;
                if (0 == (mask >>= 1))
                {
                  goto label_nextbyte;
/// gotoを使用してループ外に出る理由は速度向上のため。連続ループ時にループ内の処理を短くする効果がある;
/// 「データ取得が必要な場合」にgotoジャンプさせることにより、「データ取得が不要な場合」はジャンプが不要になる。;
                }
              } while (btmp & mask);
              break; /// ビットが途切れた場合はループを抜ける;

label_nextbyte: /// 次のデータを取得する;
              mask = 0x80;
              btmp = pgm_read_byte(++bitmap_) ^ (btmp < 0 ? ~0 : 0);
            } while (btmp & mask);
          }

          uint32_t l = std::min(bitlen, remain);
          remain -= l;
          bitlen -= l;
          j += l;
          uint32_t x1 = (j * sx) >> 16;
          if (btmp >= 0) {
            uint32_t fw = (x1 < limit_width && x1 == x0) ? 1 : (x1 - x0);
            gfx->writeFillRect(x + x0, y + y0, fw, fh);
          }
          x0 = x1;
        } while (remain);
      } while (i < h);
    }
    gfx->endWrite();
    return xAdvance;
  }

//----------------------------------------------------------------------------

  struct u8g2_font_decode_t
  {
    u8g2_font_decode_t(const uint8_t* ptr) : decode_ptr(ptr), decode_bit_pos(0) {}

    const uint8_t* decode_ptr;      /* pointer to the compressed data */
    uint8_t decode_bit_pos;     /* bitpos inside a byte of the compressed data */

    uint_fast8_t get_unsigned_bits(uint_fast8_t cnt)
    {
      uint_fast8_t bit_pos = this->decode_bit_pos;
      uint_fast8_t val = pgm_read_byte(this->decode_ptr) >> bit_pos;

      auto bit_pos_plus_cnt = bit_pos + cnt;
      if ( bit_pos_plus_cnt >= 8 )
      {
        bit_pos_plus_cnt -= 8;
        val |= pgm_read_byte(++this->decode_ptr) << (8-bit_pos);
      }
      this->decode_bit_pos = bit_pos_plus_cnt;
      return val & ((1U << cnt) - 1);
    }

    int_fast8_t get_signed_bits(uint_fast8_t cnt)
    {
      return (int_fast8_t)get_unsigned_bits(cnt) - (1 << (cnt-1));
    }
  };


  const uint8_t* U8g2font::getGlyph(uint16_t encoding) const
  {
    const uint8_t *font = &this->_font[23];

    if ( encoding <= 255 )
    {
      if ( encoding >= 'a' )      { font += this->start_pos_lower_a(); }
      else if ( encoding >= 'A' ) { font += this->start_pos_upper_A(); }

      for ( ; pgm_read_byte(&font[1]); font += pgm_read_byte(&font[1]))
      {
        if ( pgm_read_byte(&font[0]) == encoding ) { return font + 2; }  /* skip encoding and glyph size */
      }
    }
    else
    {
      uint_fast16_t e;
      const uint8_t *unicode_lut;

      font += this->start_pos_unicode();
      unicode_lut = font;

      do
      {
        font += (pgm_read_byte(&unicode_lut[0]) << 8) + pgm_read_byte(&unicode_lut[1]);
        e     = (pgm_read_byte(&unicode_lut[2]) << 8) + pgm_read_byte(&unicode_lut[3]);
        unicode_lut += 4;
      } while ( e < encoding );

      for ( ; 0 != (e = (pgm_read_byte(&font[0]) << 8) + pgm_read_byte(&font[1])) ; font += pgm_read_byte(&font[2]))
      {
        if ( e == encoding ) { return font + 3; }  /* skip encoding and glyph size */
      }
    }
    return nullptr;
  }

  void U8g2font::getDefaultMetric(lgfx::FontMetrics *metrics) const
  {
    metrics->height    = max_char_height();
    metrics->y_advance = metrics->height;
    metrics->baseline  = metrics->height + y_offset();
    metrics->y_offset  = -metrics->baseline;
    metrics->x_offset  = 0;
  }

  bool U8g2font::updateFontMetric(lgfx::FontMetrics *metrics, uint16_t uniCode) const
  {
    u8g2_font_decode_t decode(getGlyph(uniCode));
    if ( decode.decode_ptr )
    {
      metrics->width     = decode.get_unsigned_bits(this->bits_per_char_width());
                          decode.get_unsigned_bits(this->bits_per_char_height());
      metrics->x_offset  = decode.get_signed_bits  (this->bits_per_char_x());
                          decode.get_signed_bits  (this->bits_per_char_y());
      metrics->x_advance = decode.get_signed_bits  (this->bits_per_delta_x());
      return true;
    }
    metrics->width = metrics->x_advance = this->max_char_width();
    metrics->x_offset = 0;
    return false;
  }

  size_t U8g2font::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t uniCode, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  {
    int32_t sy = 65536 * style->size_y;
    y += (metrics->y_offset * sy) >> 16;
    u8g2_font_decode_t decode(getGlyph(uniCode));
    if ( decode.decode_ptr == nullptr ) return drawCharDummy(gfx, x, y, this->max_char_width(), metrics->height, style, filled_x);

    uint32_t w = decode.get_unsigned_bits(bits_per_char_width());
    uint32_t h = decode.get_unsigned_bits(bits_per_char_height());

    int32_t sx = 65536 * style->size_x;

    int32_t xoffset = (decode.get_signed_bits(bits_per_char_x()) * sx) >> 16;

    int32_t yoffset = -(int32_t)(decode.get_signed_bits(bits_per_char_y()) + h + metrics->y_offset);

    int32_t xAdvance = (decode.get_signed_bits(bits_per_delta_x()) * sx) >> 16;

    uint32_t colortbl[2] = {gfx->getColorConverter()->convert(style->back_rgb888), gfx->getColorConverter()->convert(style->fore_rgb888)};
    bool fillbg = (style->back_rgb888 != style->fore_rgb888);
    int32_t left  = 0;
    int32_t right = 0;
    if (fillbg) {
      left  = std::max<int>(filled_x, x + (xoffset < 0 ? xoffset : 0));
      right = x + std::max<int>(((w * sx) >> 16) + xoffset, xAdvance);
      filled_x = right;
      gfx->setRawColor(colortbl[0]);
    }
    x += xoffset;
    gfx->startWrite();

    if (left < right)
    {
      if (yoffset > 0) {
        gfx->writeFillRect(left, y, right - left, (yoffset * sy) >> 16);
      }
      int32_t y0 = ((yoffset + h)   * sy) >> 16;
      int32_t y1 = (metrics->height * sy) >> 16;
      if (y0 < y1) {
        gfx->writeFillRect(left, y + y0, right - left, y1 - y0);
      }
    }

    if ( w > 0 )
    {
      if (left < right)
      {
        int32_t y0  = (  yoffset      * sy) >> 16;
        int32_t len = (((yoffset + h) * sy) >> 16) - y0;
        if (left < x)
        {
          gfx->writeFillRect(left, y + y0, x - left, len);
        }
        int32_t xwsx = x + ((w * sx) >> 16);
        if (xwsx < right)
        {
          gfx->writeFillRect(xwsx, y + y0, right - xwsx, len);
        }
      }
      left -= x;
      uint32_t ab[2];
      uint32_t lx = 0;
      uint32_t ly = 0;
      int32_t y0 = ((yoffset    ) * sy) >> 16;
      int32_t y1 = ((yoffset + 1) * sy) >> 16;
      do
      {
        ab[0] = decode.get_unsigned_bits(bits_per_0());
        ab[1] = decode.get_unsigned_bits(bits_per_1());
        bool i = 0;
        do
        {
          uint32_t length = ab[i];
          while (length)
          {
            uint32_t len = (length > w - lx) ? w - lx : length;
            length -= len;
            if (i || fillbg)
            {
              int32_t x0 = (lx * sx) >> 16;
              if (!i && x0 < left) x0 = left;
              int32_t x1 = ((lx + len) * sx) >> 16;
              if (x0 < x1)
              {
                gfx->setRawColor(colortbl[i]);
                gfx->writeFillRect( x + x0
                                  , y + y0
                                  , x1 - x0
                                  , y1 - y0);
              }
            }
            lx += len;
            if (lx == w)
            {
              lx = 0;
              ++ly;
              y0 = y1;
              y1 = ((ly + yoffset + 1) * sy) >> 16;
            }
          }
          i = !i;
        } while (i || decode.get_unsigned_bits(1) != 0 );
      } while (ly < h);
    }
    gfx->endWrite();
    return xAdvance;
  }

//----------------------------------------------------------------------------

  void VLWfont::getDefaultMetric(FontMetrics *metrics) const
  {
    metrics->x_offset  = 0;
    metrics->y_offset  = 0;
    metrics->baseline  = maxAscent;
    metrics->y_advance = yAdvance;
    metrics->height    = yAdvance;
  }

  VLWfont::~VLWfont() {
    unloadFont();
  }

  bool VLWfont::unloadFont(void)
  {
    _fontLoaded = false;
    if (gUnicode)  { heap_free(gUnicode);  gUnicode  = nullptr; }
    if (gWidth)    { heap_free(gWidth);    gWidth    = nullptr; }
    if (gxAdvance) { heap_free(gxAdvance); gxAdvance = nullptr; }
    if (gdX)       { heap_free(gdX);       gdX       = nullptr; }
    if (gBitmap)   { heap_free(gBitmap);   gBitmap   = nullptr; }
    if (_fontData) {
      _fontData->preRead();
      _fontData->close();
      _fontData->postRead();
      _fontData = nullptr;
    }
    return true;
  }

  bool VLWfont::getUnicodeIndex(uint16_t unicode, uint16_t *index) const
  {
    if (gUnicode[gCount-1] < unicode) return false;
    auto poi = std::lower_bound(gUnicode, &gUnicode[gCount], unicode);
    *index = std::distance(gUnicode, poi);
    return (*poi == unicode);
  }

  bool VLWfont::updateFontMetric(FontMetrics *metrics, uint16_t uniCode) const {
    uint16_t gNum = 0;
    if (getUnicodeIndex(uniCode, &gNum)) {
      if (gWidth && gxAdvance && gdX[gNum]) {
        metrics->width     = gWidth[gNum];
        metrics->x_advance = gxAdvance[gNum];
        metrics->x_offset  = gdX[gNum];
      } else {
        auto file = _fontData;

        file->preRead();

        file->seek(28 + gNum * 28);  // headerPtr
        uint32_t buffer[6];
        file->read((uint8_t*)buffer, 24);
        metrics->width     = getSwap32(buffer[1]); // Width of glyph
        metrics->x_advance = getSwap32(buffer[2]); // xAdvance - to move x cursor
        metrics->x_offset  = (int32_t)((int8_t)getSwap32(buffer[4])); // x delta from cursor

        file->postRead();
      }
      return true;
    }
    metrics->width = metrics->x_advance = this->spaceWidth;
    metrics->x_offset = 0;
    return (uniCode == 0x20);
  }


  bool VLWfont::loadFont(DataWrapper* data) {
    _fontData = data;
    {
      uint32_t buf[6];
      data->read((uint8_t*)buf, 6 * 4); // 24 Byte read

      gCount   = getSwap32(buf[0]); // glyph count in file
                //getSwap32(buf[1]); // vlw encoder version - discard
      yAdvance = getSwap32(buf[2]); // Font size in points, not pixels
                //getSwap32(buf[3]); // discard
      ascent   = getSwap32(buf[4]); // top of "d"
      descent  = getSwap32(buf[5]); // bottom of "p"
    }

    // These next gFont values might be updated when the Metrics are fetched
    maxAscent  = ascent;   // Determined from metrics
    maxDescent = descent;  // Determined from metrics
    yAdvance   = std::max((int)yAdvance, ascent + descent);
    spaceWidth = yAdvance * 2 / 7;  // Guess at space width

//ESP_LOGI("LGFX", "ascent:%d  descent:%d", gFont.ascent, gFont.descent);

    if (!gCount) return false;

//ESP_LOGI("LGFX", "font count:%d", gCount);

    uint32_t bitmapPtr = 24 + (uint32_t)gCount * 28;

    gBitmap   = (uint32_t*)heap_alloc_psram( gCount * 4); // seek pointer to glyph bitmap in the file
    gUnicode  = (uint16_t*)heap_alloc_psram( gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
    gWidth    =  (uint8_t*)heap_alloc_psram( gCount );    // Width of glyph
    gxAdvance =  (uint8_t*)heap_alloc_psram( gCount );    // xAdvance - to move x cursor
    gdX       =   (int8_t*)heap_alloc_psram( gCount );    // offset for bitmap left edge relative to cursor X

    if (nullptr == gBitmap  ) gBitmap   = (uint32_t*)heap_alloc( gCount * 4); // seek pointer to glyph bitmap in the file
    if (nullptr == gUnicode ) gUnicode  = (uint16_t*)heap_alloc( gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
    if (nullptr == gWidth   ) gWidth    =  (uint8_t*)heap_alloc( gCount );    // Width of glyph
    if (nullptr == gxAdvance) gxAdvance =  (uint8_t*)heap_alloc( gCount );    // xAdvance - to move x cursor
    if (nullptr == gdX      ) gdX       =   (int8_t*)heap_alloc( gCount );    // offset for bitmap left edge relative to cursor X

    if (!gUnicode
      || !gBitmap
      || !gWidth
      || !gxAdvance
      || !gdX) {
//ESP_LOGE("LGFX", "can not alloc font table");
      return false;
    }

    _fontLoaded = true;

    size_t gNum = 0;
    _fontData->seek(24);  // headerPtr
    uint32_t buffer[7];
    do {
      _fontData->read((uint8_t*)buffer, 7 * 4); // 28 Byte read
      uint16_t unicode = getSwap32(buffer[0]); // Unicode code point value
      uint32_t w = (uint8_t)getSwap32(buffer[2]); // Width of glyph
      if (gUnicode)   gUnicode[gNum]  = unicode;
      if (gWidth)     gWidth[gNum]    = w;
      if (gxAdvance)  gxAdvance[gNum] = (uint8_t)getSwap32(buffer[3]); // xAdvance - to move x cursor
      if (gdX)        gdX[gNum]       =  (int8_t)getSwap32(buffer[5]); // x delta from cursor

      uint16_t height = getSwap32(buffer[1]); // Height of glyph
      if ((unicode > 0xFF) || ((unicode > 0x20) && (unicode < 0xA0) && (unicode != 0x7F))) {
        int16_t dY =  (int16_t)getSwap32(buffer[4]); // y delta from baseline
//Serial.printf("LGFX:unicode:%x  dY:%d\r\n", unicode, dY);
        if (maxAscent < dY && unicode != 0x3000) {
          maxAscent = dY;
        }
        if (maxDescent < (height - dY) && unicode != 0x3000) {
//Serial.printf("LGFX:maxDescent:%d\r\n", maxDescent);
          maxDescent = height - dY;
        }
      }

      if (gBitmap)  gBitmap[gNum] = bitmapPtr;
      bitmapPtr += w * height;
    } while (++gNum < gCount);

    yAdvance = maxAscent + maxDescent;

//Serial.printf("LGFX:maxDescent:%d\r\n", maxDescent);
    return true;
  }

//----------------------------------------------------------------------------

  size_t VLWfont::drawChar(LGFXBase* gfx, int32_t x, int32_t y, uint16_t code, const TextStyle* style, FontMetrics* metrics, int32_t& filled_x) const
  {
    auto file = this->_fontData;

    uint32_t buffer[6] = {0};
    uint16_t gNum = 0;

    int32_t sy = 65536 * style->size_y;
    y += (metrics->y_offset * sy) >> 16;

    if (code == 0x20) {
      gNum = 0xFFFF;
      buffer[2] = getSwap32(this->spaceWidth);
    } else if (!this->getUnicodeIndex(code, &gNum)) {
      return drawCharDummy(gfx, x, y, this->spaceWidth, metrics->height, style, filled_x);
    } else {
      file->preRead();
      file->seek(28 + gNum * 28);
      file->read((uint8_t*)buffer, 24);
      file->seek(this->gBitmap[gNum]);
    }


    int32_t h        = getSwap32(buffer[0]); // Height of glyph
    int32_t w        = getSwap32(buffer[1]); // Width of glyph
    int32_t sx       = 65536 * style->size_x;
    int32_t xAdvance = (getSwap32(buffer[2]) * sx) >> 16; // xAdvance - to move x cursor
    int32_t xoffset  = ((int32_t)((int8_t)getSwap32(buffer[4])) * sx) >> 16; // x delta from cursor
    int32_t dY       = (int16_t)getSwap32(buffer[3]); // y delta from baseline
    int32_t yoffset  = (this->maxAscent - dY);
//      int32_t yoffset = (gfx->_font_metrics.y_offset) - dY;

    auto pixel = (uint8_t*)alloca(w * h);
    if (gNum != 0xFFFF) {
      file->read(pixel, w * h);
      file->postRead();
    }

    gfx->startWrite();

    uint32_t colortbl[2] = {gfx->getColorConverter()->convert(style->back_rgb888), gfx->getColorConverter()->convert(style->fore_rgb888)};
    bool fillbg = (style->back_rgb888 != style->fore_rgb888);
    int32_t left  = 0;
    int32_t right = 0;
    if (fillbg) {
      left  = std::max(filled_x, x + (xoffset < 0 ? xoffset : 0));
      right = x + std::max<int>(((w * sx) >> 16) + xoffset, xAdvance);
      filled_x = right;
    }
    x += xoffset;

    int32_t clip_left;
    int32_t clip_top;
    int32_t clip_w;
    int32_t clip_h;

    gfx->getClipRect(&clip_left, &clip_top, &clip_w, &clip_h);
    int32_t clip_right = clip_left + clip_w-1;
    int32_t clip_bottom = clip_top + clip_h-1;

    int32_t bx = x;
    int32_t bw = (w * sx) >> 16;
    if (x < clip_left) { bw += (x - clip_left); bx = clip_left; }

    if (bw > clip_right+1 - bx) bw = clip_right+1 - bx;

    if (bw >= 0)
    {
      int32_t fore_r = ((style->fore_rgb888>>16)&0xFF);
      int32_t fore_g = ((style->fore_rgb888>> 8)&0xFF);
      int32_t fore_b = ((style->fore_rgb888)    &0xFF);

      if (fillbg || !gfx->isReadable() || gfx->hasPalette())
      { // fill background mode  or unreadable panel  or palette sprite mode
        if (left < right && fillbg) {
          gfx->setRawColor(colortbl[0]);
          if (yoffset > 0) {
            gfx->writeFillRect(left, y, right - left, (yoffset * sy) >> 16);
          }
          int32_t y0 = ((yoffset + h)   * sy) >> 16;
          int32_t y1 = (metrics->height * sy) >> 16;
          if (y0 < y1) {
            gfx->writeFillRect(left, y + y0, right - left, y1 - y0);
          }
        }

        if (0 < w) {
          uint32_t back = fillbg ? style->back_rgb888 : gfx->getBaseColor();
          int32_t back_r = ((back>>16)&0xFF);
          int32_t back_g = ((back>> 8)&0xFF);
          int32_t back_b = ( back     &0xFF);
          int32_t i = 0;
          int32_t y0, y1 = (yoffset * sy) >> 16;
          do {
            y0 = y1;
            if (y0 > (clip_bottom - y)) break;
            y1 = ((yoffset + i + 1) * sy) >> 16;
            if (left < right) {
              gfx->setRawColor(colortbl[0]);
              gfx->writeFillRect(left, y + y0, right - left, y1 - y0);
            }
            int32_t j = 0;
            do {
              int32_t x0 = (j * sx) >> 16;
              while (pixel[j] != 0xFF) {
                int32_t x1 = ((j + 1) * sx) >> 16;
                if (pixel[j] != 0 && x0 < x1) {
                  int32_t p = 1 + (uint32_t)pixel[j];
                  gfx->setColor(color888( ( fore_r * p + back_r * (257 - p)) >> 8
                                        , ( fore_g * p + back_g * (257 - p)) >> 8
                                        , ( fore_b * p + back_b * (257 - p)) >> 8 ));
                  gfx->writeFillRect(x + x0, y + y0, x1 - x0, y1 - y0);
                }
                x0 = x1;
                if (++j == w || (clip_right - x) < x0) break;
              }
              if (j == w || (clip_right - x) < x0) break;
              gfx->setRawColor(colortbl[1]);
              do { ++j; } while (j != w && pixel[j] == 0xFF);
              gfx->writeFillRect(x + x0, y + y0, ((j * sx) >> 16) - x0, y1 - y0);
            } while (j != w);
            pixel += w;
          } while (++i < h);
        }
      }
      else // alpha blend mode
      {
        auto buf = (bgr888_t*)alloca((bw * ((sy + 65535) >> 16)) * sizeof(bgr888_t));

        pixelcopy_t p_(buf, gfx->getColorConverter()->depth, rgb888_3Byte, gfx->hasPalette());
        int32_t y0, y1 = (yoffset * sy) >> 16;
        int32_t i = 0;
        do {
          y0 = y1;
          if (y0 > (clip_bottom - y)) break;
          y1 = ((yoffset + i + 1) * sy) >> 16;
          int32_t by = y + y0;
          int32_t bh = y1 - y0;

          if (by < clip_top) { bh += by - clip_top; by = clip_top; }
          if (bh > 0) {
            int32_t j0 = 0;

            // search first and last pixel
            while (j0 != w && !pixel[j0    ]) { ++j0; }
            if (j0 != w) {
              int32_t j1 = w;
              while (j0 != j1 && !pixel[j1 - 1]) { --j1; }
              int32_t rx = (j0 * sx) >> 16;
              int32_t rw = (j1 * sx) >> 16;
              if (rx < bx    -x) rx = bx    -x;
              if (rw > bx+bw -x) rw = bx+bw -x;
              rw -= rx;

              if (0 < rw) {
                gfx->readRectRGB(x + rx, by, rw, bh, (uint8_t*)buf);

                int32_t x0, x1 = ((j0 * sx) >> 16) - rx;
                do {
                  x0 = x1;
                  if (x0 < 0) x0 = 0;
                  x1 = (int)(((j0 + 1) * sx) >> 16) - rx;
                  if (x1 > rw) x1 = rw;
                  if (pixel[j0] && x0 < x1) {
                    int32_t p = 1 + pixel[j0];
                    do {
                      int32_t yy = 0;
                      do {
                        auto bgr = &buf[x0 + yy * rw];
                        bgr->r = ( fore_r * p + bgr->r * (257 - p)) >> 8;
                        bgr->g = ( fore_g * p + bgr->g * (257 - p)) >> 8;
                        bgr->b = ( fore_b * p + bgr->b * (257 - p)) >> 8;
                      } while (++yy != bh);
                    } while (++x0 != x1);
                  }
                } while (++j0 < j1);
                gfx->pushImage(x + rx, by, rw, bh, &p_);
              }
            }
          }
          pixel += w;
        } while (++i < h);
      }
    }
    gfx->endWrite();
    return xAdvance;
  }

//----------------------------------------------------------------------------

  // deprecated array.
  const IFont* fontdata [] =
  {
    &fonts::Font0,  // GLCD font (Font 0)
    &fonts::Font0,  // Font 1 current unused
    &fonts::Font2,
    &fonts::Font0,  // Font 3 current unused
    &fonts::Font4,
    &fonts::Font0,  // Font 5 current unused
    &fonts::Font6,
    &fonts::Font7,
    &fonts::Font8,
    nullptr,
  };

//----------------------------------------------------------------------------
  namespace fonts
  {
    using namespace lgfx;

    // Original Adafruit_GFX "Free Fonts"
    #include "../Fonts/GFXFF/TomThumb.h"  // TT1

    #include "../Fonts/GFXFF/FreeMono9pt7b.h"  // FF1 or FM9
    #include "../Fonts/GFXFF/FreeMono12pt7b.h" // FF2 or FM12
    #include "../Fonts/GFXFF/FreeMono18pt7b.h" // FF3 or FM18
    #include "../Fonts/GFXFF/FreeMono24pt7b.h" // FF4 or FM24

    #include "../Fonts/GFXFF/FreeMonoOblique9pt7b.h"  // FF5 or FMO9
    #include "../Fonts/GFXFF/FreeMonoOblique12pt7b.h" // FF6 or FMO12
    #include "../Fonts/GFXFF/FreeMonoOblique18pt7b.h" // FF7 or FMO18
    #include "../Fonts/GFXFF/FreeMonoOblique24pt7b.h" // FF8 or FMO24

    #include "../Fonts/GFXFF/FreeMonoBold9pt7b.h"  // FF9  or FMB9
    #include "../Fonts/GFXFF/FreeMonoBold12pt7b.h" // FF10 or FMB12
    #include "../Fonts/GFXFF/FreeMonoBold18pt7b.h" // FF11 or FMB18
    #include "../Fonts/GFXFF/FreeMonoBold24pt7b.h" // FF12 or FMB24

    #include "../Fonts/GFXFF/FreeMonoBoldOblique9pt7b.h"  // FF13 or FMBO9
    #include "../Fonts/GFXFF/FreeMonoBoldOblique12pt7b.h" // FF14 or FMBO12
    #include "../Fonts/GFXFF/FreeMonoBoldOblique18pt7b.h" // FF15 or FMBO18
    #include "../Fonts/GFXFF/FreeMonoBoldOblique24pt7b.h" // FF16 or FMBO24

    // Sans serif fonts
    #include "../Fonts/GFXFF/FreeSans9pt7b.h"  // FF17 or FSS9
    #include "../Fonts/GFXFF/FreeSans12pt7b.h" // FF18 or FSS12
    #include "../Fonts/GFXFF/FreeSans18pt7b.h" // FF19 or FSS18
    #include "../Fonts/GFXFF/FreeSans24pt7b.h" // FF20 or FSS24

    #include "../Fonts/GFXFF/FreeSansOblique9pt7b.h"  // FF21 or FSSO9
    #include "../Fonts/GFXFF/FreeSansOblique12pt7b.h" // FF22 or FSSO12
    #include "../Fonts/GFXFF/FreeSansOblique18pt7b.h" // FF23 or FSSO18
    #include "../Fonts/GFXFF/FreeSansOblique24pt7b.h" // FF24 or FSSO24

    #include "../Fonts/GFXFF/FreeSansBold9pt7b.h"  // FF25 or FSSB9
    #include "../Fonts/GFXFF/FreeSansBold12pt7b.h" // FF26 or FSSB12
    #include "../Fonts/GFXFF/FreeSansBold18pt7b.h" // FF27 or FSSB18
    #include "../Fonts/GFXFF/FreeSansBold24pt7b.h" // FF28 or FSSB24

    #include "../Fonts/GFXFF/FreeSansBoldOblique9pt7b.h"  // FF29 or FSSBO9
    #include "../Fonts/GFXFF/FreeSansBoldOblique12pt7b.h" // FF30 or FSSBO12
    #include "../Fonts/GFXFF/FreeSansBoldOblique18pt7b.h" // FF31 or FSSBO18
    #include "../Fonts/GFXFF/FreeSansBoldOblique24pt7b.h" // FF32 or FSSBO24

    // Serif fonts
    #include "../Fonts/GFXFF/FreeSerif9pt7b.h"  // FF33 or FS9
    #include "../Fonts/GFXFF/FreeSerif12pt7b.h" // FF34 or FS12
    #include "../Fonts/GFXFF/FreeSerif18pt7b.h" // FF35 or FS18
    #include "../Fonts/GFXFF/FreeSerif24pt7b.h" // FF36 or FS24

    #include "../Fonts/GFXFF/FreeSerifItalic9pt7b.h"  // FF37 or FSI9
    #include "../Fonts/GFXFF/FreeSerifItalic12pt7b.h" // FF38 or FSI12
    #include "../Fonts/GFXFF/FreeSerifItalic18pt7b.h" // FF39 or FSI18
    #include "../Fonts/GFXFF/FreeSerifItalic24pt7b.h" // FF40 or FSI24

    #include "../Fonts/GFXFF/FreeSerifBold9pt7b.h"  // FF41 or FSB9
    #include "../Fonts/GFXFF/FreeSerifBold12pt7b.h" // FF42 or FSB12
    #include "../Fonts/GFXFF/FreeSerifBold18pt7b.h" // FF43 or FSB18
    #include "../Fonts/GFXFF/FreeSerifBold24pt7b.h" // FF44 or FSB24

    #include "../Fonts/GFXFF/FreeSerifBoldItalic9pt7b.h"  // FF45 or FSBI9
    #include "../Fonts/GFXFF/FreeSerifBoldItalic12pt7b.h" // FF46 or FSBI12
    #include "../Fonts/GFXFF/FreeSerifBoldItalic18pt7b.h" // FF47 or FSBI18
    #include "../Fonts/GFXFF/FreeSerifBoldItalic24pt7b.h" // FF48 or FSBI24

    // Custom fonts
    #include "../Fonts/Custom/Orbitron_Light_24.h"
    #include "../Fonts/Custom/Orbitron_Light_32.h"
    #include "../Fonts/Custom/Roboto_Thin_24.h"
    #include "../Fonts/Custom/Satisfy_24.h"
    #include "../Fonts/Custom/Yellowtail_32.h"
    #include "../Fonts/Custom/DejaVu9.h"
    #include "../Fonts/Custom/DejaVu12.h"
    #include "../Fonts/Custom/DejaVu18.h"
    #include "../Fonts/Custom/DejaVu24.h"
    #include "../Fonts/Custom/DejaVu40.h"
    #include "../Fonts/Custom/DejaVu56.h"
    #include "../Fonts/Custom/DejaVu72.h"

    #include "../Fonts/glcdfont.h"
    #include "../Fonts/Font16.h"
    #include "../Fonts/Font32rle.h"
    #include "../Fonts/Font64rle.h"
    #include "../Fonts/Font7srle.h"
    #include "../Fonts/Font72rle.h"
    #include "../Fonts/Font8x8C64.h"
    #include "../Fonts/Ascii24x48.h"
    #include "../Fonts/Ascii8x16.h"

    static constexpr uint8_t font0_info[]         = {  0, 255, 5 }; // start code, end code, width
    static constexpr uint8_t font8x8c64_info[]    = { 32, 143, 8 }; // start code, end code, width
    static constexpr uint8_t fontlib24x48_info[]  = { 32, 126, 0 }; // start code, end code

    const GLCDfont Font0 = { font      , font0_info, 6, 8, 7 };
    const BMPfont  Font2 = { chrtbl_f16, widtbl_f16, 0, chr_hgt_f16, baseline_f16 };
    const RLEfont  Font4 = { chrtbl_f32, widtbl_f32, 0, chr_hgt_f32, baseline_f32 };
    const RLEfont  Font6 = { chrtbl_f64, widtbl_f64, 0, chr_hgt_f64, baseline_f64 };
    const RLEfont  Font7 = { chrtbl_f7s, widtbl_f7s, 0, chr_hgt_f7s, baseline_f7s };
    const RLEfont  Font8 = { chrtbl_f72, widtbl_f72, 0, chr_hgt_f72, baseline_f72 };
    const GLCDfont Font8x8C64 = { font8x8_c64, font8x8c64_info, 8, 8, 7 };
    const FixedBMPfont AsciiFont8x16  = { FontLib8x16 , font0_info,  8, 16, 13 };
    const FixedBMPfont AsciiFont24x48 = { FontLib24x48, fontlib24x48_info, 24, 48, 40 };

    const U8g2font lgfxJapanMincho_8   = { lgfx_font_japan_mincho_8    };
    const U8g2font lgfxJapanMincho_12  = { lgfx_font_japan_mincho_12   };
    const U8g2font lgfxJapanMincho_16  = { lgfx_font_japan_mincho_16   };
    const U8g2font lgfxJapanMincho_20  = { lgfx_font_japan_mincho_20   };
    const U8g2font lgfxJapanMincho_24  = { lgfx_font_japan_mincho_24   };
    const U8g2font lgfxJapanMincho_28  = { lgfx_font_japan_mincho_28   };
    const U8g2font lgfxJapanMincho_32  = { lgfx_font_japan_mincho_32   };
    const U8g2font lgfxJapanMincho_36  = { lgfx_font_japan_mincho_36   };
    const U8g2font lgfxJapanMincho_40  = { lgfx_font_japan_mincho_40   };
    const U8g2font lgfxJapanMinchoP_8  = { lgfx_font_japan_mincho_p_8  };
    const U8g2font lgfxJapanMinchoP_12 = { lgfx_font_japan_mincho_p_12 };
    const U8g2font lgfxJapanMinchoP_16 = { lgfx_font_japan_mincho_p_16 };
    const U8g2font lgfxJapanMinchoP_20 = { lgfx_font_japan_mincho_p_20 };
    const U8g2font lgfxJapanMinchoP_24 = { lgfx_font_japan_mincho_p_24 };
    const U8g2font lgfxJapanMinchoP_28 = { lgfx_font_japan_mincho_p_28 };
    const U8g2font lgfxJapanMinchoP_32 = { lgfx_font_japan_mincho_p_32 };
    const U8g2font lgfxJapanMinchoP_36 = { lgfx_font_japan_mincho_p_36 };
    const U8g2font lgfxJapanMinchoP_40 = { lgfx_font_japan_mincho_p_40 };
    const U8g2font lgfxJapanGothic_8   = { lgfx_font_japan_gothic_8    };
    const U8g2font lgfxJapanGothic_12  = { lgfx_font_japan_gothic_12   };
    const U8g2font lgfxJapanGothic_16  = { lgfx_font_japan_gothic_16   };
    const U8g2font lgfxJapanGothic_20  = { lgfx_font_japan_gothic_20   };
    const U8g2font lgfxJapanGothic_24  = { lgfx_font_japan_gothic_24   };
    const U8g2font lgfxJapanGothic_28  = { lgfx_font_japan_gothic_28   };
    const U8g2font lgfxJapanGothic_32  = { lgfx_font_japan_gothic_32   };
    const U8g2font lgfxJapanGothic_36  = { lgfx_font_japan_gothic_36   };
    const U8g2font lgfxJapanGothic_40  = { lgfx_font_japan_gothic_40   };
    const U8g2font lgfxJapanGothicP_8  = { lgfx_font_japan_gothic_p_8  };
    const U8g2font lgfxJapanGothicP_12 = { lgfx_font_japan_gothic_p_12 };
    const U8g2font lgfxJapanGothicP_16 = { lgfx_font_japan_gothic_p_16 };
    const U8g2font lgfxJapanGothicP_20 = { lgfx_font_japan_gothic_p_20 };
    const U8g2font lgfxJapanGothicP_24 = { lgfx_font_japan_gothic_p_24 };
    const U8g2font lgfxJapanGothicP_28 = { lgfx_font_japan_gothic_p_28 };
    const U8g2font lgfxJapanGothicP_32 = { lgfx_font_japan_gothic_p_32 };
    const U8g2font lgfxJapanGothicP_36 = { lgfx_font_japan_gothic_p_36 };
    const U8g2font lgfxJapanGothicP_40 = { lgfx_font_japan_gothic_p_40 };

    const U8g2font efontCN_10     = { lgfx_efont_cn_10    };
    const U8g2font efontCN_10_b   = { lgfx_efont_cn_10_b  };
    const U8g2font efontCN_10_bi  = { lgfx_efont_cn_10_bi };
    const U8g2font efontCN_10_i   = { lgfx_efont_cn_10_i  };
    const U8g2font efontCN_12     = { lgfx_efont_cn_12    };
    const U8g2font efontCN_12_b   = { lgfx_efont_cn_12_b  };
    const U8g2font efontCN_12_bi  = { lgfx_efont_cn_12_bi };
    const U8g2font efontCN_12_i   = { lgfx_efont_cn_12_i  };
    const U8g2font efontCN_14     = { lgfx_efont_cn_14    };
    const U8g2font efontCN_14_b   = { lgfx_efont_cn_14_b  };
    const U8g2font efontCN_14_bi  = { lgfx_efont_cn_14_bi };
    const U8g2font efontCN_14_i   = { lgfx_efont_cn_14_i  };
    const U8g2font efontCN_16     = { lgfx_efont_cn_16    };
    const U8g2font efontCN_16_b   = { lgfx_efont_cn_16_b  };
    const U8g2font efontCN_16_bi  = { lgfx_efont_cn_16_bi };
    const U8g2font efontCN_16_i   = { lgfx_efont_cn_16_i  };
    const U8g2font efontCN_24     = { lgfx_efont_cn_24    };
    const U8g2font efontCN_24_b   = { lgfx_efont_cn_24_b  };
    const U8g2font efontCN_24_bi  = { lgfx_efont_cn_24_bi };
    const U8g2font efontCN_24_i   = { lgfx_efont_cn_24_i  };

    const U8g2font efontJA_10     = { lgfx_efont_ja_10    };
    const U8g2font efontJA_10_b   = { lgfx_efont_ja_10_b  };
    const U8g2font efontJA_10_bi  = { lgfx_efont_ja_10_bi };
    const U8g2font efontJA_10_i   = { lgfx_efont_ja_10_i  };
    const U8g2font efontJA_12     = { lgfx_efont_ja_12    };
    const U8g2font efontJA_12_b   = { lgfx_efont_ja_12_b  };
    const U8g2font efontJA_12_bi  = { lgfx_efont_ja_12_bi };
    const U8g2font efontJA_12_i   = { lgfx_efont_ja_12_i  };
    const U8g2font efontJA_14     = { lgfx_efont_ja_14    };
    const U8g2font efontJA_14_b   = { lgfx_efont_ja_14_b  };
    const U8g2font efontJA_14_bi  = { lgfx_efont_ja_14_bi };
    const U8g2font efontJA_14_i   = { lgfx_efont_ja_14_i  };
    const U8g2font efontJA_16     = { lgfx_efont_ja_16    };
    const U8g2font efontJA_16_b   = { lgfx_efont_ja_16_b  };
    const U8g2font efontJA_16_bi  = { lgfx_efont_ja_16_bi };
    const U8g2font efontJA_16_i   = { lgfx_efont_ja_16_i  };
    const U8g2font efontJA_24     = { lgfx_efont_ja_24    };
    const U8g2font efontJA_24_b   = { lgfx_efont_ja_24_b  };
    const U8g2font efontJA_24_bi  = { lgfx_efont_ja_24_bi };
    const U8g2font efontJA_24_i   = { lgfx_efont_ja_24_i  };

    const U8g2font efontKR_10     = { lgfx_efont_kr_10    };
    const U8g2font efontKR_10_b   = { lgfx_efont_kr_10_b  };
    const U8g2font efontKR_10_bi  = { lgfx_efont_kr_10_bi };
    const U8g2font efontKR_10_i   = { lgfx_efont_kr_10_i  };
    const U8g2font efontKR_12     = { lgfx_efont_kr_12    };
    const U8g2font efontKR_12_b   = { lgfx_efont_kr_12_b  };
    const U8g2font efontKR_12_bi  = { lgfx_efont_kr_12_bi };
    const U8g2font efontKR_12_i   = { lgfx_efont_kr_12_i  };
    const U8g2font efontKR_14     = { lgfx_efont_kr_14    };
    const U8g2font efontKR_14_b   = { lgfx_efont_kr_14_b  };
    const U8g2font efontKR_14_bi  = { lgfx_efont_kr_14_bi };
    const U8g2font efontKR_14_i   = { lgfx_efont_kr_14_i  };
    const U8g2font efontKR_16     = { lgfx_efont_kr_16    };
    const U8g2font efontKR_16_b   = { lgfx_efont_kr_16_b  };
    const U8g2font efontKR_16_bi  = { lgfx_efont_kr_16_bi };
    const U8g2font efontKR_16_i   = { lgfx_efont_kr_16_i  };
    const U8g2font efontKR_24     = { lgfx_efont_kr_24    };
    const U8g2font efontKR_24_b   = { lgfx_efont_kr_24_b  };
    const U8g2font efontKR_24_bi  = { lgfx_efont_kr_24_bi };
    const U8g2font efontKR_24_i   = { lgfx_efont_kr_24_i  };

    const U8g2font efontTW_10     = { lgfx_efont_tw_10    };
    const U8g2font efontTW_10_b   = { lgfx_efont_tw_10_b  };
    const U8g2font efontTW_10_bi  = { lgfx_efont_tw_10_bi };
    const U8g2font efontTW_10_i   = { lgfx_efont_tw_10_i  };
    const U8g2font efontTW_12     = { lgfx_efont_tw_12    };
    const U8g2font efontTW_12_b   = { lgfx_efont_tw_12_b  };
    const U8g2font efontTW_12_bi  = { lgfx_efont_tw_12_bi };
    const U8g2font efontTW_12_i   = { lgfx_efont_tw_12_i  };
    const U8g2font efontTW_14     = { lgfx_efont_tw_14    };
    const U8g2font efontTW_14_b   = { lgfx_efont_tw_14_b  };
    const U8g2font efontTW_14_bi  = { lgfx_efont_tw_14_bi };
    const U8g2font efontTW_14_i   = { lgfx_efont_tw_14_i  };
    const U8g2font efontTW_16     = { lgfx_efont_tw_16    };
    const U8g2font efontTW_16_b   = { lgfx_efont_tw_16_b  };
    const U8g2font efontTW_16_bi  = { lgfx_efont_tw_16_bi };
    const U8g2font efontTW_16_i   = { lgfx_efont_tw_16_i  };
    const U8g2font efontTW_24     = { lgfx_efont_tw_24    };
    const U8g2font efontTW_24_b   = { lgfx_efont_tw_24_b  };
    const U8g2font efontTW_24_bi  = { lgfx_efont_tw_24_bi };
    const U8g2font efontTW_24_i   = { lgfx_efont_tw_24_i  };
  }
 }
}

