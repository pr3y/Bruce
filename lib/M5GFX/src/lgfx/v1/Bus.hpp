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

#include "misc/enum.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct pixelcopy_t;

  enum bus_type_t
  {
    bus_unknown,
    bus_spi,
    bus_i2c,
    bus_parallel8,
    bus_parallel16,
    bus_stream,
    bus_image_push,
  };

  struct IBus
  {
    virtual ~IBus(void) = default;

    virtual bus_type_t busType(void) const = 0;

    /// ペリフェラルの準備を行う。;
    virtual bool init(void) = 0;

    /// ペリフェラルを解放する。;
    virtual void release(void) = 0;

    /// 通信速度を取得する。;
    virtual uint32_t getClock(void) const { return 0; }

    /// 受信時の通信速度を取得する。;
    virtual uint32_t getReadClock(void) const { return 0; }

    /// 通信速度を設定する。;
    virtual void setClock(uint32_t) {};

    /// 受信時の通信速度を設定する。;
    virtual void setReadClock(uint32_t) {};

    /// 通信トランザクションを開始する。（ペリフェラルを占有する）;
    virtual void beginTransaction(void) = 0;

    /// 通信トランザクションを終了する。（ペリフェラルの占有を終了する）;
    virtual void endTransaction(void) = 0;

    /// 通信が完了するのを待機する;
    virtual void wait(void) = 0;

    /// 現在通信中か否かを返す。true:通信中;
    virtual bool busy(void) const = 0;

    /// DMA転送に必要なペリフェラルの準備を行う。;
    virtual void initDMA(void) = 0;

    /// DMA転送キューを追加する。;
    virtual void addDMAQueue(const uint8_t* data, uint32_t length) = 0; // { writeBytes(data, length, true); }

    /// 蓄積したDMA転送キューの送信を実行する。;
    virtual void execDMAQueue(void) = 0;

    /// DMA用のバッファを取得する。バスの実装によっては内部的には2個のバッファを交互に使用する。;
    /// 繰返し実行した場合は前回と異なるポインタを得るが、前々回と同じになる場合がある点に注意すること。;
    virtual uint8_t* getDMABuffer(uint32_t length) = 0;

    /// 未送信のデータがあれば送信を開始する。;
    virtual void flush(void) = 0;

    /// D/Cピンをlowにしてデータを送信する。;
    virtual bool writeCommand(uint32_t data, uint_fast8_t bit_length) = 0;

    /// D/Cピンをhighにしてデータを送信する。;
    virtual void writeData(uint32_t data, uint_fast8_t bit_length) = 0;

    /// D/Cピンをhighにして指定回数繰り返しデータを送信する。;
    virtual void writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count) = 0;

    /// pixelcopy構造体を利用してピクセルデータを送信する。;
    virtual void writePixels(pixelcopy_t* pc, uint32_t length) = 0;

    /// 引数のバイト列を送信する。;
    virtual void writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma) = 0;

    virtual void beginRead(uint_fast8_t dummy_bits) { beginRead(); if (dummy_bits) { readData(dummy_bits); } }
    virtual void beginRead(void) = 0;
    virtual void endRead(void) = 0;
    virtual uint32_t readData(uint_fast8_t bit_length) = 0;
    virtual bool readBytes(uint8_t* dst, uint32_t length, bool use_dma = false) = 0;
    virtual bool readBytes(uint8_t* dst, uint32_t length, bool use_dma, bool last_nack) { (void)last_nack; return readBytes(dst, length, use_dma); }
    virtual void readPixels(void* dst, pixelcopy_t* pc, uint32_t length) = 0;
  };

  struct Bus_NULL : public IBus
  {
    bus_type_t busType(void) const override { return bus_type_t::bus_unknown; }
    bool init(void) override { return false; }
    void release(void) override {}
    void beginTransaction(void) override {}
    void endTransaction(void) override {}
    void wait(void) override {}
    bool busy(void) const override { return false; }

    void initDMA(void) override {}
    void addDMAQueue(const uint8_t*, uint32_t) override {}
    void execDMAQueue(void) override {}
    uint8_t* getDMABuffer(uint32_t) override { return nullptr; }

    void flush(void) override {}
    bool writeCommand(uint32_t, uint_fast8_t) override { return false; }
    void writeData(uint32_t, uint_fast8_t) override {}
    void writeDataRepeat(uint32_t, uint_fast8_t, uint32_t) override {}
    void writePixels(pixelcopy_t*, uint32_t) override {}
    void writeBytes(const uint8_t*, uint32_t, bool, bool) override {}

    void beginRead(void) override {}
    void endRead(void) override {}
    uint32_t readData(uint_fast8_t) override { return 0; }
    bool readBytes(uint8_t*, uint32_t, bool) override { return false; }
    void readPixels(void*, pixelcopy_t*, uint32_t) override {}
  };

  /// @brief コマンド体系を持たず、画面全体を送信するタイプのバスの基本形として使用する
  struct Bus_ImagePush : public Bus_NULL
  {
    bus_type_t busType(void) const override { return bus_type_t::bus_image_push; }
    virtual void setImageBuffer(void* buffer, color_depth_t depth) { (void)buffer; (void)depth; }
    virtual void setBrightness(uint8_t brightness) { (void)brightness; }
    virtual void setInvert(uint8_t invert) { (void)invert; }
  };

//----------------------------------------------------------------------------
 }
}
