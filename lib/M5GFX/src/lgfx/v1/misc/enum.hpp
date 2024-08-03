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
#pragma once

#include <stdint.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  namespace jpeg_div
  {
    enum jpeg_div_t
    {
      JPEG_DIV_NONE,
      JPEG_DIV_2,
      JPEG_DIV_4,
      JPEG_DIV_8,
      JPEG_DIV_MAX
    };
  }

//----------------------------------------------------------------------------

  namespace epd_mode
  {
    enum epd_mode_t
    {
      epd_quality = 1,
      epd_text    = 2,
      epd_fast    = 3,
      epd_fastest = 4,
    };
  }
  using namespace epd_mode;

//----------------------------------------------------------------------------

  namespace colors  // Colour enumeration
  {
    #ifdef TFT_BLACK
    #undef TFT_BLACK
    #undef TFT_NAVY
    #undef TFT_DARKGREEN
    #undef TFT_DARKCYAN
    #undef TFT_MAROON
    #undef TFT_PURPLE
    #undef TFT_OLIVE
    #undef TFT_LIGHTGREY
    #undef TFT_DARKGREY
    #undef TFT_BLUE
    #undef TFT_GREEN
    #undef TFT_CYAN
    #undef TFT_RED
    #undef TFT_MAGENTA
    #undef TFT_YELLOW
    #undef TFT_WHITE
    #undef TFT_ORANGE
    #undef TFT_GREENYELLOW
    #undef TFT_PINK
    #undef TFT_BROWN
    #undef TFT_GOLD
    #undef TFT_SILVER
    #undef TFT_SKYBLUE
    #undef TFT_VIOLET
    #undef TFT_TRANSPARENT
    #endif

    static constexpr int TFT_BLACK       = 0x0000;      /*   0,   0,   0 */
    static constexpr int TFT_NAVY        = 0x000F;      /*   0,   0, 128 */
    static constexpr int TFT_DARKGREEN   = 0x03E0;      /*   0, 128,   0 */
    static constexpr int TFT_DARKCYAN    = 0x03EF;      /*   0, 128, 128 */
    static constexpr int TFT_MAROON      = 0x7800;      /* 128,   0,   0 */
    static constexpr int TFT_PURPLE      = 0x780F;      /* 128,   0, 128 */
    static constexpr int TFT_OLIVE       = 0x7BE0;      /* 128, 128,   0 */
    static constexpr int TFT_LIGHTGREY   = 0xD69A;      /* 211, 211, 211 */
    static constexpr int TFT_LIGHTGRAY   = 0xD69A;      /* 211, 211, 211 */
    static constexpr int TFT_DARKGREY    = 0x7BEF;      /* 128, 128, 128 */
    static constexpr int TFT_DARKGRAY    = 0x7BEF;      /* 128, 128, 128 */
    static constexpr int TFT_BLUE        = 0x001F;      /*   0,   0, 255 */
    static constexpr int TFT_GREEN       = 0x07E0;      /*   0, 255,   0 */
    static constexpr int TFT_CYAN        = 0x07FF;      /*   0, 255, 255 */
    static constexpr int TFT_RED         = 0xF800;      /* 255,   0,   0 */
    static constexpr int TFT_MAGENTA     = 0xF81F;      /* 255,   0, 255 */
    static constexpr int TFT_YELLOW      = 0xFFE0;      /* 255, 255,   0 */
    static constexpr int TFT_WHITE       = 0xFFFF;      /* 255, 255, 255 */
    static constexpr int TFT_ORANGE      = 0xFDA0;      /* 255, 180,   0 */
    static constexpr int TFT_GREENYELLOW = 0xB7E0;      /* 180, 255,   0 */
    static constexpr int TFT_PINK        = 0xFE19;      /* 255, 192, 203 */ //Lighter pink, was 0xFC9F
    static constexpr int TFT_BROWN       = 0x9A60;      /* 150,  75,   0 */
    static constexpr int TFT_GOLD        = 0xFEA0;      /* 255, 215,   0 */
    static constexpr int TFT_SILVER      = 0xC618;      /* 192, 192, 192 */
    static constexpr int TFT_SKYBLUE     = 0x867D;      /* 135, 206, 235 */
    static constexpr int TFT_VIOLET      = 0x915C;      /* 180,  46, 226 */
    static constexpr int TFT_TRANSPARENT = 0x0120;
  }

//----------------------------------------------------------------------------

  namespace gradient_fill_styles
  {
    enum fill_style_t
    {
      horizontal_linear = 0,
      vertical_linear   = 1,
      radial_center     = 2
    };
    static constexpr const fill_style_t HLINEAR = fill_style_t::horizontal_linear;
    static constexpr const fill_style_t VLINEAR = fill_style_t::vertical_linear;
    static constexpr const fill_style_t RADIAL  = fill_style_t::radial_center;
  }
  using namespace gradient_fill_styles;

