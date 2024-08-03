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
#if defined (ESP_PLATFORM)
#include <sdkconfig.h>

#include "Panel_M5HDMI.hpp"
#include "Panel_M5HDMI_FS.h"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"
#include "../misc/colortype.hpp"
#include "../../internal/alloca.h"

#include <stdint.h>
#include <stddef.h>
#include <esp_log.h>
#include <soc/gpio_periph.h>
#include <soc/gpio_reg.h>
#include <soc/io_mux_reg.h>
#if __has_include(<hal/gpio_types.h>)
 #include <hal/gpio_types.h>
#endif

#define TAG "M5HDMI"

namespace lgfx
{
 inline namespace v1
 {

//----------------------------------------------------------------------------
  static constexpr const uint32_t base_clock = 74250000;

  enum GWFPGA_Inst_Def
  {
    ISC_NOOP          = 0x02,
    ISC_ERASE         = 0x05,
    ERASE_DONE        = 0x09,
    READ_ID_CODE      = 0x11,
    ISC_ENABLE        = 0x15,
    FAST_PROGRAM      = 0x17,
    STATUS_CODE       = 0x41,
    JTAG_EF_PROGRAM   = 0x71,
    JTAG_EF_READ      = 0x73,
    JTAG_EF_ERASE     = 0x75,
    ISC_DISABLE       = 0x3A,
    REPROGRAM         = 0x3C,
    Bypass            = 0xFF
  };

  enum GWFPGA_StatusReg_Def
  {
    STATUS_CRC_ERROR            = (1<<0),
    STATUS_BAD_COMMAND          = (1<<1),
    STATUS_ID_VERIFY_FAILED     = (1<<2),
    STATUS_TIMEOUT              = (1<<3),
    STATUS_MEMORY_ERASE         = (1<<5),
    STATUS_PREAMBLE             = (1<<6),
    STATUS_SYSTEM_EDIT_MODE     = (1<<7),
    STATUS_PRG_SPIFLASH_DIRECT  = (1<<8),
    STATUS_NON_JTAG_CNF_ACTIVE  = (1<<10),
    STATUS_BYPASS               = (1<<11),
    STATUS_GOWIN_VLD            = (1<<12),
    STATUS_DONE_FINAL           = (1<<13),
    STATUS_SECURITY_FINAL       = (1<<14),
    STATUS_READY                = (1<<15),
    STATUS_POR                  = (1<<16),
    STATUS_FLASH_LOCK           = (1<<17)
  };

  Panel_M5HDMI::LOAD_FPGA::LOAD_FPGA(uint_fast8_t _TCK_PIN, uint_fast8_t _TDI_PIN, uint_fast8_t _TDO_PIN, uint_fast8_t _TMS_PIN)
  {
    _tdi_reg[0] = lgfx::get_gpio_lo_reg(_TDI_PIN);
    _tdi_reg[1] = lgfx::get_gpio_hi_reg(_TDI_PIN);
    _tck_reg[0] = lgfx::get_gpio_lo_reg(_TCK_PIN);
    _tck_reg[1] = lgfx::get_gpio_hi_reg(_TCK_PIN);
    _tms_reg[0] = lgfx::get_gpio_lo_reg(_TMS_PIN);
    _tms_reg[1] = lgfx::get_gpio_hi_reg(_TMS_PIN);

    lgfx::pinMode(_TCK_PIN, lgfx::pin_mode_t::output);
    lgfx::pinMode(_TDI_PIN, lgfx::pin_mode_t::output);
    lgfx::pinMode(_TMS_PIN, lgfx::pin_mode_t::output);
    lgfx::pinMode(_TDO_PIN, lgfx::pin_mode_t::input);

    TCK_MASK = 1 << (31 & _TCK_PIN);
    TDI_MASK = 1 << (31 & _TDI_PIN);
    TMS_MASK = 1 << (31 & _TMS_PIN);
    TDO_PIN = _TDO_PIN;

    *_tms_reg[0] = TMS_MASK;
    *_tdi_reg[0] = TDI_MASK;
    *_tck_reg[0] = TCK_MASK;

    int retry = 128;
    do
    { // FPGAのロットによって待ち時間に差がある。
      // 先に進んで良いかステータスレジスタの状態をチェックする。
      if ((JTAG_ReadStatus() & 0x200) == 0) { break; }
      delay(1);
    } while (--retry);

    JTAG_MoveTap(TAP_UNKNOWN, TAP_IDLE);

    ESP_LOGI(TAG, "Erase FPGA SRAM...");

    JTAG_WriteInst(ISC_ENABLE);
    JTAG_WriteInst(ISC_ERASE);
    JTAG_WriteInst(ISC_NOOP);

    JTAG_DUMMY_CLOCK(4);

    JTAG_WriteInst(ERASE_DONE);
    JTAG_WriteInst(ISC_NOOP);

    JTAG_WriteInst(ISC_DISABLE);
    JTAG_WriteInst(ISC_NOOP);

    JTAG_DUMMY_CLOCK(4);

    ESP_LOGI(TAG, "Starting Writing to SRAM...");
    JTAG_WriteInst(ISC_ENABLE);
    JTAG_WriteInst(FAST_PROGRAM);

    JTAG_MoveTap(TAP_IDLE, TAP_DRSHIFT);

    int32_t rle_len = -1;
    int32_t direct_len = -1;
    for (size_t i = 0; i < sizeof(fs_bitstream_rle); ++i)
    {
      bool tx_end = (i == sizeof(fs_bitstream_rle) - 1);

      if (rle_len < 0)
      {
        rle_len = fs_bitstream_rle[i];
        direct_len = -1;
      }
      else
      if (rle_len == 0)
      { // direct mode
        if (direct_len == -1)
        {
          direct_len = fs_bitstream_rle[i];
        }
        else
        {
          JTAG_Write(fs_bitstream_rle[i], tx_end, 0x00);
          if (0 == --direct_len)
          {
            rle_len = -1;
          }
        }
      }
      else
      { // rle mode
        JTAG_Write(fs_bitstream_rle[i], tx_end, 0x00, rle_len);
        rle_len = -1;
      }
    }

    JTAG_MoveTap(TAP_DREXIT1,  TAP_IDLE);
    JTAG_WriteInst(ISC_DISABLE);
    JTAG_WriteInst(ISC_NOOP);

    ESP_LOGI(TAG, "SRAM Prog Finish...");
  }

  void Panel_M5HDMI::LOAD_FPGA::JTAG_DUMMY_CLOCK(uint32_t msec)
  {
    uint32_t erase_time = lgfx::millis();
    auto lo_reg = _tck_reg[0];
    auto hi_reg = _tck_reg[1];
    do
    {
      *lo_reg = TCK_MASK;
      *hi_reg = TCK_MASK;
      *lo_reg = TCK_MASK;
      *hi_reg = TCK_MASK;
    } while (lgfx::millis() - erase_time < msec);
  }

  void Panel_M5HDMI::LOAD_FPGA::JTAG_MoveTap(TAP_TypeDef TAP_From, TAP_TypeDef TAP_To)
  {
    int high = 1;
    int low = 2;
    if ((TAP_From == TAP_UNKNOWN) && (TAP_To==TAP_IDLE) )
    {
      high = 8;
    }
    else if ((TAP_From == TAP_IDLE) && (TAP_To==TAP_IDLE) )
    {
      high = 0;
      low = 3;
    }
    else if ((TAP_From == TAP_IDLE) && (TAP_To==TAP_IRSHIFT) )
    {
      high = 2;
    }
    else if ((TAP_From == TAP_IDLE) && (TAP_To==TAP_DRSHIFT) )
    {}
    else if ((TAP_From == TAP_IREXIT1) && (TAP_To==TAP_IDLE) )
    {
      low = 11;  // IREXIT1->IDLE + (IDLE->IDLE x3)
    }
    else if ((TAP_From == TAP_DREXIT1) && (TAP_To==TAP_IDLE) )
    {}
    else
    {
      ESP_LOGI(TAG, "error tap walking.");
      return;
    }

    if (high)
    {
      JTAG_TapMove_Inner(true, high);
    }
    JTAG_TapMove_Inner(false, low);
  }

  void Panel_M5HDMI::LOAD_FPGA::JTAG_TapMove_Inner(bool tms_value, size_t clock_count)
  {
    *_tms_reg[tms_value] = TMS_MASK;
    auto tck_mask = TCK_MASK;
    do
    {
      *_tck_reg[0] = tck_mask;
      *_tck_reg[1] = tck_mask;
    } while (--clock_count);
  }

  void Panel_M5HDMI::LOAD_FPGA::JTAG_Write(uint_fast8_t din, bool tms, bool LSB, size_t len)
  {
    if (LSB && din)
    {
      din = ((din & 0x55) << 1) + ((din >> 1) & 0x55);
      din = ((din & 0x33) << 2) + ((din >> 2) & 0x33);
      din = ((din & 0x0F) << 4) + ((din >> 4) & 0x0F);
    }
    *_tms_reg[0] = TMS_MASK;
    uint32_t tdi_mask = TDI_MASK;
    uint32_t tck_mask = TCK_MASK;
    uint_fast8_t prev = ~0;
    do
    {
      size_t i = 8;
      do
      {
        if (!--i && tms && (len == 1))
        {
          *_tms_reg[1] = TMS_MASK;
        }
        uint_fast8_t lv = (1 & (din >> i));
        if (prev != lv)
        {
          prev = lv;
          *_tdi_reg[lv] = tdi_mask;
        }
        *_tck_reg[0] = tck_mask;
        *_tck_reg[1] = tck_mask;
      } while (i);
    } while (--len);
  }

  void Panel_M5HDMI::LOAD_FPGA::JTAG_WriteInst(uint8_t inst)
  {
    JTAG_MoveTap(TAP_IDLE, TAP_IRSHIFT);
    JTAG_Write(inst, 0x1, true);
    JTAG_MoveTap(TAP_IREXIT1, TAP_IDLE);
  }

  uint32_t Panel_M5HDMI::LOAD_FPGA::JTAG_ReadStatus()
  {
    uint32_t out = 0;
    JTAG_WriteInst(STATUS_CODE);
    JTAG_MoveTap(TAP_IDLE, TAP_DRSHIFT);

    *_tms_reg[0] = TMS_MASK;
    for (size_t i = 0; i < 32; i++)
    {
      if (i == 31) { *_tms_reg[1] = TMS_MASK; }
      *_tck_reg[0] = TCK_MASK;
      *_tck_reg[1] = TCK_MASK;
      if (lgfx::gpio_in(TDO_PIN)) { out += 1 << i; }
    };
    JTAG_MoveTap(TAP_DREXIT1,  TAP_IDLE);
    return out;
  }

//----------------------------------------------------------------------------

  uint8_t Panel_M5HDMI::HDMI_Trans::readRegister(uint8_t register_address)
  {
    uint8_t buffer;
    lgfx::i2c::transactionWriteRead(this->HDMI_Trans_config.i2c_port, this->HDMI_Trans_config.i2c_addr, &register_address, 1, &buffer, 1, this->HDMI_Trans_config.freq_read);
    return buffer;
  }

  uint16_t Panel_M5HDMI::HDMI_Trans::readRegister16(uint8_t register_address)
  {
    uint8_t buffer[2];
    lgfx::i2c::transactionWriteRead(this->HDMI_Trans_config.i2c_port, this->HDMI_Trans_config.i2c_addr, &register_address, 1, buffer, 2, this->HDMI_Trans_config.freq_read);
    return (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
  }

  bool Panel_M5HDMI::HDMI_Trans::writeRegister(uint8_t register_address, uint8_t value)
  {
    uint8_t buffer[2] = {
      register_address,
      value,
    };
    int retry = 4;
    while (lgfx::i2c::transactionWrite(this->HDMI_Trans_config.i2c_port, this->HDMI_Trans_config.i2c_addr, buffer, 2, this->HDMI_Trans_config.freq_write).has_error() && --retry)
    {
      lgfx::delay(1);
    }
    if (!retry)
    {
      ESP_LOGI(TAG, "i2c write err  reg:%02x val:%02x", register_address, value);
    }
    return retry != 0;
  }

  bool Panel_M5HDMI::HDMI_Trans::writeRegisterSet(const uint8_t *reg_data_pair, size_t len)
  {
    size_t idx = 0;
    do
    {
      if (!this->writeRegister(reg_data_pair[idx], reg_data_pair[idx + 1]))
      {
        return false;
      }
    } while ((idx += 2) < len);
    return true;
  }

  Panel_M5HDMI::HDMI_Trans::ChipID Panel_M5HDMI::HDMI_Trans::readChipID(void)
  {
    ChipID chip_id = { 0,0,0 };

    if (this->writeRegister(0xff, 0x80)
     && this->writeRegister(0xee, 0x01))
    {
      chip_id.id[0] = this->readRegister(0x00);
      chip_id.id[1] = this->readRegister(0x01);
      chip_id.id[2] = this->readRegister(0x02);
    }
    return chip_id;
  }

  void Panel_M5HDMI::HDMI_Trans::reset(void)
  {
    static constexpr const uint8_t data[] = { 0xff, 0x81, 0x30, 0x00, 0x02, 0x66, 0x0a, 0x06, 0x15, 0x06, 0x4e, 0xa8, 0xff, 0x80, 0xee, 0x01, 0x11, 0x00, 0x13, 0xf1, 0x13, 0xf9, 0x0a, 0x80, 0xff, 0x82, 0x1b, 0x77, 0x1c, 0xec, 0x45, 0x00, 0x4f, 0x40, 0x50, 0x00, 0x47, 0x07 };
    this->writeRegisterSet(data, sizeof(data));
  }

  bool Panel_M5HDMI::HDMI_Trans::init(void)
  {
    auto id = this->readChipID();
    {
// 96kHz audio setting.
//    static constexpr const uint8_t data_1[] = { 0xff, 0x82, 0xD6, 0x8E, 0xD7, 0x04, 0xff, 0x84, 0x06, 0x08, 0x07, 0x10, 0x09, 0x00, 0x0F, 0xAB, 0x34, 0xD5, 0x35, 0x00, 0x36, 0x30, 0x37, 0x00, 0x3C, 0x21,
//                                                0xff, 0x82, 0xde, 0x00, 0xde, 0xc0, 0xff, 0x81, 0x23, 0x40, 0x24, 0x64, 0x26, 0x55, 0x29, 0x04, 0x4d, 0x00, 0x27, 0x60, 0x28, 0x00, 0x25, 0x01, 0x2c, 0x94, 0x2d, 0x99 };

// 48kHz audio setting.
      static constexpr const uint8_t data_1[] = { 0xff, 0x82, 0xD6, 0x8E, 0xD7, 0x04, 0xff, 0x84, 0x06, 0x08, 0x07, 0x10, 0x09, 0x00, 0x0F, 0x2B, 0x34, 0xD5, 0x35, 0x00, 0x36, 0x18, 0x37, 0x00, 0x3C, 0x21,
                                                  0xff, 0x82, 0xde, 0x00, 0xde, 0xc0, 0xff, 0x81, 0x23, 0x40, 0x24, 0x64, 0x26, 0x55, 0x29, 0x04, 0x4d, 0x00, 0x27, 0x60, 0x28, 0x00, 0x25, 0x01, 0x2c, 0x94, 0x2d, 0x99 };

// disable audio setting.
//    static constexpr const uint8_t data_1[] = { 0xff, 0x82, 0xde, 0x00, 0xde, 0xc0, 0xff, 0x81, 0x23, 0x40, 0x24, 0x64, 0x26, 0x55, 0x29, 0x04, 0x4d, 0x00, 0x27, 0x60, 0x28, 0x00, 0x25, 0x01, 0x2c, 0x94, 0x2d, 0x99 };

      this->writeRegisterSet(data_1, sizeof(data_1));
    }
    this->writeRegister(0x2b, this->readRegister(0x2b) & 0xfd);
    this->writeRegister(0x2e, this->readRegister(0x2e) & 0xfe);

    if ( id.id[2] == 0xE2 )
    {
      static constexpr const uint8_t data_u3[] = { 0x4d, 0x09, 0x27, 0x66, 0x28, 0x88, 0x2a, 0x00, 0x2a, 0x20, 0x25, 0x00, 0x2c, 0x9e, 0x2d, 0x99 };
      this->writeRegisterSet(data_u3, sizeof(data_u3));
    }

    for (int i = 0; i < 8; ++i)
    {
      static constexpr const uint8_t data_pll[] = { 0xff, 0x80, 0x16, 0xf1, 0x18, 0xdc, 0x18, 0xfc, 0x16, 0xf3, 0x16, 0xe3, 0x16, 0xf3, 0xff, 0x82 };
      this->writeRegisterSet(data_pll, sizeof(data_pll));
      auto locked = (this->readRegister(0x15) & 0x80) != 0;
      auto value = this->readRegister(0xea);
      auto done = (this->readRegister(0xeb) & 0x80) != 0;
      if ( locked && done && value != 0xff)
      {
        static constexpr const uint8_t data[] = { 0xb9, 0x00, 0xff, 0x84, 0x43, 0x31, 0x44, 0x10, 0x45, 0x2a, 0x47, 0x04, 0x10, 0x2c, 0x12, 0x64, 0x3d, 0x0a, 0xff, 0x80, 0x11, 0x00, 0x13, 0xf1, 0x13, 0xf9, 0xff, 0x81, 0x31, 0x44, 0x32, 0x4a, 0x33, 0x0b, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x44, 0x3f, 0x0f, 0x40, 0xa0, 0x41, 0xa0, 0x42, 0xa0, 0x43, 0xa0, 0x44, 0xa0, 0x30, 0xea };
        this->writeRegisterSet(data, sizeof(data));
        return true;
      }
    }
    ESP_LOGE(TAG, "failed to initialize the HDMI transmitter.");
    return false;
  }

  size_t Panel_M5HDMI::HDMI_Trans::readEDID(uint8_t* EDID, size_t len)
  {
    static constexpr const uint8_t data[] = { 0xff, 0x85 ,0x03, 0xc9 ,0x04, 0xA0 ,0x06, 0x20 ,0x14, 0x7f };
    this->writeRegisterSet(data, sizeof(data));

    size_t i_end = std::min<size_t>(16u, len >> 5);
    size_t result = 0;
    for ( size_t i = 0; i < i_end; ++i )
    {
      static constexpr const uint8_t data2[2][6] = { { 0x07, 0x36 ,0x07, 0x34 ,0x07, 0x37 }, { 0x07, 0x76 ,0x07, 0x74 ,0x07, 0x77 } };
      this->writeRegister( 0x05, (i & 7) << 5 );
      this->writeRegisterSet(data2[i >> 3], 6);
      delay( 5 );
      if ( 0x02 != ( 0x52 & this->readRegister( 0x40 )))
      {
        break;
      }
      uint8_t* dst = &EDID[result];
      result += 32;
      dst[0] = 0x83;
      lgfx::i2c::transactionWriteRead(this->HDMI_Trans_config.i2c_port, this->HDMI_Trans_config.i2c_addr, dst, 1, dst, 32, this->HDMI_Trans_config.freq_read);

      if (i == 3)
      {
        i_end = std::min<size_t>(i_end, ((dst[30] & 0x03) + 1) << 2);
      }
    }
    static constexpr const uint8_t data3[] = { 0x03, 0xc2 ,0x07, 0x1f };
    this->writeRegisterSet(data3, sizeof(data3));
    return result;
  }

//----------------------------------------------------------------------------

  uint32_t Panel_M5HDMI::_read_fpga_id(void)
  {
    startWrite();
    _bus->writeData(CMD_NOP, 32);
    endWrite();
    _bus->writeData(CMD_NOP, 32);
    startWrite();
    _bus->writeData(CMD_READ_ID, 8); // READ_ID
    _bus->beginRead();
    uint32_t retry = 16;
    while (_bus->readData(8) == 0xFF && --retry) {}
    _bus->readData(8); // skip 0xFF
    uint32_t fpga_id = _bus->readData(32);
    endWrite();
    ESP_LOGI(TAG, "FPGA ID:%08x", (int)__builtin_bswap32(fpga_id));
    return fpga_id;
  }

  bool Panel_M5HDMI::init(bool use_reset)
  {
    ESP_LOGI(TAG, "i2c port:%d sda:%d scl:%d", _HDMI_Trans_config.i2c_port, _HDMI_Trans_config.pin_sda, _HDMI_Trans_config.pin_scl);

    lgfx::i2c::init(_HDMI_Trans_config.i2c_port, _HDMI_Trans_config.pin_sda, _HDMI_Trans_config.pin_scl);

    HDMI_Trans driver(_HDMI_Trans_config);

    auto result = driver.readChipID();
    ESP_LOGI(TAG, "Chip ID: %02x %02x %02x", result.id[0], result.id[1], result.id[2]);
    if (result.id[0] == result.id[1] && result.id[0] == result.id[2])
    {
      return false;
    }

    ESP_LOGI(TAG, "Resetting HDMI transmitter...");
    driver.reset();

    if (!Panel_Device::init(false)) { return false; }

    if ((_read_fpga_id() & 0xFFFF) != ('H' | 'D' << 8))
    {
      auto bus_cfg = reinterpret_cast<lgfx::Bus_SPI*>(_bus)->config();
      gpio::pin_backup_t backup_pins[] = { bus_cfg.pin_sclk, bus_cfg.pin_mosi, bus_cfg.pin_miso };
      LOAD_FPGA fpga(bus_cfg.pin_sclk, bus_cfg.pin_mosi, bus_cfg.pin_miso, _cfg.pin_cs);
      for (auto &bup : backup_pins) { bup.restore(); }

      // Initialize and read ID
      ESP_LOGI(TAG, "Waiting the FPGA gets idle...");
      startWrite();
      _bus->beginRead();
      _bus->readData(32);
      uint32_t retry = 1024;
      do {
        lgfx::delay(10);
      } while ((0xFFFFFFFFu != _bus->readData(32)) && --retry);
      endWrite();

      if (retry == 0) {
        ESP_LOGW(TAG, "Waiting for FPGA idle timed out.");
        return false;
      }
    }

    uint32_t apbfreq = lgfx::getApbFrequency();
    uint_fast8_t div_write = apbfreq / (_bus->getClock() + 1) + 1;
    uint_fast8_t div_read  = apbfreq / (_bus->getReadClock() + 1) + 1;

    uint32_t retry = 8;
    do
    {
   // ESP_LOGI(TAG, "FREQ:%lu , %lu  DIV_W:%lu , %lu", _bus->getClock(), _bus->getReadClock(), div_write, div_read);
      uint32_t fpga_id = _read_fpga_id();
      // 受信したIDの先頭が "HD" なら正常動作
      if ((fpga_id & 0xFFFF) == ('H' | 'D' << 8))
      {
        break;
      }

      if (fpga_id == 0 || fpga_id == ~0u)
      { // MISOが変化しない場合、コマンドが正しく受理されていないと仮定し送信速度を下げる。
        _bus->setClock(apbfreq / ++div_write);
      }
      else
      { // 受信データの先頭が HD でない場合は受信速度を下げる。
        _bus->setReadClock(apbfreq / ++div_read);
      }
    } while (--retry);

    if (retry == 0) {
      ESP_LOGW(TAG, "read FPGA ID failed.");
      return false;
    }

    startWrite();
    bool res = _init_resolution();
    endWrite();

    ESP_LOGI(TAG, "Initialize HDMI transmitter...");
    if (!driver.init() )
    {
      ESP_LOGW(TAG, "HDMI transmitter Initialize failed.");
      return false;
    }

    ESP_LOGI(TAG, "done.");
    return res;
  }

  uint32_t getPllParams(Panel_M5HDMI::video_clock_t* vc, uint32_t target_clock) {

    uint32_t fb_clock = base_clock;
    uint32_t save_diff = ~0u;
    uint32_t fb_div = 1;
    uint32_t in_div = base_clock / (target_clock + 1);
    if (in_div == 0) { in_div = 1; }
    for (;;)
    {
      uint32_t tmp_clock = fb_clock / in_div;
      uint32_t diff = abs((int32_t)target_clock - (int32_t)tmp_clock);
//    ESP_LOGE("M5HDMI", "FB:%d IN:%d  diff:%d", fb_div, in_div, diff);
      if (save_diff > diff)
      {
        save_diff = diff;
        vc->feedback_divider = fb_div;
        vc->input_divider = in_div;
        if (diff == 0) { break; }
      }
      if (target_clock < tmp_clock)
      {
        if (++in_div > 24) { break; }
      }
      else
      {
        if (++fb_div > 64) { break; }
        fb_clock = base_clock * fb_div;
      }
    }

    uint32_t result = base_clock * vc->feedback_divider / vc->input_divider;

    save_diff = ~0u;
    static constexpr const uint8_t odiv_tbl[] = { 2, 4, 8, 16, 32, 48, 64, 80, 96, 112, 128 };
    static constexpr const int32_t vco_target = 800000000; // 800 MHz
    for (auto odiv : odiv_tbl) {
      uint32_t diff = abs((int32_t)(result * odiv) - vco_target);
//    ESP_LOGE("M5HDMI", "DIFF:%d ODIV:%d", diff, odiv);
      if (save_diff < diff) { break; }
      save_diff = diff;
      vc->output_divider = odiv;
    }
    // Use half of the pixel clock if the target clock is greater than the base clock.
    vc->use_half_clock = target_clock > base_clock;

    return result;
  }

  bool Panel_M5HDMI::_init_resolution(void)
  {
    video_clock_t vc;
    int32_t OUTPUT_CLOCK = getPllParams(&vc, _pixel_clock);

    bool use_half_clock = vc.use_half_clock;
    int32_t TOTAL_RESOLUTION = (OUTPUT_CLOCK >> use_half_clock) / _refresh_rate;

    int mem_width  = _cfg.memory_width ;
    int mem_height = _cfg.memory_height;

    uint32_t diff = ~0u;
    int vert_total = mem_height + 9;
    int hori_total = TOTAL_RESOLUTION / vert_total;
    int hori_tmp = hori_total, vert_tmp = vert_total;
    int hori_min = mem_width + (( 32 + (mem_width >> (1+_scale_w))) >> use_half_clock);
    int hori_max = mem_width + ((768 + (mem_width >> 3)) >> use_half_clock);
    if (hori_tmp > hori_max) { hori_tmp = hori_max; }
    for (;;)
    {
      int d1 = TOTAL_RESOLUTION - (hori_tmp * vert_tmp);
      uint32_t diff_abs = abs(d1);
      if (diff > diff_abs)
      {
        diff = diff_abs;
        hori_total = hori_tmp;
        vert_total = vert_tmp;
        if (diff == 0) { break; }
      }
      if (d1 >= 0) { ++vert_tmp; }
      else if (--hori_tmp < hori_min) { break; }
    }

    bool res = (hori_total > hori_min);
    if (!res)
    { // If the blanking period is too small, it will not work properly.
      hori_total = hori_min;
      ESP_LOGE(TAG, "resolution error. out of range  %dx%d %.2f Hz", mem_width, mem_height, (double)_refresh_rate);
    }

    video_timing_t vt;

    vt.v.active = mem_height;
    uint32_t remain = vert_total - mem_height;
    int sync = 1;
    remain -= sync;
    int porch = remain >> 1;
    remain -= porch;
    vt.v.front_porch = porch;
    vt.v.sync = sync;
    vt.v.back_porch = remain;

    vt.h.active = mem_width;
    remain = hori_total - mem_width;
    sync  = 24 + (remain >> 8);
    remain -= sync;
    porch = (remain * 131) >> 8;
    remain -= porch;
    vt.h.front_porch = porch;
    vt.h.sync = sync;
    vt.h.back_porch = remain;
/*
    // Force to 960x540
    vt.v.front_porch = 4; //porch;
    vt.v.sync = 5; //sync;
    vt.v.back_porch = 36; //remain;
    vt.h.front_porch = 88/2; //porch;
    vt.h.sync = 44/2; //sync;
    vt.h.back_porch = 148/2; //remain;
//*/
    setVideoTiming(&vt);
    setScaling(_scale_w, _scale_h);
    _set_video_clock(&vc);

    {
      ESP_LOGD(TAG, "PLL feedback_div:%d  input_div:%d  output_div:%d  OUTPUT_CLOCK:%d", vc.feedback_divider, vc.input_divider, vc.output_divider, (int)OUTPUT_CLOCK);
      ESP_LOGD(TAG, "logical resolution: w:%d h:%d", _cfg.panel_width, _cfg.panel_height);
      ESP_LOGD(TAG, "scaling resolution: w:%d h:%d", _cfg.panel_width * _scale_w, _cfg.panel_height * _scale_h);
      ESP_LOGD(TAG, " output resolution: w:%d h:%d", _cfg.memory_width, _cfg.memory_height);
      ESP_LOGD(TAG, "video timing(Hori) total:%d active:%d frontporch:%d sync:%d backporch:%d", vt.h.active + vt.h.front_porch + vt.h.sync + vt.h.back_porch, vt.h.active, vt.h.front_porch, vt.h.sync, vt.h.back_porch);
      ESP_LOGD(TAG, "video timing(Vert) total:%d active:%d frontporch:%d sync:%d backporch:%d", vt.v.active + vt.v.front_porch + vt.v.sync + vt.v.back_porch, vt.v.active, vt.v.front_porch, vt.v.sync, vt.v.back_porch);
    }

    return res;
  }

  bool Panel_M5HDMI::setResolution( uint16_t logical_width, uint16_t logical_height, float refresh_rate, uint16_t output_width, uint16_t output_height, uint8_t scale_w, uint8_t scale_h, uint32_t pixel_clock)
  {
    config_resolution_t cfg_reso;
    cfg_reso.logical_width  = logical_width;
    cfg_reso.logical_height = logical_height;
    cfg_reso.refresh_rate   = refresh_rate;
    cfg_reso.output_width   = output_width;
    cfg_reso.output_height  = output_height;
    cfg_reso.scale_w        = scale_w;
    cfg_reso.scale_h        = scale_h;
    cfg_reso.pixel_clock    = pixel_clock;
    return setResolution( cfg_reso );
  }

  bool Panel_M5HDMI::setResolution( const config_resolution_t& cfg_reso )
  {
    config_resolution(cfg_reso);
    bool res = _init_resolution();

    union cmd_t
    {
      uint8_t raw[10];
      struct __attribute__((packed))
      {
        uint8_t cmd;
        uint32_t xy1;
        uint32_t xy2;
        uint8_t color;
      };
    };
    cmd_t cmd;
    cmd.cmd = CMD_FILLRECT_8;
    cmd.xy1 = 0;
    cmd.color = 0;
    static constexpr uint32_t mask = 0xFF00FF;
    uint32_t xy = (_cfg.memory_width - 1) + ((_cfg.memory_height - 1) << 16);
    cmd.xy2 = ((xy >> 8) & mask) + ((xy & mask) << 8);
    startWrite();
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
    endWrite();

    return res;
  }

  void Panel_M5HDMI::config_resolution( const config_resolution_t& cfg_reso )
  {
    static constexpr int SCALE_MAX = 8;
    static constexpr int RANGE_MAX = 2048;

    uint_fast16_t logical_width  = cfg_reso.logical_width;
    uint_fast16_t logical_height = cfg_reso.logical_height;
    float refresh_rate           = cfg_reso.refresh_rate;
    uint_fast16_t output_width   = cfg_reso.output_width;
    uint_fast16_t output_height  = cfg_reso.output_height;
    uint_fast8_t scale_w         = cfg_reso.scale_w;
    uint_fast8_t scale_h         = cfg_reso.scale_h;
    _pixel_clock                 = cfg_reso.pixel_clock;

    if (output_width)
    {
      if (output_width  > RANGE_MAX) { output_width  = RANGE_MAX; }
      if (logical_width > output_width) { logical_width = output_width; }
    }
    if (output_height)
    {
      if (output_height > RANGE_MAX) { output_height = RANGE_MAX; }
      if (logical_height > output_height) { logical_height = output_height; }
    }
    if (logical_width == 0)
    {
      if (logical_height == 0)
      {
        logical_width  = 1280;
        logical_height = 720;
      }
      else
      {
        logical_width = (logical_height << 4) / 9;
      }
    }
    else
    if (logical_height == 0)
    {
      logical_height = (logical_width * 9) >> 4;
    }
    if (logical_width  > RANGE_MAX) { logical_width  = RANGE_MAX; }
    if (logical_height > RANGE_MAX) { logical_height = RANGE_MAX; }

    if (refresh_rate > 512.0f) { refresh_rate = 512.0f; }
    else if (refresh_rate < 1.0f)
    {
      auto total = logical_width * logical_height;
      if (total > 1843200) // over 1920x960
      {
        refresh_rate = 24.0f;
      }
      else
      if (total > 1024000) // over 1280x800
      {
        refresh_rate = 30.0f;
      }
      else
      {
        refresh_rate = 60.0f;
      }
    }

    int limit = 55296000 / refresh_rate;
    _refresh_rate = refresh_rate;

    if (output_width == 0 && output_height == 0 && scale_w == 0 && scale_h == 0)
    {
      scale_w = 1280 / logical_width;
      scale_h = 720 / logical_height;
      if ((scale_w > SCALE_MAX)
      || (scale_h > SCALE_MAX)
      || (limit != 1280 * 720)
      || (scale_w * logical_width != 1280)
      || (scale_h * logical_height != 720))
      {
        scale_w = 1;
        scale_h = 1;
        for (int scale = 2; scale <= SCALE_MAX; ++scale)
        {
          uint32_t scale_height = scale * logical_height;
          uint32_t scale_width = scale * logical_width;
          uint32_t total = scale_width * scale_height;
          if (scale_width > 1920 || scale_height > 1920 || total > limit) { break; }
          scale_w = scale;
          scale_h = scale;
        }
      }
      output_width  = scale_w * logical_width;
      output_height = scale_h * logical_height;
      if (output_height & 1) { output_height++; }
      if ((output_width & 1) && (scale_w & 1)) { output_width += scale_w; }
    }
    else
    {
      if (scale_h == 0)
      {
        scale_h = output_height / logical_height;
      }
      if (scale_h > SCALE_MAX) { scale_h = SCALE_MAX; }
      while (logical_height * scale_h > output_height) { --scale_h; }

      if (scale_w == 0)
      {
        scale_w = output_width  / logical_width;
      }
      uint32_t w = output_width / scale_w;
      while (scale_w > SCALE_MAX || w * scale_w != output_width || logical_width * scale_w > output_width)
      {
        w = output_width / --scale_w;
      }
    }
    if (_pixel_clock > base_clock && scale_w >= 2) { // use_half_clock
      scale_w >>= 1;
      output_width >>= 1;
    }
    _scale_w = scale_w;
    _scale_h = scale_h;
    _cfg.memory_width  = output_width  ;
    _cfg.memory_height = output_height ;
    _cfg.panel_width   = logical_width ;
    _cfg.panel_height  = logical_height;
    _cfg.offset_x      = (output_width  / scale_w - logical_width ) >> 1;
    _cfg.offset_y      = (output_height / scale_h - logical_height) >> 1;
    setRotation(getRotation());
  }

  void Panel_M5HDMI::beginTransaction(void)
  {
    if (_in_transaction) { return; }
    _in_transaction = true;
    _bus->beginTransaction();
    cs_control(false);
  }

  void Panel_M5HDMI::endTransaction(void)
  {
    if (!_in_transaction) return;
    _in_transaction = false;

    _last_cmd = 0;
    _bus->wait();
    cs_control(true);
    _bus->endTransaction();
  }

  void Panel_M5HDMI::waitDisplay(void)
  {
    while (displayBusy()) {}
  }

  bool Panel_M5HDMI::displayBusy(void)
  {
    if ((_last_cmd & ~7) == CMD_WRITE_RAW)
    {
      _bus->wait();
      cs_control(true);
      _total_send = 0;
      _last_cmd = 0;
      cs_control(false);
      return false;
    }
    bool res = _total_send;
    if (res)
    {
      startWrite();
      _bus->beginRead();
      res = (_bus->readData(8) == 0x00);
      cs_control(true);
      _bus->endRead();
      if (!res) { _total_send = 0; }
      cs_control(false);
      endWrite();
    }
    return res;
  }

  void Panel_M5HDMI::_check_busy(uint32_t length, bool force)
  {
    if ((_last_cmd & ~7) == CMD_WRITE_RAW)
    {
      _total_send = 0;

      _bus->beginRead();
      while (_bus->readData(8) == 0x00);
      cs_control(true);
      _bus->endRead();
      cs_control(false);
    }
    _last_cmd = 0;

    if ((force && _total_send) || (_total_send += length) >= 512)
    {
      _total_send = 0;
      uint32_t wait = 0;
      _bus->beginRead();
      while (_bus->readData(8) == 0x00)
      {
        delayMicroseconds(++wait);
      }
      cs_control(true);
      _bus->endRead();
      cs_control(false);
    }
  }

  color_depth_t Panel_M5HDMI::setColorDepth(color_depth_t depth)
  {
    auto bits = (depth & color_depth_t::bit_mask);
    if      (bits > 16) { depth = color_depth_t::rgb888_3Byte; }
    else if (bits < 16) { depth = color_depth_t::rgb332_1Byte; }
    else                { depth = color_depth_t::rgb565_2Byte; }

    _read_depth = _write_depth = depth;
    return depth;
  }

  void Panel_M5HDMI::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;
    if (_internal_rotation & 1) std::swap(_width, _height);
  }

  void Panel_M5HDMI::setInvert(bool invert)
  {
  }

  void Panel_M5HDMI::setSleep(bool flg)
  {
    HDMI_Trans driver(_HDMI_Trans_config);
    if (flg)
    {
      driver.reset();
    }
    else
    {
      driver.init();
    }
  }

  void Panel_M5HDMI::setPowerSave(bool flg)
  {
  }

  void Panel_M5HDMI::setBrightness(uint8_t brightness)
  {
  }

  void Panel_M5HDMI::writeBlock(uint32_t rawcolor, uint32_t length)
  {
    do
    {
      uint32_t h = 1;
      auto w = std::min<uint32_t>(length, _xe + 1 - _xpos);
      if (length >= (w << 1) && _xpos == _xs)
      {
        h = std::min<uint32_t>(length / w, _ye + 1 - _ypos);
      }
      writeFillRectPreclipped(_xpos, _ypos, w, h, rawcolor);
      if ((_xpos += w) <= _xe) return;
      _xpos = _xs;
      if (_ye < (_ypos += h)) { _ypos = _ys; }
      length -= w * h;
    } while (length);
  }

  void Panel_M5HDMI::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    startWrite();
    writeFillRectPreclipped(x, y, 1, 1, rawcolor);
    endWrite();
  }

