#include "modules/wifi/listenTCP.h"
#include "core/wifi_common.h"

void listenTcpPort() {
    if (!wifiConnected) wifiConnectMenu();

    WiFiClient tcpClient;
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    String portNumber = keyboard("", 5, "TCP port to listen");
    int portNumberInt = atoi(portNumber.c_str());

    WiFiServer tcpServer(portNumberInt);
    tcpServer.begin();

    tft.println("Listening...");
    tft.print(WiFi.localIP().toString().c_str());
    tft.println(":" + portNumber);

    bool inputMode;

    for (;;) {
        WiFiClient client = tcpServer.available();  // Wait for a client to connect

        if (client) {
            Serial.println("Client connected");
            tft.println("Client connected");

            while (client.connected()) {
                if (inputMode) {
                    String keyString = keyboard("", 16, "send input data");
                    delay(300);
                    inputMode = false;
                    tft.fillScreen(TFT_BLACK);
                    tft.setCursor(0,0);
                    if (keyString.length() > 0) {
                        client.print(keyString);  // Send the entire string to the client
                        Serial.print(keyString);
                    }
                } else {
                    if (client.available()) {
                        char incomingChar = client.read();  // Read one byte at time from the client
                        tft.print(incomingChar);
                        Serial.print(incomingChar);
                    }
                    if (checkSelPress()) {
                        delay(300);
                        inputMode = true;
                    }
                }
            }
            client.stop();
            Serial.println("Client disconnected");
            displayError("Client disconnected");

        }
        if (checkEscPress()) {
            displayError("Exiting Listener");
            tcpServer.stop();
            break;
        }
    }
}
