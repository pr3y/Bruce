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

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_LCD : public Panel_Device
  {
  public:
    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    color_depth_t setColorDepth(color_depth_t depth) override;
    void setRotation(uint_fast8_t r) override;
    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool flg) override;

    void waitDisplay(void) override {}
    bool displayBusy(void) override { return false; }

    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;
    void writeBlock(uint32_t rawcolor, uint32_t len) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;

    uint32_t readCommand(uint_fast16_t cmd, uint_fast8_t index, uint_fast8_t len) override;
    uint32_t readData(uint_fast8_t index, uint_fast8_t len) override;
    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;

    int32_t getScanLine(void) override;

  protected:

    uint16_t _colstart = 0;
    uint16_t _rowstart = 0;
    bool _in_transaction = false;
    uint8_t _cmd_nop = CMD_NOP;
    uint8_t _cmd_ramrd = CMD_RAMRD;
    bool _nop_closing = true; // トランザクション終了時にnopを送るか否か

    enum mad_t
    { MAD_MY  = 0x80
    , MAD_MX  = 0x40
    , MAD_MV  = 0x20
    , MAD_ML  = 0x10
    , MAD_BGR = 0x08
    , MAD_MH  = 0x04
    , MAD_HF  = 0x02
    , MAD_VF  = 0x01
    , MAD_RGB = 0x00
    };

    enum colmod_t
    { RGB444_12bit = 0x33
    , RGB565_2BYTE = 0x55
    , RGB888_3BYTE = 0x66
    };

    virtual uint8_t getMadCtl(uint8_t r) const
    {
      static constexpr uint8_t madctl_table[] =
      {
                                         0,
        MAD_MV|MAD_MX|MAD_MH              ,
               MAD_MX|MAD_MH|MAD_MY|MAD_ML,
        MAD_MV|              MAD_MY|MAD_ML,
                             MAD_MY|MAD_ML,
        MAD_MV                            ,
               MAD_MX|MAD_MH              ,
        MAD_MV|MAD_MX|MAD_MY|MAD_MH|MAD_ML,
      };
      return madctl_table[r];
    }

    static constexpr uint8_t CMD_NOP     = 0x00;
    static constexpr uint8_t CMD_SWRESET = 0x01;
    static constexpr uint8_t CMD_RDDID   = 0x04;
    static constexpr uint8_t CMD_RDDST   = 0x09;
    static constexpr uint8_t CMD_SLPIN   = 0x10;
    static constexpr uint8_t CMD_SLPOUT  = 0x11;
    static constexpr uint8_t CMD_PTLON   = 0x12;
    static constexpr uint8_t CMD_NORON   = 0x13;
    static constexpr uint8_t CMD_INVOFF  = 0x20;
    static constexpr uint8_t CMD_INVON   = 0x21;
    static constexpr uint8_t CMD_GAMMASET= 0x26;
    static constexpr uint8_t CMD_DISPOFF = 0x28;
    static constexpr uint8_t CMD_DISPON  = 0x29;
    static constexpr uint8_t CMD_CASET   = 0x2A;
    static constexpr uint8_t CMD_RASET   = 0x2B;
    static constexpr uint8_t CMD_PASET   = 0x2B;
    static constexpr uint8_t CMD_RAMWR   = 0x2C;
    static constexpr uint8_t CMD_RAMRD   = 0x2E;
    static constexpr uint8_t CMD_MADCTL  = 0x36;
    static constexpr uint8_t CMD_IDMOFF  = 0x38;
    static constexpr uint8_t CMD_IDMON   = 0x39;
    static constexpr uint8_t CMD_COLMOD  = 0x3A;
    static constexpr uint8_t CMD_PIXSET  = 0x3A;
    static constexpr uint8_t CMD_GETSCANLINE= 0x45;

    void begin_transaction(void);
    void end_transaction(void);
    uint32_t read_bits(uint_fast8_t bit_index, uint_fast8_t bit_len);

    void write_command(uint32_t data);
    void write_bytes(const uint8_t* data, uint32_t len, bool use_dma);
    void set_window_8(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye, uint32_t cmd);
    void set_window_16(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye, uint32_t cmd);

    virtual void update_madctl(void);

    virtual uint8_t getColMod(uint8_t bpp) const { return (bpp > 16) ? RGB888_3BYTE : RGB565_2BYTE; }

    /// 引数に応じて _write_depth と _read_depth を設定する。_read_depthはドライバによってrgb888_3Byte固定のものや_write_depthと同じなるものがある。違いに注意が必要。;
    virtual void setColorDepth_impl(color_depth_t depth) { _write_depth = ((int)depth & color_depth_t::bit_mask) > 16 ? rgb888_3Byte : rgb565_2Byte; _read_depth = rgb888_3Byte; }
//  virtual void setColorDepth_impl(color_depth_t depth) { _write_depth = ((int)depth & color_depth_t::bit_mask) > 16 ? rgb888_3Byte : rgb565_2Byte; _read_depth = _write_depth; }
  };

//----------------------------------------------------------------------------
 }
}
