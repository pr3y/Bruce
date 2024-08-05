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

#include "../Panel.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------
  struct IBus;
  struct ILight;
  struct ITouch;
  struct touch_point_t;

  struct Panel_Device : public IPanel
  {
  public:
    Panel_Device(void);

    struct config_t
    {
      /// CS ピン番号;
      /// Number of CS pin
      int16_t pin_cs = -1;

      /// RST ピン番号;
      /// Number of RST pin
      int16_t pin_rst = -1;

      /// BUSY ピン番号;
      /// Number of BUSY pin
      int16_t pin_busy = -1;

      /// LCDドライバが扱える画像の最大幅;
      /// The maximum width of an image that the LCD driver can handle.
      uint16_t memory_width = 240;

      /// LCDドライバが扱える画像の最大高さ;
      /// The maximum height of an image that the LCD driver can handle.
      uint16_t memory_height = 240;

      /// 実際に表示できる幅;
      /// Actual width of the display.
      uint16_t panel_width = 240;

      /// 実際に表示できる高さ;
      /// Actual height of the display.
      uint16_t panel_height = 240;

      /// パネルのX方向オフセット量;
      /// Number of offset pixels in the X direction.
      uint16_t offset_x = 0;

      /// パネルのY方向オフセット量;
      /// Number of offset pixels in the Y direction.
      uint16_t offset_y = 0;

      /// 回転方向のオフセット 0~7 (4~7は上下反転);
      /// Offset value in the direction of rotation. 0~7 (4~7 is upside down)
      uint8_t offset_rotation = 0;

      /// ピクセル読出し前のダミーリードのビット数;
      /// Number of bits in dummy read before pixel readout.
      uint8_t dummy_read_pixel = 8;

      /// データ読出し前のダミーリードのビット数;
      /// Number of bits in dummy read before data readout.
      uint8_t dummy_read_bits = 1;

      /// データ読出し終了時のウェイト(ST7796で必要);
      uint16_t end_read_delay_us = 0;

      /// データ読出しが可能か否か;
      /// Whether the data is readable or not.
      bool readable = true;

      /// 明暗の反転 (IPSパネルはtrueに設定);
      /// brightness inversion (e.g. IPS panel)
      bool invert = false;

      /// RGB=true / BGR=false パネルの赤と青が入れ替わってしまう場合 trueに設定;
      /// Set the RGB/BGR color order.
      bool rgb_order = false;

      /// 送信データの16bitアライメント データ長を16bit単位で送信するパネルの場合 trueに設定;
      /// 16-bit alignment of transmitted data
      bool dlen_16bit = false;

      /// SD等のファイルシステムとのバス共有の有無 (trueに設定するとdrawJpgFile等でバス制御が行われる);
      /// Whether or not to share the bus with the file system (if set to true, drawJpgFile etc. will control the bus)
      bool bus_shared = true;
    };

    const config_t& config(void) const { return _cfg; }
    void config(const config_t& cfg) { _cfg = cfg; }

    virtual bool init(bool use_reset);
    virtual bool initTouch(void);

    virtual void initBus(void);
    virtual void releaseBus(void);
    void setBus(IBus* bus);
    void bus(IBus* bus) { setBus(bus); };
    IBus* getBus(void) const { return _bus; }
    IBus* bus(void) const { return _bus; }

    void setLight(ILight* light) { _light = light; }
    void light(ILight* light) { _light = light; }
    ILight* getLight(void) const { return _light; }
    ILight* light(void) const { return _light; }
    void setBrightness(uint8_t brightness) override;


    void setTouch(ITouch* touch);
    void touch(ITouch* touch) { setTouch(touch); }
    ITouch* getTouch(void) const { return _touch; }
    ITouch* touch(void) const { return _touch; }
    virtual uint_fast8_t getTouchRaw(touch_point_t* tp, uint_fast8_t count);
    uint_fast8_t getTouch(touch_point_t* tp, uint_fast8_t count);
    void convertRawXY(touch_point_t *tp, uint_fast8_t count);
    void touchCalibrate(void);
    void setCalibrateAffine(float affine[6]);
    void setCalibrate(uint16_t *parameters);


    bool isReadable(void) const override { return _cfg.readable; }
    bool isBusShared(void) const override { return _cfg.bus_shared; }

    void initDMA(void) override;
    void waitDMA(void) override;
    bool dmaBusy(void) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

    void writeCommand(uint32_t data, uint_fast8_t length) override;
    void writeData(uint32_t data, uint_fast8_t length) override;
    //void writePixelsDMA(const uint8_t* data, uint32_t length) override;
    void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;

  protected:

    static constexpr uint8_t CMD_INIT_DELAY = 0x80;

    config_t _cfg;

    IBus* _bus = nullptr;
    ILight* _light = nullptr;
    ITouch* _touch = nullptr;
    bool _has_align_data = false;
    uint8_t _internal_rotation = 0;

    float _affine[6] = {1,0,0,0,1,0};  /// touch affine parameter

    /// CSピンの準備処理を行う。CSピンを自前で制御する場合、この関数をoverrideして実装すること。;
    /// Performs preparation processing for the CS pin.
    /// If you want to control the CS pin on your own, override this function and implement it.
    virtual void init_cs(void);

    /// 引数に応じてCSピンを制御する。false=LOW / true=HIGH。CSピンを自前で制御する場合、この関数をoverrideして実装すること。;
    /// Controls the CS pin to go HIGH when the argument is true.
    /// If you want to control the CS pin on your own, override this function and implement it.
    virtual void cs_control(bool level);

    /// RSTピンの準備処理を行う。RSTピンを自前で制御する場合、この関数をoverrideして実装すること。;
    /// Performs preparation processing for the RST pin.
    /// If you want to control the RST pin on your own, override this function and implement it.
    virtual void init_rst(void);

    /// 引数に応じてRSTピンを制御する。false=LOW / true=HIGH。RSTピンを自前で制御する場合、この関数をoverrideして実装すること。;
    /// Controls the RST pin to go HIGH when the argument is true.
    /// If you want to control the RST pin on your own, override this function and implement it.
    virtual void rst_control(bool level);

    /// パネルの初期化コマンド列を得る。無い場合はnullptrを返す。;
    /// Get the panel initialization command sequence.
    virtual const uint8_t* getInitCommands(uint8_t listno) const { (void)listno; return nullptr; }

    enum fastread_dir_t
    {
      fastread_nothing,
      fastread_horizontal,
      fastread_vertical,
    };
    virtual fastread_dir_t get_fastread_dir(void) const { return fastread_nothing; }

    void command_list(const uint8_t *addr);

  };

