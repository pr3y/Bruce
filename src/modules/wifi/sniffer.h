#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include <WiFi.h>

void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf,File pcap_file);

void openFile(FS &Fs);

bool writeHeader(File file);

void sniffer_setup();

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);