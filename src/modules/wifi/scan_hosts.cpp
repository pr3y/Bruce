#include "lwip/etharp.h"
#include "core/globals.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "scan_hosts.h"
#include "clients.h"

//thx to 7h30th3r0n3, which made scanHosts faster using ARP

std::vector<IPAddress> hostslist;

void read_arp_table(char * from_ip, int read_from, int read_to, std::vector<IPAddress>& hostslist) {
  Serial.printf("Reading ARP table from: %d to %d\n", read_from, read_to);
  for (int i = read_from; i <= read_to; i++) {
    char test[32];
    sprintf(test, "%s%d", from_ip, i);
    ip4_addr_t test_ip;
    ipaddr_aton(test, (ip_addr_t*)&test_ip);

    const ip4_addr_t *ipaddr_ret = NULL;
    struct eth_addr *eth_ret = NULL;
    if (etharp_find_addr(NULL, &test_ip, &eth_ret, &ipaddr_ret) >= 0) {
      IPAddress foundIP;
      foundIP.fromString(ipaddr_ntoa((ip_addr_t*)&test_ip));
      hostslist.push_back(foundIP);
      //tft.println(foundIP.toString());
      String result = foundIP.toString().substring(foundIP.toString().lastIndexOf('.') - 1);
      options.push_back({result.c_str(), [=](){afterScanOptions(foundIP); }});
      Serial.printf("Adding found IP: %s\n", ipaddr_ntoa((ip_addr_t*)&test_ip));
    }
  }
}

void send_arp(char * from_ip, std::vector<IPAddress>& hostslist) {
  Serial.println("Sending ARP requests to the whole network");
  const TickType_t xDelay = (10) / portTICK_PERIOD_MS;
  void * netif = NULL;
  tcpip_adapter_get_netif(TCPIP_ADAPTER_IF_STA, &netif);
  struct netif *netif_interface = (struct netif *)netif;

  for (char i = 1; i < 254; i++) {
    char test[32];
    sprintf(test, "%s%d", from_ip, i);
    ip4_addr_t test_ip;
    ipaddr_aton(test, (ip_addr_t*)&test_ip);

    int8_t arp_request_ret = etharp_request(netif_interface, &test_ip);
    vTaskDelay(xDelay);
  }
  // Lire toutes les entrées de la table ARP
  read_arp_table(from_ip, 1, 254, hostslist);
}

void logARPResult(IPAddress host, bool responded) {
  char buffer[64];
  if (responded) {
    sprintf(buffer, "Host %s respond to ARP.", host.toString().c_str());
  } else {
    sprintf(buffer, "Host %s did not respond to ARP.", host.toString().c_str());
  }
  Serial.println(buffer);
}

bool arpRequest(IPAddress host) {
  char ipStr[16];
  sprintf(ipStr, "%s", host.toString().c_str());
  ip4_addr_t test_ip;
  ipaddr_aton(ipStr, (ip_addr_t*)&test_ip);

  struct eth_addr *eth_ret = NULL;
  const ip4_addr_t *ipaddr_ret = NULL;
  bool responded = etharp_find_addr(NULL, &test_ip, &eth_ret, &ipaddr_ret) >= 0;
  logARPResult(host, responded);
  return responded;
}


void local_scan_setup() {
    bool doScan = false;
    if(!wifiConnected) doScan=wifiConnectMenu(false);

    if (doScan) {
        int lastDot = WiFi.localIP().toString().lastIndexOf('.');
        String networkRange = WiFi.localIP().toString().substring(0, lastDot + 1);
        char networkRangeChar[12];

        networkRange.toCharArray(networkRangeChar, sizeof(networkRangeChar));

        send_arp(networkRangeChar, hostslist);

        options = {};

        IPAddress gatewayIP;
        IPAddress subnetMask;
        std::vector<IPAddress> hostslist;

        gatewayIP = WiFi.gatewayIP();
        subnetMask = WiFi.subnetMask();

        IPAddress network = WiFi.localIP();
        network[3] = 0;

        int numHosts = 254 - subnetMask[3];

        displayRedStripe("Probing " + String(numHosts) + " hosts",TFT_WHITE, FGCOLOR);

        bool foundHosts;
        bool stopScan;

        char base_ip[16];
        sprintf(base_ip, "%d.%d.%d.", network[0], network[1], network[2]);

        send_arp(base_ip, hostslist);

        for (int i = 1; i <= numHosts; i++) {
            if (stopScan) {
                break;
            }

            IPAddress currentIP = network;
            currentIP[3] = i;

            if (arpRequest(currentIP)) {
                hostslist.push_back(currentIP);
                foundHosts = true;
            }
        }

        if (!foundHosts) {
            tft.println("No hosts found");
            delay(2000);
            return;
        }


        delay(200);
        loopOptions(options);
        delay(200);
    }
}


void afterScanOptions(IPAddress ip) {
  options = {
    {"Scan Ports", [=](){ scanPorts(ip); }},
  #ifndef LITE_VERSION
    {"SSH Connect", [=](){ ssh_setup(ip.toString()); }},
  #endif
  };
  loopOptions(options);
  delay(200);
}


void scanPorts(IPAddress host) {
  WiFiClient client;
  const int ports[] = {20, 21, 22, 23, 25, 80, 137, 139, 443, 3389, 8080, 8443, 9090};
  const int numPorts = sizeof(ports) / sizeof(ports[0]);
  drawMainBorder();
  tft.setTextSize(FP);
  tft.setCursor(8,30);
  tft.print("Host: " + host.toString());
  tft.setCursor(8,42);
  tft.print("Ports Opened: ");
  //for (int port = start; port <= stop; port++) {
  for (int i = 0; i < numPorts; i++) {
    int port = ports[i];
    if (client.connect(host, port)) {
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
