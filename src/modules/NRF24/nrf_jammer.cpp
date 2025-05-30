#include "nrf_jammer.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "nrf_common.h"
#include <globals.h>

/* **************************************************************************************
 ** name : nrf_jammer
 ** details : Starts 2.4Gz jammer usinf NRF24
 ************************************************************************************** */
void nrf_jammer() {
    RF24 radio(NRF24_CE_PIN, NRF24_SS_PIN);
    // https://www.allaboutcircuits.com/uploads/articles/Bluetooth_and_WLAN_frequencies.jpg
    byte wifi_channels[] = {
        6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,         // Channel 1
        22, 24, 26, 28,                                             // mid band
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, // Channel 6
        46, 48, 50, 52,                                             // mid band
        55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,     // Channel 11
    }; // WiFi Channels
    byte ble_channels[] = {1, 2, 3, 25, 26, 27, 79, 80, 81}; // BLE Advertising Channels: 2, 26 and 80
    byte bluetooth_channels[] = {
        // 1,  3,  5,  7,  9,  11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, // odds
        // 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, // odds
        2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, // even
        42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80  // even
    }; // Bluetooth Channels
    byte usb_channels[] = {40, 50, 60};   // USB Wireless Channels
    byte video_channels[] = {70, 75, 80}; // Video Streaming Channels
    byte rc_channels[] = {1, 3, 5, 7};    // RC Toys/ Drones Channels
    byte full_channels[] = {
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
        22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
        43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
        85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
        //, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
        // 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124
    }; // All Channels

    if (nrf_start()) {
        Serial.println("NRF24 turned On");

        int NRF_MODE_N = 1;
        int NRF_MODE_N_X = 0;
        String NRF_MODE = "WiFi";

        // String NRF_STATUS = "OFF";
        int NRF_STATUS_X = 0;

        int ptr_hop = 0;

        NRFradio.setPALevel(RF24_PA_MAX);
        NRFradio.startConstCarrier(RF24_PA_MAX, 45);
        NRFradio.setAddressWidth(3); // optional
        NRFradio.setPayloadSize(2);  // optional
        if (!NRFradio.setDataRate(RF24_2MBPS)) Serial.println("Fail setting data Rate");
        bool redraw = true;
        drawMainBorder();
        while (!check(SelPress)) {
            if (redraw) {
                tft.setCursor(10, 35);
                tft.setTextSize(FM);
                tft.println("NRF X Jammer ");
                tft.setCursor(10, tft.getCursorY() + 25);
                tft.println("STATUS : ACTIVE");
                tft.setCursor(10, 100);
                tft.fillRect(10, 100, tftWidth - 20, FM * LH, bruceConfig.bgColor);
                tft.print("MODE : " + NRF_MODE);
                if (NRF_MODE_N == 1) {
                    NRF_MODE = "WiFi        ";
                    Serial.println("WiFi");
                } else if (NRF_MODE_N == 2) {
                    NRF_MODE = "BLE Adv ch. ";
                    Serial.println("BLE Advertising");
                } else if (NRF_MODE_N == 3) {
                    NRF_MODE = "Bluetooth   ";
                    Serial.println("Bluetooth   ");
                } else if (NRF_MODE_N == 4) {
                    NRF_MODE = "USB         ";
                    Serial.println("USB");
                } else if (NRF_MODE_N == 5) {
                    NRF_MODE = "Video Stream";
                    Serial.println("Video Stream");
                } else if (NRF_MODE_N == 6) {
                    NRF_MODE = "RC          ";
                    Serial.println("RC");
                } else if (NRF_MODE_N == 7) {
                    NRF_MODE = "Full        ";
                    Serial.println("Full");
                }
                tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, bruceConfig.priColor);
                delay(200);
                redraw = false;
            }
            ptr_hop++;
            if (NRF_MODE_N == 1) {

                if (ptr_hop >= sizeof(wifi_channels)) ptr_hop = 0;
                NRFradio.setChannel(wifi_channels[ptr_hop]);
            } else if (NRF_MODE_N == 2) {
                if (ptr_hop >= sizeof(ble_channels)) ptr_hop = 0;
                NRFradio.setChannel(ble_channels[ptr_hop]);
            } else if (NRF_MODE_N == 3) {
                if (ptr_hop >= sizeof(bluetooth_channels)) ptr_hop = 0;
                NRFradio.setChannel(bluetooth_channels[ptr_hop]);
            } else if (NRF_MODE_N == 4) {
                if (ptr_hop >= sizeof(usb_channels)) ptr_hop = 0;
                NRFradio.setChannel(usb_channels[ptr_hop]);
            } else if (NRF_MODE_N == 5) {
                if (ptr_hop >= sizeof(video_channels)) ptr_hop = 0;
                NRFradio.setChannel(video_channels[ptr_hop]);
            } else if (NRF_MODE_N == 6) {
                if (ptr_hop >= sizeof(rc_channels)) ptr_hop = 0;
                NRFradio.setChannel(rc_channels[ptr_hop]);
            } else if (NRF_MODE_N == 7) {
                if (ptr_hop >= sizeof(full_channels)) ptr_hop = 0;
                NRFradio.setChannel(full_channels[ptr_hop]);
            }

            // Change channel
            if (check(NextPress)) {
                if (NRF_MODE_N < 7) {
                    NRF_MODE_N++;
                } else {
                    NRF_MODE_N = 1;
                }
                ptr_hop = 0;
            }

            if (check(PrevPress)) {
                if (NRF_MODE_N < 2) {
                    NRF_MODE_N = 7;
                } else {
                    NRF_MODE_N--;
                }
                ptr_hop = 0;
            }

            if (NRF_MODE_N != NRF_MODE_N_X) {
                redraw = true;
                NRF_MODE_N_X = NRF_MODE_N;
            }
        }

        NRFradio.stopConstCarrier(); // this will stop jamming without powering down nrf
                                     // NRFradio.powerDown(); power down without powering up on scanner

    } else {
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
    }
}

