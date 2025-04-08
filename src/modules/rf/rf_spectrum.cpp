#include "rf_spectrum.h"
#include "rf_utils.h"
#include "structs.h"
#include <RCSwitch.h>

bool setup_rf_spectrum(RingbufHandle_t *rb) {
    if (!initRfModule("rx", bruceConfig.rfFreq)) return false;
    initRMT();
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, rb);
    rmt_rx_start(RMT_RX_CHANNEL, true);

    Serial.println("Rf Spectrum setup complete");
    return true;
}

//@IncursioHack - https://github.com/IncursioHack ----thanks @aat440hz - RF433ANY-M5Cardputer
void rf_spectrum() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.println("");
    tft.println("  RF - Spectrum");

    RingbufHandle_t rb = nullptr;
    if (!setup_rf_spectrum(&rb)) return;

#ifdef FASTLED_RMT_BUILTIN_DRIVER
    // Run twice
    if (!setup_rf_spectrum(&rb)) return;
#endif

    while (rb) {
        size_t rx_size = 0;
        rmt_item32_t *item = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, 500);
        if (item != nullptr) {
            if (rx_size != 0) {
                // Clear the display area
                tft.fillRect(0, 20, tftWidth, tftHeight, bruceConfig.bgColor);
                // Draw waveform based on signal strength
                for (size_t i = 0; i < rx_size; i++) {
                    int lineHeight = map(
                        item[i].duration0 + item[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0, tftHeight / 2
                    );
                    int lineX = map(i, 0, rx_size - 1, 0, tftWidth - 1); // Map i to within the display width
                    // Ensure drawing coordinates stay within the box bounds
                    int startY = constrain(20 + tftHeight / 2 - lineHeight / 2, 20, 20 + tftHeight);
                    int endY = constrain(20 + tftHeight / 2 + lineHeight / 2, 20, 20 + tftHeight);
                    tft.drawLine(lineX, startY, lineX, endY, bruceConfig.priColor);
                }
            }
            vRingbufferReturnItem(rb, (void *)item);
        }
        // Checks to leave while
        if (check(EscPress)) { break; }
    }
    returnToMenu = true;
    rmt_rx_stop(RMT_RX_CHANNEL);
    deinitRMT();
    delay(10);
}

//@Pirata
void rf_SquareWave() {
    RCSwitch rcswitch;
    if (!initRfModule("rx", bruceConfig.rfFreq)) return;

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) rcswitch.enableReceive(bruceConfig.CC1101_bus.io0);
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
#define TIME_DIVIDER tftWidth / 30
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
    rmt_rx_stop(RMT_RX_CHANNEL);
    delay(10);
}
