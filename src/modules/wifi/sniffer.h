#pragma once
#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include <WiFi.h>
#include <set>

struct BeaconList {
    char MAC[6];
    uint8_t channel;
// Definindo o operador de comparação
    bool operator<(const BeaconList& other) const {
        // Compara os MACs (usando memcmp)
        int cmp = memcmp(MAC, other.MAC, sizeof(MAC));
        if (cmp != 0) {
            return cmp < 0;  // Se MACs forem diferentes, compara lexicograficamente
        }
        return channel < other.channel;  // Se MACs forem iguais, compara pelo canal
    }    
};

extern bool _only_HS;

extern int num_HS;

extern uint8_t ch;

void setHandshakeSniffer();

extern std::set<BeaconList> registeredBeacons;

void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf,File pcap_file);

void openFile(FS &Fs);

bool writeHeader(File file);

void sniffer_setup();

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);