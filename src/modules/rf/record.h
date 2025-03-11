#include "core/display.h"

struct RawRecordingStatus {
    float frequency = 0.f;
    int rssiCount = 0;       // Counter for the number of RSSI readings
    int latestRssi = 0;      // Store the latest RSSI value
    bool recording = false;
    unsigned long firstSignalTime = 0; // Store the time of the latest signal
    unsigned long lastSignalTime = 0; // Store the time of the latest signal
    unsigned long lastRssiUpdate = 0;
};

void rf_raw_record_draw(RawRecordingStatus status);
void rf_raw_record();