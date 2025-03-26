#include "record.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

// RMT configuration
#define RMT_MAX_PULSES 10000 // Maximum number of pulses to record
#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

void init_rmt_raw_recording() {
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_uninstall((rmt_channel_t)RMT_RX_CHANNEL));

    rmt_config_t rxconfig;
    rxconfig.gpio_num            = gpio_num_t(bruceConfig.rfRx);
    #ifdef USE_CC1101_VIA_SPI
    if(bruceConfig.rfModule==CC1101_SPI_MODULE)
        rxconfig.gpio_num            = gpio_num_t(bruceConfig.CC1101_bus.io0);
    #endif
    rxconfig.rmt_mode            = RMT_MODE_RX;
    rxconfig.channel             = RMT_RX_CHANNEL;
    rxconfig.clk_div             = RMT_CLK_DIV; // RMT_DEFAULT_CLK_DIV=32
    rxconfig.mem_block_num       = 2;
    rxconfig.flags               = 0;
    rxconfig.rx_config.idle_threshold = 4300,
    rxconfig.rx_config.filter_ticks_thresh = 5;
    rxconfig.rx_config.filter_en = true;

    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_install(RMT_RX_CHANNEL, 8192, 0));
}

float phase = 0.0;
float lastPhase = 2 * PI;
unsigned long lastAnimationUpdate = 0;
void sinewave_animation(){
    if(millis() - lastAnimationUpdate < 10) return;

    tft.drawPixel(0,0,0);

    int centerY = (TFT_WIDTH / 2) + 20;
    int amplitude = (TFT_WIDTH / 2) - 40;
    int sinewaveWidth = 5;

    for (int x = 20; x < TFT_HEIGHT - 20; x++) {
        int lastY = centerY + amplitude * sin(lastPhase + x * 0.05);
        int y = centerY + amplitude * sin(phase + x * 0.05);
        tft.drawFastVLine(x, lastY, sinewaveWidth, bruceConfig.bgColor);
        tft.drawFastVLine(x, y, sinewaveWidth, bruceConfig.priColor);
    }

    lastPhase = phase;
    phase += 0.15;
    if (phase >= 2 * PI) {
        phase = 0.0;
    }
    lastAnimationUpdate = millis();
}

void rf_raw_record_draw(RawRecordingStatus status) {
    tft.setCursor(20, 38);
    tft.setTextSize(FP);
    if (status.frequency <= 0) {
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.print("Looking for frequency...");
        tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
        tft.println("   Press [ESC] to exit  ");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        // The frequency scan function calls the animation
    }else if (!status.recordingStarted) {
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.print("Waiting for signal...");
        sinewave_animation();
    } else if(status.recordingFinished){
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.print("Recording finished.");
        tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
        tft.println("   Press [OK] to save   ");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }else if(status.latestRssi < 0) {
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.print("Recording: ");
        tft.print(status.frequency);
        tft.print(" MHz");
        tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
        tft.println("   Press [OK] to stop ");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        // Calculate bar dimensions
        int centerY = (TFT_WIDTH / 2) + 20;       // Center axis for the bars
        int maxBarHeight = (TFT_WIDTH / 2) - 50; // Maximum height of the bars

        // Draw the latest bar
        int rssi = status.latestRssi;
        // Normalize RSSI to bar height (RSSI values are typically negative)
        int barHeight = map(rssi, -90, -45, 1, maxBarHeight);

        // Calculate bar position
        int x = 20 + (int)(status.rssiCount * 1.35);
        int yTop = centerY - barHeight;

        // Draw the bar
        tft.drawFastVLine(x, yTop, barHeight * 2, bruceConfig.priColor);
    }
}