//----------------------------------------------------------------------------

  struct Panel_NULL : public Panel_Device
  {
    Panel_NULL(void) = default;

    void initBus(void) override {}
    void releaseBus(void) override {}

    bool init(bool) override { return false; }

    void beginTransaction(void) override {}
    void endTransaction(void) override {}

    color_depth_t setColorDepth(color_depth_t depth) override { return depth; }

    void setInvert(bool) override {}
    void setRotation(uint_fast8_t) override {}
    void setSleep(bool) override {}
    void setPowerSave(bool) override {}

    void writeCommand(uint32_t, uint_fast8_t) override {}
    void writeData(uint32_t, uint_fast8_t) override {}

    void initDMA(void) override {}
    void waitDMA(void) override {}
    bool dmaBusy(void) override { return false; }
    void waitDisplay(void) override {}
    bool displayBusy(void) override { return false; }
    void display(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t) override {}
    bool isReadable(void) const override { return false; }
    bool isBusShared(void) const override { return false; }

    void writeBlock(uint32_t, uint32_t) override {}
    void setWindow(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t) override {}
    void drawPixelPreclipped(uint_fast16_t, uint_fast16_t, uint32_t) override {}
    void writeFillRectPreclipped(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, uint32_t) override {}
    void writeImage(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, pixelcopy_t*, bool) override {}
    void writeImageARGB(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, pixelcopy_t*) override {}
    void writePixels(pixelcopy_t*, uint32_t, bool) override {}

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }
    void readRect(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, void*, pixelcopy_t*) override {}
    void copyRect(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t) override {}
  };

//----------------------------------------------------------------------------
 }
}
