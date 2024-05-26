#include "globals.h"
#include "scan_hosts.h"
#include "display.h"
#include "mykeyboard.h"
#include "wifi_common.h"



void logPingResult(IPAddress host, bool responded) {
  char buffer[64]; // Tamanho do buffer ajustável conforme necessário
  if (responded) {
    sprintf(buffer, "Host %s respondeu ao ping.", host.toString().c_str());
  } else {
    sprintf(buffer, "Host %s não respondeu ao ping.", host.toString().c_str());
  }
  log_d("%s",buffer);
}

bool pingHost(IPAddress host) {
  int count = Ping.ping(host, 1); // Ping com 1 tentativa
  bool responded = (count > 0);
  logPingResult(host, responded);
  return responded;
}

void local_scan_setup() {
    if(!wifiConnected) wifiConnectMenu();

    IPAddress gatewayIP;
    IPAddress subnetMask;
    std::vector<IPAddress> responderam;
    drawMainBorder();
    tft.setTextSize(FP);
    tft.setCursor(8,30);

    gatewayIP = WiFi.gatewayIP();
    subnetMask = WiFi.subnetMask();

    IPAddress network = WiFi.localIP();
    network[3] = 0; // Define o endereço de rede para o primeiro host
    
    int numHosts = 254 - subnetMask[3]; // Calcula o número de hosts possíveis na rede
    tft.println("Probing " + String(numHosts) + " hosts (" +  String(numHosts/40 + 1).substring(0,4) + " lines)");
    tft.fillRect(0,38,WIDTH,LH*(numHosts/40 + 1), BGCOLOR);
    tft.drawRect(0,38,WIDTH,LH*(numHosts/40 + 1), FGCOLOR);
    tft.fillRect(6,38,WIDTH-12,LH*(numHosts/40 + 1), BGCOLOR);

    for (int i = 1; i <= numHosts; i++) {
        IPAddress currentIP = network;
        currentIP[3] = i;
        
        if (pingHost(currentIP)) {
        tft.print("x");
        responderam.push_back(currentIP);
        } else tft.print(".");

        if(checkEscPress()) i=256; //ends for loop

    }
    options = {};
    log_d("Hosts que responderam ao ping:");
    for (IPAddress ip : responderam) {
        String txt = "..." + String(ip[2]) + "." + String(ip[3]);
        options.push_back({ txt.c_str(), [=](){ displayInfo(ip.toString().c_str()); }});
    }

    loopOptions(options);

    while(!checkEscPress()) yield();
    
}
