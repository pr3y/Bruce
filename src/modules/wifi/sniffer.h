#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <WiFi.h>
#include <set>

struct BeaconList {
    char MAC[6];
    uint8_t channel;
    // Define comparison operator to use <set>
    bool operator<(const BeaconList &other) const {
        // Compare MACs (using memcmp)
        int cmp = memcmp(MAC, other.MAC, sizeof(MAC));
        if (cmp != 0) {
            return cmp < 0; // if MACs are diferent, compares lexicografically
        }
        return channel < other.channel; // If MACs are equal, compare by channel
    }
};

enum class SnifferMode : uint8_t {
    Full,
    HandshakesOnly,
    DeauthOnly,
};

extern int num_HS;
extern int num_EAPOL;
extern bool isLittleFS;
extern uint8_t ch;

void setHandshakeSniffer();
void sniffer_set_mode(SnifferMode mode);
SnifferMode sniffer_get_mode();
bool sniffer_full_mode_available();
bool sniffer_prepare_storage(FS *fs, bool sdDetected);
void sniffer_wait_for_flush(uint32_t timeoutMs = 2000);
void sniffer_reset_handshake_cache();
void markHandshakeReady(uint64_t key);

extern std::set<BeaconList> registeredBeacons;
extern std::set<String> SavedHS;

void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t *buf, File pcap_file);

void openFile(FS &Fs);

bool writeHeader(File file);

void sniffer_setup();

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);