//----------------------------------------------------------------------------

  namespace textdatum
  {
    enum textdatum_t : uint8_t
    //  0:left   1:centre   2:right
    //  0:top    4:middle   8:bottom   16:baseline
    { top_left        =  0  // Top left (default)
    , top_center      =  1  // Top center
    , top_centre      =  1  // Top center
    , top_right       =  2  // Top right
    , middle_left     =  4  // Middle left
    , middle_center   =  5  // Middle center
    , middle_centre   =  5  // Middle center
    , middle_right    =  6  // Middle right
    , bottom_left     =  8  // Bottom left
    , bottom_center   =  9  // Bottom center
    , bottom_centre   =  9  // Bottom center
    , bottom_right    = 10  // Bottom right
    , baseline_left   = 16  // Baseline left (Line the 'A' character would sit on)
    , baseline_center = 17  // Baseline center
    , baseline_centre = 17  // Baseline center
    , baseline_right  = 18  // Baseline right
    };

    #ifdef TL_DATUM
    #undef TL_DATUM
    #undef TC_DATUM
    #undef TR_DATUM
    #undef ML_DATUM
    #undef CL_DATUM
    #undef MC_DATUM
    #undef CC_DATUM
    #undef MR_DATUM
    #undef CR_DATUM
    #undef BL_DATUM
    #undef BC_DATUM
    #undef BR_DATUM
    #undef L_BASELINE
    #undef C_BASELINE
    #undef R_BASELINE
    #endif

    static constexpr textdatum_t TL_DATUM   = textdatum_t::top_left;
    static constexpr textdatum_t TC_DATUM   = textdatum_t::top_center;
    static constexpr textdatum_t TR_DATUM   = textdatum_t::top_right;
    static constexpr textdatum_t ML_DATUM   = textdatum_t::middle_left;
    static constexpr textdatum_t CL_DATUM   = textdatum_t::middle_left;
    static constexpr textdatum_t MC_DATUM   = textdatum_t::middle_center;
    static constexpr textdatum_t CC_DATUM   = textdatum_t::middle_center;
    static constexpr textdatum_t MR_DATUM   = textdatum_t::middle_right;
    static constexpr textdatum_t CR_DATUM   = textdatum_t::middle_right;
    static constexpr textdatum_t BL_DATUM   = textdatum_t::bottom_left;
    static constexpr textdatum_t BC_DATUM   = textdatum_t::bottom_center;
    static constexpr textdatum_t BR_DATUM   = textdatum_t::bottom_right;
    static constexpr textdatum_t L_BASELINE = textdatum_t::baseline_left;
    static constexpr textdatum_t C_BASELINE = textdatum_t::baseline_center;
    static constexpr textdatum_t R_BASELINE = textdatum_t::baseline_right;
  };
  using namespace textdatum;

//----------------------------------------------------------------------------

  namespace datum
  {
    using datum_t = textdatum::textdatum_t;
  }
  using namespace datum;

//----------------------------------------------------------------------------

  namespace attribute
  {
    enum attribute_t
    { cp437_switch = 1
    , utf8_switch  = 2
    , epd_mode_switch = 4
    };

    #ifdef CP437_SWITCH
    #undef CP437_SWITCH
    #undef UTF8_SWITCH
//  #undef PSRAM_ENABLE
    #endif

    static constexpr attribute_t CP437_SWITCH = attribute_t::cp437_switch;
    static constexpr attribute_t UTF8_SWITCH  = attribute_t::utf8_switch;
  }
  using namespace attribute;

//----------------------------------------------------------------------------

  enum color_depth_t : uint16_t
  {
    bit_mask     = 0x00FF   , /// ビット数取得用マスク値 (下位1Byteはビット数表現専用とする。変更しないこと);
    has_palette  = 0x0800   , /// パレット値として扱う;
    nonswapped   = 0x0100   , /// バイトスワップしていない値;
    alternate    = 0x1000   , /// ビット数が同一な色表現が複数ある場合の相違表現用;

    grayscale_1bit      =   1                         , //                            _______L
    grayscale_2bit      =   2                         , //                            ______LL
    grayscale_4bit      =   4                         , //                            ____LLLL
    grayscale_8bit      =   8 |              alternate, // ________ ________ ________ LLLLLLLL
    palette_1bit        =   1 | has_palette           , //                            _______I   2 color
    palette_2bit        =   2 | has_palette           , //                            ______II   4 color
    palette_4bit        =   4 | has_palette           , //                            ____IIII  16 color
    palette_8bit        =   8 | has_palette           , //                            IIIIIIII 256 color
    rgb332_1Byte        =   8                         , // ________ ________ ________ RRRGGGBB
    rgb565_2Byte        =  16                         , // ________ ________ GGGBBBBB RRRRRGGG
    rgb666_3Byte        =  24 |              alternate, // ________ __BBBBBB __GGGGGG __RRRRRR
    rgb888_3Byte        =  24                         , // ________ BBBBBBBB GGGGGGGG RRRRRRRR
    argb8888_4Byte      =  32                         , // BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
    rgb565_nonswapped   =  16 | nonswapped            , // ________ ________ RRRRRGGG GGGBBBBB
    rgb666_nonswapped   =  24 | nonswapped | alternate, // ________ __RRRRRR __GGGGGG __BBBBBB
    rgb888_nonswapped   =  24 | nonswapped            , // ________ RRRRRRRR GGGGGGGG BBBBBBBB
    argb8888_nonswapped =  32 | nonswapped            , // AAAAAAAA RRRRRRRR GGGGGGGG BBBBBBBB
  };

//----------------------------------------------------------------------------

  enum error_t
  { unknown_err
  , invalid_arg         // 引数が範囲外等でinvalidな場合のエラー;
  , connection_lost     // 通信が切断されたり正しく行えない場合のエラー;
  , mode_mismatch       // I2C通信が書込みモード時に読込みを指示するなど不一致な操作を行った場合のエラー;
  , periph_device_err   // ペリフェラルが動作していない等のエラー;
  };

//----------------------------------------------------------------------------
 }
}

using namespace lgfx::jpeg_div;
using namespace lgfx::colors;
using namespace lgfx::textdatum;
using namespace lgfx::datum;
using namespace lgfx::attribute;
using namespace lgfx::epd_mode;
