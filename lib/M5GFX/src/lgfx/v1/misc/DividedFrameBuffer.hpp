/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include <stdint.h>
#include <stddef.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

    /// @brief メモリ領域が複数ブロックに分割されたフレームバッファ
  class DividedFrameBuffer
  {
  public:
    DividedFrameBuffer(void) : _block_array(nullptr), _line_size(0), _total_lines(0), _block_lines(0), _block_count(0) {}

    enum psram_setting_t
    {
      no_psram,
      half_psram,
      full_psram
    };

    /// @brief 初期化を実施し、メモリを割当てる
    /// @param line_size 幅方向のバイト数
    /// @param total_lines 高さ方向のライン数
    /// @param block_lines メモリブロックひとつ当たりのライン数
    /// @param use_psram ESP32でPSRAMを使用するか否か指定する
    uint8_t** create(size_t line_size, size_t total_lines, size_t block_lines, psram_setting_t use_psram = no_psram);

    /// @brief 割当済みメモリを解放する
    void release(void);

    inline size_t getLineSize(void) const { return _line_size; }
    inline size_t getTotalLines(void) const { return _total_lines; }
    inline size_t getBlockCount(void) const { return _block_count; }

    /// @brief ブロック番号を指定してバッファのポインタを取得する
    /// @param index ブロック番号
    /// @return 指定したブロックのバッファ先頭ポインタ
    inline uint8_t* getBlockBuffer(size_t index) const { return index < _block_count ? _block_array[index] : nullptr; }

    /// @brief Y座標番号を指定してバッファのポインタを取得する
    /// @param y ライン番号
    /// @return 指定したラインの先頭ポインタ (ブロックの先頭とは限らない)
    inline uint8_t* getLineBuffer(size_t y) const { return &_block_array[y / _block_lines][_line_size * (y % _block_lines)]; }

    inline bool isInitialized(void) const { return _block_array != nullptr; }

  private:
    uint8_t** _block_array;
    uint16_t _line_size;  // ラインひとつあたりのバイト数
    uint16_t _total_lines; // 全体のライン数
    uint16_t _block_lines; // メモリブロックひとつに含まれるライン数
    uint16_t _block_count; // メモリブロックの数
  };

//----------------------------------------------------------------------------
 }
}
