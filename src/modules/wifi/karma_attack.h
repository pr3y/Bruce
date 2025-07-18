
#include <Arduino.h>
#include <vector>
#include <set>
#include "FS.h"
#include "esp_wifi_types.h"

// Forward declaration for FS class
namespace fs {
    class FS;
}

// Probe Request Structure
typedef struct {
    String mac;
    String ssid;
    int rssi;
    unsigned long timestamp;
    uint8_t channel;
} ProbeRequest;

//===== FUNCTION DECLARATIONS =====//

// Main sniffer function
void karma_setup();

// Packet processing functions
bool isProbeRequestWithSSID(const wifi_promiscuous_pkt_t *packet);
String extractSSID(const wifi_promiscuous_pkt_t *packet);
String extractMAC(const wifi_promiscuous_pkt_t *packet);

// Data management functions
void saveProbeRequest(const ProbeRequest &probe, fs::FS &fs);
void saveProbesToFile(fs::FS &fs);
void clearProbes();

// Data retrieval functions
std::vector<ProbeRequest> getUniqueProbes();
std::vector<ProbeRequest> getAllProbes();

// Utility functions
void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);

//===== GLOBAL VARIABLES =====//
extern std::vector<ProbeRequest> probeRequests;
extern std::set<String> uniqueProbes;
extern uint32_t packet_counter;
extern uint8_t ch;
extern bool isLittleFS;
