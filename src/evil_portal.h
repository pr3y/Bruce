#include <WiFi.h>
#include "AsyncTCP.h"
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include <SD.h>
#include <SPI.h>
#include <ESPmDNS.h>

// function defaults

void startEvilPortal();

void chooseHtml(bool def = true);

String getHtmlContents(String body);

String creds_GET();

String index_GET();

String clear_GET();

