#include "net_utils.h"

#include <WiFi.h>
#include <ESPping.h>
#include <HTTPClient.h>

bool internetConnection(){
    return Ping.ping(IPAddress(8,8,8,8));
}

String getManufacturer(const String& mac){
  if( !internetConnection() ){ return "NO_INTERNET_ACCESS"; }

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

String MAC(uint8_t* data){
    char macStr[18];  
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            data[0], data[1], data[2],
            data[3], data[4], data[5]);

    return macStr;
}