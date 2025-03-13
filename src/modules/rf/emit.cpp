#include "emit.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

void rf_raw_emit_draw(uint16_t rssiCount, bool fakeRssiPresent){
    // Calculate bar dimensions
    int centerY = (TFT_WIDTH / 2) + 20;       // Center axis for the bars
    int maxBarHeight = (TFT_WIDTH / 2) - 50; // Maximum height of the bars

    // Draw the latest bar
    int rssi = -90;
    if(fakeRssiPresent) rssi = -45;
    // Normalize RSSI to bar height (RSSI values are typically negative)
    int barHeight = map(rssi, -90, -45, 1, maxBarHeight);

    // Calculate bar position
    int x = 20 + (int)(rssiCount * 1.35);
    int yTop = centerY - barHeight;

    // Draw the bar
    tft.drawFastVLine(x, yTop, barHeight * 2, bruceConfig.priColor);
}

void rf_raw_emit(struct RawRecording recorded) {
    bool returnToMenu = false;
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.print("Emitting: ");
    tft.print(recorded.frequency);
    tft.print(" MHz");
    tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
    tft.println("   Press [OK] to stop ");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

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
        if(check(SelPress)) break;
        if(check(EscPress)) {
            returnToMenu = true;
            break;
        }
    }
    deinitRfModule();
    
    if(returnToMenu) return;

    int option=0;
    options = {
        { "Replay",  [&]()  { option = 1; } },
        { "Save",    [&]()  { option = 2; } },
        { "Emit",    [&]()  { option = 3; } },
    };
    loopOptions(options);
    
    if(option == 1){ // Replay
        rf_raw_emit(recorded);
    }
    return;
}