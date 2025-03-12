#include "replay.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

void replayRecording(struct RawRecording recorded) {
    initRfModule("tx", recorded.frequency);

    // Initialize RMT
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_CHANNEL_6;
    rmt_tx.gpio_num = gpio_num_t(bruceConfig.rfTx);
    #ifdef USE_CC1101_VIA_SPI
    if(bruceConfig.rfModule==CC1101_SPI_MODULE) rmt_tx.gpio_num = gpio_num_t(bruceConfig.CC1101_bus.io0);
    #endif
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = 80;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_en = false;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);

    for (size_t i = 0; i < recorded.codes.size(); ++i) {
        // Send the RMT code
        rmt_write_items(rmt_tx.channel, recorded.codes[i], recorded.codeLengths[i], true);
        rmt_wait_tx_done(rmt_tx.channel, portMAX_DELAY);

        // Wait for the gap duration
        delay(recorded.gaps[i]);
    }

    // Clean up
    rmt_driver_uninstall(rmt_tx.channel);
}