//TODO: replace frequency scans throughout rf.cpp with this unified function
#define FREQUENCY_SCAN_MAX_TRIES 5
float rf_freq_scan(){
    float frequency = 0;
    int idx = range_limits[bruceConfig.rfScanRange][0];
    uint8_t attempt = 0;
    int rssi=-80, rssiThreshold = -65;

	FreqFound best_frequencies[FREQUENCY_SCAN_MAX_TRIES];
    for(int i=0; i<FREQUENCY_SCAN_MAX_TRIES;i++) {best_frequencies[i].freq=433.92; best_frequencies[i].rssi=-75; }

    while(frequency <= 0 && !check(EscPress)){ // FastScan
        sinewave_animation();
        previousMillis = millis();
        #if defined(USE_CC1101_VIA_SPI)

        if (idx < range_limits[bruceConfig.rfScanRange][0] || idx > range_limits[bruceConfig.rfScanRange][1]) {
            idx = range_limits[bruceConfig.rfScanRange][0];
        }
        float checkFrequency = subghz_frequency_list[idx];
        ELECHOUSE_cc1101.setMHZ(checkFrequency);
        tft.drawPixel(0,0,0); // To make sure CC1101 shared with TFT works properly
        delay(5);
        rssi = ELECHOUSE_cc1101.getRssi();
        if (rssi > rssiThreshold) {
            best_frequencies[attempt].freq = checkFrequency;
            best_frequencies[attempt].rssi = rssi;
            attempt++;
            if (attempt >= FREQUENCY_SCAN_MAX_TRIES) {
                int max_index = 0;
                for (int i = 1; i < FREQUENCY_SCAN_MAX_TRIES; ++i) {
                    if (best_frequencies[i].rssi > best_frequencies[max_index].rssi) {
                        max_index = i;
                    }
                }

                bruceConfig.setRfFreq(best_frequencies[max_index].freq, 0);
                frequency = best_frequencies[max_index].freq;
                Serial.println("Frequency Found: " + String(frequency));
            }
        }
        ++idx;
        #else
        frequency = 433.92;
        bruceConfig.setRfFreq(433.92, 2);
        #endif
    }
    return frequency;
}

//TODO: replace frequency selection throughout rf.cpp with this unified function
void rf_range_selection(float currentFrequency = 0.0) {
    int option=0;
    options = {
        { String("Fixed [" + String(bruceConfig.rfFreq) + "]").c_str(), [=]()  { bruceConfig.setRfFreq(bruceConfig.rfFreq,2); } },
        { String("Choose Fixed").c_str(), [&]()  { option = 1; } },
        { subghz_frequency_ranges[0], [=]()  { bruceConfig.setRfScanRange(0); } },
        { subghz_frequency_ranges[1], [=]()  { bruceConfig.setRfScanRange(1); } },
        { subghz_frequency_ranges[2], [=]()  { bruceConfig.setRfScanRange(2); } },
        { subghz_frequency_ranges[3], [=]()  { bruceConfig.setRfScanRange(3); } },
    };

    loopOptions(options);
    options.clear();

    if(option == 1) { // Fixed Frequency Selector
        options = {};
        int ind=0;
        int arraySize = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
        for(int i=0; i<arraySize;i++) {
            String tmp = String(subghz_frequency_list[i], 2) + "Mhz";
            options.push_back({ tmp.c_str(), [=]()  { bruceConfig.setRfFreq(subghz_frequency_list[i],2); } });
            if(int(currentFrequency*100)==int(subghz_frequency_list[i]*100)) ind=i;
        }
        loopOptions(options,ind);
        options.clear();
    }

    if (bruceConfig.rfFxdFreq) displayTextLine("Scan freq set to " + String(bruceConfig.rfFreq));
    else displayTextLine("Range set to " + String(subghz_frequency_ranges[bruceConfig.rfScanRange]));
}

