#ifndef LGFX_CONFIG_H_
#define LGFX_CONFIG_H_

#include <LovyanGFX.hpp>

// Custom configuration for Waveshare ESP32-S3-Touch-AMOLED-1.8
class LGFX : public lgfx::LGFX_Device
{
public:
    // QSPI bus configuration for SH8601 (Quad SPI)
    lgfx::Bus_QSPI _bus_instance;
    // Custom Panel configuration for SH8601
    lgfx::Panel_Device _panel_instance;
    // Touch configuration for FT3168
    lgfx::Touch_FT5x06 _touch_instance;
    // Custom Backlight control via QSPI commands
    lgfx::Light_Device _light_instance;

    LGFX(void)
    {
        // === Bus Configuration ===
        {
            auto cfg = _bus_instance.config();
            cfg.pin_cs = -1;    // CS is controlled by QSPI peripheral
            cfg.pin_sclk = 2;
            cfg.pin_d0 = 1;
            cfg.pin_d1 = 3;
            cfg.pin_d2 = 4;
            cfg.pin_d3 = 5;
            // No octal mode for this board
            cfg.freq_write = 80000000; // 80MHz
            cfg.freq_read = 16000000;
            cfg.spi_mode = 0;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // === Panel Configuration (SH8601) ===
        {
            auto cfg = _panel_instance.config();
            cfg.panel_width = 368;
            cfg.panel_height = 448;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.memory_width = 368;
            cfg.memory_height = 448;
            cfg.pin_rst = -1; // No dedicated RST pin
            cfg.pin_busy = -1;
            cfg.bus_shared = true;
            cfg.invert = true; // AMOLED panels are often inverted
            cfg.rgb_order = false; // RGB
            cfg.dlen_16bit = false;
            _panel_instance.config(cfg);

            // SH8601 Initialization Commands
            static const uint8_t init_cmds[] = {
                0xFE, 0, // Page select
                0x01,
                0x6A, 1, 0x10,
                0xFE, 1, 0,
                0x11, 0,
                0xFE, 0,
                0x35, 0,
                0xFE, 4,
                0x51, 1, 0xFF,
                0xFE, 0,
                0x29, 0,
            };
            _panel_instance.setInitCommands(sizeof(init_cmds), init_cmds);
        }

        // === Custom Backlight Configuration ===
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = -1; // Not used
            cfg.invert = false;
            cfg.freq = 0;
            cfg.pwm_channel = 0;
            _light_instance.config(cfg);

            // Override the setBrightness function
            _light_instance.setCallback([this](uint8_t brightness) {
                _bus_instance.beginTransaction();
                _bus_instance.writeCommand(0x51, 8);
                _bus_instance.writeData(brightness, 8);
                _bus_instance.endTransaction();
            });
            _panel_instance.setLight(&_light_instance);
        }

        // === Touch Configuration (FT3168) ===
        {
            auto cfg = _touch_instance.config();
            cfg.i2c_addr = 0x38;
            cfg.pin_sda = 6;
            cfg.pin_scl = 7;
            cfg.pin_int = -1; // Polling mode
            cfg.freq = 400000;
            cfg.x_min = 0;
            cfg.x_max = 367;
            cfg.y_min = 0;
            cfg.y_max = 447;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;
            // Set screen size to the touch driver
            cfg.panel_width = 368;
            cfg.panel_height = 448;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};

#endif
