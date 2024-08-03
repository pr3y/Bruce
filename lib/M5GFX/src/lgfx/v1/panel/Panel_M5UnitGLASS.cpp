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
#include "Panel_M5UnitGLASS.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/colortype.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  bool Panel_M5UnitGlass::init(bool use_reset)
  {
    if (!Panel_HasBuffer::init(use_reset))
    {
      return false;
    }

    bool connected = false;
    uint8_t res = 0xFFu;
    int retry = 4;
    waitBusy();
    do {
      _bus->beginTransaction();
      if (_bus->writeCommand(REG_INDEX_FIRMWARE_VERSION, 8))
      {
        _bus->endTransaction();
        _bus->beginRead();
        connected = _bus->readBytes(&res, 1, false, true);
      }
      _bus->endTransaction();
    } while (!connected && --retry >= 0);

    if (!connected) { return false; }

    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;

    setInvert(_invert);
    setRotation(_rotation);

    return true;
  }

  void Panel_M5UnitGlass::setSleep(bool flg)
  {
    waitBusy();
    _bus->beginTransaction();
    _bus->writeCommand(REG_INDEX_DISPLAY_ON_OFF | flg << 8, 16);
    _bus->endTransaction();
  }

  void Panel_M5UnitGlass::setInvert(bool invert)
  {
    waitBusy();
    int retry = 4;
    do {
      _bus->beginTransaction();
      if (_bus->writeCommand(REG_INDEX_COLOR_INVERT | invert << 8, 16)) {
        _invert = invert;
        retry = 1;
      }
      _bus->endTransaction();
    } while (--retry);
  }

  void Panel_M5UnitGlass::display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h)
  {
    if (0 < w && 0 < h)
    {
      _range_mod.left   = std::min<int_fast16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int_fast16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int_fast16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int_fast16_t>(_range_mod.bottom, y + h - 1);
    }
    if (_range_mod.empty()) { return; }

    uint_fast8_t xs = _range_mod.left;
    uint_fast8_t xe = _range_mod.right;
    uint_fast8_t ys = _range_mod.top    >> 3;
    uint_fast8_t ye = _range_mod.bottom >> 3;

    _range_mod.top    = INT16_MAX;
    _range_mod.left   = INT16_MAX;
    _range_mod.right  = 0;
    _range_mod.bottom = 0;

    y = ys;
    waitBusy();
    do
    {
      x = xs;
      uint_fast8_t x_start = x;
      int index = 0;
      auto buf = &_buf[y * _cfg.panel_width];
      do
      {
        if (index == 0) {
          while (0 == (_modified_flags[y] & 0x8000 >> (x >> 3)))
          {
            x = 8 + (x & ~7u);
            if (x > xe) { break; }
          }
          x_start = x;
        }
        if (x <= xe) {
          _bus->beginTransaction();
          _bus->writeCommand(REG_INDEX_PICTURE_BUFFER | index << 8 | buf[x] << 24 , 32);
          _bus->endTransaction();

          ++index;
          ++x;

          bool flg_draw_picture = (x > xe);
          if (!flg_draw_picture && 0 == (x & 7)) {
            flg_draw_picture = (0 == (_modified_flags[y] & 0x8000 >> (x >> 3)));
          }
          if (flg_draw_picture) {
            _bus->beginTransaction();
            _bus->writeCommand(REG_INDEX_DRAW_PICTURE | x_start << 8 | y << (3 + 16), 24);
            _bus->writeCommand(index | 1 << (3+8) | 1 << 16, 24);
            _bus->endTransaction();
            lgfx::delayMicroseconds(index << 5); // 描画が終わるまで少し待つ
            index = 0;
          }
        }
      } while (x <= xe);
      _modified_flags[y] = 0;
    } while (++y <= ye);

    _bus->beginTransaction();
    _bus->writeCommand(REG_INDEX_SHOW | 0x100, 16);
    _bus->endTransaction();
    _msec_busy = lgfx::millis() + 5;
  }

  uint8_t Panel_M5UnitGlass::getKey(uint_fast8_t idx)
  {
    uint8_t res = 1;
    int retry = 4;
    waitBusy();
    do {
      _bus->beginTransaction();
      if (_bus->writeCommand(REG_INDEX_READ_KEY + (idx & 1), 8))
      {
        _bus->endTransaction();
        lgfx::delayMicroseconds(512); // データ取得可能になるまで少し待つ
        _bus->beginRead();
        if (_bus->readBytes(&res, 1, false, true)) { retry = 0; }
      }
      _bus->endTransaction();
    } while (--retry >= 0);
    _msec_busy = lgfx::millis() + 1;
    return res;
  }

  uint8_t Panel_M5UnitGlass::getFirmwareVersion(void)
  {
    uint8_t res = 1;
    int retry = 4;
    waitBusy();
    do {
      _bus->beginTransaction();
      if (_bus->writeCommand(REG_INDEX_FIRMWARE_VERSION, 8))
      {
        _bus->endTransaction();
        _bus->beginRead();
        if (_bus->readBytes(&res, 1, false, true)) { retry = 0; }
      }
      _bus->endTransaction();
    } while (--retry >= 0);
    _msec_busy = lgfx::millis() + 1;
    return res;
  }

  void Panel_M5UnitGlass::setBuzzer(uint16_t freq, uint8_t duty) {
    waitBusy();
    _bus->beginTransaction();
    _bus->writeCommand(REG_INDEX_BUZZER, 8);
    _bus->writeCommand(freq | duty << 16 | _enable_buzzer_flg << 24, 32);
    _bus->endTransaction();
    _msec_busy = lgfx::millis() + 1;
  }

  void Panel_M5UnitGlass::setBuzzerEnable(bool enable) {
    _enable_buzzer_flg = enable;
    waitBusy();
    _bus->beginTransaction();
    _bus->writeCommand((REG_INDEX_BUZZER + 3) | enable << 8, 16);
    _bus->endTransaction();
    _msec_busy = lgfx::millis() + 1;
  }

  void Panel_M5UnitGlass::waitBusy(void)
  {
    uint32_t diff = _msec_busy - lgfx::millis();
    if (diff < 16) {
      lgfx::delay(diff);
    }
  }

  void Panel_M5UnitGlass::_update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye)
  {
    Panel_1bitOLED::_update_transferred_rect(xs, ys, xe, ye);
    int y = ys >> 3;
    int y_end = ye >> 3;

    uint_fast8_t msb = (xs >> 3);
    uint_fast8_t lsb = (xe >> 3);
    uint_fast16_t bitmask = (0xFFFF >> msb) ^ (0x7FFFu >> lsb);
    do
    {
      _modified_flags[y] |= bitmask;
    } while (++y <= y_end);
  }

//----------------------------------------------------------------------------
 }
}
