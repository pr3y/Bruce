#include "core/globals.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "scan_hosts.h"
#include "clients.h"

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

void local_scan_setup() {
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

        displayRedStripe("Probing " + String(broadcast - networkAddress - 1) + " hosts",TFT_WHITE, bruceConfig.priColor); // minus broadcast and subnet mask

        // send arp requests, read table each ARP_TABLE_SIZE requests
        uint16_t tableReadCounter = 0;
        for( uint32_t ip_le = networkAddress + 1; ip_le < broadcast; ++ip_le ){
          if( ip_le == localIp ) continue;

          ip4_addr_t ip_be{htonl(ip_le)}; // big endian
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
          String result = host.ip.toString().substring(host.ip.toString().lastIndexOf('.') - 1);
          if( host.ip == gateway ) result += "(GATE)";
          options.push_back({result.c_str(), [=](){ afterScanOptions(host); }});
        }
        options.push_back({"Main Menu", [=]() { backToMenu(); }});

        delay(200);
        loopOptions(options);
        delay(200);
        if(!returnToMenu) goto ScanHostMenu;
    }
    hostslist.clear();
}

void afterScanOptions(const Host& host) {
  options = {
    {"Host info", [=](){ hostInfo(host); }},
  #ifndef LITE_VERSION
    {"SSH Connect", [=](){ ssh_setup(host.ip.toString()); }},
  #endif
  };
  loopOptions(options);
  delay(200);
}

void hostInfo(const Host& host) {
  WiFiClient client;
  const int ports[] = {20, 21, 22, 23, 25, 80, 137, 139, 443, 3389, 8080, 8443, 9090};
  const int numPorts = sizeof(ports) / sizeof(ports[0]);
  drawMainBorder();
  tft.setTextSize(FP);
  tft.setCursor(8,30);

  tft.print("Host: " + host.ip.toString());
  tft.setCursor(8,42);

  tft.print("Mac: " + host.mac);
  tft.setCursor(8,54);

  tft.print("Manufacturer: " + getManufacturer(host.mac));
  tft.setCursor(8,66);

  tft.print("Ports Opened: ");
  for (int i = 0; i < numPorts; i++) {
    int port = ports[i];

    // TODO: print the ports being scanned
    // right now there is no idea to understand which ports are being scanned
    // without looking at the code
    if (client.connect(host.ip, port)) {
      if (tft.getCursorX()>(240-LW*4)) tft.setCursor(7,tft.getCursorY() + LH);
      tft.print(port);
      tft.print(", ");
      client.stop();
    } else tft.print(".");
  }
  tft.setCursor(8,tft.getCursorY()+16);
  tft.print("Done!");

  while(checkSelPress()) yield();
  while(!checkSelPress()) yield();
}
