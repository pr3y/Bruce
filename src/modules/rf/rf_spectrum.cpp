#include "rf_spectrum.h"
#include "rf_utils.h"
#include "structs.h"
#include <RCSwitch.h>
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) // RMT
static bool spectrum_rmt_rx_done_callback(
    rmt_channel_t *channel, const rmt_rx_done_event_data_t *edata, void *user_data
) {
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    // send the received RMT symbols to the parser task
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

#define RF_RESOLUTION_HZ 1000000 // 1MHz resolution, 1 tick = 1us
#else
bool setup_rf_spectrum(RingbufHandle_t *rb) {
    if (!initRfModule("rx", bruceConfig.rfFreq)) return false;
    setMHZ(bruceConfig.rfFreq);
    deinitRMT();

    rmt_config_t rxconfig;
    rxconfig.rmt_mode = RMT_MODE_RX;
    rxconfig.channel = RMT_RX_CHANNEL;
    rxconfig.gpio_num = gpio_num_t(bruceConfig.rfRx);

    if (bruceConfig.rfModule == CC1101_SPI_MODULE)
        rxconfig.gpio_num = gpio_num_t(bruceConfigPins.CC1101_bus.io0);

    rxconfig.clk_div = RMT_CLK_DIV; // RMT_DEFAULT_CLK_DIV=32
    rxconfig.mem_block_num = 2;
    rxconfig.flags = 0;
    rxconfig.rx_config.idle_threshold = 3 * RMT_1MS_TICKS;
    rxconfig.rx_config.filter_ticks_thresh = 200 * RMT_1US_TICKS;
    rxconfig.rx_config.filter_en = true;

    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_install(rxconfig.channel, 2048, 0));

    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, rb);
    rmt_rx_start(RMT_RX_CHANNEL, true);

    Serial.println("Rf Spectrum setup complete");
    return true;
}

#endif

void rf_spectrum() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.println("");
    tft.println("  RF - Spectrum");
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) // RMT
    rmt_channel_handle_t rx_ch = NULL;
    rx_ch = setup_rf_rx();
    if (rx_ch == NULL) return;
    ESP_LOGI("RMT_SPECTRUM", "register RX done callback");
    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = spectrum_rmt_rx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_ch, &cbs, receive_queue));
    ESP_ERROR_CHECK(rmt_enable(rx_ch));
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 3000,     // 6us minimum signal duration
        .signal_range_max_ns = 12000000, // 24ms maximum signal duration
    };
    rmt_symbol_word_t item[64];
    rmt_rx_done_event_data_t rx_data;
    ESP_ERROR_CHECK(rmt_receive(rx_ch, item, sizeof(item), &receive_config));
#else

    RingbufHandle_t rb = nullptr;
    if (!setup_rf_spectrum(&rb)) return;

#ifdef FASTLED_RMT_BUILTIN_DRIVER
    // Run twice
    if (!setup_rf_spectrum(&rb)) return;
    rmt_item32_t *item;
#endif
#endif
    size_t rx_size = 0;
    while (1) {
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) // RMT
        rmt_symbol_word_t *rx_items = NULL;
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS) {
            rx_size = rx_data.num_symbols;
            rx_items = rx_data.received_symbols;
        }
        if (rx_size != 0)
#else
        item = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, 500);
        if (item != nullptr)
#endif
        {
            if (rx_size != 0) {
                // Clear the display area
                tft.fillRect(0, 20, tftWidth, tftHeight, bruceConfig.bgColor);
                // Draw waveform based on signal strength
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) // RMT
                for (size_t i = 0; i < rx_size; i++) {
                    int lineHeight = map(
                        rx_items[i].duration0 + rx_items[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0,
                        tftHeight / 2
                    );
                    int lineX = map(i, 0, rx_size - 1, 0, tftWidth - 1); // Map i to within the display width
                    int startY = constrain(20 + tftHeight / 2 - lineHeight / 2, 20, 20 + tftHeight);
                    int endY = constrain(20 + tftHeight / 2 + lineHeight / 2, 20, 20 + tftHeight);
                    tft.drawLine(lineX, startY, lineX, endY, bruceConfig.priColor);
                }
#else
                for (size_t i = 0; i < rx_size; i++) {
                    int lineHeight = map(
                        item[i].duration0 + item[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0, tftHeight / 2
                    );
                    int lineX = map(i, 0, rx_size - 1, 0, tftWidth - 1); // Map i to within the display width
                    int startY = constrain(20 + tftHeight / 2 - lineHeight / 2, 20, 20 + tftHeight);
                    int endY = constrain(20 + tftHeight / 2 + lineHeight / 2, 20, 20 + tftHeight);
                    tft.drawLine(lineX, startY, lineX, endY, bruceConfig.priColor);
                }
#endif
            }
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) // RMT
            ESP_ERROR_CHECK(rmt_receive(rx_ch, item, sizeof(item), &receive_config));
            rx_size = 0;
#else
            vRingbufferReturnItem(rb, (void *)item);
#endif
        }
        // Checks to leave while
        if (check(EscPress)) { break; }
    }
    returnToMenu = true;
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) // RMT
    rmt_disable(rx_ch);
    rmt_del_channel(rx_ch);
    vQueueDelete(receive_queue);
#else
    rmt_rx_stop(RMT_RX_CHANNEL);
    deinitRMT();
#endif
    deinitRfModule();
    delay(10);
}

#define TIME_DIVIDER (tftWidth / 10)
//@Pirata
void rf_SquareWave() {
    RCSwitch rcswitch;
    if (!initRfModule("rx", bruceConfig.rfFreq)) return;

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) rcswitch.enableReceive(bruceConfigPins.CC1101_bus.io0);
    else rcswitch.enableReceive(bruceConfig.rfRx);

    tft.drawPixel(0, 0, 0);
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.println("");
    tft.setCursor(3, 2);
    tft.println("  RF - SquareWave");
    int line_w = 0;
    int line_h = 15;
    unsigned int *raw;
    while (1) {
        if (rcswitch.RAWavailable()) {
            raw = rcswitch.getRAWReceivedRawdata();
            // Clear the display area
            // tft.fillRect(0, 0, tftWidth, tftHeight, bruceConfig.bgColor);
            // Draw waveform based on signal strength
            for (int i = 0; i < RCSWITCH_RAW_MAX_CHANGES - 1; i += 2) {
                if (raw[i] == 0) break;

                if (raw[i] > 20000) raw[i] = 20000;
                if (raw[i + 1] > 20000) raw[i + 1] = 20000;
                if (line_w + (raw[i] + raw[i + 1]) / TIME_DIVIDER > tftWidth) {
                    line_w = 10;
                    line_h += 10;
                }
                if (line_h > tftHeight) {
                    line_h = 15;
                    tft.fillRect(0, 12, tftWidth, tftHeight, bruceConfig.bgColor);
                }
                tft.drawFastVLine(line_w, line_h, 6, bruceConfig.priColor);
                tft.drawFastHLine(line_w, line_h, raw[i] / TIME_DIVIDER, bruceConfig.priColor);

                tft.drawFastVLine(line_w + raw[i] / TIME_DIVIDER, line_h, 6, bruceConfig.priColor);
                tft.drawFastHLine(
                    line_w + raw[i] / TIME_DIVIDER,
                    line_h + 6,
                    raw[i + 1] / TIME_DIVIDER,
                    bruceConfig.priColor
                );
                line_w += (raw[i] + raw[i + 1]) / TIME_DIVIDER;
            }
            rcswitch.resetAvailable();
        }
        // Checks to leave while
        if (check(EscPress)) { break; }
    }
    returnToMenu = true;
    delay(10);
}
