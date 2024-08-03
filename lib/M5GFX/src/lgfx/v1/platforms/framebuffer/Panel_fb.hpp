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

Porting for Linux FrameBuffer:
 [imliubo](https://github.com/imliubo)
/----------------------------------------------------------------------------*/
#pragma once

#include "../../panel/Panel_Device.hpp"
#include "../../misc/range.hpp"
#include "../../Touch.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_fb : public Panel_Device
  {

  public:
    Panel_fb(void);
    virtual ~Panel_fb(void);

    struct config_detail_t
    {
      // 操作対象とするフレームバッファのパス名、または、デバイス名称 ("st7789") 等の文字列へのポインタを指定する。
      const char* device_name = "/dev/fb0";
    };

    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    color_depth_t setColorDepth(color_depth_t depth);
    void setRotation(uint_fast8_t r) override;
    void setInvert(bool invert) override {}
    void setSleep(bool flg) override {}
    void setPowerSave(bool) override {}

    void waitDisplay(void) override {}
    bool displayBusy(void) override { return false; }

    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;
    void writeBlock(uint32_t rawcolor, uint32_t length) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;
    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param) override;

    uint32_t readCommand(uint_fast16_t cmd, uint_fast8_t index, uint_fast8_t len) override { return 0; }
    uint32_t readData(uint_fast8_t index, uint_fast8_t len) override { return 0; }
    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;

    uint_fast8_t getTouchRaw(touch_point_t* tp, uint_fast8_t count) override;

    // init前に使用し、操作対象とするフレームバッファのパス名、または、デバイス名称 ("st7789") 等の文字列へのポインタを指定する。
    void setDeviceName(const char* device_name) { _config_detail.device_name = device_name; };

    const config_detail_t& config_detail(void) const { return _config_detail; }
    void config_detail(const config_detail_t& config_detail);

  protected:

    config_detail_t _config_detail;

    touch_point_t _touch_point;
    // framebuffer
    int _fbfd = 0;
    char* _fbp = 0;
    long int _screensize = 0;
    struct fb_var_screeninfo _var_info;
    struct fb_fix_screeninfo _fix_info;

    int32_t _xpos = 0;
    int32_t _ypos = 0;

    void _rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty);

  private:
    void fb_draw_rgb_pixel(int x, int y, uint32_t rawcolor);
    void fb_draw_argb_pixel(int x, int y, uint32_t rawcolor);
  };

//----------------------------------------------------------------------------
 }
}
