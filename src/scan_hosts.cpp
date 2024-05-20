#include "scan_hosts.h"
#include "display.h"
#include "globals.h"

std::vector<std::string> successful_hosts;

void local_net_scan_setup() {
  
      if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("Connect to wifi before using arpscan");
      displayRedStripe("CONNECT TO WIFI",TFT_WHITE, TFT_RED);
      delay(5000);
      return;
    }
    tft.fillScreen(TFT_BLACK);
    

    displayRedStripe("Scanning..",TFT_WHITE,FGCOLOR);

  String localIP = WiFi.localIP().toString();
  int lastDotIndex = localIP.lastIndexOf('.');
  String baseIP = localIP.substring(0, lastDotIndex + 1);

  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.setTextColor(FGCOLOR, BGCOLOR);

  for (int i = 1; i <= 255; i++) {
    String host = baseIP + String(i); 
    Serial.print("Pinging ");
    Serial.print(host);
    Serial.print(": ");

    if (Ping.ping(host.c_str(),1)) {
      Serial.println("Success!");
      successful_hosts.push_back(host.c_str());
      tft.println(host.c_str());
      
    } else {
      Serial.println("Failed.");
    }
  }
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);

    options = { };
    for(int i=0; i<sizeof(successful_hosts); i++){
      options.push_back({successful_hosts[i], [=]() { displayRedStripe("SCAN PORTS"); }});
    }
    delay(200);
    loopOptions(options);

    Serial.println("ping sweep done");

    tft.setTextColor(TFT_DARKGREEN, BGCOLOR);
    tft.println(WiFi.localIP().toString().c_str());
    tft.setTextColor(FGCOLOR, BGCOLOR);
    delay(5000);

}

