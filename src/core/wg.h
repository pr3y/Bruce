#include <LittleFS.h>
#include <WiFi.h>
#include <WireGuard-ESP32.h>

extern bool isConnectedWireguard;

void parse_config_file(fs::File configFile);

void read_and_parse_file();

void wg_setup();