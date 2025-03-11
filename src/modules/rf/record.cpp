#include "record.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "rf.h"
#include <driver/rmt.h>

// RMT configuration
#define RMT_MAX_PULSES 10000 // Maximum number of pulses to record
#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

// RMT buffer to store raw signal data
rmt_item32_t rmt_buffer[RMT_MAX_PULSES];

void new_initRMT() {
    deinitRMT();

    rmt_config_t rxconfig;
    rxconfig.rmt_mode            = RMT_MODE_RX;
    rxconfig.channel             = RMT_RX_CHANNEL;
    rxconfig.gpio_num            = gpio_num_t(bruceConfig.rfRx);
    #ifdef USE_CC1101_VIA_SPI
    if(bruceConfig.rfModule==CC1101_SPI_MODULE)
        rxconfig.gpio_num            = gpio_num_t(bruceConfig.CC1101_bus.io0);
    #endif
    rxconfig.clk_div             = RMT_CLK_DIV; // RMT_DEFAULT_CLK_DIV=32
    rxconfig.mem_block_num       = 2;
    rxconfig.flags               = 0;
    rxconfig.rx_config.idle_threshold = 4300,
    rxconfig.rx_config.filter_ticks_thresh = 5;
    rxconfig.rx_config.filter_en = true;

    // rmt_set_mem_block_num(RMT_RX_CHANNEL, 4);
    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_install(RMT_RX_CHANNEL, 32768, 0));
}

