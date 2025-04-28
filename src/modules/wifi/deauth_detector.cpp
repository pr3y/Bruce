#include "deauth_detector.h"
#include <esp_wifi.h>
#include <WiFi.h>
#include <vector>
#include <string>
#include <iostream>

std::vector<std::string> detectedDeauthPackets;

void deauthSniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    if (type == WIFI_PKT_MGMT) {
        uint8_t *payload = pkt->payload;
        if (payload[0] == 0xC0 || payload[0] == 0xA0) { // Deauth or Disassoc frame
            std::string mac = "";
            for (int i = 10; i < 16; i++) {
                mac += String(payload[i], HEX);
                if (i < 15) mac += ":";
            }
            detectedDeauthPackets.push_back(mac);
            Serial.println("Deauth packet detected from: " + mac);
        }
    }
}

void startDeauthDetector() {
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&deauthSniffer);
    Serial.println("Deauth detector started.");
}

void stopDeauthDetector() {
    esp_wifi_set_promiscuous(false);
    Serial.println("Deauth detector stopped.");
}
