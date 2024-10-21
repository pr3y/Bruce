#include "core/globals.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "scan_hosts.h"
#include "clients.h"
#include <ESPping.h>
#include <HTTPClient.h>

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
    if(!wifiConnected) doScan=wifiConnectMenu(false);

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
<<<<<<< HEAD
        etharp_cleanup_netif(net_iface); // to avoid gateway duplication
=======
>>>>>>> scan_hosts rework/expansion

        displayRedStripe("Probing " + String(broadcast - networkAddress - 1) + " hosts",TFT_WHITE, FGCOLOR); // minus broadcast and subnet mask

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

// TODO create config option to toggle this info
String getManufacturer(const String& mac){
  // TODO move it to core functions that checks internet access
  if( !Ping.ping(IPAddress(8,8,8,8)) ){ return "NO_INTERNET_ACCESS"; }

  // there is an official(IEEE) doc that contains all registered mac prefixes
  // but it is around 700kb and i don't know a way to get specific part
  // without downloading the whole txt
  HTTPClient http;
  http.begin("https://api.maclookup.app/v2/macs/" + mac); 
  int httpCode = http.GET();  // Send the request
  if( httpCode != 200 ){ http.end(); return "GET failed"; } 

  // payload is a json of the format
  // {"success":true,"found":true,"macPrefix":"2C3358","company":"Intel Corporate","address":"Lot 8, Jalan Hi-Tech 2/3, Kulim Kedah 09000, MY","country":"MY","blockStart":"2C3358000000","blockEnd":"2C3358FFFFFF","blockSize":16777215,"blockType":"MA-L","updated":"2021-10-13","isRand":false,"isPrivate":false}
  // company field is going to be empty if none found
  String payload{http.getString()};
  size_t company_start_idx = payload.indexOf("company") + 10; // + 7(company) + 3(":")
  String manufacturer = payload.substring(company_start_idx, payload.indexOf('"', company_start_idx));
  if( manufacturer.isEmpty() ) return "UNKNOWN";

  return manufacturer;
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
