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

#include "Panel_Device.hpp"
#include "../platforms/common.hpp"
#include "../platforms/device.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_M5HDMI : public Panel_Device
  {
  public:
    struct video_timing_t
    {
      struct info_t
      {
        uint16_t sync;
        uint16_t back_porch;
        uint16_t active;
        uint16_t front_porch;
      };
      info_t v;
      info_t h;
    };

    // ピクセルクロックの設定用構造体
    struct video_clock_t
    {
      // ピクセルクロックの求め方
      // (入力クロック74.25MHz) * feedback_divider / input_divider = pixel clock (出力クロック)

      // 制約: 値の範囲は 1<= input_divider <= 24 であること
      uint8_t input_divider = 2;

      // 制約: 値の範囲は 1<= feedback_divider <= 64 であること
      uint8_t feedback_divider = 2;

      // 制約: output_divider は次のいずれかであること。 2,4,8,16,32, 48, 64, 80, 96, 112, 128
      uint8_t output_divider = 8;

      // また、 pixel_clock * output_divider = vco_clock
      // 制約: 400MHz <= vco_clock <= 1200MHz であること。800MHzが理想値。

      // ビデオ処理回路の駆動にピクセルクロックの半分を使うかどうか。デフォルトはfalse
      // Full HD @ 60Hz (ピクセルクロック = 148.5MHz) の映像信号のみを受け付ける Xreal Airのような機器に対して、
      // FPGA側ビデオ出力は半分のクロックで動いており、2ピクセルクロック分同じデータを出力していることにして対応するために使う。
      bool use_half_clock = false;
    };

    Panel_M5HDMI(void)
    {
      _cfg.memory_width  = _cfg.panel_width = 1280;
      _cfg.memory_height = _cfg.panel_height = 720;
      _cfg.dummy_read_pixel =     0;
      _cfg.dummy_read_bits  =     0;
      _cfg.readable         =  true;
      _cfg.invert           = false;
      _cfg.rgb_order        = false;
      _cfg.dlen_16bit       = false;
      _cfg.bus_shared       =  true;
    }

    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    using Panel_Device::config;

    color_depth_t setColorDepth(color_depth_t depth) override;
    void setRotation(uint_fast8_t r) override;
    void setBrightness(uint8_t brightness) override;
    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool flg) override;

    void waitDisplay(void) override;
    bool displayBusy(void) override;
    void display(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t) override {}

    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;
    void writeBlock(uint32_t rawcolor, uint32_t len) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeFillRectAlphaPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t argb8888) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }
    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;

    struct config_resolution_t
    {
      uint16_t logical_width  = 0;
      uint16_t logical_height = 0;
      float refresh_rate      = 0.0f;
      uint16_t output_width   = 0;
      uint16_t output_height  = 0;
      uint8_t scale_w         = 0;
      uint8_t scale_h         = 0;
      uint32_t pixel_clock    = 74250000;
    };

    void config_resolution( const config_resolution_t& cfg_resolution );
    bool setResolution( const config_resolution_t& cfg_resolution );
    bool setResolution( uint16_t logical_width  = 0
                      , uint16_t logical_height = 0
                      , float refresh_rate      = 0
                      , uint16_t output_width   = 0
                      , uint16_t output_height  = 0
                      , uint8_t scale_w         = 0
                      , uint8_t scale_h         = 0
                      , uint32_t pixel_clock    = 74250000
                      );

    size_t readEDID(uint8_t* EDID, size_t len);
    void setVideoTiming(const video_timing_t* param);
    void setScaling(uint_fast8_t x_scale, uint_fast8_t y_scale);
    void setViewPort(uint_fast16_t x, uint_fast16_t y);

    static constexpr uint8_t CMD_NOP          = 0x00; // 1Byte 何もしない;
    static constexpr uint8_t CMD_READ_ID      = 0x04; // 1Byte ID読出し  スレーブからの回答は4Byte ([0]=0x48 [1]=0x44 [2]=メジャーバージョン [3]=マイナーバージョン);

    static constexpr uint8_t CMD_SCREEN_SCALING=0x18; // 8Byte 表示倍率設定 [1]=横倍率 [2]=縦倍率 [3~4]=横論理解像度 [5~6]=縦論理解像度 [7]=チェックサム ( ~([0]+[1]+[2]+[3]+[4]+[5]+[6]) );
    static constexpr uint8_t CMD_SCREEN_ORIGIN= 0x19; // 6Byte 表示起点座標 [1~2]=X座標 [3~4]=Y座標 [5]=チェックサム ( ~([0]+[1]+[2]+[3]+[4]) );
