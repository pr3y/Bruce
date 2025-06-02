#include "WiFi.h"
#include "core/display.h"
#include "esp_now.h"
#include "mykeyboard.h"

bool parseMacAddress(const String &macStr, uint8_t *macArray) {
    int values[6];
    if (sscanf(
            macStr.c_str(),
            "%x:%x:%x:%x:%x:%x",
            &values[0],
            &values[1],
            &values[2],
            &values[3],
            &values[4],
            &values[5]
        ) == 6) {
        for (int i = 0; i < 6; ++i) { macArray[i] = (uint8_t)values[i]; }
        return true;
    }
    return false;
}

String macAddress = "";
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void initEspNow() {
    if (esp_now_init() != ESP_OK) {
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        displayRedStripe("Error initializing ESP-NOW", true);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        return;
    }

    // If you want to register a callback, etc., do it here:
    // esp_now_register_send_cb(OnDataSent);
    // esp_now_register_recv_cb(OnDataRecv);
}

void Pair() {
    macAddress = keyboard("", 17, "MAC ADDRESS");

    uint8_t peerAddress[6];
    if (!parseMacAddress(macAddress, peerAddress)) {
        Serial.println("Invalid format! Use XX:XX...");
        return;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    //     Serial.printf("Peer %s added successfully!\n", macAddress.c_str());
    // } else {
    //     Serial.printf("Failed to add peer %s\n", macAddress.c_str());
    // }
}

void startPair() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    initEspNow();
    Pair();
}
