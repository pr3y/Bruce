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

#include "DataWrapper.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct bitmap_header_t
  {
#pragma pack(push)
#pragma pack(1)
    union
    {
      uint8_t raw[54];
      struct {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;

        uint32_t biSize;
        int32_t  biWidth;
        int32_t  biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t  biXPelsPerMeter;
        int32_t  biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
      };
    };
#pragma pack(pop)

    bool load_bmp_header(DataWrapper* data)
    {
      data->read((uint8_t*)this, sizeof(bitmap_header_t));
      return ( (bfType == 0x4D42)   // bmp header "BM"
            && (biPlanes == 1)  // bcPlanes always 1
            && (biWidth > 0)
            && (biHeight != 0)
            && (biBitCount <= 32)
            && (biBitCount != 0));
    }

    static bool load_bmp_rle8(DataWrapper* data, uint8_t* linebuf, uint_fast16_t width)
    {
      width = (width + 3) & ~3;
      uint8_t code[2];
      uint_fast16_t xidx = 0;
      bool eol = false;
      do {
        data->read(code, 2);
        if (code[0] == 0) {
          switch (code[1]) {
          case 0x00: // EOL
          case 0x01: // EOB
            eol = true;
            break;

          case 0x02: // move info  (not support)
            return false;

          default:
            data->read(&linebuf[xidx], (code[1] + 1) & ~1); // word align
            xidx += code[1];
            break;
          }
        } else if (xidx + code[0] <= width) {
          memset(&linebuf[xidx], code[1], code[0]);
          xidx += code[0];
        } else {
          return false;
        }
      } while (!eol);
      return true;
    }

    static bool load_bmp_rle4(DataWrapper* data, uint8_t* linebuf, uint_fast16_t width)
    {
      width = (width + 3) & ~3;
      uint8_t code[2];
      uint_fast16_t xidx = 0;
      bool eol = false;
      do {
        data->read(code, 2);
        if (code[0] == 0) {
          switch (code[1]) {
          case 0x00: // EOL
          case 0x01: // EOB
            eol = true;
            break;

          case 0x02: // move info  (not support)
            return false;

          default:  // 絶対モードデータ;
            {
              int_fast16_t len = code[1];
              int_fast16_t dbyte = ((int_fast16_t)code[1] + 1) >> 1;

              data->read(&linebuf[(xidx + 1) >> 1], (dbyte + 1) & ~1); // word align
              if (xidx & 1) {
                linebuf[xidx >> 1] |= linebuf[(xidx >> 1) + 1] >> 4;
                for (long i = 1; i < dbyte; ++i) {
                  linebuf[((xidx + i) >> 1)] = (linebuf[((xidx + i) >> 1)    ] << 4)
                                              |  linebuf[((xidx + i) >> 1) + 1] >> 4;
                }
              }
              xidx += len;
            }
            break;
          }
        } else if (xidx + code[0] <= width) {
          if (xidx & 1) {
            linebuf[xidx >> 1] |= code[1] >> 4;
            code[1] = (code[1] >> 4) | (code[1] << 4);
          }
          memset(&linebuf[(xidx + 1) >> 1], code[1], (code[0] + 1) >> 1);
          xidx += code[0];
          if (xidx & 1) linebuf[xidx >> 1] &= 0xF0;
        } else {
          return false;
        }
      } while (!eol);
      return true;
    }
  };

//----------------------------------------------------------------------------
 }
}