//  static constexpr uint8_t CMD_INVOFF       = 0x20; // 1Byte 色反転を解除;
//  static constexpr uint8_t CMD_INVON        = 0x21; // 1Byte 色反転を有効;

    static constexpr uint8_t CMD_COPYRECT     = 0x23; //13Byte 矩形範囲コピー [1~2]==XS [3~4]==YS [5~6]==XE [7~8]==YE [9~10]==DST_X [11~12]==DST_Y
    static constexpr uint8_t CMD_CASET        = 0x2A; // 5Byte X方向の範囲選択 data[1~2]==XS  data[3~4]==XE
    static constexpr uint8_t CMD_RASET        = 0x2B; // 5Byte Y方向の範囲選択 data[1~2]==YS  data[3~4]==YE

    static constexpr uint8_t CMD_WRITE_RAW    = 0x40;
    static constexpr uint8_t CMD_WRITE_RAW_8  = 0x41; // 不定長 RGB332   1Byteのピクセルデータを連続送信;
    static constexpr uint8_t CMD_WRITE_RAW_16 = 0x42; // 不定長 RGB565   2Byteのピクセルデータを連続送信;
    static constexpr uint8_t CMD_WRITE_RAW_24 = 0x43; // 不定長 RGB888   3Byteのピクセルデータを連続送信;
    static constexpr uint8_t CMD_WRITE_RAW_32 = 0x44; // 不定長 ARGB8888 4Byteのピクセルデータを連続送信;
    static constexpr uint8_t CMD_WRITE_RAW_A  = 0x45; // 不定長 A8       1Byteのピクセルデータを連続送信(アルファチャネルのみ、描画色は最後に使用したものを再利用する);

//  static constexpr uint8_t CMD_WRITE_RLE    = 0x48;
//  static constexpr uint8_t CMD_WRITE_RLE_8  = 0x49; // 不定長 RGB332   1Byteのピクセルデータを連続送信(RLE圧縮);
//  static constexpr uint8_t CMD_WRITE_RLE_16 = 0x4A; // 不定長 RGB565   2Byteのピクセルデータを連続送信(RLE圧縮);
//  static constexpr uint8_t CMD_WRITE_RLE_24 = 0x4B; // 不定長 RGB888   3Byteのピクセルデータを連続送信(RLE圧縮);
//  static constexpr uint8_t CMD_WRITE_RLE_32 = 0x4C; // 不定長 ARGB8888 4Byteのピクセルデータを連続送信(RLE圧縮);
//  static constexpr uint8_t CMD_WRITE_RLE_A  = 0x4D; // 不定長 A8       1Byteのピクセルデータを連続送信(RLE圧縮 アルファチャネルのみ、描画色は最後に使用したものを再利用する);

