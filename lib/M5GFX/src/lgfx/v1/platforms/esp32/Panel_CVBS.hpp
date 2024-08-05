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

#include "../../panel/Panel_FrameBufferBase.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_CVBS : public Panel_FrameBufferBase
  {
  public:
    Panel_CVBS();

    ~Panel_CVBS();

    struct config_detail_t
    {
      enum signal_type_t
      {
        NTSC,    // black = 7.5IRE
        NTSC_J,  // black = 0IRE (for Japan)
        PAL,
        PAL_M,
        PAL_N,
        signal_type_max,     // dummy, can't select this.
      };

      signal_type_t signal_type = signal_type_t::NTSC;

      // output gpio ( ESP32: only 25 or 26 )
      int8_t pin_dac = 26;

      // luminance_gain default:128  0=no signal
      uint8_t output_level = 128;

      // default:128  0=monochrome
      uint8_t chroma_level = 128;

      // 0=SRAM only (no use PSRAM) / 1=both(half PSRAM and half SRAM) / 2=PSRAM only (no use SRAM)
      uint8_t use_psram = 0;

      /// background PSRAM read task priority
      uint8_t task_priority = 25;

      /// background PSRAM read task pinned core. (APP_CPU_NUM or PRO_CPU_NUM)
      uint8_t task_pinned_core = -1;
    };

    color_depth_t setColorDepth(color_depth_t) override;
    void setResolution(uint16_t width, uint16_t height, config_detail_t::signal_type_t type = config_detail_t::signal_type_max, int output_width = -1, int output_height = -1, int offset_x = -1, int offset_y = -1);
    void setOutputLevel(uint8_t output_level);
    void setChromaLevel(uint8_t chroma);
    void setSignalType(config_detail_t::signal_type_t signal_type) { _config_detail.signal_type = signal_type; config_detail(_config_detail); }
    void setPsram( bool use_psram ) { _config_detail.use_psram = use_psram; config_detail(_config_detail); }

    bool init(bool) override;

    int32_t getScanLine(void) override;

    const config_detail_t& config_detail(void) const { return _config_detail; }
    void config_detail(const config_detail_t& config_detail);

  protected:

    config_detail_t _config_detail;

    void deinit(void);

    void updateSignalLevel(void);
    bool initFrameBuffer(size_t w, size_t h, uint8_t use_psram);
    void deinitFrameBuffer(void);

  private:

    bool _started = false;
  };

//----------------------------------------------------------------------------
 }
}
