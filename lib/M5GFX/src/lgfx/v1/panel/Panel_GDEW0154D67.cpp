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
#include "Panel_GDEW0154D67.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/colortype.hpp"

#ifdef min
#undef min
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr uint8_t Bayer[16] = { 8, 200, 40, 232, 72, 136, 104, 168, 56, 248, 24, 216, 120, 184, 88, 152 };

  static constexpr uint8_t CMD_DEEP_SLEEP_MODE   = 0x10; // スリープの設定。スリープからの復帰にはハードウェアリセットが必要
  static constexpr uint8_t CMD_MASTER_ACTIVATION = 0x20; // 画面の描画更新を実施する
  static constexpr uint8_t CMD_DISPLAY_UPDATE_CONTROL_1 = 0x21;
  static constexpr uint8_t CMD_DISPLAY_UPDATE_CONTROL_2 = 0x22;
  static constexpr uint8_t CMD_WRITE_RAM_BW  = 0x24;
  static constexpr uint8_t CMD_WRITE_RAM_RED = 0x26;


  Panel_GDEW0154D67::Panel_GDEW0154D67(void)
  {
    _cfg.dummy_read_bits = 0;
    _epd_mode = epd_mode_t::epd_quality;
  }

  color_depth_t Panel_GDEW0154D67::setColorDepth(color_depth_t depth)
  {
    (void)depth;
    _write_depth = color_depth_t::rgb565_2Byte;
    _read_depth = color_depth_t::rgb565_2Byte;
    return color_depth_t::rgb565_2Byte;
  }

  size_t Panel_GDEW0154D67::_get_buffer_length(void) const
  {
    return ((_cfg.panel_width + 7) & ~7) * _cfg.panel_height >> 3;
  }

  bool Panel_GDEW0154D67::init(bool use_reset)
  {
    pinMode(_cfg.pin_busy, pin_mode_t::input_pullup);

    if (!Panel_HasBuffer::init(use_reset))
    {
      return false;
    }

    memset(_buf, 0xFF, _get_buffer_length());
    _after_wake();

    return true;
  }

  void Panel_GDEW0154D67::_after_wake(void)
  {
    startWrite(true);
    for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
    {
      _wait_busy();
      command_list(cmds);
    }

    _last_epd_mode = (epd_mode_t)~0u;
    _initialize_seq = true;
    _need_flip_draw = false;
    _epd_frame_back = false;

    setInvert(_invert);

    setRotation(_rotation);

    _range_old.top = 0;
    _range_old.left = 0;
    _range_old.right = _width - 1;
    _range_old.bottom = _height - 1;
    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;

    endWrite();
  }

  void Panel_GDEW0154D67::waitDisplay(void)
  {
    _wait_busy();
  }

  bool Panel_GDEW0154D67::displayBusy(void)
  {
    return _cfg.pin_busy >= 0 && gpio_in(_cfg.pin_busy);
  }

  void Panel_GDEW0154D67::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      _range_mod.left   = std::min<int16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int16_t>(_range_mod.bottom, y + h - 1);
    }
    if (_range_mod.empty()) { return; }
    auto epd_mode = getEpdMode();
    bool need_flip_draw = _need_flip_draw || (epd_mode_t::epd_quality < epd_mode && epd_mode < epd_mode_t::epd_fast);
    _need_flip_draw = false;

    bool flg_mode_changed = (_last_epd_mode != epd_mode);