void rf_raw_record_draw(RawRecordingStatus status) {
    tft.setCursor(20, 38);
    tft.setTextSize(FP);
    if (status.frequency <= 0) {
        tft.println("Looking for frequency...");
    } else if(status.latestRssi < 0) {
        tft.print("Recording: ");
        tft.print(status.frequency);
        tft.println(" MHz");
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

#define FREQUENCY_SCAN_MAX_TRIES 5

float phase = 0.0;
unsigned long lastAnimationUpdate = 0;

void sinewave_animation(){
    if(millis() - lastAnimationUpdate < 100) return;

    tft.fillRect(10, 50, TFT_HEIGHT - 20, TFT_WIDTH - 60, bruceConfig.bgColor);
    tft.fillRect(10, 50, TFT_HEIGHT - 20, TFT_WIDTH - 60, bruceConfig.bgColor); // At least the T-Embed CC1101 needs both calls
    
    int centerY = (TFT_WIDTH / 2) + 20;
    int amplitude = (TFT_WIDTH / 2) - 40;
    int squareSize = 5;
    int halfSize = squareSize / 2;
    
    for (int x = 20; x < TFT_HEIGHT - 20; x++) {
        int y = centerY + amplitude * sin(phase + x * 0.05);
        tft.fillRect(x - halfSize, y - halfSize, squareSize, squareSize, bruceConfig.priColor);
    }

    phase += 1;
    if (phase >= 2 * PI) {
        phase = 0.0;
    }
    lastAnimationUpdate = millis();
}

//TODO: replace frequency scans throughout rf.cpp with this unified function
float rf_freq_scan(){
    float frequency = 0;
    int idx = range_limits[bruceConfig.rfScanRange][0];
    uint8_t attempt = 0;
    int rssi=-80, rssiThreshold = -65;
    
	FreqFound best_frequencies[FREQUENCY_SCAN_MAX_TRIES];
    for(int i=0; i<FREQUENCY_SCAN_MAX_TRIES;i++) {best_frequencies[i].freq=433.92; best_frequencies[i].rssi=-75; }
    
    while(frequency <= 0){ // FastScan
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
        displayWarning("Freq Scan not available", true);
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
        { String("Fixed [" + String(bruceConfig.rfFreq) + "]").c_str(), [=]()  { bruceConfig.setRfScanRange(bruceConfig.rfScanRange, 1); } },
        { String("Choose Fixed").c_str(), [&]()  { option = 1; } },
        { subghz_frequency_ranges[0], [=]()  { bruceConfig.setRfScanRange(0); } },
        { subghz_frequency_ranges[1], [=]()  { bruceConfig.setRfScanRange(1); } },
        { subghz_frequency_ranges[2], [=]()  { bruceConfig.setRfScanRange(2); } },
        { subghz_frequency_ranges[3], [=]()  { bruceConfig.setRfScanRange(3); } },
    };

    loopOptions(options);

    if(option == 1) { // Fixed Frequency Selector
        options = {};
        int ind=0;
        int arraySize = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
        for(int i=0; i<arraySize;i++) {
            options.push_back({ String(String(subghz_frequency_list[i],2) + "Mhz").c_str(), [=]()  { bruceConfig.rfFreq=subghz_frequency_list[i]; } });
            if(int(currentFrequency*100)==int(subghz_frequency_list[i]*100)) ind=i;
        }
        loopOptions(options,ind);
        bruceConfig.setRfScanRange(bruceConfig.rfScanRange, 1);
    }

    if (bruceConfig.rfFxdFreq) displayTextLine("Scan freq set to " + String(bruceConfig.rfFreq));
    else displayTextLine("Range set to " + String(subghz_frequency_ranges[bruceConfig.rfScanRange]));
}

void rf_raw_record() {
    RawRecordingStatus status;

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();
    
    rf_range_selection(status.frequency);
    
    new_initRMT();

    RingbufHandle_t rb;
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
    
    if (rb == NULL) {
        Serial.println("Failed to get ring buffer handle!");
        return; // Exit if ring buffer handle is not valid
    }
    // Initialize RF module and update display
    initRfModule("rx", bruceConfig.rfFreq);
    Serial.println("RF Module Initialized");

    // Start recording
    rmt_rx_start(RMT_RX_CHANNEL, true);
    Serial.println("RMT Initialized");

    // Set frequency if fixed frequency mode is enabled
    if (bruceConfig.rfFxdFreq) {
        status.frequency = bruceConfig.rfFreq;
    }
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();
    rf_raw_record_draw(status);

    // Scan for frequency if not set
    if (status.frequency <= 0) {
        status.frequency = rf_freq_scan();
    }

    bool recording = true;
    bool returnToMenu = false;

    tft.fillRect(10, 30, TFT_HEIGHT - 20, TFT_WIDTH - 40, bruceConfig.bgColor);
    tft.fillRect(10, 30, TFT_HEIGHT - 20, TFT_WIDTH - 40, bruceConfig.bgColor);
    rf_raw_record_draw(status);

    while (recording) {
        previousMillis = millis();
        rf_raw_record_draw(status);
        size_t rx_size = 0;
        rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 0);
    
        if (item != nullptr) {
            if (rx_size != 0) {
                // For gap calculation
                unsigned long receivedTime = millis();
                unsigned long long signalDuration = 0;

                // Read RMT buffer
                size_t item_count = rx_size / sizeof(rmt_item32_t);
                for (size_t i = 0; i < item_count; i++) {
                    rmt_buffer[i] = item[i];
                    signalDuration += item[i].duration0 + item[i].duration1;
                }

                // Gap calculation
                if (status.lastSignalTime != 0) {
                    unsigned long signalDurationMs = signalDuration / RMT_1MS_TICKS;
                    uint16_t gap = (uint16_t)(receivedTime - status.lastSignalTime - signalDurationMs - 5);
                }else{
                    status.firstSignalTime = receivedTime;
                }
                status.lastSignalTime = receivedTime;
            }
            vRingbufferReturnItem(rb, (void*)item);
        }
    
        // Periodically update RSSI
        if (status.lastRssiUpdate == 0 || millis() - status.lastRssiUpdate >= 100) {
            int rssi = ELECHOUSE_cc1101.getRssi();
            status.rssiCount++;
            status.latestRssi = rssi;
            status.lastRssiUpdate = millis();
        }

        //Stop recording after 20 seconds
        if(status.firstSignalTime > 0 && millis() - status.firstSignalTime >= 20000) recording = false;
        if(check(SelPress)) recording = false;
        if(check(EscPress)) {
            returnToMenu = true;
            recording = false;
        }
    }
    Serial.println("Recording stopped.");
    rmt_rx_stop(RMT_RX_CHANNEL);
    deinitRMT();
    deinitRfModule();

    while(!returnToMenu && !check(EscPress)) delay(100);
}