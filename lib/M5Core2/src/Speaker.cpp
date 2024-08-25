#if defined (CORE2)
#include "Speaker.h"

bool Speaker::InitI2SSpeakOrMic(int mode) {  // Init I2S.  初始化I2S
    esp_err_t err = ESP_OK;

    i2s_driver_uninstall(
        Speak_I2S_NUMBER);  // Uninstall the I2S driver.  卸载I2S驱动
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),  // Set the I2S operating mode.
                                                // 设置I2S工作模式
        .sample_rate = 44100,  // Set the I2S sampling rate.  设置I2S采样率
        .bits_per_sample =
            I2S_BITS_PER_SAMPLE_16BIT,  // Fixed 12-bit stereo MSB.
        // 固定为12位立体声MSB
        .channel_format =
            I2S_CHANNEL_FMT_ONLY_RIGHT,  // Set the channel format. 设置频道格式
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 1, 0)
        .communication_format =
            I2S_COMM_FORMAT_STAND_I2S,  // Set the format of the communication.
                                        // 设置通讯格式
#else
        .communication_format = I2S_COMM_FORMAT_I2S,
#endif
        .intr_alloc_flags =
            ESP_INTR_FLAG_LEVEL1,  // Set the interrupt flag.  设置中断的标志
        .dma_buf_count      = 2,    // DMA buffer count.  DMA缓冲区计数
        .dma_buf_len        = 128,  // DMA buffer length.  DMA缓冲区长度
        .use_apll           = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk         = -1,
        .mclk_multiple      = I2S_MCLK_MULTIPLE_DEFAULT,
        .bits_per_chan      = I2S_BITS_PER_CHAN_DEFAULT,
    };
    if (mode == MODE_MIC) {
        i2s_config.mode =
            (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    } else {
        i2s_config.mode     = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.use_apll = false;  // I2S clock setup.  I2S时钟设置
        i2s_config.tx_desc_auto_clear =
            true;  // Enables auto-cleanup descriptors for understreams.
                   // 开启欠流自动清除描述符
    }
    // Install and drive I2S.  安装并驱动I2S
    err += i2s_driver_install(Speak_I2S_NUMBER, &i2s_config, 0, NULL);

    i2s_pin_config_t tx_pin_config;

#if (ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 3, 0))
    tx_pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif
    tx_pin_config.bck_io_num =
        CONFIG_I2S_BCK_PIN;  // Link the BCK to the CONFIG_I2S_BCK_PIN pin.
                             // 将BCK链接至CONFIG_I2S_BCK_PIN引脚
    tx_pin_config.ws_io_num    = CONFIG_I2S_LRCK_PIN;     //          ...
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;     //       ...
    tx_pin_config.data_in_num  = CONFIG_I2S_DATA_IN_PIN;  //      ...
    err +=
        i2s_set_pin(Speak_I2S_NUMBER,
                    &tx_pin_config);  // Set the I2S pin number. 设置I2S引脚编号
    err += i2s_set_clk(
        Speak_I2S_NUMBER, 44100, I2S_BITS_PER_SAMPLE_16BIT,
        I2S_CHANNEL_MONO);  // Set the clock and bitwidth used by I2S Rx and Tx.
                            // 设置I2S RX、Tx使用的时钟和位宽
    return true;
}

void Speaker::begin(void) {  // 初始化扬声器
    uint8_t val = Read8bit(0x03);
    bool axp192 = false;
    if (val == 0x03) {
        _pmic = pmic_axp192;
    } else if (val == 0x4A) {
        _pmic = pmic_axp2101;
    } else {
        _pmic = pmic_unknown;
    }
    Serial.printf("\n_pmic:%d", _pmic);
    if (_pmic) {
        uint8_t reg_addr = 0x94;
        uint8_t gpio_bit = 0x04;
        uint8_t data;
        data = Read8bit(reg_addr);
        data |= gpio_bit;
        Write1Byte(reg_addr, data);
    } else if (!_pmic) {
        uint8_t reg_addr = 0x94;
        uint8_t data     = 0x1C;
        Write1Byte(reg_addr, data);
    }

    InitI2SSpeakOrMic(MODE_SPK);
}

size_t Speaker::PlaySound(const unsigned char* data,
                          const size_t& amount_of_bytes) {
    size_t bytes_written = 0;
    if (data == nullptr) {
        return bytes_written;
    }
    i2s_write(Speak_I2S_NUMBER, data, amount_of_bytes, &bytes_written,
              portMAX_DELAY);
    return bytes_written;
}
#endif