void rf_raw_record_create(RawRecording &recorded, bool &returnToMenu) {
    RawRecordingStatus status;
    RingbufHandle_t rb;

    bool fakeRssiPresent = false;
    bool rssiFeature = false;
    #ifdef USE_CC1101_VIA_SPI
    rssiFeature = bruceConfig.rfModule==CC1101_SPI_MODULE;
    #endif

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();

    #ifdef USE_CC1101_VIA_SPI
    if(rssiFeature)rf_range_selection(bruceConfig.rfFreq);
    #endif

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();
    rf_raw_record_draw(status);

    // Initialize RF module and update display
    initRfModule("rx", 433.92); // Frequency scan doesnt work when initializing the module with a different frequency
    Serial.println("RF Module Initialized");

    // Set frequency if fixed frequency mode is enabled
    #ifdef USE_CC1101_VIA_SPI
    if (bruceConfig.rfFxdFreq || !rssiFeature) status.frequency = bruceConfig.rfFreq;
    else status.frequency = rf_freq_scan();
    #else
    status.frequency = bruceConfig.rfFreq;
    #endif

    //Something went wrong with scan, probably it was cancelled
    if(status.frequency < 300) return;
    recorded.frequency = status.frequency;
    setMHZ(status.frequency);

    // Erase sinewave animation
    tft.drawPixel(0,0,0);
    tft.fillRect(10, 30, TFT_HEIGHT - 20, TFT_WIDTH - 40, bruceConfig.bgColor);
    rf_raw_record_draw(status);

    // Start recording
    delay(200);
    init_rmt_raw_recording();
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
    if (rb == NULL) {
        Serial.println("Failed to get ring buffer handle!");
        return; // Exit if ring buffer handle is not valid
    }
    rmt_rx_start(RMT_RX_CHANNEL, true);
    Serial.println("RMT Initialized");

    while (!status.recordingFinished) {
        previousMillis = millis();
        size_t rx_size = 0;
        rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 0);

        if (item != nullptr) {
            if (rx_size >= 5 * sizeof(rmt_item32_t)) { // ignore codes shorter than 5 items
                fakeRssiPresent = true; // For rssi display on single-pinned RF Modules
                size_t item_count = rx_size / sizeof(rmt_item32_t);
                rmt_item32_t* code = (rmt_item32_t*)malloc(rx_size);

                // Gap calculation
                unsigned long receivedTime = millis();
                unsigned long long signalDuration = 0;
                for (size_t i = 0; i < item_count; i++) {
                    code[i] = item[i];
                    signalDuration += item[i].duration0 + item[i].duration1;
                }
                recorded.codes.push_back(code);
                recorded.codeLengths.push_back(item_count);

                if (status.lastSignalTime != 0) {
                    unsigned long signalDurationMs = signalDuration / RMT_1MS_TICKS;
                    uint16_t gap = (uint16_t)(receivedTime - status.lastSignalTime - signalDurationMs - 5);
                    recorded.gaps.push_back(gap);
                }else{
                    status.firstSignalTime = receivedTime;
                    status.recordingStarted = true;
                    // Erase sinewave animation
                    tft.drawPixel(0,0,0);
                    tft.fillRect(10, 30, TFT_HEIGHT - 20, TFT_WIDTH - 40, bruceConfig.bgColor);
                }
                status.lastSignalTime = receivedTime;
            }
            vRingbufferReturnItem(rb, (void*)item);
        }

        // Periodically update RSSI
        if (status.recordingStarted && (status.lastRssiUpdate == 0 || millis() - status.lastRssiUpdate >= 100)) {
            if(fakeRssiPresent) status.latestRssi = -45;
            else status.latestRssi = -90;
            fakeRssiPresent = false;

            #ifdef USE_CC1101_VIA_SPI
            if(rssiFeature) status.latestRssi = ELECHOUSE_cc1101.getRssi();
            #endif

            status.rssiCount++;
            status.lastRssiUpdate = millis();
        }

        //Stop recording after 20 seconds
        if(status.firstSignalTime > 0 && millis() - status.firstSignalTime >= 20000) status.recordingFinished = true;
        if(check(SelPress) && status.recordingStarted) status.recordingFinished = true;
        if(check(EscPress)) {
            status.recordingFinished = true;
            returnToMenu = true;
        }
        rf_raw_record_draw(status);
    }
    Serial.println("Recording stopped.");
    rmt_rx_stop(RMT_RX_CHANNEL);
    deinitRMT();
    deinitRfModule();
}

int rf_raw_record_options(bool saved){
    int option=0;
    options = {
        { "Replay",  [&]()  { option = 1; } },
        { "Save",    [&]()  { option = 2; } },
        { "Exit",    [&]()  { option = 4; } },
    };
    if(saved){
        options.erase(options.begin() + 1);
        options.insert(options.begin() + 1, { "Record another", [&]()  { option = 3; } });
    }else{
        options.insert(options.begin() + 1, { "Discard", [&]()  { option = 3; } });
    }
    loopOptions(options);

    return option;
}

void rf_raw_record(){
    bool replaying = false;
    bool returnToMenu = false;
    bool saved = false;
    int option = 3;
    RawRecording recorded;
    while(option != 4){
        if(option == 1){ // Replay
            rf_raw_emit(recorded, returnToMenu);
        }else if(option == 2){ // Save
            saved = true;
            rf_raw_save(recorded);
        }else if(option == 3){ // Discard
            saved=false;
            for(auto &code : recorded.codes) free(code);
            recorded.codes.clear();
            recorded.codeLengths.clear();
            recorded.gaps.clear();
            recorded.frequency = 0;
            rf_raw_record_create(recorded, returnToMenu);
        }

        if(returnToMenu || check(EscPress)) return;
        option = rf_raw_record_options(saved);
    }
    for(auto &code : recorded.codes) free(code);
    recorded.codes.clear();
    recorded.codeLengths.clear();
    recorded.gaps.clear();
    recorded.frequency = 0;
    return;
}
