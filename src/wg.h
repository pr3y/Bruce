#include <SPIFFS.h>
#include <WiFi.h>

extern bool is_connected;

void parse_config_file(fs::File configFile);

void read_and_parse_file();

void wg_setup();