  void Panel_M5HDMI::_fill_rect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint_fast8_t bytes)
  {
    bool rect = (w > 1) || (h > 1);
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + h); }
      if (r & 2)                  { x = _width  - (x + w); }
      if (r & 1) { std::swap(x, y);  std::swap(w, h); }
    }
    uint32_t buf[4];
    buf[0] = ((rect ? CMD_FILLRECT : CMD_DRAWPIXEL) | bytes) << 24;
    bytes += 5;
    x += _cfg.offset_x;
    y += _cfg.offset_y;
    uint32_t mask = 0xFF00FF;
    uint32_t tmp = x + (y << 16);
    buf[1] = ((tmp >> 8) & mask) + ((tmp & mask) << 8);
    buf[2] = _raw_color;
    if (rect)
    {
      x += w - 1;
      y += h - 1;
      tmp = x + (y << 16);
      buf[2] = ((tmp >> 8) & mask) + ((tmp & mask) << 8);
      buf[3] = _raw_color;
      bytes += 4;
    }
    _check_busy(bytes);
    _bus->writeBytes(((uint8_t*)buf)+3, bytes, false, false);
  }

  void Panel_M5HDMI::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    size_t bytes = 0;
    if (_raw_color != rawcolor)
    {
      _raw_color = rawcolor;
      bytes = _write_bits >> 3;
    }
    _fill_rect(x, y, w, h, bytes);
  }

  void Panel_M5HDMI::writeFillRectAlphaPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t argb8888)
  {
    _raw_color = getSwap32(argb8888);
    _fill_rect(x, y, w, h, 4);
    _raw_color = ~0u;
  }

  void Panel_M5HDMI::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    _xpos = xs;
    _ypos = ys;
    _xs = xs;
    _ys = ys;
    _xe = xe;
    _ye = ye;
  }
  void Panel_M5HDMI::_set_window(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye, uint_fast8_t cmd_write)
  {
    union cmd_t
    {
      uint8_t raw[11];
      struct __attribute__((packed))
      {
        uint8_t cmd_x;
        uint32_t data_x;
        uint8_t cmd_y;
        uint32_t data_y;
        uint8_t cmd_write;
      };
    };

    static constexpr uint32_t mask = 0xFF00FF;

    cmd_t cmd;
    cmd.cmd_x = CMD_CASET;
    cmd.cmd_y = CMD_RASET;
    cmd.cmd_write = cmd_write;
    xs += _cfg.offset_x + ((xe + _cfg.offset_x) << 16);
    cmd.data_x = ((xs >> 8) & mask) + ((xs & mask) << 8);
    ys += _cfg.offset_y + ((ye + _cfg.offset_y) << 16);
    cmd.data_y = ((ys >> 8) & mask) + ((ys & mask) << 8);
    _check_busy(sizeof(cmd_t), true);
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
    _last_cmd = cmd_write;
  }

  void Panel_M5HDMI::_rotate_pixelcopy(uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& w, uint_fast16_t& h, pixelcopy_t* param, uint32_t& nextx, uint32_t& nexty)
  {
    uint32_t addx = param->src_x32_add;
    uint32_t addy = param->src_y32_add;
    uint_fast8_t r = _internal_rotation;
    uint_fast8_t bitr = 1u << r;
    // if (bitr & 0b10011100)
    // {
    //   nextx = -nextx;
    // }
    if (bitr & 0b10010110) // case 1:2:4:7:
    {
      param->src_y32 += nexty * (h - 1);
      nexty = -(int32_t)nexty;
      y = _height - (y + h);
    }
    if (r & 2)
    {
      param->src_x32 += addx * (w - 1);
      param->src_y32 += addy * (w - 1);
      addx = -(int32_t)addx;
      addy = -(int32_t)addy;
      x = _width  - (x + w);
    }
    if (r & 1)
    {
      std::swap(x, y);
      std::swap(w, h);
      std::swap(nextx, addx);
      std::swap(nexty, addy);
    }
    param->src_x32_add = addx;
    param->src_y32_add = addy;
  }

  void Panel_M5HDMI::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    uint_fast16_t xs = _xs;
    uint_fast16_t xe = _xe;
    uint_fast16_t ys = _ys;
    uint_fast16_t ye = _ye;
    uint_fast16_t x = _xpos;
    uint_fast16_t y = _ypos;
    auto bytes = (_write_bits >> 3);
    uint32_t cmd = CMD_WRITE_RAW + bytes;

    uint_fast8_t r = _internal_rotation;

    int_fast16_t ay = 1;
    if ((1u << r) & 0b10010110) { y = _height - (y + 1); ys = _height - (ys + 1); ye = _height - (ye + 1); ay = -1; }
    if (r & 2)
    {
      auto linebuf = (uint8_t*)alloca((xe - xs + 1) * bytes);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
      /// Not actually used uninitialized. Just grabbing a copy of the pointer before we start the loop that fills it.
      pixelcopy_t pc((void*)linebuf, _write_depth, _write_depth);
#pragma GCC diagnostic pop
      pc.src_x32_add = ~0u << pixelcopy_t::FP_SCALE;

      x = _width  - (x + 1);
      xs = _width - (xs + 1);
      xe = _width  - (xe + 1);
      uint_fast16_t linelength;
      do
      {
        linelength = std::min<uint_fast16_t>(x - xe + 1, length);
        param->fp_copy(linebuf, 0, linelength, param);
        pc.src_x32 = (linelength - 1) << pixelcopy_t::FP_SCALE;
        if (r & 1)
        {
          _set_window(y, x - linelength + 1, y, x, cmd);
        }
        else
        {
          _set_window(x - linelength + 1, y, x, y, cmd);
        }
        _bus->writePixels(&pc, linelength);
        if ((x -= linelength) < xe)
        {
          x = xs;
          y = (y != ye) ? (y + ay) : ys;
        }
      } while (length -= linelength);
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
      _ypos = y;
      _xpos = _width - (x + 1);
    }
    else
    {
      uint_fast16_t linelength;
      do
      {
        linelength = std::min<uint_fast16_t>(xe - x + 1, length);
        if (r & 1)
        {
          _set_window(y, x, y, x + linelength - 1, cmd);
        }
        else
        {
          _set_window(x, y, x + linelength - 1, y, cmd);
        }
        _bus->writePixels(param, linelength);

        if ((x += linelength) > xe)
        {
          x = xs;
          y = (y != ye) ? (y + ay) : ys;
        }
      } while (length -= linelength);
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
      _ypos = y;
      _xpos = x;
    }
  }

  void Panel_M5HDMI::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    _raw_color = ~0u;
    uint32_t nextx = 0;
    uint32_t nexty = 1 << pixelcopy_t::FP_SCALE;
    auto r = _internal_rotation;
    if (r)
    {
      _rotate_pixelcopy(x, y, w, h, param, nextx, nexty);
      param->no_convert = false;
    }

    uint32_t sx32 = param->src_x32;
    uint32_t sy32 = param->src_y32;

    auto bytes = (_write_bits >> 3) & 3;
    uint32_t cmd = CMD_WRITE_RAW + bytes;

    if (param->transp == pixelcopy_t::NON_TRANSP)
    {
      _set_window(x, y, x+w-1, y+h-1, cmd);

      if (param->no_convert)
      {
        if (param->src_bitwidth == w || h == 1)
        {
          w *= h;
          h = 1;
        }
        do
        {
          uint32_t i = (param->src_x + param->src_y * param->src_bitwidth) * bytes;
          auto src = &((const uint8_t*)param->src_data)[i];
          _bus->writeBytes(src, w * bytes, false, use_dma);
          param->src_x32 = (sx32 += nextx);
          param->src_y32 = (sy32 += nexty);
        } while (--h);
      }
      else
      {
        if (w == 1 && h > 1)
        {
          param->src_x32_add = nextx;
          param->src_y32_add = nexty;
          w = h;
          h = 1;
        }
        do
        {
          _bus->writePixels(param, w);
          param->src_x32 = (sx32 += nextx);
          param->src_y32 = (sy32 += nexty);
        } while (--h);
      }
    }
    else
    {
      uint32_t wb = w * bytes;
      do
      {
        uint32_t i = 0;
        while (w != (i = param->fp_skip(i, w, param)))
        {
          auto dmabuf = _bus->getDMABuffer(wb + 1);
          int32_t len = param->fp_copy(dmabuf, 0, w - i, param);
          _set_window(x + i, y, x + i + len - 1, y, cmd);
          _bus->writeBytes(dmabuf, len * bytes, false, true);
          if (w == (i += len)) break;
        }
        param->src_x32 = (sx32 += nextx);
        param->src_y32 = (sy32 += nexty);
        y ++;
      } while (--h);
    }
  }

  void Panel_M5HDMI::writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param)
  {
    // ToDo:unimplemented
  }

  void Panel_M5HDMI::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    // ToDo:unimplemented
  }

  void Panel_M5HDMI::_copy_rect(uint32_t dst_xy, uint32_t src_xy, uint32_t wh)
  {
    union cmd_t
    {
      uint8_t raw[13];
      struct __attribute__((packed))
      {
        uint8_t cmd;
        uint32_t src_xy1;
        uint32_t src_xy2;
        uint32_t dst_xy;
      };
    };
    cmd_t cmd;
    cmd.cmd = CMD_COPYRECT;
    static constexpr uint32_t mask = 0xFF00FF;
    cmd.src_xy1 = ((src_xy >> 8) & mask) + ((src_xy & mask) << 8);
    src_xy += wh;
    cmd.src_xy2 = ((src_xy >> 8) & mask) + ((src_xy & mask) << 8);
    cmd.dst_xy  = ((dst_xy >> 8) & mask) + ((dst_xy & mask) << 8);

    _check_busy(sizeof(cmd_t));
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
  }

  void Panel_M5HDMI::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { src_y = _height - (src_y + h);  dst_y = _height - (dst_y + h); }
      if (r & 2)                  { src_x = _width  - (src_x + w);  dst_x = _width  - (dst_x + w); }
      if (r & 1) { std::swap(src_x, src_y);  std::swap(dst_x, dst_y);  std::swap(w, h); }
    }
    src_x += _cfg.offset_x;
    dst_x += _cfg.offset_x;
    src_y += _cfg.offset_y;
    dst_y += _cfg.offset_y;

    --w;
    --h;

    if (dst_y > src_y) {
      dst_y += h;
      src_y += h;
      h = -h;
    }
    startWrite();
    _copy_rect(dst_x + (dst_y << 16), src_x + (src_y << 16), w + (h << 16));
    endWrite();
  }

  void Panel_M5HDMI::setVideoTiming(const video_timing_t* param)
  {
    _set_video_timing(&param->v, CMD_VIDEO_TIMING_V);
    _set_video_timing(&param->h, CMD_VIDEO_TIMING_H);
  }

  void Panel_M5HDMI::_set_video_timing(const video_timing_t::info_t* param, uint8_t command)
  {
    union cmd_t
    {
      uint8_t raw[10];
      struct __attribute__((packed))
      {
        uint8_t cmd;
        uint16_t sync;
        uint16_t back;
        uint16_t active;
        uint16_t front;
        uint8_t chksum;
      };
    };
    cmd_t cmd;
    cmd.cmd = command;
    cmd.sync = getSwap16(param->sync);
    cmd.back = getSwap16(param->back_porch);
    cmd.active = getSwap16(param->active);
    cmd.front = getSwap16(param->front_porch);
    uint_fast8_t sum = 0;
    for (size_t i = 0; i < sizeof(cmd_t)-1; ++i)
    {
      sum += cmd.raw[i];
    }
    cmd.chksum = ~sum;

    startWrite();
    waitDisplay();
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
    endWrite();
  }

  void Panel_M5HDMI::_set_video_clock(const video_clock_t* param)
  {
    union cmd_t
    {
      uint8_t raw[9];
      struct __attribute__((packed))
      {
        uint8_t cmd;
        uint16_t input_divider;
        uint16_t feedback_divider;
        uint16_t output_divider;
        uint8_t flags;
        uint8_t chksum;
      };
    };
    cmd_t cmd;
    cmd.cmd = CMD_VIDEO_CLOCK;
    cmd.input_divider = param->input_divider << 8;
    cmd.feedback_divider = param->feedback_divider << 8;
    cmd.output_divider = param->output_divider << 8;
    cmd.flags = param->use_half_clock ? 1 : 0;
    uint_fast8_t sum = 0;
    for (size_t i = 0; i < sizeof(cmd_t)-1; ++i)
    {
      sum += cmd.raw[i];
    }
    cmd.chksum = ~sum;

    startWrite();
    waitDisplay();
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
    endWrite();
  }

  void Panel_M5HDMI::setScaling(uint_fast8_t x_scale, uint_fast8_t y_scale)
  {
    union cmd_t
    {
      uint8_t raw[8];
      struct __attribute__((packed))
      {
        uint8_t cmd;
        uint8_t x_scale;
        uint8_t y_scale;
        uint32_t width_height;
        uint8_t chksum;
      };
    };

    static constexpr uint32_t mask = 0xFF00FF;

    uint32_t w = _cfg.memory_width / x_scale;

    while (w * x_scale != _cfg.memory_width)
    {
      w = _cfg.memory_width / --x_scale;
    }
    uint32_t h = _cfg.memory_height / y_scale;
    uint32_t wh = w + (h << 16);

    cmd_t cmd;
    cmd.cmd = CMD_SCREEN_SCALING;
    cmd.x_scale = x_scale;
    cmd.y_scale = y_scale;
    cmd.width_height = ((wh >> 8) & mask) + ((wh & mask) << 8);

    uint_fast8_t sum = ~0;
    for (size_t i = 0; i < sizeof(cmd_t)-1; ++i)
    {
      sum -= cmd.raw[i];
    }
    cmd.chksum = sum;

    startWrite();
    waitDisplay();
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
    endWrite();
  }

  void Panel_M5HDMI::setViewPort(uint_fast16_t x, uint_fast16_t y)
  {
    union cmd_t
    {
      uint8_t raw[6];
      struct __attribute__((packed))
      {
        uint8_t cmd;
        uint32_t xy;
        uint8_t chksum;
      };
    };
    static constexpr uint32_t mask = 0xFF00FF;

    cmd_t cmd;
    cmd.cmd = CMD_SCREEN_ORIGIN;
    uint32_t xy = x + (y << 16);
    cmd.xy = ((xy >> 8) & mask) + ((xy & mask) << 8);

    uint_fast8_t sum = ~0;
    for (size_t i = 0; i < sizeof(cmd_t)-1; ++i)
    {
      sum -= cmd.raw[i];
    }
    cmd.chksum = sum;

    startWrite();
    waitDisplay();
    _bus->writeBytes(cmd.raw, sizeof(cmd_t), false, false);
    endWrite();
  }

  size_t Panel_M5HDMI::readEDID(uint8_t* EDID, size_t len)
  {
    HDMI_Trans driver(_HDMI_Trans_config);
    return driver.readEDID(EDID, len);
  }

//----------------------------------------------------------------------------
 }
}

#endif
