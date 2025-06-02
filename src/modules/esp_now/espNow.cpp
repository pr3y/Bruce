#include "WiFi.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "esp_now.h"
#include "mymackeyboard.h"

char messageToSend[250] = "";
String messageReceived = "";
String macAddress = "";
String success;
uint8_t peerAddress[6] = {0};

void onReceiveCallback(const uint8_t *mac_addr, const uint8_t *data, int len) {
    char incoming[251];
    if (len > 250) len = 250;
    memcpy(incoming, data, len);
    incoming[len] = '\0';
    messageReceived = String(incoming);

    tft.setTextSize(2);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.drawCentreString(messageReceived, tft.width() / 2, tft.height() / 2, 1);
}

void onSendCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
    success = (status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
    tft.setTextSize(2);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.drawCentreString(success, tft.width() / 2, tft.height() / 2, 1);
}

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

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void initEspNow() {
    if (esp_now_init() != ESP_OK) {
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        displayRedStripe("Error initializing ESP-NOW", true);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        return;
    }
    esp_now_register_recv_cb(onReceiveCallback);
    esp_now_register_send_cb(onSendCallback);
}

void Pair() {
    tft.setRotation(bruceConfig.rotation); // issues with rotation
    macAddress = MacKeyboard("", 17, "MAC ADDRESS");

    Serial.printf("Paired: \"%s\"\n", macAddress.c_str());

    if (!parseMacAddress(macAddress, peerAddress)) {
        Serial.println("Invalid format! Use XX:XX...");
        return;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        displayRedStripe("Peer added successfully", true);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    } else {
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        displayRedStripe("Failed to add peer", true);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }
}

void startPair() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(1);
    WiFi.mode(WIFI_STA);
    initEspNow();
    Pair();
}

void Send() {
    if (strlen(messageToSend) == 0) {
        Serial.println("No message!");
        return;
    }
    esp_err_t result = esp_now_send(peerAddress, (uint8_t *)messageToSend, strlen(messageToSend));
    if (result != ESP_OK) {
        char errorMsg[50];
        snprintf(errorMsg, sizeof(errorMsg), "Failed! error: 0x%x", result);
        tft.setTextSize(2);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawCentreString(errorMsg, tft.width() / 2, tft.height() / 2, 1);
    }
}

void loopForEspNow() {
    while (true) {
        if (check(PrevPress)) { return; }
        String inputMsg = keyboard("", 76, "Message:");

        Serial.printf("Message: \"%s\"\n", inputMsg.c_str());

        if (inputMsg.length() > 0) {
            strncpy(messageToSend, inputMsg.c_str(), sizeof(messageToSend));
            messageToSend[sizeof(messageToSend) - 1] = '\0';
            Send();
        } else {
            Serial.println("No message");
        }

        delay(10);
    }
}
