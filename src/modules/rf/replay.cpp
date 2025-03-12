#include "replay.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

// void replayRecording(struct RawRecording recorded) {
//     initRfModule("tx", recorded.frequency);

//     // Deinit RMT channel
//     ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_uninstall(RMT_CHANNEL_6));

//     // Initialize RMT
//     rmt_config_t txconfig;
//     txconfig.channel = RMT_CHANNEL_6;
//     txconfig.gpio_num = gpio_num_t(bruceConfig.rfTx);
//     #ifdef USE_CC1101_VIA_SPI
//     if(bruceConfig.rfModule==CC1101_SPI_MODULE) txconfig.gpio_num = gpio_num_t(bruceConfig.CC1101_bus.io2);
//     #endif
//     txconfig.mem_block_num = 1;
//     txconfig.clk_div = 80;
//     txconfig.tx_config.loop_en = false;
//     txconfig.tx_config.carrier_en = false;
//     txconfig.tx_config.idle_output_en = true;
//     txconfig.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
//     txconfig.rmt_mode = RMT_MODE_TX;
//     ESP_LOGI("RMT", "Using GPIO: %d", txconfig.gpio_num);
//     esp_err_t ret = rmt_config(&txconfig);
//     if (ret != ESP_OK) {
//         ESP_LOGE("RMT", "Failed to configure RMT: %s", esp_err_to_name(ret));
//     }
//     rmt_driver_install(txconfig.channel, 0, 0);

//     for (size_t i = 0; i < recorded.codes.size(); ++i) {
//         // Send the RMT code
//         esp_err_t ret = rmt_write_items(txconfig.channel, recorded.codes[i], recorded.codeLengths[i], true);
//         if (ret == ESP_OK) {
//             ESP_LOGI("RMT", "Data sent successfully");
//         } else {
//             ESP_LOGE("RMT", "Failed to send data");
//         }
//         // Wait for the gap duration
//         delay(recorded.gaps[i]);
//     }

//     // Clean up
//     rmt_driver_uninstall(txconfig.channel);
// }

void replayRecording(struct RawRecording recorded) {
    initRfModule("tx", recorded.frequency);

    gpio_num_t txPin = gpio_num_t(bruceConfig.rfTx);
    #ifdef USE_CC1101_VIA_SPI
    if(bruceConfig.rfModule==CC1101_SPI_MODULE) txPin = gpio_num_t(bruceConfig.CC1101_bus.io0);
    #endif
    pinMode(txPin, OUTPUT);

    for (size_t i = 0; i < recorded.codes.size(); ++i) {
        // Send the RMT code
        for(int j=0; j<recorded.codeLengths[i]; j++) {
            if(recorded.codes[i][j].level0 == 1) digitalWrite(txPin, HIGH);
            else digitalWrite(txPin, LOW);
            delayMicroseconds(recorded.codes[i][j].duration0);
            if(recorded.codes[i][j].level1 == 1) digitalWrite(txPin, HIGH);
            else digitalWrite(txPin, LOW);
            delayMicroseconds(recorded.codes[i][j].duration1);
        }
        if(i < recorded.codes.size() - 1) {
            delay(recorded.gaps[i]);
        }
    }

    deinitRfModule();
}