/*
このEPDは内部にフレームバッファを2つ持ち、2通りの表示更新モードで使用方法が異なる。
方法 1: フレーム バッファーを 1つのみ使用し、明暗を何度か反転させて更新する。 (epd_quality)
方法 2: フレーム バッファーを 2つ交互に使用し、他方のバッファーとの差分のみを駆動する (上記以外のモード)

動作中にリフレッシュ方法を切り替えると、直後の描画が正しく行われなくなったり、表示が乱れたりする。
そのため、現在どちらのフレームバッファを使用しているかをGFX側で推測し、モード変更直後に表示が崩れないように調整する必要がある。

epd_mode == epd_quality の場合、EPDは内部の2つのフレームバッファのうち最初のものだけを使用してフリッキング更新を行う。
epd_mode != epd_quality の場合、EPDは内部の2つのフレームバッファを交互に使用し、他方との差分更新を行う。
このため、EPDが現在どちらのフレームバッファを使用しているのかを把握しておく必要がある。

★epd_quality から他のモードに変更した場合
 必ずフレームバッファ1番にデータが送信されるが、フレームバッファ2番の状態が不定のため正しく描画できない。
 そのためフレームバッファ1番に反転した描画を行い、直後にもう一度通常の描画を行う必要がある。

★epd_quality以外のモードからepd_qualityモードに変更した場合
 どちらのフレームバッファに送信されるかは直前の状況による。
 フレームバッファ1番に送信される場合は何もしなくてよい。
 フレームバッファ2番に送信される場合は、モード変更前の状態で一度 描画更新をしておく必要がある。

リセット直後 (_initialize_seq == true) の場合は CMD_DISPLAY_UPDATE_CONTROL_2 を使ってEPDの起動処理が必要
epd_mode が epd_qualityか否かの変化をした場合も同様にCMD_DISPLAY_UPDATE_CONTROL_2を使ってEPDのモード変更処理が必要

*/
    if (_initialize_seq || flg_mode_changed)
    {
    // CMD_DISPLAY_UPDATE_CONTROL_2 parameter
    // 0b10000000 = Enable Clock signal
    // 0b01000000 = Enable Analog
    // 0b00100000 = Load temperature value
    // 0b00010000 = Load LUT with DISPLAY Mode 1
    // 0b00011000 = Load LUT with DISPLAY Mode 2
    // 0b00000100 = Display with DISPLAY Mode 1
    // 0b00001100 = Display with DISPLAY Mode 2
    // 0b00000010 = Disable Analog
    // 0b00000001 = Disable clock signal
    // epd_quality高品質モードではフリッキング更新を行う
      _range_mod.left = 0;
      _range_mod.right = _width - 1;
      _range_mod.top = 0;
      _range_mod.bottom = _height - 1;

      if (_initialize_seq) {
        _initialize_seq = false;
        // リセット直後は起動シーケンス設定およびフレームバッファの転送を行う。ここではリフレッシュは行わない。
        _bus->writeCommand(CMD_DISPLAY_UPDATE_CONTROL_2, 8);
        _bus->writeData(0xF8, 8);
        _exec_transfer(CMD_WRITE_RAM_BW, _range_mod, true);
        _bus->writeCommand(CMD_MASTER_ACTIVATION, 8);
        _send_msec = millis();
      }

      // epd_qualityの場合は反転描画は不要になる。
      // 他のモードに変更した直後は反転描画を行う。
      need_flip_draw = (epd_mode != epd_mode_t::epd_quality);
      _epd_frame_switching = need_flip_draw;
      if (!need_flip_draw)
      {
        if (_epd_frame_back)
        {  // フレームバッファ2番に送信される場合はモード変更前に一度描画更新を行う
          _epd_frame_back = false;
          _exec_transfer(CMD_WRITE_RAM_BW, _range_mod);
          _bus->writeCommand(CMD_MASTER_ACTIVATION, 8); // Active Display update
          _send_msec = millis();
        }
      }
      _wait_busy();
      _bus->writeCommand(CMD_DISPLAY_UPDATE_CONTROL_2, 8); // Display update seq opt
      uint8_t refresh_param = (epd_mode == epd_mode_t::epd_quality)
                          ? 0x14   // DISPLAY Mode1 (flicking)
                          : 0x1C;  // DISPLAY Mode2 (no flick)
      _bus->writeData(refresh_param, 8);
      _last_epd_mode = epd_mode;
    }
    range_rect_t tr = _range_mod;
    if (tr.top > _range_old.top) { tr.top = _range_old.top; }
    if (tr.left > _range_old.left) { tr.left = _range_old.left; }
    if (tr.right < _range_old.right) { tr.right = _range_old.right; }
    if (tr.bottom < _range_old.bottom) { tr.bottom = _range_old.bottom; }
    _range_old = _range_mod;

    _exec_transfer(CMD_WRITE_RAM_BW, tr, need_flip_draw);
    _bus->writeCommand(CMD_MASTER_ACTIVATION, 8); // Active Display update
    _send_msec = millis();
    if (need_flip_draw)
    { // 反転リフレッシュを自前でやる場合
      _exec_transfer(CMD_WRITE_RAM_BW, tr);
      _bus->writeCommand(CMD_MASTER_ACTIVATION, 8); // Active Display update
      _send_msec = millis();
    } else {
      if (_epd_frame_switching) { _epd_frame_back = !_epd_frame_back; }
      else { _epd_frame_back = false; }
    }

    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;
  }

  void Panel_GDEW0154D67::setInvert(bool invert)
  {
    if (_invert == invert && !_initialize_seq) { return; }
    _invert = invert;
    startWrite();
    _wait_busy();
    _bus->writeCommand(CMD_DISPLAY_UPDATE_CONTROL_1, 8);
    _bus->writeData((invert ^ _cfg.invert) ? 0x88 : 0x00, 8);
    _need_flip_draw = true;
    _range_mod.top = 0;
    _range_mod.left = 0;
    _range_mod.right = _width - 1;
    _range_mod.bottom = _height - 1;
    endWrite();
  }

  void Panel_GDEW0154D67::setSleep(bool flg)
  {
    if (flg)
    {
      startWrite();
      _wait_busy();
      _bus->writeCommand(CMD_DISPLAY_UPDATE_CONTROL_2, 8);
      _bus->writeData(0x03, 8); // Disable Analog , Disable clock signal
      _bus->writeCommand(CMD_DEEP_SLEEP_MODE, 8);
      _bus->writeData(0x03, 8);
      endWrite();
    }
    else
    {
      rst_control(false);
      delay(10);
      rst_control(true);
      delay(10);
      _after_wake();
    }
  }

  void Panel_GDEW0154D67::setPowerSave(bool flg)
  {
    startWrite();
    _wait_busy();
    _bus->writeCommand(CMD_DISPLAY_UPDATE_CONTROL_2, 8);
    _bus->writeData(flg ? 0x03 : 0xE0, 8);
    _wait_busy();
    _bus->writeCommand(CMD_MASTER_ACTIVATION, 8);
    endWrite();
  }

  void Panel_GDEW0154D67::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    uint_fast16_t xs = x, xe = x + w - 1;
    uint_fast16_t ys = y, ye = y + h - 1;
    _xs = xs;
    _ys = ys;
    _xe = xe;
    _ye = ye;
    _update_transferred_rect(xs, ys, xe, ye);

    swap565_t color;
    color.raw = rawcolor;
    uint32_t value = (color.R8() + (color.G8() << 1) + color.B8()) >> 2;

    y = ys;
    do
    {
      x = xs;
      uint32_t idx = ((_cfg.panel_width + 7) & ~7) * y + x;
      auto btbl = &Bayer[(y & 3) << 2];
      do
      {
        bool flg = 256 <= value + btbl[x & 3];
        if (flg) _buf[idx >> 3] |=   0x80 >> (idx & 7);
        else     _buf[idx >> 3] &= ~(0x80 >> (idx & 7));
        ++idx;
      } while (++x <= xe);
    } while (++y <= ye);
  }

  void Panel_GDEW0154D67::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    uint_fast16_t xs = x, xe = x + w - 1;
    uint_fast16_t ys = y, ye = y + h - 1;
    _update_transferred_rect(xs, ys, xe, ye);

    auto readbuf = (swap565_t*)alloca(w * sizeof(swap565_t));
    auto sx = param->src_x32;
    h += y;
    do
    {
      uint32_t prev_pos = 0, new_pos = 0;
      do
      {
        new_pos = param->fp_copy(readbuf, prev_pos, w, param);
        if (new_pos != prev_pos)
        {
          do
          {
            auto color = readbuf[prev_pos];
            _draw_pixel(x + prev_pos, y, (color.R8() + (color.G8() << 1) + color.B8()) >> 2);
          } while (new_pos != ++prev_pos);
        }
      } while (w != new_pos && w != (prev_pos = param->fp_skip(new_pos, w, param)));
      param->src_x32 = sx;
      param->src_y++;
    } while (++y < h);
  }

  void Panel_GDEW0154D67::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    {
      uint_fast16_t xs = _xs;
      uint_fast16_t xe = _xe;
      uint_fast16_t ys = _ys;
      uint_fast16_t ye = _ye;
      _update_transferred_rect(xs, ys, xe, ye);
    }
    uint_fast16_t xs   = _xs  ;
    uint_fast16_t ys   = _ys  ;
    uint_fast16_t xe   = _xe  ;
    uint_fast16_t ye   = _ye  ;
    uint_fast16_t xpos = _xpos;
    uint_fast16_t ypos = _ypos;

    static constexpr uint32_t buflen = 16;
    swap565_t colors[buflen];
    int bufpos = buflen;
    do
    {
      if (bufpos == buflen) {
        param->fp_copy(colors, 0, std::min(length, buflen), param);
        bufpos = 0;
      }
      auto color = colors[bufpos++];
      _draw_pixel(xpos, ypos, (color.R8() + (color.G8() << 1) + color.B8()) >> 2);
      if (++xpos > xe)
      {
        xpos = xs;
        if (++ypos > ye)
        {
          ypos = ys;
        }
      }
    } while (--length);
    _xpos = xpos;
    _ypos = ypos;
  }

  void Panel_GDEW0154D67::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    auto readbuf = (swap565_t*)alloca(w * sizeof(swap565_t));
    param->src_data = readbuf;
    int32_t readpos = 0;
    h += y;
    do
    {
      uint32_t idx = 0;
      do
      {
        readbuf[idx] = _read_pixel(x + idx, y) ? -1 : 0;
      } while (++idx != w);
      param->src_x32 = 0;
      readpos = param->fp_copy(dst, readpos, readpos + w, param);
    } while (++y < h);
  }

  bool Panel_GDEW0154D67::_wait_busy(uint32_t timeout)
  {
    _bus->wait();
    if (_cfg.pin_busy >= 0 && gpio_in(_cfg.pin_busy))
    {
      uint32_t start_time = millis();
      uint32_t delay_msec = _refresh_msec - (start_time - _send_msec);
      if (delay_msec && delay_msec < timeout) { delay(delay_msec); }
      do
      {
        if (millis() - start_time > timeout) {
// printf("TIMEOUT\n");
          return false;
        }
        delay(1);
      } while (gpio_in(_cfg.pin_busy));
// printf("time:%d\n", millis() - start_time);
    }
    return true;
  }

  void Panel_GDEW0154D67::_draw_pixel(uint_fast16_t x, uint_fast16_t y, uint32_t value)
  {
    _rotate_pos(x, y);
    uint32_t idx = ((_cfg.panel_width + 7) & ~7) * y + x;
    bool flg = 256 <= value + Bayer[(x & 3) | (y & 3) << 2];
    if (flg) _buf[idx >> 3] |=   0x80 >> (idx & 7);
    else     _buf[idx >> 3] &= ~(0x80 >> (idx & 7));
  }

  bool Panel_GDEW0154D67::_read_pixel(uint_fast16_t x, uint_fast16_t y)
  {
    _rotate_pos(x, y);
    uint32_t idx = ((_cfg.panel_width + 7) & ~7) * y + x;
    return _buf[idx >> 3] & (0x80 >> (idx & 7));
  }

  void Panel_GDEW0154D67::_exec_transfer(uint32_t cmd, const range_rect_t& range, bool invert)
  {
    int32_t xs = range.left & ~7;
    int32_t xe = range.right & ~7;

    _wait_busy();

    uint32_t data_tmp = (xs>>3) | (xe>>3)<<8;
    _bus->writeCommand(0x44, 8);
    _bus->writeData(data_tmp, 16);
    _bus->writeCommand(0x4E, 8);
    _bus->writeData(data_tmp, 8);

    data_tmp = range.top | range.bottom << 16;
    _bus->writeCommand(0x45, 8);
    _bus->writeData(data_tmp, 32);
    _bus->writeCommand(0x4F, 8);
    _bus->writeData(data_tmp, 16);

    _wait_busy();

    _bus->writeCommand(cmd, 8);
    int32_t y = range.top;
    int32_t h = range.bottom - y + 1;
    int32_t add = ((_cfg.panel_width + 7) & ~7) >> 3;
    auto b = &_buf[(xs >> 3) + (y * add)];
    int32_t w = ((xe - xs) >> 3) + 1;

    if (invert)
    {
      do
      {
        int32_t i = 0;
        do
        {
          _bus->writeData(~b[i], 8);
        } while (++i != w);
        b += add;
      } while (--h);
    }
    else
//*/
    {
      if (add == w) {
        _bus->writeBytes(b, w * h, true, true);
      }
      else
      {
        do
        {
          _bus->writeBytes(b, w, true, true);
          b += add;
        } while (--h);
      }
    }
  }

  void Panel_GDEW0154D67::_update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye)
  {
    _rotate_pos(xs, ys, xe, ye);

    int32_t x1 = xs & ~7;
    int32_t x2 = (xe & ~7) + 7;

    _range_mod.top    = std::min<int32_t>(ys, _range_mod.top);
    _range_mod.left   = std::min<int32_t>(x1, _range_mod.left);
    _range_mod.right  = std::max<int32_t>(x2, _range_mod.right);
    _range_mod.bottom = std::max<int32_t>(ye, _range_mod.bottom);
  }

//----------------------------------------------------------------------------
 }
}