//  static constexpr uint8_t CMD_RAM_FILL     = 0x50; // 1Byte 現在の描画色で選択範囲全塗り;
//  static constexpr uint8_t CMD_SET_COLOR    = 0x50;
//  static constexpr uint8_t CMD_SET_COLOR_8  = 0x51; // 2Byte 描画色をRGB332で指定;
//  static constexpr uint8_t CMD_SET_COLOR_16 = 0x52; // 3Byte 描画色をRGB565で指定;
//  static constexpr uint8_t CMD_SET_COLOR_24 = 0x53; // 4Byte 描画色をRGB888で指定;
//  static constexpr uint8_t CMD_SET_COLOR_32 = 0x54; // 5Byte 描画色をARGB8888で指定;

    static constexpr uint8_t CMD_DRAWPIXEL    = 0x60; // 5Byte ドット描画 [1~2]==X [3~4]==Y
    static constexpr uint8_t CMD_DRAWPIXEL_8  = 0x61; // 6Byte ドット描画 [1~2]==X [3~4]==Y [5  ]==RGB332
    static constexpr uint8_t CMD_DRAWPIXEL_16 = 0x62; // 7Byte ドット描画 [1~2]==X [3~4]==Y [5~6]==RGB565
    static constexpr uint8_t CMD_DRAWPIXEL_24 = 0x63; // 8Byte ドット描画 [1~2]==X [3~4]==Y [5~7]==RGB888
    static constexpr uint8_t CMD_DRAWPIXEL_32 = 0x64; // 9Byte ドット描画 [1~2]==X [3~4]==Y [5~8]==ARGB8888

    static constexpr uint8_t CMD_FILLRECT     = 0x68; // 9Byte 矩形塗潰 [1~2]==XS [3~4]==YS [5~6]==XE [7~8]==YE
    static constexpr uint8_t CMD_FILLRECT_8   = 0x69; //10Byte 矩形塗潰 [1~2]==XS [3~4]==YS [5~6]==XE [7~8]==YE [9  ]==RGB332
    static constexpr uint8_t CMD_FILLRECT_16  = 0x6A; //11Byte 矩形塗潰 [1~2]==XS [3~4]==YS [5~6]==XE [7~8]==YE [9~10]==RGB565
    static constexpr uint8_t CMD_FILLRECT_24  = 0x6B; //12Byte 矩形塗潰 [1~2]==XS [3~4]==YS [5~6]==XE [7~8]==YE [9~11]==RGB888
    static constexpr uint8_t CMD_FILLRECT_32  = 0x6C; //13Byte 矩形塗潰 [1~2]==XS [3~4]==YS [5~6]==XE [7~8]==YE [9~12]==ARGB8888

    static constexpr uint8_t CMD_READ_RAW     = 0x80;
    static constexpr uint8_t CMD_READ_RAW_8   = 0x81; // 1Byte RGB332のピクセルデータを読出し;
    static constexpr uint8_t CMD_READ_RAW_16  = 0x82; // 1Byte RGB565のピクセルデータを読出し;
    static constexpr uint8_t CMD_READ_RAW_24  = 0x83; // 1Byte RGB888のピクセルデータを読出し;

//  static constexpr uint8_t CMD_CHANGE_ADDR  = 0xA0;

    static constexpr uint8_t CMD_VIDEO_TIMING_V = 0xB0; // 10Byte 垂直信号のビデオタイミングパラメータ設定
    static constexpr uint8_t CMD_VIDEO_TIMING_H = 0xB1; // 10Byte 水平信号のビデオタイミングパラメータ設定
    static constexpr uint8_t CMD_VIDEO_CLOCK    = 0xB2; // 7Byte  ビデオ信号のピクセルクロック用パラメータ設定

