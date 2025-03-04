
#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include <ESPmDNS.h>
#include <typeinfo>

extern WebServer* server;  // used to check if the webserver is running

// function defaults
String humanReadableSize(uint64_t bytes);
String listFiles(FS fs, bool ishtml, String folder, bool isLittleFS);
String processor(const String& var);
String readLineFromFile(File myFile);

void loopOptionsWebUi();

void configureWebServer();
void startWebUi(bool mode_ap = false);
