#include <globals.h>
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "scan_printers.h"
#include "clients.h"
#include "wifi_atks.h" // to use Station Deauth
#include "core/utils.h"

//thx to 7h30th3r0n3, which made scanHosts faster using ARP

static std::vector<Host> hostslist;

// TODO: resolve clients name when in host mode through dhcp

// TODO: move to a config
TickType_t arpRequestDelay = 20u / portTICK_PERIOD_MS; // can be relatively low, helps to not overwhelm the stream

void readArpTable(netif * iface) {
  for( uint32_t i = 0; i < ARP_TABLE_SIZE; ++i ){
    ip4_addr_t* ip_ret;
    eth_addr* eth_ret;
    if( etharp_get_entry(i, &ip_ret, &iface, &eth_ret) ){
      hostslist.emplace_back(ip_ret, eth_ret);
    }
  }
  etharp_cleanup_netif(iface);
}

void scanForPrinters() {
    bool doScan = true;
    if(!wifiConnected) doScan=wifiConnectMenu();

    if (doScan) {
        hostslist.clear();

        // IPAddress uint32_t op returns number in big-endian
        // for simplicity of iteration and arithmetics convert to little-endian
        const uint32_t localIp = ntohl(WiFi.localIP());
        const IPAddress gateway = WiFi.gatewayIP();
        const uint32_t subnetMask = ntohl(WiFi.subnetMask());
        const uint32_t networkAddress = ntohl(gateway) & subnetMask;
        const uint32_t broadcast = networkAddress | ~subnetMask;

        // get iface
        void * netif = nullptr;
        tcpip_adapter_get_netif(TCPIP_ADAPTER_IF_STA, &netif);
        struct netif *net_iface = (struct netif *)netif;
        etharp_cleanup_netif(net_iface); // to avoid gateway duplication


        // send arp requests, read table each ARP_TABLE_SIZE requests
        uint16_t tableReadCounter = 0;
        uint32_t hostsScanned = 0;
        const uint32_t totalHosts = broadcast - networkAddress - 1;
        static uint32_t lastUpdate = 0;

        for( uint32_t ip_le = networkAddress + 1; ip_le < broadcast; ++ip_le ){
          if( ip_le == localIp ) continue;

          ip4_addr_t ip_be{htonl(ip_le)}; // big endian

          hostsScanned++;
          if (millis() - lastUpdate > 500) { // Update display every 500ms
            displayRedStripe("Probing " + String(hostsScanned) + " of " + String(totalHosts) + " hosts", getComplementaryColor2(bruceConfig.priColor), bruceConfig.priColor);
            lastUpdate = millis();
          }

          err_t res = etharp_request(net_iface, &ip_be);

          if( res != ERR_OK ){
            Serial.println("Arp req for: " + IPAddress(ip_be.addr).toString() + "failed with ec: " + res);
          } else {
            ++tableReadCounter;
          }

          vTaskDelay(arpRequestDelay);

          // read table if we sent ARP_TABLE_SIZE requests
          if( tableReadCounter == ARP_TABLE_SIZE ){
            readArpTable(net_iface);
            tableReadCounter = 0;
          }
        }

        ScanHostMenu:
        if (hostslist.empty()) {
            tft.println("No hosts found");
            delay(2000);
            return;
        }

        options = {};
        for(auto host:hostslist) {
          String result = host.ip.toString();
          if( host.ip == gateway ) result += "(GTW)";
          options.push_back({result.c_str(), [=](){ afterScanOptions(host); }});
        }
        options.push_back({"Main Menu", [=]() { backToMenu(); }});

        loopOptions(options);

        if(!returnToMenu) goto ScanHostMenu;
    }
    hostslist.clear();
}

void afterScanOptions(const Host& host) {
  int opt=0;
  options = {
    {"Host info",       [=](){ hostInfo(host); }}
  };
  loopOptions(options);
  if(opt==3) stationDeauth(host);
  if(opt==5)  {
    Serial.println("Starting MITM");
    arpSpoofing(host, true);
  }
}

struct PrintPortScan { //struct pra holdar info das portas
    int port;
    unsigned long startTime;
    WiFiClient client;
    bool inProgress;
};
std::map<int, std::string> portServices = {
    {515, "LPD, Line Printer Daemon"},
    {631, "IPP, Internet Printing Protocol, CUPS"},
    {9100, "Raw Printing (JetDirect)"},
};

void hostInfo(const Host& host) {
    const int MAX_SIMULTANEOUS = 10;  // Number of simultaneous connection attempts
    const int TIMEOUT_MS = 1000;      // Timeout for each connection attempt
    int scannedPorts = 0;
    // Initialize display
    drawMainBorder();
    tft.setTextSize(FP);
    tft.setCursor(8,30);
    tft.print("Host: " + host.ip.toString());
    tft.setCursor(8,42);
    tft.print("Mac: " + host.mac);
    tft.setCursor(8,54);
    tft.print("Manufacturer: " + getManufacturer(host.mac));
    tft.setCursor(8,66);
    tft.print("Scanning Ports...(hold esc to cancel)");
    tft.setCursor(8,78);
    tft.print("Ports Open: ");

    std::vector<PrintPortScan> scans(MAX_SIMULTANEOUS);
    auto portIter = portServices.begin();
    int activeScanCount = 0;

    // Initialize scans
    for(auto& scan : scans) {
        scan.inProgress = false;
    }

    bool scanCanceled = false;
    while((portIter != portServices.end() || activeScanCount > 0) && !scanCanceled) {
        // Check for escape press
        if(check(EscPress)) {
            scanCanceled = true;
            // Stop all active scans
            for(auto& scan : scans) {
                if(scan.inProgress) {
                    scan.client.stop();
                    scan.inProgress = false;
                }
            }
            break;
        }

        // Start new scans if possible
        while(activeScanCount < MAX_SIMULTANEOUS && portIter != portServices.end()) {
            for(auto& scan : scans) {
                if(!scan.inProgress) {
                    scan.port = portIter->first;
                    scan.startTime = millis();
                    scan.inProgress = true;
                    printFootnote("scannng port: "+String(scan.port)+ " | remaining: "+String(portServices.size()-scannedPorts)); //printa portas escaneadas e restantes
                    scan.client.connect(host.ip, scan.port);
                    activeScanCount++;
                    portIter++;
                    break;
                }
            }
        }

        // Check ongoing scans
        for(auto& scan : scans) {
            if(scan.inProgress) {
                // Check if connected
                if(scan.client.connected()) {
                    if (tft.getCursorX()>(240-LW*4))
                        tft.setCursor(7,tft.getCursorY() + LH);
                    tft.setCursor(7, tft.getCursorY() + LH);
                    tft.print(scan.port);
                    tft.print( " (" + String(portServices[scan.port].c_str())+ ")");
                    scan.client.stop();
                    scan.inProgress = false;
                    activeScanCount--;
                                   }
                // Check for timeout
                else if(millis() - scan.startTime > TIMEOUT_MS) {
                    scan.client.stop();
                    scan.inProgress = false;
                    activeScanCount--;
                    scannedPorts++;
                }
            }
        }
        yield(); // Allow other tasks to run
    }

    tft.setCursor(8,tft.getCursorY()+16);
    if(scanCanceled) {
        tft.print("Scan Canceled!");
    } else {
        tft.print("Done!");
    }

    while(check(SelPress)) yield();
    while(!check(SelPress)) yield();
}

