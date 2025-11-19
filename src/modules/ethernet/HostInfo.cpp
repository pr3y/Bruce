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

    // Array of TCP ports to scan
    const int portNumbers[] = {
        19, 20, 21, 22, 23, 25, 42, 53, 67, 68, 69, 80, 88,
        110, 111, 113, 119, 123, 135, 137, 139, 143, 161, 162,
        179, 194, 389, 427, 443, 445, 464, 465, 500, 514, 515,
        520, 554, 587, 631, 636, 873, 902, 989, 990, 993, 995,
        1024, 1025, 1080, 1194, 1337, 1352, 1433, 1434, 1512,
        1521, 1604, 1701, 1720, 1723, 1812, 1813, 1883, 1900,
        2049, 2082, 2083, 2086, 2087, 2095, 2181, 2222, 2375,
        2376, 2379, 2380, 3128, 3306, 3389, 3690, 4000, 4500,
        4789, 5000, 5060, 5061, 5222, 5353, 5432, 5671, 5672,
        5900, 5985, 5986, 6379, 6443, 6514, 6667, 7001, 7077,
        8000, 8080, 8081, 8443, 8444, 8888, 9000, 9042, 9090,
        9100, 9200, 9300, 9418, 10000, 11211, 15672, 27017,
        32768, 49152, 49153, 49154, 49155, 49156, 49157
    };
    const int portCount = sizeof(portNumbers) / sizeof(portNumbers[0]);

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
    int currentPortIndex = 0;
    int activeScanCount = 0;

    for (auto &scan : scans) { scan.inProgress = false; }

    bool scanCanceled = false;
    while ((currentPortIndex < portCount || activeScanCount > 0) && !scanCanceled) {
        if (check(EscPress)) {
            scanCanceled = true;
            for (auto &scan : scans) {
                if (scan.inProgress) {
                    client_stop();
                    scan.inProgress = false;
                }
            }
            break;
        }

        while (activeScanCount < MAX_SIMULTANEOUS && currentPortIndex < portCount) {
            for (auto &scan : scans) {
                if (!scan.inProgress) {
                    scan.port = portNumbers[currentPortIndex];
                    scan.startTime = millis();
                    scan.inProgress = true;

                    printFootnote(
                        "scanning port: " + String(scan.port) +
                        " | remaining: " + String(portCount - scannedPorts)
                    );

                    client_connect(host.ip, scan.port);
                    activeScanCount++;
                    currentPortIndex++;
                    break;
                }
            }
        }

        for (auto &scan : scans) {
            if (scan.inProgress) {
              if (client_connected()) {
                    if (tft.getCursorX() > (240 - LW * 4)) tft.setCursor(7, tft.getCursorY() + LH);
                    tft.setCursor(7, tft.getCursorY() + LH);
                    tft.print(scan.port, DEC);  // print port number as decimal
                    client_stop();
                    scan.inProgress = false;
                    activeScanCount--;
                } else if (millis() - scan.startTime > TIMEOUT_MS) {
                    client_stop();
                    scan.inProgress = false;
                    activeScanCount--;
                    scannedPorts++;
                }
            }
        }
        yield();
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