void nrf_channel_jammer() {
    if (nrf_start()) {
        Serial.println("NRF24 turned On");

        int NRF_MODE_N = 1;
        int NRF_FREQ = 400 + NRF_MODE_N;
        int NRF_MODE_N_X = 0;
        String NRF_MODE = "CH 1  ";

        // String NRF_STATUS = "OFF";
        int NRF_STATUS_X = 0;

        int ptr_hop = 0;

        NRFradio.setPALevel(RF24_PA_MAX);
        NRFradio.startConstCarrier(RF24_PA_MAX, 45);
        NRFradio.setAddressWidth(3); // optional
        NRFradio.setPayloadSize(2);  // optional
        if (!NRFradio.setDataRate(RF24_2MBPS)) Serial.println("Fail setting data Rate");

        bool redraw = true;
        drawMainBorder();
        while (!check(SelPress)) {
            if (redraw) {
                tft.setCursor(10, 35);
                tft.setTextSize(FM);
                tft.println("NRF Channel Jammer");
                tft.setCursor(10, tft.getCursorY() + 25);
                tft.println("STATUS : ACTIVE");
                tft.fillRect(10, 100, tftWidth - 20, FM * LH, bruceConfig.bgColor);
                tft.setCursor(10, 100);
                tft.print("MODE : " + NRF_MODE + "  ");
                tft.setCursor(10, 116);
                tft.fillRect(10, 116, tftWidth - 20, FM * LH, bruceConfig.bgColor);
                tft.printf("Freq : 2.%03dGhz", NRF_FREQ);
                Serial.println("CH " + String(NRF_MODE_N));
                tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, bruceConfig.priColor);
                redraw = false;
                delay(200);
            }

            if (check(NextPress)) {
                if (NRF_MODE_N < 127) {
                    NRF_MODE_N++;
                } else {
                    NRF_MODE_N = 1;
                }
                NRF_MODE = "CH " + String(NRF_MODE_N);
                NRFradio.setChannel(NRF_MODE_N);
            }
            if (check(PrevPress)) {
                if (NRF_MODE_N < 2) {
                    NRF_MODE_N = 127;
                } else {
                    NRF_MODE_N--;
                }
                NRF_MODE = "CH " + String(NRF_MODE_N);
                NRFradio.setChannel(NRF_MODE_N);
            }
            if (NRF_MODE_N != NRF_MODE_N_X) {
                NRF_MODE_N_X = NRF_MODE_N;
                redraw = true;
            }
        }

        NRFradio.stopConstCarrier(); // this will stop jamming without powering down nrf
                                     // NRFradio.powerDown(); power down without powering up on scanner

    } else {
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
    }
}