//  static constexpr uint8_t CMD_UPDATE_BEGIN = 0xF0;
//  static constexpr uint8_t CMD_UPDATE_DATA  = 0xF1;
//  static constexpr uint8_t CMD_UPDATE_END   = 0xF2;
//  static constexpr uint8_t CMD_RESET        = 0xFF;
//  static constexpr uint8_t UPDATE_RESULT_BROKEN = 0x01;
//  static constexpr uint8_t UPDATE_RESULT_ERROR  = 0x00;
//  static constexpr uint8_t UPDATE_RESULT_OK     = 0xF1;
//  static constexpr uint8_t UPDATE_RESULT_BUSY   = 0xFF;



    class HDMI_Trans
    {
    public:
      typedef lgfx::Bus_I2C::config_t config_t;
    private:
      config_t HDMI_Trans_config;

      uint8_t readRegister(uint8_t register_address);
      uint16_t readRegister16(uint8_t register_address);
      bool writeRegister(uint8_t register_address, uint8_t value);
      bool writeRegisterSet(const uint8_t *reg_value_pair, size_t len);

    public:

      struct ChipID
      {
        uint8_t id[3];
      };

      HDMI_Trans(const lgfx::Bus_I2C::config_t& i2c_config) : HDMI_Trans_config(i2c_config) {}
      HDMI_Trans(const HDMI_Trans&) = delete;
      HDMI_Trans(HDMI_Trans&&) = delete;
      ChipID readChipID(void);
      void reset(void);
      bool init(void);
      size_t readEDID(uint8_t* EDID, size_t len);
    };

    class LOAD_FPGA
    {
    public:
      LOAD_FPGA(uint_fast8_t _TCK_PIN, uint_fast8_t, uint_fast8_t _TDO_PIN, uint_fast8_t);

    private:
      enum TAP_TypeDef
      {
        TAP_RESET,

        TAP_IDLE,
        TAP_DRSELECT,
        TAP_DRCAPTURE,
        TAP_DRSHIFT,
        TAP_DREXIT1,
        TAP_DRPAUSE,
        TAP_DREXIT2,
        TAP_DRUPDATE,

        TAP_IRSELECT,
        TAP_IRCAPTURE,
        TAP_IRSHIFT,
        TAP_IREXIT1,
        TAP_IRPAUSE,
        TAP_IREXIT2,
        TAP_IRUPDATE,
        TAP_UNKNOWN
      };

      volatile uint32_t *_tdi_reg[2];
      volatile uint32_t *_tck_reg[2];
      volatile uint32_t *_tms_reg[2];
      uint32_t TCK_MASK;
      uint32_t TDI_MASK;
      uint32_t TMS_MASK;
      uint8_t TDO_PIN;

      void JTAG_MoveTap(TAP_TypeDef TAP_From, TAP_TypeDef TAP_To);
      void JTAG_Write(uint_fast8_t din, bool tms, bool LSB, size_t len = 1);
      void JTAG_WriteInst(uint8_t inst);
      void JTAG_TapMove_Inner(bool tms_value, size_t clock_count);
      void JTAG_DUMMY_CLOCK(uint32_t msec);
      uint32_t JTAG_ReadStatus();
    };

    const HDMI_Trans::config_t& config_transmitter(void) const { return _HDMI_Trans_config; }
    void config_transmitter(const HDMI_Trans::config_t& cfg) { _HDMI_Trans_config = cfg; }

  protected:
    HDMI_Trans::config_t _HDMI_Trans_config;
    uint32_t _raw_color = ~0u;
    uint32_t _xpos;
    uint32_t _ypos;
    uint32_t _last_cmd;
    size_t _total_send = 0;
    uint8_t _scale_w = 1;
    uint8_t _scale_h = 1;
    float _refresh_rate = 60.0f;
    uint32_t _pixel_clock = 74250000;
    bool _in_transaction = false;

    bool _init_resolution(void);
    void _set_window(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye, uint_fast8_t cmd);
    void _fill_rect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint_fast8_t bytes);
    void _check_busy(uint32_t length, bool force = false);
    void _rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty);
    void _set_video_timing(const video_timing_t::info_t* param, uint8_t cmd);
    void _set_video_clock(const video_clock_t* param);
    void _copy_rect(uint32_t dst_xy, uint32_t src_xy, uint32_t wh);
    uint32_t _read_fpga_id(void);
  };

//----------------------------------------------------------------------------
 }
}
