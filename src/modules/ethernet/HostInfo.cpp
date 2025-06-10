/**
 * @file HostInfo.cpp
 * @brief HostInfo module for every esp-netif
 * @version 0.1
 * @date 2025-05-15
 */

#include "HostInfo.h"
#include "ESPNetifEthernetClient.h"
#include "core/display.h"
#include "core/net_utils.h"

HostInfo::HostInfo(const Host &host, bool wifi) {
#if !defined(LITE_VERSION)
    if (!wifi) {
        eth_client = new ESPNetifEthernetClient();
    } else {
        wifi_client = new WiFiClient();
    }
#else
    wifi_client = new WiFiClient();
#endif

    setup(host);
}

HostInfo::~HostInfo() {
#if !defined(LITE_VERSION)
    if (eth_client != nullptr) {
        delete eth_client;
    } else {
        delete wifi_client;
    }
#else
    delete wifi_client;
#endif
}

struct PortScan { // struct pra holdar info das portas
    int port;
    unsigned long startTime;
    bool inProgress;
};

void HostInfo::client_stop() {
#if !defined(LITE_VERSION)
    if (eth_client != nullptr) {
        eth_client->client_close(sockfd);
    } else {
        wifi_client->stop();
    }
#else
    wifi_client->stop();
#endif
}

void HostInfo::client_connect(IPAddress ip, int port) {
#if !defined(LITE_VERSION)
    if (eth_client != nullptr) {
        sockfd = eth_client->connect(ip, port, 3000);
    } else {
        wifi_client->connect(ip, port);
    }
#else
    wifi_client->connect(ip, port);
#endif
};

bool HostInfo::client_connected() {
#if !defined(LITE_VERSION)
    if (eth_client != nullptr) {
        return eth_client->connected(sockfd);
    } else {
        return wifi_client->connected();
    }
#else
    return wifi_client->connected();
#endif
}

void HostInfo::setup(const Host &host) {
    const int MAX_SIMULTANEOUS = 10; // Number of simultaneous connection attempts
    const int TIMEOUT_MS = 1000;     // Timeout for each connection attempt
    int scannedPorts = 0;
    // Initialize display
    drawMainBorder();
    tft.setTextSize(FP);
    tft.setCursor(8, 30);
    tft.print("Host: " + host.ip.toString());
    tft.setCursor(8, 42);
    tft.print("Mac: " + host.mac);
    tft.setCursor(8, 54);
    tft.print("Manufacturer: " + getManufacturer(host.mac));
    tft.setCursor(8, 66);
    tft.print("Scanning Ports...(hold esc to cancel)");
    tft.setCursor(8, 78);
    tft.print("Ports Open: ");

    std::vector<PortScan> scans(MAX_SIMULTANEOUS);
    auto portIter = portServices.begin();
    int activeScanCount = 0;

    // Initialize scans
    for (auto &scan : scans) { scan.inProgress = false; }

    bool scanCanceled = false;
    while ((portIter != portServices.end() || activeScanCount > 0) && !scanCanceled) {
        // Check for escape press
        if (check(EscPress)) {
            scanCanceled = true;
            // Stop all active scans
            for (auto &scan : scans) {
                if (scan.inProgress) {
                    client_stop();
                    scan.inProgress = false;
                }
            }
            break;
        }

        // Start new scans if possible
        while (activeScanCount < MAX_SIMULTANEOUS && portIter != portServices.end()) {
            for (auto &scan : scans) {
                if (!scan.inProgress) {
                    scan.port = portIter->first;
                    scan.startTime = millis();
                    scan.inProgress = true;
                    printFootnote(
                        "scannng port: " + String(scan.port) +
                        " | remaining: " + String(portServices.size() - scannedPorts)
                    ); // printa portas escaneadas e restantes

                    client_connect(host.ip, scan.port);
                    activeScanCount++;
                    portIter++;
                    break;
                }
            }
        }

        // Check ongoing scans
        for (auto &scan : scans) {
            if (scan.inProgress) {
                // Check if connected
                if (client_connected()) {
                    if (tft.getCursorX() > (240 - LW * 4)) tft.setCursor(7, tft.getCursorY() + LH);
                    tft.setCursor(7, tft.getCursorY() + LH);
                    tft.print(scan.port);
                    tft.print(" (" + String(portServices[scan.port]) + ")");
                    client_stop();
                    scan.inProgress = false;
                    activeScanCount--;
                }
                // Check for timeout
                else if (millis() - scan.startTime > TIMEOUT_MS) {
                    client_stop();
                    scan.inProgress = false;
                    activeScanCount--;
                    scannedPorts++;
                }
            }
        }
        yield(); // Allow other tasks to run
    }

    tft.setCursor(8, tft.getCursorY() + 16);
    if (scanCanceled) {
        tft.print("Scan Canceled!");
    } else {
        tft.print("Done!");
    }

    while (check(SelPress)) yield();
    while (!check(SelPress)) yield();
}
